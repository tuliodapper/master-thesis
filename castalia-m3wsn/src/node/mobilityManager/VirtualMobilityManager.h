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

#ifndef _VIRTUALBILITYMANAGER_H_
#define _VIRTUALBILITYMANAGER_H_

#include "WirelessChannelMessages_m.h"
#include "ResourceManager.h"
#include "CastaliaModule.h"

using namespace std;

enum movingTrajectories {
	WAY_POINT	= 1,
	STAY_AT		= 2,
	SCAN		= 3,
	HUIT		= 4,
	OVAL		= 5,
};

struct NodeLocation {
	double x;
	double y;
	double z;
	double phi;		// orientation info provided by 2 angles.
	double theta;
	int cell;		// store the cell ID that corresponds to coordinates xyz so we do not have to recompute it
};

class VirtualMobilityManager: public CastaliaModule {
 protected:
	NodeLocation nodeLocation;
	double energyToFly;
	double speed, maxSpeed, old_speed, speed_pre, speed_final, xx, yy, zz;
	double startTime_final, endTime_final, x_final, y_final ;
	int index;
	bool disabled;
	int movingTrajectory;
	
	/**radius of stay at*/
	double rayon;

	cModule *node, *wchannel, *network;
	WirelessChannelNodeMoveMessage *positionUpdateMsg;

	virtual void initialize();
	virtual void notifyWirelessChannel();
	virtual void setLocation(double x, double y, double z, double phi = 0, double theta = 0);
	virtual void setLocation(NodeLocation);
	virtual void handleMessage(cMessage * msg);
	void toApplicationLayer(cMessage *);
	void fromApplicationLayer(cMessage *);

	void parseDeployment();

 public:
	virtual NodeLocation getLocation();
	virtual double getEnergyToFly();
	virtual double getSpeed();
	virtual int getMovingTrajectory();
	virtual double getMaxSpeed();
	virtual double getMovimentRange();
	virtual double getStartTime_Final();
	virtual double getEndTime_Final();
	virtual double getX_Final();
	virtual double getY_Final();
	
	virtual double getWaypointX();
	virtual double getWaypointY();
	virtual double getWaypointZ();
	virtual double getEndTime();
};

#endif
