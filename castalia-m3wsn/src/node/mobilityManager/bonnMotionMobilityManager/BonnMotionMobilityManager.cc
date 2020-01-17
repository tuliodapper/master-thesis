//*******************************************************************************
//*	Copyright (c) 2013. Federal University of Para (UFPA), Brazil and 	*
//*			    University of Bern (UBern), Switzerland		*
//*	Developed by Research Group on Computer Network and Multimedia		*
//*	Communication (GERCOM) of UFPA in collaboration to Communication and 	*
//*	Distributed Systems (CDS) research group of UBern.			*
//*	All rights reserved							*
//*										*
//*	Permission to use, copy, modify, and distribute this protocol and its	*
//*	documentation for any purpose, without fee, and without written		*
//*	agreement is hereby granted, provided that the above copyright notice,	*
//*	and the author appear in all copies of this protocol.			*
//*										*
//*  	Module: Bonnmotion: a mobility scenario generation and analysis tool	*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Claudio Silva <claudio@ufpa.br>				*
//*		 Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/

#include "BonnMotionMobilityManager.h"

Define_Module(BonnMotionMobilityManager);

void BonnMotionMobilityManager::initialize(){
	VirtualMobilityManager::initialize();
	mobilityFile = par("mobilityFile");
	isMobile = par("isMobile");
	node = getParentModule();
	index = node->getIndex();
	network = node->getParentModule();
	wchannel = network->getSubmodule("wirelessChannel");
	updateInterval = par("updateInterval");
	energyToFly = par("energyToFly");
	traceMobility = new cMessage("process trace", MOVE);
	vecSpeed = par("vecSpeed");

    	if (!network)
		opp_error("Unable to obtain SN pointer for deployment parameter");
	if (!wchannel)
		opp_error("Unable to obtain wchannel pointer");

	if (isMobile) {
		getMovements(index, mobilityFile, nodePosition); /*--- get mobility ---*/
		scheduleAt(updateIntervalTargetPoint, new MobilityManagerMessage("Periodic location update message", TARGET_POSITION));/*--- schedule new positions ---*/
	} else{
		setLocation(node->par("xCoor"), node->par("yCoor"), 0.0);
		nodeLocation.x = node->par("xCoor");
		nodeLocation.y = node->par("yCoor");
		nodeLocation.z = 0.0; //lrss
		notifyWirelessChannel();
	}

	if (isMobile)
	    	trace() << "initial location(x:y:z) is " << nodeLocation.x << ":" << nodeLocation.y << ":" << nodeLocation.z << "\n";
	else
	    	trace() << "initial location(x:y:z) is " << nodeLocation.x << ":" << nodeLocation.y << ":" << nodeLocation.z;
}

void BonnMotionMobilityManager::finish() {
	cOwnedObject *Del=NULL;
	int OwnedSize = this->defaultListSize();
	for(int i=0; i<OwnedSize; i++){
		Del = this->defaultListGet(0);
		this->drop(Del);
		delete Del;
	}
}

void BonnMotionMobilityManager::handleMessage(cMessage * msg){
	int msgKind = msg->getKind();
	switch (msgKind) {
		case MOBILE_MESSAGE:{
			fomApplicationLayer(msg);
			break;
		}
		case TARGET_POSITION:{
			current_x = nodeLocation.x;
			current_y = nodeLocation.y;
			current_z = 0.0; //nodeLocation.z;
			dest_x = it->posX;
			dest_y = it->posY;
			dest_z =  0.0; //it->posZ;
			//cout << "Dest_x: " << dest_x << "\n";
			nodeDestination.x = dest_x;
			nodeDestination.y = dest_y;
			nodeDestination.z = dest_z; //L
			nodeDestination.phi = 0;
			nodeDestination.theta = 0;
			nodeDestination.cell = 0;
			distance = sqrt(pow(current_x - dest_x, 2) + pow(current_y - dest_y, 2) + pow(current_z - dest_z, 2));
			startTime = it->startTime;
			endTime = it->endTime;
			difTime = endTime - startTime;
			speed = distance / difTime;
			vx = (sqrt(pow(current_x - dest_x,2)))/ difTime; //L
			//cout<< "DifTime: " << difTime << "\n";
			vy = (sqrt(pow(current_y - dest_y,2)))/ difTime; //L
			vz = 0.0; //(sqrt(pow(current_z - dest_z,2)))/ difTime; //L

			trace() << "-----------------------------------------------";
			trace() << "current location(x:y:z) is " << current_x << ":" << current_y << ":" << current_z;
			trace() << "destination location(x:y:z) is " << dest_x << ":" << dest_y << ":" << dest_z;
			trace() << "distance to move " << distance;
			trace() << "startTime " << startTime;
			trace() << "endTime " << endTime;
			trace() << "difTime " << difTime;
			trace() << "speed " << speed << " m/s" << "\n";
			trace() << "velocity on x axis: " << vx << "\n";
			trace() << "velocity on y axis: " << vy << "\n";
			trace() << "velocity on z axis: " << vz << "\n";

			if (speed > 0 && distance > 0 && vecSpeed==false) {
				double tmp = (distance / speed) / updateInterval;
				incr_x = (dest_x - current_x) / tmp;
				incr_y = (dest_y - current_y) / tmp;
				incr_z = (dest_z - current_z) / tmp;
		      //  cout << "Incr X: " << incr_x << "\n";
		       // cout << "Incr Y: " << incr_y << "\n";
		       // cout << "Incr Z: " << incr_z << "\n";
				cancelEvent(traceMobility);
				scheduleAt(simTime() + updateInterval, traceMobility);
			} else if(speed > 0 && distance > 0 && vecSpeed == true) { //L
			    double tmpX = (distance / vx) / updateInterval;
			    double tmpY = (distance / vy) / updateInterval;
			    double tmpZ = (distance / vz) / updateInterval;

			    incr_x = (dest_x - current_x) / tmpX;
			    incr_y = (dest_y - current_y) / tmpY;
			    incr_z = (dest_z - current_z) / tmpZ;
			    /*cout << "Incr X: " << incr_x << "\n";
			    cout << "Incr Y: " << incr_y << "\n";
			    cout << "Incr Z: " << incr_z << "\n";*/
			    //setVelocity(vx, vy, vz);
			    cancelEvent(traceMobility);
			    scheduleAt(simTime() + updateInterval, traceMobility);


			}

			MobilityManagerMessage *mobileMSG = new MobilityManagerMessage("routing msg", TOPOLOGY_CONTROL);
			mobileMSG->setXCoorDestination(dest_x);
			mobileMSG->setYCoorDestination(dest_y);
			mobileMSG->setZCoorDestination(dest_z);
			toApplicationLayer(mobileMSG);

 			++it;
 			// condition is used in case of finish struct or it check if it haven't mobility
 			if( it != nodePosition.end()) {
                		updateIntervalTargetPoint = it->startTime;
               			scheduleAt(updateIntervalTargetPoint, new MobilityManagerMessage("Periodic location update message", TARGET_POSITION));
 			}
			break;
		}
		case MOVE:{
			temp_x = nodeLocation.x;
			temp_y = nodeLocation.y;
			temp_z = nodeLocation.z;
			nodeLocation.x += incr_x;
			nodeLocation.y += incr_y;
			nodeLocation.z += incr_z;
			double dist = sqrt(pow(nodeLocation.x - temp_x, 2) + pow(nodeLocation.y - temp_y, 2) + pow(nodeLocation.z - temp_z, 2));
			double energy = (dist * energyToFly) + uniform(0, 1) * ((dist * energyToFly) / 100);
			powerDrawn(energy);
			trace() << "current location(x:y:z) to " << temp_x << ":" << temp_y << ":" << temp_z;
			trace() << "changed location(x:y:z) to " << nodeLocation.x << ":" << nodeLocation.y << ":" << nodeLocation.z;
			trace() << "destination location(x:y:z) is " << dest_x << ":" << dest_y << ":" << dest_z;
			trace() << "updateInterval " << updateInterval;
			trace() << "Energy consumed to move (" << dist << " m)" << energy << "\n";
			if (simTime() + updateInterval  < endTime){
				cancelEvent(traceMobility);
				scheduleAt(simTime() + updateInterval, traceMobility);
			} else {
				nodeLocation.x = dest_x;
				nodeLocation.y = dest_y;
				nodeLocation.z = dest_z;
				trace() << "changed location(x:y:z) to " << nodeLocation.x << ":" << nodeLocation.y << ":" << nodeLocation.z;
				trace() << "destination location(x:y:z) is " << dest_x << ":" << dest_y << ":" << dest_z << "\n";
			}

			notifyWirelessChannel();
			break;
		}
		default:{
			trace() << "WARNING: Unexpected message " << msgKind;
		}
	}

	//delete msg;
	//msg = NULL;
}

void BonnMotionMobilityManager::fomApplicationLayer(cMessage * msg){
	MobilityManagerMessage *rcvPkt = check_and_cast <MobilityManagerMessage*>(msg);
	trace() << "Node received a message from applicationb layer";


//	Commented due to unkown error
//	switch (rcvPkt->getMobilePacketKind()) {
//			case MOBILE_MESSAGE:{
//				trace() << "to develop";
//				break;
//			}
//	}
}

void BonnMotionMobilityManager::getMovements( const int index, const char *mobilityFile, list<BonnMotionMovements> &nodePosition){
	/*--- Open File ---*/
	char _line[301]; 							// read lines of file
	ifstream fin(mobilityFile, ios::in);		// read file xml

	if(!fin){
		opp_error( "Problems in file. It didn't read. See the name or local of file.");
	}

	fin.getline(_line,300);						// head file not necessary
	fin.getline(_line,300);						// head file not necessary
	/*--- Startup words ---*/
	char nodeSetings[16] = "<node_settings>";
	char mobility[11] = "<mobility>";
	char nodeIdent[25];
	stringstream ss;
	string nodeId = "<node_id>";
	string nodeId2 = "</node_id>";

	ss << nodeId << index << nodeId2;
	ss >> nodeIdent; 							// result: <node_id>##</node_id>

	string positions;
	while (!fin.eof()){
		fin.getline(_line,300);
		fin >> skipws >> _line;

		if( strcmp(_line, nodeSetings) ==0 ) {
			fin.getline(_line,300);
			fin >> skipws >> _line;
			fin.getline(_line,300);
			fin >> skipws >> _line;
		}
		if( strcmp(_line, nodeIdent) == 0 ){
			fin.getline(_line,300);
			fin >> skipws >> _line;
			fin.getline(_line,300);
			fin >> skipws >> _line;
			fin.getline(_line,300);
			fin >> skipws >> _line;

			/*--- get positionX ---*/
			positions = _line;
			positions.erase(0,6);
			positions.erase(15,positions.size()-15);

			auxMovements.posX = atof(positions.c_str());
			//trace() << "-" << _line << "--->" << fin.gcount() << "@" << positions << ")(" << auxMovements.posX << "!!" << ss.str() << endl;

			/*--- get positionY ---*/
			fin.getline(_line,300);
			fin >> skipws >> _line;
			positions = _line;
			positions.erase(0,6);
			positions.erase(15,positions.size()-15);

			auxMovements.posY = atof(positions.c_str());
			//trace() << "-" << _line << "--->" << fin.gcount() << "@" << positions << ")(" << auxMovements.posY << "!!" << ss.str() << endl;

			/*--- get time inicial---*/
			auxMovements.startTime = 0.0;
			auxMovements.endTime = 0.0;

			//trace() << "settings " << auxMovements.startTime << " | " << auxMovements.posX << " | " << auxMovements.posY;
			nodePosition.push_back(auxMovements);
		}
		if( strcmp(_line, mobility) == 0 ){
			while(!fin.eof()) {
				fin.getline(_line,300);
				fin >> skipws >> _line;
				if( strcmp(_line, nodeIdent) == 0 ){
					fin.getline(_line,300);
					fin >> skipws >> _line;

					/*--- get startTime ---*/
					positions = _line;
					positions.erase(0,12); 		// erase start_time

					int i=0;
					while(positions[i]!='\0'){
						if(positions[i] == '<')
							break;
						i++;
					}
					positions.erase(i,(positions.size()-i));
					auxMovements.startTime = atof(positions.c_str());

					/*--- get endTime ---*/
					fin.getline(_line,300);
					fin >> skipws >> _line;

					positions = _line;
					positions.erase(0,10); 		// erase endTime

					i=0;
					while(positions[i]!='\0'){
						if(positions[i] == '<')
							break;
						i++;
					}

					positions.erase(i,(positions.size()-i));
					auxMovements.endTime = atof(positions.c_str());

					/*--- get posX ---*/
					fin.getline(_line,300);
					fin >> skipws >> _line;
					fin.getline(_line,300);
					fin >> skipws >> _line;

					positions = _line;
					positions.erase(0,6);
					positions.erase(15,positions.size()-15);
					auxMovements.posX = atof(positions.c_str());

					/*--- get posY ---*/
					fin.getline(_line,300);
					fin >> skipws >> _line;

					positions = _line;
					positions.erase(0,6);
					positions.erase(15,positions.size()-15);
					auxMovements.posY = atof(positions.c_str());
					nodePosition.push_back(auxMovements);
				}
			}
		}
	}
	//trace() << endl;
	//for(it = nodePosition.begin(); it != nodePosition.end(); ++it)
	//	trace() << "settings: " << it->startTime << " | " << it->endTime << " | "<< it->posX << " | " << it->posY;

	it = nodePosition.begin();
	setLocation(it->posX, it->posY, 0.0);
	++it;
	updateIntervalTargetPoint = it->startTime;
}



double BonnMotionMobilityManager::getVX(){
    if(vx<0){
        return 0;
    }

    return vx;
}



double BonnMotionMobilityManager::getVY(){
    if(vy<0){
          return 0;
      }
    return vy;
}


double BonnMotionMobilityManager::getVZ(){
    if(vz<0){
          return 0;
      }
    return vz;
}

double BonnMotionMobilityManager::getdifTime(){
    return difTime;
}

NodeLocation BonnMotionMobilityManager::getDestination(){  //L

    return nodeDestination;
}
