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
//*  	Module: sender trace creation to Evalvid				*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/ 

#include <map>
#include <algorithm>

#include "VideoSensor.h"
#include "Segment.h"
#include "WiseBaseApplication.h"
#include "WiseMovingTargetMessage_m.h"

Define_Module(VideoSensor);

VideoSensor::VideoSensor() {
    // TODO Auto-generated constructor stub
}

VideoSensor::~VideoSensor() {
    // TODO Auto-generated destructor stub
}

/**
 * Compute sets of nodes that cover this sensors FOV.
 */
void VideoSensor::computeCoverSets() {
    std::vector<Node> neighbors = app->getNeighbors();
    std::set<unsigned int> A, B, C, G, GP, GV, GB, GC, AG, BG, CG;

    // Build set A, B, C, G to compute cover-sets
    std::vector<Node>::const_iterator it = neighbors.begin();
    for ( ; it != neighbors.end(); ++it ) {
        if ( it->fov.contains(p) ) {
            A.insert(it->addr);
        }

        if ( it->fov.contains(b) ) {
            B.insert(it->addr);
        }

        if ( it->fov.contains(c) ) {
            C.insert(it->addr);
        }

        if ( it->fov.contains(g) ) {
            G.insert(it->addr);
        }
    }

    AG = computeIntersection(A, G);
    BG = computeIntersection(B, G);
    CG = computeIntersection(C, G);

    // Construct cover-sets by computing AG x BG x CG
    std::map<std::string, std::set<unsigned int> > AGxBG;
    std::set<unsigned int>::iterator ag, bg;

    availableCoverSets.clear();

    // Compute AG x BG
    for (ag = AG.begin(); ag != AG.end(); ++ag) {
        for (bg = BG.begin(); bg != BG.end(); ++bg) {
            std::set<unsigned int> pair;
            pair.insert(*ag);
            pair.insert(*bg);
            std::string key = keyFromSet(pair);
            if (AGxBG.count(key) == 0) {
                AGxBG[key] = pair;
            }
        }
    }

    std::map<std::string, std::set<unsigned int> > AGxBGxCG;
    std::map<std::string, std::set<unsigned int> >::iterator agxbg;
    std::set<unsigned int>::iterator cg;

    // Compute (AG x BG) X CG
    for (agxbg = AGxBG.begin(); agxbg != AGxBG.end(); ++agxbg) {
        for (cg = CG.begin(); cg != CG.end(); ++cg) {
            agxbg->second.insert(*cg);
            std::string key = keyFromSet(agxbg->second);
            if (AGxBGxCG.count(key) == 0) {
                AGxBGxCG[key] = agxbg->second;
                availableCoverSets.insert(agxbg->second);
            }
        }
    }

    numCoverSets = availableCoverSets.size();

    // print cover sets
    std::set<std::set<unsigned int> >::iterator it1 = availableCoverSets.begin();
    EV << "Available cover sets:" << std::endl;

    for ( ; it1 != availableCoverSets.end(); ++it1 ) {
        std::set<unsigned int>::iterator it2 = it1->begin();
        for ( ; it2 != it1->end(); ++it2 ) {
            EV << *it2;
        }
        EV << std::endl;
    }
}

/**
 *
 */
void VideoSensor::initialize() {
    CritSensor::initialize();
    isSink = par("isSink");
    fileIn = par("video");
    max = par("max_fragmented_size");
    draw_node();
    setupMsg = new cMessage("setup", 10);

    contSd = 0;
    nLin = 0;
    nLT = 0;
}


/**
 *
 */
void VideoSensor::finish() {
    CritSensor::finish();
    cancelAndDelete(setupMsg);
}

/**
 *
 */
std::set<unsigned int> VideoSensor::computeIntersection(const std::set<unsigned int>& A, const std::set<unsigned int>& B) const {
    std::vector<int> vectorA(A.begin(), A.end());
    std::vector<int> vectorG(B.begin(), B.end());
    std::vector<int> intersection;

    std::set_intersection(vectorA.begin(), vectorA.end(), vectorG.begin(), vectorG.end(),
            std::back_inserter(intersection));

    return std::set<unsigned int>(intersection.begin(), intersection.end());
}

/**
 * Handle sample from physical process
 *
 * TODO extend/modify for WiseVideFile
 */
void VideoSensor::handleSample(cMessage* msg) {
    int msgKind = msg->getKind();
    switch (msgKind) {
	case PHYSICAL_PROCESS_SCALAR_DATA:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    if (reqMsg->getSource() == self){
		intruder.x = reqMsg->getX();
		intruder.y = reqMsg->getY();
	    	increaseCriticality();
		if (reqMsg->getAngle() == 1)
		    change_angle();
	    } else
		dinamicIncreaseOfCriticality();
	    break;
	}

	/*
	 * received a msg from App layer to start send multimedia content
	*/
	case SENSOR_READING_MESSAGE:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    dataTracer.nodeId = reqMsg->getIdNode();
	    dataTracer.seqNum = reqMsg->getIdVideo();
	    trace() << "Node " << dataTracer.nodeId << " should start send multimedia with seq number " << dataTracer.seqNum;
	    scheduleAt(simTime() + 0, setupMsg);
	    break;
	}
	case MULTIMEDIA_REQUEST_MESSAGE:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    dataTracer.nodeId = reqMsg->getIdNode();
	    dataTracer.seqNum = reqMsg->getIdVideo();
	    trace() << "Node " << dataTracer.nodeId << " should start send multimedia with seq number " << dataTracer.seqNum;
	    scheduleAt(simTime() + 0, setupMsg);
	    break;
	}
    }
    delete msg;
}

void VideoSensor::handleSensorMessage(cMessage* msg) {
	if (msg->getKind() == 10){
		trace() << "SETUP_TRACE timmer";
		setup();
		getNextFrame(ndx, trec_);
		id = 0;
		cancelEvent(processTrace);
		scheduleAt(simTime() + 0, processTrace);
	} else if (msg->getKind() == 11){//PROCESS_TRACE timmer
		trace() << "node " << dataTracer.nodeId << " sending multimedia pkts with seq number " << dataTracer.idFrame +1;
		double energy = energyPerSample + uniform(0, 1) * (energyPerSample / 100);
		powerDrawn(energy);
		processStreamData();
	}
}

void VideoSensor::setup(){
trace() << "setup";
    unsigned long id, size, prio;
    double time;
    char type;
    tracerec* t;
    ndx = 0;
    nrec = 0;
    id = 0;
    a_ = 0;
    contFrm = 0;
    contSd = 0;
    nLT = 0;
    pos_frame = 0;
    uid = 0;


    char frdTrace[55];
    sprintf(frdTrace,"rd_sn_%i_nodeId_%i",dataTracer.seqNum, dataTracer.nodeId);
    rdTrace.open(frdTrace, ios::out);
    rdTrace.close();
    rdTrace.clear();

    char fTrace[55];
    sprintf(fTrace,"sd_sn_%i_nodeId_%i",dataTracer.seqNum, dataTracer.nodeId);
    sdTrace.open(fTrace, ios::out);
    if (sdTrace.bad()) {
    	trace() << "Error while opening " << fTrace;
    	error("Error while opening input file\n");
	return;
    }
    sdTrace.close();
    sdTrace.clear();
    sdTrace.open(fTrace, ios::out);

    ifstream traceFile(fileIn, ios::in);
    if (traceFile.bad()) {
        trace() << "Error while opening " << fileIn;
        error("Error while opening input file\n");
        return;
    }

    while (traceFile >> id >> type >> size >> prio >> time) {
        nrec++;
    }
		
    traceFile.clear();
    traceFile.seekg(0);
    tracer = new struct tracerec[nrec];
    t = tracer;
    last_rand_id = 0;

    while (traceFile >> id >> type >> size >> prio >> time) {
        t->trec_id = id;
	t->trec_type = type;
        t->trec_size = size;
        t->trec_prio = prio;
        t->trec_time = time;
        t++;
    }
}

void VideoSensor::getNextFrame(unsigned int& ndx, struct tracerec& t) {
trace() << "getNextFrame";
    if (uid != 0)
    	uid = dataTracer.idFrame + 1;
    t.trec_time = tracer[ndx].trec_time;
    t.trec_size = tracer[ndx].trec_size;
    t.trec_type = tracer[ndx].trec_type;
    t.trec_prio = tracer[ndx].trec_prio;
    t.trec_id   = tracer[ndx].trec_id;
	
    if (ndx++ == nrec) {
	ndx = 0;
        a_ = 1;
    }
}

void VideoSensor::processStreamData() {
    	fullPkt = trec_.trec_size / max;
    	restPkt = trec_.trec_size % max;
    	if (fullPkt > 0) {
	    for (i = 0; i < fullPkt; i++) {
	    	SendPkts(max);
	  	//trace() << "send pkt - id " << dataTracer.id << " size " << dataTracer.byteLength << "\n";
	    }
    	}
    	if (restPkt != 0) {
    		SendPkts(max);
	    //trace() << "send pkt - id " << dataTracer.id << " size " << dataTracer.byteLength << "\n";
	}
	simtime_t interval = trec_.trec_time;       
	getNextFrame(ndx, trec_);	
	simtime_t time = trec_.trec_time - interval;
	
	if (a_ == 0) {
		scheduleAt(simTime() + time, processTrace);
	} else {	
		cancelEvent(processTrace);
	}
}

void VideoSensor::SendPkts (int nbytes){
    trace() << "Not using fec - Frame type " << trec_.trec_type << " number " << trec_.trec_id << " with size " << max;
    WiseSensorMessage *pkt = new WiseSensorMessage("VIDEO TRACES", MULTIMEDIA);    
    dataTracer.idFrame = id++;
    pkt->setByteLength(nbytes);
    dataTracer.byteLength = pkt->getByteLength();
    dataTracer.frameType = trec_.trec_type;
    pkt->setIdFrame(dataTracer.idFrame);
    pkt->setInfo(dataTracer);
    double time = SIMTIME_DBL(simTime());
    //trace() << "send pkt - id " << dataTracer.id << " size " << dataTracer.byteLength << "\n";
    // creating sender tracer 
    sdTrace << time << setfill(' ') << setw(16)
            << " id " 	<< dataTracer.idFrame 	<< setfill(' ') << setw(16)
	    << " udp " 	<< pkt->getByteLength() << endl;
    send(pkt, "toApplicationModule"); //send the video tracer to the Application module
}

// uso o parametro generate_trace para indicar se o pacote precisa ser adicionado
// ao trace do evalvid. eh necessario pois os pacotes "a mais" gerados pelo FEC
// não devem aparecer no trace que sera utilizado para remontar os frames posteriormente
// se nao for passado o parametro generate_trace assumo que deve ser gerado trace no evalvid
// OBS: por enquanto nao estou me preocupando com o tamanho dos pacotes pois o 
// evalvid nao usa esta informacao para remontar os frames
void VideoSensor::Send (WiseSensorMessage *pkt, bool generateTrace) {
	if (generateTrace){
		//trace() << "Add packet to Evalvid Tracer";
		dataTracer.idFrame = id++;
		double time = SIMTIME_DBL(simTime());
		// creating sender tracer 
    		sdTrace << time << setfill(' ') << setw(16)
            		<< " id " 	<< dataTracer.idFrame 	<< setfill(' ') << setw(16)
	    		<< " udp " 	<< pkt->getByteLength() << endl;
	} 

	dataTracer.byteLength = pkt->getByteLength();
	dataTracer.frameType = trec_.trec_type;
	//pkt->setIdFrame(dataTracer.id);
	pkt->setIdFrame(uid);
	pkt->setInfo(dataTracer);
	uid ++;
	//trace() << "send pkt - id " << dataTracer.id << " size " << dataTracer.byteLength;
	send(pkt, "toApplicationModule"); //send the video tracer to the Application module
}

void VideoSensor::change_angle(){
    int intersections = 0;
    lineOfSight = 0;

    Segment intruderNode[8] = {
      Segment(intruder.x + fov_radius, intruder.y, intruder.x + fov_radius*0.75, intruder.y + fov_radius*0.75),
      Segment(intruder.x + fov_radius*0.75, intruder.y + fov_radius*0.75, intruder.x, intruder.y + fov_radius),
      Segment(intruder.x, intruder.y + fov_radius, intruder.x - fov_radius*0.75, intruder.y + fov_radius*0.75),
      Segment(intruder.x - fov_radius*0.75, intruder.y + fov_radius*0.75, intruder.x - fov_radius, intruder.y),
      Segment(intruder.x - fov_radius, intruder.y, intruder.x - fov_radius*0.75, intruder.y - fov_radius*0.75),
      Segment(intruder.x - fov_radius*0.75, intruder.y - fov_radius*0.75, intruder.x, intruder.y- fov_radius),
      Segment(intruder.x, intruder.y- fov_radius, intruder.x + fov_radius*0.75, intruder.y - fov_radius*0.75),
      Segment(intruder.x + fov_radius*0.75, intruder.y - fov_radius*0.75, intruder.x + fov_radius, intruder.y)
    };

    // Find intersections between the sensor area and the line of sight of the camera
    for (int i=0; i<32; i++) {
    	lineOfSight = lineOfSight + 0.25;
	temp_v.x = dov * cos(lineOfSight) + p.x;
	temp_v.y = dov * sin(lineOfSight) + p.y;

	Segment line[1] = {
          Segment(p.x, p.y, temp_v.x, temp_v.y)
	};

    	for ( int i = 0; i < 1; i++ ) {
            for ( int l = 0; l < 8; l++ ) {
		if ( line[i].intersects(intruderNode[l]) )
                    intersections++;
            }
        }

	if (intersections > 2)
	    break;
    }

    if (intersections > 2) {
        //remove the moving object and the sensor node fov
	terrain->clean_target();
	terrain->remove_camera(caminfo);

    	// find point v, b, c
	v.x = temp_v.x;
	v.y = temp_v.y;

    	double h = dov * sin(aov);
    	double d = dov * cos(aov);

    	b.x = v.x + h * (v.y - p.y) / d;
    	b.y = v.y - h * (v.x - p.x) / d;

    	c.x = v.x - h * (v.y - p.y) / d;
    	c.y = v.y + h * (v.x - p.x) / d;

    	// point g (center of mass)
    	g.x = (p.x + b.x + c.x) / 3;
    	g.y = (p.y + b.y + c.y) / 3;

    	// fill in the Triangle structure for geometric manipulation
    	myFoV = Triangle(p, b, c);

    	// draw node and camera FOV in opencv GUI
    	WiseCameraInfo::fov_triangle_t fov;
    	fov.a_x = myFoV.v1.x;
    	fov.a_y = myFoV.v1.y;
    	fov.b_x = myFoV.v2.x;
    	fov.b_y = myFoV.v2.y;
    	fov.c_x = myFoV.v3.x;
    	fov.c_y = myFoV.v3.y;

    	trace() << "Node " << self << " has to change the angle" << "\n";
    	//trace() << "Sensor - Node " << self << " received a vibration report  (" << intruder.x << "," << intruder.y << ")";

    	if(!isSink)
    	    caminfo.set_triangle_fov(fov);
    	caminfo.set_position(p.x, p.y, 0);
    	terrain->place_camera(caminfo);
    }
}

void VideoSensor::draw_node(){
    NodeLocation location = mobilityModule->getLocation();
    p.x = location.x;
    p.y = location.y;

    //alpha = PI / 10; 
    aov = par("aov"); // angle of view
    dov = par("dov"); // depth of view

    // find coordinate of point v of vector pv: set a direction for the camera
    // node that can rotate their camera will update their lineOfSight during the simulation
    lineOfSight = par("lineOfSight").longValue() == -1 ? uniform(0, 1) * 2 * PI : par("lineOfSight");

    // find point v, b, c for each sensor node
    v.x = dov * cos(lineOfSight) + p.x;
    v.y = dov * sin(lineOfSight) + p.y;

    double h = dov * sin(aov);
    double d = dov * cos(aov);

    b.x = v.x + h * (v.y - p.y) / d;
    b.y = v.y - h * (v.x - p.x) / d;

    c.x = v.x - h * (v.y - p.y) / d;
    c.y = v.y + h * (v.x - p.x) / d;

    // point g (center of mass)
    g.x = (p.x + b.x + c.x) / 3;
    g.y = (p.y + b.y + c.y) / 3;

    // fill in the Triangle structure for geometric manipulation
    myFoV = Triangle(p, b, c);

    // draw node and camera FOV in opencv GUI
    WiseCameraInfo::fov_triangle_t fov;
    fov.a_x = myFoV.v1.x;
    fov.a_y = myFoV.v1.y;
    fov.b_x = myFoV.v2.x;
    fov.b_y = myFoV.v2.y;
    fov.c_x = myFoV.v3.x;
    fov.c_y = myFoV.v3.y;

    if(!isSink){
    	caminfo.set_triangle_fov(fov);
    	caminfo.set_position(location.x, location.y, 0);
    	terrain->place_camera(caminfo);
    } else {
    	caminfo.set_position(location.x, location.y, 0);
    	terrain->place_BS(caminfo);
    }
}

/**
 * Output set into a std::string.
 *
 * Since std::sets are ordered, the returned string might be used for testing
 * set equality without iterating over all elements.
 */
std::string VideoSensor::keyFromSet(const std::set<unsigned int>& set) const {
    std::stringstream ss;
    for (std::set<unsigned int>::const_iterator it = set.begin(); it != set.end(); ++it) {
        ss << *it;
    }
    return ss.str();
}

/**
 *
 */
bool VideoSensor::covers(double x, double y) {
    return myFoV.contains(Point(x, y));
}



