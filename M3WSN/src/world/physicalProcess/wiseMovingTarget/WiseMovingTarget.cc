// *****************************************************************************
//  Copyright (C): Christian Nastasi, 2011                                    
//  Author(s): Christian Nastasi                                              
//  Developed at the:                                                         
//  - Retis LAB, TeCIP Institute, Scuola Superiore Sant'Anna (Pisa)           
//  - School of Elec Eng and Computer Science, Queen Mary University (London) 
//  This file is distributed under the terms in the attached LICENSE_2 file.  
//  If you do not find this file, copies can be found by writing to:          
//  - c.nastasi@sssup.it                                                      
//  - nastasichr@gmail.com                                                    
//  - andrea.cavallaro@eecs.qmul.ac.uk                                        
// *****************************************************************************

#include "WiseMovingTarget.h"
#include "WiseDebug.h"
#include <sstream>
#include <cmath>

#define perr_app(m) opp_error("\n[WiseMovingTarget]:\n ")
#define perr(m) opp_error(m)

#ifndef PI
#define PI (3.141592653589793)
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define TMP_STEP_SIZE 2

using namespace std;

Define_Module(WiseMovingTarget);

ofstream *WiseMovingTarget::writer = NULL;

WiseMovingTarget::~WiseMovingTarget() {
	if (writer != NULL) {
		writer->close();
		delete writer;
		writer = NULL;
	}
	if (inputType == SCENARIO_BASED) {
		// deallocate sourcesEvolution
		for (int i = 0; i < numSources; i++) {
			delete[]sources_snapshots[i];
		}
		delete[]sources_snapshots;
		delete[]curr_source_state;
		delete[]source_index;
	} else {
		delete[]valuesTable;
	}
}
	
void WiseMovingTarget::initialize() {
	WiseBasePhysicalProcess::initialize();
	WISE_DEBUG_3("WiseMovingTarget::initialize()");

	load_parameters();
	x_curr = x_init;
	y_curr = y_init;
	z_curr = z_init;
	target_info.pos_x = x_init;
	target_info.pos_y = y_init;
	target_info.pos_z = z_init;
	target_info.bb_width = target_width;
	target_info.bb_height = target_height;
	target_info.bb_depth = target_depth;
	lin_x_step = TMP_STEP_SIZE;
	lin_y_step = TMP_STEP_SIZE;
	diagonal = lin_x_step / sqrt(TMP_STEP_SIZE);
	rotation_angle = 0;
	radius = MIN(terrain->get_x_size(), terrain->get_y_size()) / 3.0;
	rotation_step = (5.0 / 180.0) * PI;
	if (move_func != NULL)
		scheduleAt(simTime() + update_time + move_later, 
			   new cMessage("Move", TIMER_SERVICE));
	else
		terrain->place_target(target_info);
	if (log_enabled) {
		if (writer == NULL)
			writer = new ofstream("targets_info.txt");
		log_information();
	}

	readIniFileParameters();
	if (inputType == SCENARIO_BASED) {
		readScenariosFromIniFile();
		initHelpStructures();	// Allocate and Initialize sourcesEvolution
		time = -1;
	}
}


void WiseMovingTarget::readIniFileParameters() {
	inputType = par("inputType");
	numNodes = getParentModule()->par("numNodes");

	switch (inputType) {
		case 0: {
			inputType = DIRECT_NODE_VALUES;
			break;
		}

		case 1: {
			inputType = SCENARIO_BASED;
			break;
		}

		case 2: {
			inputType = TRACE_FILE;
			break;
		}

		default: {
			opp_error("\nError! Illegal value of parameter \"inputType\" of CustomizablePhysicalProcess module.\n");
		}
	}

	if (inputType == DIRECT_NODE_VALUES) {
		cStringTokenizer valuesTokenizer(par("directNodeValueAssignment"), " ");
		int totalTokens = 0;
		int isFirstToken = 1;

		while (valuesTokenizer.hasMoreTokens()) {
			totalTokens++;
			string token(valuesTokenizer.nextToken());

			if (isFirstToken) {
				string::size_type posA = token.find("(", 0);
				string::size_type posB = token.find(")", 0);

				if ((posA != 0) || (posB == string::npos))
					opp_error("\nError!(A) Illegal parameter format \"directNodeValueAssignment\" of CustomizablePhysicalProcess module.\n");

				token = token.substr(1, token.size() - 1);

				if (token.size() < 1)
					opp_error("\nError!(B) Illegal parameter format \"directNodeValueAssignment\" of CustomizablePhysicalProcess module.\n");

				defaultValue = atof(token.c_str());
				valuesTable = new double[numNodes];
				for (int i = 0; i < numNodes; i++)
					valuesTable[i] = defaultValue;
				isFirstToken = 0;
			} else {
				string::size_type tokenSize = token.length();
				string::size_type posA = token.find(":");

				if (tokenSize >= 3) {
					if ((posA == string::npos) || (posA == 0) || (posA == tokenSize - 1))
						opp_error("\nError! Illegal parameter format \"directNodeValueAssignment\" of CustomizablePhysicalProcess module.\n");

					string tokA;
					string tokB;
					tokA = token.substr(0, posA);
					tokB = token.substr(posA + 1);

					int nodeId = atoi(tokA.c_str());
					double nodeVal = atof(tokB.c_str());

					if (nodeId < 0 || nodeId > numNodes - 1)
						opp_error("\nError! Illegal parameter format \"directNodeValueAssignment\" of CustomizablePhysicalProcess module.\n");

					valuesTable[nodeId] = nodeVal;
				}
			}
		}
	} else if (inputType == SCENARIO_BASED) {
		k = par("multiplicative_k");
		a = par("attenuation_exp_a");
		sigma = par("sigma");
		max_num_snapshots = par("max_num_snapshots");
		description = par("description");
		numSources = par("numSources");
		if (numSources > 5)
			opp_error("Physical Process parameter \"numSources\" has been initialized with invalid value \"%d\"",
numSources);

		/* 
		 * ALLOCATE memory space for the sourceSnapshot 2D array that holds the list of snapshots for every source
		 */

		int i, j;
		sources_snapshots = new sourceSnapshot *[numSources];
		for (i = 0; i < numSources; i++) {
			sources_snapshots[i] = new sourceSnapshot[max_num_snapshots];
			for (j = 0; j < max_num_snapshots; j++)
				sources_snapshots[i][j].time = -1;
		}
	} else {
		/*
		 * TODO: add this functionality in the future for the reading of the TraceFile.
		 */
	}
}

void WiseMovingTarget::initHelpStructures() {
	int i;
	simtime_t starting_time;
	curr_source_state = new sourceSnapshot[numSources];
	source_index = new int[numSources];
	for (i = 0; i < numSources; i++)
		source_index[i] = -1;
}

void WiseMovingTarget::readScenariosFromIniFile() {
	int i, j;
	int totalValidSourcesScenarios = 0, validated;
	char buffer[100];

	for (i = 0; i < numSources; i++) {	//for every source_X string parameter that contains the scenarios for physical process X
		sprintf(buffer, "source_%d", i);
		cStringTokenizer snapshotTokenizer(par(buffer), ";");
		validated = 0;
		j = 0;		//the snapshot index
		while (snapshotTokenizer.hasMoreTokens()) {	//for every snapshot inside the scenario string DO...
			if (!validated) {
				validated = 1;
				totalValidSourcesScenarios++;	//non empty scenario
			}

			cStringTokenizer valuesTokenizer(snapshotTokenizer.nextToken());	//get the next snapshot and split it with " " as delimiter

			if (valuesTokenizer.hasMoreTokens()) {	//get each one of the snapshot parameters
				sources_snapshots[i][j].time = (simtime_t) atof(valuesTokenizer.nextToken());
				if (!valuesTokenizer.hasMoreTokens())
					opp_error("\nPhysical Process parameter error. Malformed  parameter : source_%d\n", i);

				sources_snapshots[i][j].x = atof(valuesTokenizer.nextToken());
				if (!valuesTokenizer.hasMoreTokens())
					opp_error("\nPhysical Process parameter error. Malformed  parameter : source_%d\n", i);

				sources_snapshots[i][j].y = atof(valuesTokenizer.nextToken());
				if (!valuesTokenizer.hasMoreTokens())
					opp_error("\nPhysical Process parameter error. Malformed  parameter : source_%d\n", i);

				sources_snapshots[i][j].value = atof(valuesTokenizer.nextToken());
				if (valuesTokenizer.hasMoreTokens())
					opp_error("\nPhysical Process parameter error. Malformed  parameter : source_%d\n", i);
			}

			j++;
		}
		if (j > max_num_snapshots)
			opp_error("\nPhysical Process intialization ERROR! You tried to pass more snapshots than the parameter \"max_num_snapshots\" specifies.");
	}
}

void WiseMovingTarget::handleMessage(cMessage * msg){
	WISE_DEBUG_3("WiseMovingTarget::handleMessage()");
        //*writer << simTime() << " handleMessage() " << msg->getKind(); 

	WiseMovingTargetMessage *tgt_msg;
	WiseTargetBoundingBox bb;
	double returnValue;

	switch (msg->getKind()) {
		case PHYSICAL_PROCESS_SAMPLING: {
			// NOTE: substitute the req message with a specific one
			WisePhysicalProcessMessage *s_msg;
			s_msg = check_and_cast <WisePhysicalProcessMessage*>(msg);
			WISE_DEBUG_2("WiseMovingTarget: PHYSICAL_PROCESS_SAMPLING:");
			WISE_DEBUG_2("\tsrc_node = " << s_msg->getSrcID()); 
			WISE_DEBUG_2("\tsrc_sensor = " << s_msg->getSensorIndex());
			WISE_DEBUG_2("\ttarget ID (phyProc ID) = " << self); 

			returnValue = calculateSensedValue(x_curr, y_curr, s_msg->getSendingTime());
			tgt_msg = new WiseMovingTargetMessage("Target Message", PHYSICAL_PROCESS_SAMPLING);
			tgt_msg->setTargetID(self);
			calculate_bounding_box(bb);
			tgt_msg->setBoundingBox(bb);
			tgt_msg->setValue(returnValue);
			//tgt_msg->setValue();
			// Send reply back to the node which made the request
			send(tgt_msg, "toNode", s_msg->getSrcID());
			delete s_msg;
			s_msg = tgt_msg;

			//*writer << "objeto -  x " << x_curr  << " y " << y_curr  << " src  " << s_msg->getSrcID() << " timmer " << s_msg->getSendingTime() << " temp " << returnValue << endl;
		        /**writer << ": PHYSICAL_PROCESS_SAMPLING: " ; 
		        *writer << "\tsrc_node = " << s_msg->getSrcID(); 
		        *writer << "\tsrc_sensor = " << s_msg->getSensorIndex(); 
		        *writer << "\ttarget ID (phyProc ID) = " << self << endl; */
			break;
		}
		case TIMER_SERVICE: {
			WISE_DEBUG_2("WiseMovingTarget: TIME_SERVICE:");
		        /**writer << ": TIME_SERVICE" ; 
		        *writer << "\tx_curr = " << x_curr; 
		        *writer << "\ty_curr= " << y_curr; 
		        *writer << "\tz_curr = " << z_curr << endl;*/
		        //terrain->remove_target(target_info);
			if (move_func) // Should not be necessary
				(this->*move_func)();
			if (log_enabled)
				log_information();
		        target_info.pos_x = x_curr;
		        target_info.pos_y = y_curr;
		        target_info.pos_z = z_curr;
		        terrain->place_target(target_info);
			scheduleAt(simTime() + update_time, msg);
			break;
		}
		case PHYSICAL_PROCESS_SCALAR_DATA:{
			// NOTE: substitute the req message with a specific one
			WisePhysicalProcessMessage *rcv_msg;
			rcv_msg = check_and_cast <WisePhysicalProcessMessage*>(msg);
			WISE_DEBUG_2("WiseMovingTarget: PHYSICAL_PROCESS_SAMPLING:");
			WISE_DEBUG_2("\trcv_node = " << rcv_msg->getSrcID()); 
			WISE_DEBUG_2("\trcv_sensor = " << rcv_msg->getSensorIndex());
			WISE_DEBUG_2("\ttarget ID (phyProc ID) = " << self); 

			returnValue = calculateSensedValue(rcv_msg->getXCoor(), rcv_msg->getYCoor(), rcv_msg->getSendingTime());
			tgt_msg = new WiseMovingTargetMessage("sensor reading msg", SENSOR_READING_MESSAGE);
			tgt_msg->setTargetID(self);
			tgt_msg->setValue(returnValue);
			// Send reply back to the node which made the request
			send(tgt_msg, "toNode", rcv_msg->getSrcID());
			delete rcv_msg;
			rcv_msg = tgt_msg;

			//*writer << "Req -  x " << rcv_msg->getXCoor() << " y " << rcv_msg->getYCoor() << " src  " << rcv_msg->getSrcID() << " timmer " << rcv_msg->getSendingTime() << " temp " << returnValue << endl;
			/*writer << simTime() << " msg kind " << msg->getKind(); 
		        *writer << ": PHYSICAL_PROCESS_SCALAR_DATA: " ; 
			*writer << simTime() ; 
		        *writer << "\tsrc_node = " << rcv_msg->getSrcID(); 
		        *writer << "\tsrc_sensor = " << rcv_msg->getSensorIndex();
		        *writer << "\ttarget ID (phyProc ID) = " << self;
			*writer << "\t temp " << returnValue << endl;*/
			break;
		}
		default:
			perr_app("Received message other than PHYSICAL_PROCESS_SAMPLING or TIME_SERVICE");
		delete msg;
	}
}

void WiseMovingTarget::finishSpecific(){
	WISE_DEBUG_3("WiseMovingTarget::finishSpecific()");
}

void WiseMovingTarget::load_parameters(){
	x_init = par("x_init");
	y_init = par("y_init");
	z_init = par("z_init");
	target_width = par("bb_width");
	target_height = par("bb_height");
	target_depth = par("bb_height");
	update_time = par("update_time");
	move_min_x = par("move_min_x");
	move_max_x = par("move_max_x");
	move_min_y = par("move_min_y");
	move_max_y = par("move_max_y");
	move_later = par("move_later");
	log_enabled = par("log_enabled");
	noise = par("noise");
	if (move_min_x != 0 && x_init < move_min_x)
		x_init = move_min_x;
	else if (move_max_x != 0 && x_init > move_max_x)
		x_init = move_max_x;
	if (move_min_y != 0 && y_init < move_min_y)
		y_init = move_min_y;
	else if (move_max_y != 0 && y_init > move_max_y)
		y_init = move_max_y;
	if (par("move_type").stdstringValue() == "lin_noise")
		move_func = &WiseMovingTarget::move_linear_noise;
	else if (par("move_type").stdstringValue() == "circ_noise")
		move_func = &WiseMovingTarget::move_circular_noise;
	else if (par("move_type").stdstringValue() == "lin_random")
		move_func = &WiseMovingTarget::move_linear_random;
	else if (par("move_type").stdstringValue() == "lincirc_noise")
		move_func = &WiseMovingTarget::move_lincirc_noise;
	else if (par("move_type").stdstringValue() == "random_walk")
		move_func = &WiseMovingTarget::move_random_walk;
	else if (par("move_type").stdstringValue() == "random_walk8")
		move_func = &WiseMovingTarget::move_random_walk_8;
	else
		move_func = NULL;
}

void WiseMovingTarget::log_information() {
	static bool first_row = true;
	WiseTargetBoundingBox bb;
	calculate_bounding_box(bb);
	const char *sep = "  ";
	if (first_row) {
		*writer << " sim_time" << sep << "nID" << sep;
		*writer << " x_center" << sep; 
		*writer << " y_center" << sep; 
		*writer << " z_center" << sep; 
		*writer << "  bb_x_tl" << sep; 
		*writer << "  bb_y_tl" << sep; 
		*writer << "  bb_z_tl" << sep; 
		*writer << "  bb_x_br" << sep; 
		*writer << "  bb_y_br" << sep; 
		*writer << "  bb_z_br" << endl; 
		first_row = false;
	}
	*writer << setw(9) << simTime() << sep;
	*writer << setw(3) << self << sep;
	*writer << setw(9) << x_curr << sep;
	*writer << setw(9) << y_curr << sep;
	*writer << setw(9) << z_curr << sep;
	*writer << setw(9) << bb.x_tl << sep;
	*writer << setw(9) << bb.y_tl << sep;
	*writer << setw(9) << bb.z_tl << sep;
	*writer << setw(9) << bb.x_br << sep;
	*writer << setw(9) << bb.y_br << sep;
	*writer << setw(9) << bb.z_br << endl;
}

void WiseMovingTarget::calculate_bounding_box(WiseTargetBoundingBox &bb) {
	double w, h, d;

	w = target_width / 2;
	h = target_height / 2;
	d = target_depth / 2;

	bb.x_tl = (x_curr >= w) ? x_curr - w : 0;
	bb.y_tl = (y_curr >= h) ? y_curr - h : 0;
	bb.z_tl = (z_curr >= d) ? z_curr - d : 0;
	bb.x_br = x_curr + w;
	bb.y_br = y_curr + h;
	bb.z_br = z_curr + d;
}
	
void WiseMovingTarget::move_linear_noise() {
	double s, min, max, rnd;

	min = (move_min_x != 0) ? move_min_x : 0;
	max = (move_max_x != 0) ? move_max_x : terrain->get_x_size();
	//rnd = normal(0, noise, 0);
	rnd = uniform(-(noise / 2), (noise / 2), 0);
	s = x_curr + lin_x_step + rnd;
	if (s > max) {
		s = max;
		lin_x_step *= -1;
	} else if (s < min) { 
		s = min;
		lin_x_step *= -1;
	}
	x_curr = s;

	min = (move_min_y != 0) ? move_min_y : 0;
	max = (move_max_y != 0) ? move_max_y : terrain->get_y_size();
	//rnd = normal(0, noise, 0);
	rnd = uniform(-(noise / 2), (noise / 2), 0);
	s = y_curr + lin_y_step + rnd;
	if (s > max) {
		s = max;
		lin_y_step *= -1;
	} else if (s < min) { 
		s = min;
		lin_y_step *= -1;
	}
	y_curr = s;
}

void WiseMovingTarget::move_circular_noise(double r) {
	double x, y, cos_diff, sin_diff, min, max, rnd;

	// motion equation for x (y uses sin):
	//    cx = x - r*cos(a);
	//    x = r*cos(a + h) + cx = cos (a + h) + x - r *cos(a)
	//    x = x + r*cos(a + h) - r*cos(a) = x + r*(cos(a+h) - cos(a))

	if (r <= 0)
		return;

	cos_diff = cos(rotation_angle + rotation_step) - cos(rotation_angle);
	sin_diff = sin(rotation_angle + rotation_step) - sin(rotation_angle);
	rotation_angle += rotation_step;

	x = x_curr + (cos_diff * r); 
	y = y_curr + (sin_diff * r); 

	min = (move_min_x != 0) ? move_min_x : 0;
	max = (move_max_x != 0) ? move_max_x : terrain->get_x_size();
	//rnd = normal(0, noise, 0);
	rnd = uniform(-(noise / 2), (noise / 2), 0);
	x += rnd;
	x_curr = (x > max) ? max : (x < min) ? min : x;

	min = (move_min_y != 0) ? move_min_y : 0;
	max = (move_max_y != 0) ? move_max_y : terrain->get_y_size();
	//rnd = normal(0, noise, 0);
	rnd = uniform(-(noise / 2), (noise / 2), 0);
	y += rnd;
	y_curr = (y > max) ? max : (y < min) ? min : y;
}

void WiseMovingTarget::move_circular_noise() {
	move_circular_noise(radius);
}

void WiseMovingTarget::move_lincirc_noise() {
	move_linear_noise();
	move_circular_noise(radius);
}

void WiseMovingTarget::move_linear_random() {
	double rnd_x = uniform(0, 1, 0);
	double rnd_y = uniform(0, 1, 0);

	if (rnd_x < 0.100) 
		//lin_x_step = normal(0, 3, 1);
		lin_x_step = normal(0, TMP_STEP_SIZE * 0.5, 1);
		//lin_x_step = uniform(-2, 2, 1);
	if (rnd_y < 0.100) 
		//lin_y_step = normal(0, 3, 1);
		lin_y_step = normal(0, TMP_STEP_SIZE * 0.5, 1);
		//lin_y_step = uniform(-2, 2, 1);
	move_linear_noise();
}

void WiseMovingTarget::move_random_walk() {
	double x, y, rnd, min, max;

	rnd = uniform(0, 1, 0);

	x = x_curr;
	y = y_curr;
	if (rnd < 0.25)
		x = x_curr + lin_x_step;
	else if (rnd < 0.5)
		x = x_curr - lin_x_step;
	else if (rnd < 0.75)
		y = y_curr + lin_y_step;
	else
		y = y_curr - lin_y_step;

	min = (move_min_x != 0) ? move_min_x : 0;
	max = (move_max_x != 0) ? move_max_x : terrain->get_x_size();
	x_curr = (x > max) ? max : (x < min) ? min : x;

	min = (move_min_y != 0) ? move_min_y : 0;
	max = (move_max_y != 0) ? move_max_y : terrain->get_y_size();
	y_curr = (y > max) ? max : (y < min) ? min : y;
}

void WiseMovingTarget::move_random_walk_8() {
	double x, y, rnd, min, max;

	rnd = uniform(0, 1, 0);

	x = x_curr;
	y = y_curr;
	if (rnd < 0.125) {
		x = x_curr + lin_x_step;
	} else if (rnd < 0.250) {
		x = x_curr - lin_x_step;
	} else if (rnd < 0.375) {
		y = y_curr + lin_y_step;
	} else if (rnd < 0.5) {
		y = y_curr - lin_y_step;
	} else if (rnd < 0.625) {
		x = x_curr + diagonal;
		y = y_curr + diagonal;
	} else if (rnd < 0.750) {
		x = x_curr + diagonal;
		y = y_curr - diagonal;
	} else if (rnd < 0.875) {
		x = x_curr - diagonal;
		y = y_curr - diagonal;
	} else {
		x = x_curr - diagonal;
		y = y_curr + diagonal;
	}

	min = (move_min_x != 0) ? move_min_x : 0;
	max = (move_max_x != 0) ? move_max_x : terrain->get_x_size();
	x_curr = (x > max) ? max : (x < min) ? min : x;

	min = (move_min_y != 0) ? move_min_y : 0;
	max = (move_max_y != 0) ? move_max_y : terrain->get_y_size();
	y_curr = (y > max) ? max : (y < min) ? min : y;
}


double WiseMovingTarget::calculateSensedValue(const double &x_coo, const double &y_coo, const simtime_t & stime) {
	int i;
	double sensedValue, linear_coeff, distance;

	if (stime - time >= SIMTIME_STEP) {
		time = stime;
		// Update the source_index info [that is the current snapshot for each source]
		for (i = 0; i < numSources; i++) {
			if (source_index[i] >= -1) {
				if (time >= sources_snapshots[i][source_index[i] + 1].time)
					source_index[i]++;

				// check if the end is reached
				if (sources_snapshots[i][source_index[i] + 1].time == -1)
					source_index[i] = -2;
			}
		}

		// Update the current state of all sources ==> (x -- y -- value)  
		// with respect to time and the current snapshot od each source
		for (i = 0; i < numSources; i++) {
			if (source_index[i] >= 0) {
				linear_coeff =
				    (time - sources_snapshots[i][source_index[i]].time) /
				    (sources_snapshots[i][source_index[i] + 1].time -
				     sources_snapshots[i][source_index[i]].time);

				curr_source_state[i].x =
				    sources_snapshots[i][source_index[i]].x + linear_coeff *
				    (sources_snapshots[i][source_index[i] + 1].x -
				     sources_snapshots[i][source_index[i]].x);

				curr_source_state[i].y =
				    sources_snapshots[i][source_index[i]].y + linear_coeff *
				    (sources_snapshots[i][source_index[i] + 1].y -
				     sources_snapshots[i][source_index[i]].y);

				curr_source_state[i].value =
				    sources_snapshots[i][source_index[i]].value + linear_coeff *
				    (sources_snapshots[i][source_index[i] + 1].value -
				     sources_snapshots[i][source_index[i]].value);
			}
		}
	}

	/* Now that we know the current state of your process calculate its effect on all the nodes */
	// add all active sources
	sensedValue = par("initialSensedValue");
	for (i = 0; i < numSources; i++) {
		if (source_index[i] >= -2) {
			distance = sqrt((x_coo - curr_source_state[i].x) * (x_coo - curr_source_state[i].x) +
				 	(y_coo - curr_source_state[i].y) * (y_coo - curr_source_state[i].y));
			sensedValue += pow(k * distance + 1, -a) * curr_source_state[i].value;
		}
	}

	return sensedValue;
}

