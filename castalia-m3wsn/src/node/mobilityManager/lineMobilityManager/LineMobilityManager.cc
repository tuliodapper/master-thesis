/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Yuriy Tselishchev                                            *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *  
 ****************************************************************************/

#include "LineMobilityManager.h"

Define_Module(LineMobilityManager);

void LineMobilityManager::initialize(){
	VirtualMobilityManager::initialize();

    updateInterval = par("updateInterval");
    updateInterval = updateInterval / 1000;

    speed = par("speed");

	isMobile = par("isMobile");

	setTarget(par("xCoorDestination"), par("yCoorDestination"), 0);

    scheduleAt(simTime() + updateInterval,
        new MobilityManagerMessage("Periodic location update message", MOBILITY_PERIODIC));

}

void LineMobilityManager::setTarget(double loc2_x_, double loc2_y_, double loc2_z_){
    loc1_x = nodeLocation.x;
    loc1_y = nodeLocation.y;
    loc1_z = nodeLocation.z;
    loc2_x = loc2_x_;
    loc2_y = loc2_y_;
    loc2_z = loc2_z_;
    distance = sqrt(pow(loc1_x - loc2_x, 2) + pow(loc1_y - loc2_y, 2) + pow(loc1_z - loc2_z, 2));
    timeToMove = distance / speed;
    endTime = SIMTIME_DBL(simTime()) + timeToMove;
    trace() << "distance " << distance;
    trace() << "timeToMove " << timeToMove;
    trace() << "endTime " << endTime;
    trace() << "speed " << speed;
    double tmp = (distance / speed) / updateInterval;
    incr_x = (loc2_x - loc1_x) / tmp;
    incr_y = (loc2_y - loc1_y) / tmp;
    incr_z = (loc2_z - loc1_z) / tmp;
    setLocation(loc1_x, loc1_y, loc1_z);
}

void LineMobilityManager::handleMessage(cMessage * msg) {
	int msgKind = msg->getKind();
	switch (msgKind) {
		case MOBILE_MESSAGE:{
			fomApplicationLayer(msg);
			break;
		}
		case MOBILITY_PERIODIC:{
            if (simTime() + updateInterval  < endTime){
                nodeLocation.x += incr_x;
                nodeLocation.y += incr_y;
                nodeLocation.z += incr_z;
                double energy = (distance * energyToFly) + uniform(0, 1) * ((distance * energyToFly) / 100);
                powerDrawn(energy);
            } else {
                nodeLocation.x = loc2_x;
                nodeLocation.y = loc2_y;
                nodeLocation.z = loc2_z;
                trace() << "changed location(x:y:z) to " << nodeLocation.x << ":" << nodeLocation.y << ":" << nodeLocation.z;
                trace() << "destination location(x:y:z) is " << loc2_x << ":" << loc2_y << ":" << loc2_z << "\n";
            }
            notifyWirelessChannel();
			scheduleAt(simTime() + updateInterval,
				new MobilityManagerMessage("Periodic location update message", MOBILITY_PERIODIC));

			trace() << "changed location(x:y:z) to " << nodeLocation.x <<":" << nodeLocation.y << ":" << nodeLocation.z;
			break;
		}
		case MOVEMENT:{
			nodeLocation.x += incr_x;
			nodeLocation.y += incr_y;
			nodeLocation.z += incr_z;
			double energy = (distance * energyToFly) + uniform(0, 1) * ((distance * energyToFly) / 100);
			powerDrawn(energy);
			trace() << "initial location(x:y:z) to " << loc1_x << ":" << loc1_y << ":" << loc1_z;
			trace() << "changed location(x:y:z) to " << nodeLocation.x << ":" << nodeLocation.y << ":" << nodeLocation.z;
			trace() << "destination location(x:y:z) is " << loc2_x << ":" << loc2_y << ":" << loc2_z;
			trace() << "updateInterval " << updateInterval;
			trace() << "speed " << speed;
			trace() << "time to move " << timeToMove;
			trace() << "end time " << endTime;
			trace() << "Energy consumed to move (" << distance << " m)" << energy << "\n";
			if (simTime() + updateInterval  < endTime){
				scheduleAt(simTime() + updateInterval, new MobilityManagerMessage("Periodic location update message", MOVEMENT));
			} else {
				nodeLocation.x = loc2_x;
				nodeLocation.y = loc2_y;
				nodeLocation.z = loc2_z;
				trace() << "changed location(x:y:z) to " << nodeLocation.x << ":" << nodeLocation.y << ":" << nodeLocation.z;
				trace() << "destination location(x:y:z) is " << loc2_x << ":" << loc2_y << ":" << loc2_z << "\n";
			}

			notifyWirelessChannel();
			break;
		}
		
		default:{
			trace() << "WARNING: Unexpected message " << msgKind;
		}
	}

	delete msg;
	msg = NULL;
}

void LineMobilityManager::fomApplicationLayer(cMessage * msg){
	MobilityManagerMessage *rcvPkt = check_and_cast <MobilityManagerMessage*>(msg);
	trace() << "Node received a message from application layer";
	switch (msg->getKind()) {
			case MOBILE_MESSAGE:{
			    setTarget(rcvPkt->getXCoorDestination(), rcvPkt->getYCoorDestination(), 0);
				break;
			}
	}
}

double LineMobilityManager::getWaypointX(){
	return loc2_x;
}

double LineMobilityManager::getWaypointY(){
	return loc2_y;
}

double LineMobilityManager::getWaypointZ(){
	return loc2_z;
}

double LineMobilityManager::getEndTime(){
	return endTime;
}

double LineMobilityManager::getSpeed(){
    return speed;
}
