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

#ifndef __WiseBaseSensorManager_h__
#define __WiseBaseSensorManager_h__

#include "WiseSensorManagerMessage_m.h"
#include "WisePhysicalProcessMessage_m.h"
#include "WiseBaseTerrain.h"
#include "VirtualMobilityManager.h"
#include "CastaliaModule.h"

class WiseBaseSensorManager: public CastaliaModule {
private:
	/*--- Custom class member variables ---*/
	bool disabled;
	vector<simtime_t> lastSampleTime;
	VirtualMobilityManager* nodeMobilityModule;

protected:
	/*--- The .ned file's parameters ---*/
	int self; // the node's ID
	int numSensingDevices;
	int numProcesses;
	vector<simtime_t> minSamplingIntervals;
	WiseBaseTerrain *terrain;

protected:
	virtual void initialize();
	virtual void finish();
	virtual void handleMessage(cMessage* msg);


	virtual void startup() {};
	virtual void handleSample(WisePhysicalProcessMessage* sample) = 0;
	virtual void processSampleRequest(WiseSensorManagerMessage* req) = 0;
	virtual void processCommandRequest(WiseSensorManagerMessage* req) = 0;

	inline NodeLocation get_node_location() { return nodeMobilityModule->getLocation(); };

public:
    virtual bool covers(double x, double y) = 0;
    bool isDisabled() { return disabled; };
};

#endif // __WiseBaseSensorManager_h__
