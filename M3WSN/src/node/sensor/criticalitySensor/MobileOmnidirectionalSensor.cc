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
//*  	Module: Sensor application to sensor node with node mobility		*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/ 

#include <map>
#include <algorithm>
#include "MobileOmnidirectionalSensor.h"
#include "Segment.h"
#include "WiseBaseApplication.h"
#include "WiseMovingTargetMessage_m.h"
#include "WisePhysicalProcessMessage_m.h"

Define_Module(MobileOmnidirectionalSensor);

MobileOmnidirectionalSensor::MobileOmnidirectionalSensor() {
    // TODO Auto-generated constructor stub
}

MobileOmnidirectionalSensor::~MobileOmnidirectionalSensor() {
    // TODO Auto-generated destructor stub
}

/**
 * Compute sets of nodes that cover this sensors FOV.
 */
void MobileOmnidirectionalSensor::computeCoverSets() {

}

/**
 *
 */
void MobileOmnidirectionalSensor::initialize() {
    CritSensor::initialize();
    draw_node();
}


/**
 *
 */
void MobileOmnidirectionalSensor::finish() {
    CritSensor::finish();
}

/**
 *
 */
std::set<unsigned int> MobileOmnidirectionalSensor::computeIntersection(const std::set<unsigned int>& A, const std::set<unsigned int>& B) const {
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
void MobileOmnidirectionalSensor::handleSample(cMessage* msg) {
    int msgKind = msg->getKind();
    switch (msgKind) {
	/**
	 * Check if target is inside our FOV.
	 * tl = top left, br = bottom right
    	*/
	case PHYSICAL_PROCESS_SAMPLING: {
		trace() << "PHYSICAL_PROCESS_SAMPLING";
		break;
	}

	case MOBILE_MESSAGE:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    if (reqMsg->getSource() == self){
		newLocation.x = reqMsg->getX();
		newLocation.y = reqMsg->getY();
		update_location();
	    }
	    break;
	}

    }
    delete msg;
}

void MobileOmnidirectionalSensor::update_location(){
    NodeLocation location = mobilityModule->getLocation();
    p.x = location.x;
    p.y = location.y;
    caminfo.set_position(location.x, location.y, 0);
    caminfo.set_fov_bb(fov_radius, fov_radius, terrain->get_x_size(), terrain->get_y_size());  
    terrain->place_mobile(caminfo);
}

/**
 *
 */
void MobileOmnidirectionalSensor::draw_node() {
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
std::string MobileOmnidirectionalSensor::keyFromSet(const std::set<unsigned int>& set) const {
    std::stringstream ss;
    for (std::set<unsigned int>::const_iterator it = set.begin(); it != set.end(); ++it) {
        ss << *it;
    }
    return ss.str();
}

/**
 *
 */
bool MobileOmnidirectionalSensor::covers(double x, double y) {
    return myFoV.contains(Point(x, y));
}

void MobileOmnidirectionalSensor::handleSensorMessage(cMessage* msg) {

}
