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
//*  	Module: Sensor application to camera node with node mobility		*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/ 

#include <map>
#include <algorithm>

#include "MobileDirectionalSensor.h"
#include "Segment.h"
#include "WiseBaseApplication.h"
#include "WiseMovingTargetMessage_m.h"

Define_Module(MobileDirectionalSensor);

MobileDirectionalSensor::MobileDirectionalSensor() {
    // TODO Auto-generated constructor stub
}

MobileDirectionalSensor::~MobileDirectionalSensor() {
    // TODO Auto-generated destructor stub
}

/**
 * Compute sets of nodes that cover this sensors FOV.
 */
void MobileDirectionalSensor::computeCoverSets() {
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
void MobileDirectionalSensor::initialize() {
    CritSensor::initialize();
    isSink = par("isSink");
    fileIn = par("video");
    max = par("max_fragmented_size");
    changeAngle = false;
    draw_node();
    setupMsg = new cMessage("setup", 10);

    contSd = 0;
    nLin = 0;
    nLT = 0;
}


/**
 *
 */
void MobileDirectionalSensor::finish() {
    CritSensor::finish();
    cancelAndDelete(setupMsg);
}

/**
 *
 */
std::set<unsigned int> MobileDirectionalSensor::computeIntersection(const std::set<unsigned int>& A, const std::set<unsigned int>& B) const {
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
void MobileDirectionalSensor::handleSample(cMessage* msg) {

    int msgKind = msg->getKind();

    switch (msgKind) {
	case PHYSICAL_PROCESS_SCALAR_DATA:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    if (reqMsg->getSource() == self){
		intruder.x = reqMsg->getX();
		intruder.y = reqMsg->getY();
	    	increaseCriticality();
		change_angle();
	    } else
		dinamicIncreaseOfCriticality();
	    break;
	}

	case MOBILE_MESSAGE:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    if (reqMsg->getSource() == self){
		newLocation.x = reqMsg->getX();
		newLocation.y = reqMsg->getY();
	    	changeAngle = true;
		update_location();
	    }
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
    }
    delete msg;
}

void MobileDirectionalSensor::update_location(){
	if (changeAngle){
    		int intersections = 0;
	    	lineOfSight = 0;
	    	Segment direction[2] = {
		 Segment(newLocation.x, newLocation.y, p.x, p.y),
	         Segment(p.x, p.y, newLocation.x, newLocation.y)
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
					if ( line[i].intersects(direction[l]) )
		                   		intersections++;
		            	}
		        }
		
			if (intersections > 30){
				changeAngle = false;
				break;
			}
		}
	}

        //remove the moving object and the sensor node fov
	terrain->clean_mobile();

    	NodeLocation location = mobilityModule->getLocation();
    	p.x = location.x;
    	p.y = location.y;

    	// find point v, b, c
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

    	//trace() << "Node " << self << " has to change the angle" << "\n";
    	//trace() << "Sensor - Node " << self << " change location to  (" << newLocation.x << "," << newLocation.y << ")";

    	if(!isSink){
    		caminfo.set_triangle_fov(fov);
	    	caminfo.set_position(location.x, location.y, 0);
    		terrain->place_mobile(caminfo);
	} else {
    		caminfo.set_position(location.x, location.y, 0);
	    	terrain->place_BS(caminfo);
    	}
    	trace() << "updated the node location\n";
}

void MobileDirectionalSensor::change_angle(){
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

void MobileDirectionalSensor::draw_node(){
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

    // additional points for computation of coversets, not needed now
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
std::string MobileDirectionalSensor::keyFromSet(const std::set<unsigned int>& set) const {
    std::stringstream ss;
    for (std::set<unsigned int>::const_iterator it = set.begin(); it != set.end(); ++it) {
        ss << *it;
    }
    return ss.str();
}

/**
 *
 */
bool MobileDirectionalSensor::covers(double x, double y) {
    return myFoV.contains(Point(x, y));
}

void MobileDirectionalSensor::handleSensorMessage(cMessage* msg) {

}
