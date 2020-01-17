#ifndef _UN_ROUTUNG_H_
#define _UN_ROUTUNG_H_

#include <cmath>
#include <queue>
#include <vector>
#include <omnetpp.h>
#include <algorithm>
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "WirelessChannel.h"
#include "Radio.h"
#include "VirtualMac.h"
#include "VirtualRouting.h"
#include "VirtualMobilityManager.h"
#include "SDNRoutingFrame_m.h"
#include "global.h"

using namespace std;

enum UNRoutingTimers {
	UN_TIMER_TRACE 			= 1,
	UN_TIMER_CONNECT 		= 2,
	UN_TIMER_CHECK 			= 3,
	UN_TIMER_STATUS 		= 4,
	UN_TIMER_CLEAN_BUFFER	= 5
};

struct routeCInfo{
	int idTransmission;
	int nodeId;
	int nextHop;
	int source;
	int destination;
	bool ack;
	bool replace;
	int lastReceivedPacket;
};

struct pktList{
	int seqNumber;
	queue <cPacket *> bufferPkt;
};

struct transmission_s {
	int id = 0;
	int lastReceivedPacket = -1;
	vector <pktList> buffer;
};

typedef map<int,transmission_s> transmission_t; 	// key: id_transmission
typedef map<int,transmission_t> buffer_t; 			// key: id_source
typedef transmission_t::iterator transmission_i;
typedef buffer_t::iterator buffer_i;

class UNRouting : public VirtualRouting {
 protected:
	// The .ned file's parameters
	int 					timeToLive;
	double 					timeOutHello;
	double 					timeOutCheck;
	double 					timeOutRefresh;
	double 					timeOutStatus;
	double 					timeOutTrace;

	// Modules
	VirtualMobilityManager	*mobilityModule;
	VirtualMac 				*macModule;
	WirelessChannel 		*wirelessModule;
	ResourceManager 		*srceRerouceModuce;
	ResourceManager 		*energy;

	// Own
	int 					identifier = 0;
	int 					idTransmission = 0;
	bool 					connected 	= false;
	bool 					sendStatus 	= false;
	vector <pktList> 		buffer;
	vector <routeCInfo> 	route;

	// Inherited
	void startup();
	void finish();
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void timerFiredCallback(int);
	void multipleTimerFiredCallback(int, int);

	double getRandomDelay();
	location_s getCurrentLocation();
	double getResidualEnergy();
	void sendPacket(int SDNRoutingPacketKind);
	void sendPacket(SDNRoutingPacket * netPacket);

	// From MAC Layer functions
	void fromMacConfirmationRegistry(SDNRoutingPacket *);
	void fromMacKeepAlive(SDNRoutingPacket *);
	void fromMacData(SDNRoutingPacket *, cPacket *);
	void fromMacResponseClose(SDNRoutingPacket *);

	// Timer functions
	void timerTrace();
	void timerConnect();
	void timerCheck();
	void timerStatus();

	void addPacketHeader(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket);

	// Message Type Video
	// buffer_t buffer;
	void processMessageTypeVideo(cPacket *pkt);
	bool searchBuffer(int, int);
};

bool SDUAVNet_sort_buffer(pktList a, pktList b);

#endif
