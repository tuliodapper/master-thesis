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
//*  	Module: Sensor application to detect intruder				*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/ 

#include <map>
#include <algorithm>
#include "IntruderSensor.h"
#include "Segment.h"
#include "WiseBaseApplication.h"
#include "WiseMovingTargetMessage_m.h"
#include "WisePhysicalProcessMessage_m.h"

Define_Module(IntruderSensor);

IntruderSensor::IntruderSensor() {
    // TODO Auto-generated constructor stub
}

IntruderSensor::~IntruderSensor() {
    // TODO Auto-generated destructor stub
}

/**
 * Compute sets of nodes that cover this sensors FOV.
 */
void IntruderSensor::computeCoverSets() {

}

/**
 *
 */
void IntruderSensor::initialize() {
    CritSensor::initialize();
    draw_node();
}


/**
 *
 */
void IntruderSensor::finish() {
    CritSensor::finish();
}

/**
 *
 */
std::set<unsigned int> IntruderSensor::computeIntersection(const std::set<unsigned int>& A, const std::set<unsigned int>& B) const {
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
 */
void IntruderSensor::handleSample(cMessage* msg) {
    int msgKind = msg->getKind();
    switch (msgKind) {
	/**
	 * Check if target is inside our FOV.
	 * tl = top left, br = bottom right
    	*/
	case PHYSICAL_PROCESS_SAMPLING: {
		trace() << "PHYSICAL_PROCESS_SAMPLING";
		WiseMovingTargetMessage* m = check_and_cast<WiseMovingTargetMessage*>(msg);
   		int intersections = 0;

    		WiseTargetBoundingBox bb = m->getBoundingBox();
    		Segment target[4] = {
		        Segment(bb.x_br, bb.y_br, bb.x_tl, bb.y_br),
		        Segment(bb.x_tl, bb.y_br, bb.x_tl, bb.y_tl),
		        Segment(bb.x_tl, bb.y_tl, bb.x_br, bb.y_tl),
		        Segment(bb.x_br, bb.y_tl, bb.x_br, bb.y_br)
		};

		Segment sensorNode[8] = {
    		    Segment(p.x + fov_radius, p.y, p.x + fov_radius*0.75, p.y + fov_radius*0.75),
    		    Segment(p.x + fov_radius*0.75, p.y + fov_radius*0.75, p.x, p.y + fov_radius),
    		    Segment(p.x, p.y + fov_radius, p.x - fov_radius*0.75, p.y + fov_radius*0.75),
    		    Segment(p.x - fov_radius*0.75, p.y + fov_radius*0.75, p.x - fov_radius, p.y),
    		    Segment(p.x - fov_radius, p.y, p.x - fov_radius*0.75, p.y - fov_radius*0.75),
    		    Segment(p.x - fov_radius*0.75, p.y - fov_radius*0.75, p.x, p.y- fov_radius),
    		    Segment(p.x, p.y- fov_radius, p.x + fov_radius*0.75, p.y - fov_radius*0.75),
    		    Segment(p.x + fov_radius*0.75, p.y - fov_radius*0.75, p.x + fov_radius, p.y)
    		};

    		// Find intersections between the sensor area and target line segments.
    		for ( int i = 0; i < 4; i++ ) {
   		     for ( int l = 0; l < 8; l++ ) {
    		        if ( target[i].intersects(sensorNode[l]) )
    		            intersections++;
    		    }
    		}

		if ( intersections > 0 && m->getValue() > threshold) {
		    trace() << "Object detected with temperature  "<< m->getValue();
		    WiseSensorMessage *reportMsg = new WiseSensorMessage("sensor reading msg", SENSOR_READING);
		    reportMsg->setSensorType(sensorTypes[0].c_str());
		    reportMsg->setSensedValue(m->getValue());
		    reportMsg->setSensorIndex(0);
		    reportMsg->setX(p.x);
		    reportMsg->setY(p.y);
		    send(reportMsg, "toApplicationModule");	//send the sensor reading to the Application module
		}
		trace() << "Object (" << bb.x_br <<", " <<  bb.y_br;
		trace() << "Node (" << p.x << ", " << p.y;
		trace() << "Intersection " << intersections;
		trace() << "Temp " << m->getValue();
		trace() << "threshold " << threshold << "\n";
		break;
	}
    }
    delete msg;
}


/**
 *
 */
void IntruderSensor::draw_node() {
    NodeLocation location = mobilityModule->getLocation();
    p.x = location.x;
    p.y = location.y;
    caminfo.set_position(location.x, location.y, 0);
    caminfo.set_fov_bb(fov_radius, fov_radius, terrain->get_x_size(), terrain->get_y_size());  
    terrain->place_camera(caminfo);
}


/**
 * Output set into a std::string.
 *
 * Since std::sets are ordered, the returned string might be used for testing
 * set equality without iterating over all elements.
 */
std::string IntruderSensor::keyFromSet(const std::set<unsigned int>& set) const {
    std::stringstream ss;
    for (std::set<unsigned int>::const_iterator it = set.begin(); it != set.end(); ++it) {
        ss << *it;
    }
    return ss.str();
}

/**
 *
 */
bool IntruderSensor::covers(double x, double y) {
    return myFoV.contains(Point(x, y));
}

void IntruderSensor::handleSensorMessage(cMessage* msg) {

}
