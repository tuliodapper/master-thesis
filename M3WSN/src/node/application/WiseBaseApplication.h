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

#ifndef __WiseBaseApplication_h__
#define __WiseBaseApplication_h__

#include <sstream>
#include <vector>
#include <string>
#include <omnetpp.h>
#include "WiseApplicationPacket_m.h"
#include "WiseSensorMessage_m.h"
#include "WisePhysicalProcessMessage_m.h"
#include "ResourceManager.h"
#include "Radio.h"
#include "VirtualMobilityManager.h"
#include "CastaliaModule.h"
#include "TimerService.h"
#include "CritSensor.h"
#include "MobilityManagerMessage_m.h"
#include "RoutingPacket_m.h"
#include "Node.h"

#define SELF_NETWORK_ADDRESS selfAddress.c_str()

class WiseBaseApplication: public CastaliaModule, public TimerService {
private:
	simtime_t initialize_time;
	bool first_initialize;

protected:
	/*--- The .ned file's parameters ---*/
	string applicationID;
	int priority;
	bool synchronizedStartup;
	int maxAppPacketSize;
	int packetHeaderOverhead;
	int constantDataPayload;
	bool isSink;
	double latencyMax;
	int latencyBuckets;

	/*--- Custom class parameters ---*/
	int self;			// the node's ID
	int n_nodes;			// number of nodes
	string selfAddress;
	ResourceManager *resMgrModule;
	VirtualMobilityManager *mobilityModule;	
	CritSensor* sensor;
	Radio *radioModule;
	int disabled;
	double cpuClockDrift;
	std::vector<Node> neighbours;
	double last_packet_latency;

public:
	std::vector<Node> getNeighbors() { return neighbours; };
	virtual int getGopSize();

protected:
	WiseBaseApplication() : first_initialize(true) {} ;
	virtual void initialize();
	virtual void startup() {} 
	virtual void handleMessage(cMessage* msg);
	virtual void finish();
	virtual void finishSpecific() {}

	void requestSensorReading();
	void toMobilityManager(cMessage *);
	void toNetworkLayer(cMessage* pkt);
	void toNetworkLayer(cPacket* pkt, const char* dst, double delay=0);
	void sendDirectApplicationMessage(WiseApplicationPacket* pkt, const char* dst,
				   unsigned int type=WISE_APP_DIRECT_COMMUNICATION);

	virtual void fromNetworkLayer(cPacket *, const char* src, double rssi, double lqi) = 0;
	virtual void handleMobilityControlMessage(MobilityManagerMessage *) {};
	virtual void handleSensorReading(WiseSensorMessage *msg) {};
	virtual void handleNetworkControlMessage(cMessage* msg) {};
	virtual void handleMacControlMessage(cMessage* msg) {};
	virtual void handleRadioControlMessage(RadioControlMessage* msg) {};
	virtual void handleDirectApplicationMessage(WiseApplicationPacket* pkt) {};

	inline bool lastNode() const { return self == (n_nodes - 1); } ;
	inline bool firstNode() const { return self == 0; } ;
	double theValue;

	void readParameter();
	void processAppPacket(WiseApplicationPacket *pkt);
	void normalAppPacket(WiseApplicationPacket *pkt);
	WiseApplicationPacket *createGenericDataPacket(double, unsigned int, int = -1);
};

#endif //__WiseBaseApplication_h__
