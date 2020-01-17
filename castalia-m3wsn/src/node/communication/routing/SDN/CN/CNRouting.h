#ifndef _CN_ROUTING_H_
#define _CN_ROUTING_H_

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
#include <iomanip>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include "WirelessChannel.h"
#include "Radio.h"
#include "VirtualMac.h"
#include "VirtualRouting.h"
#include "VirtualMobilityManager.h"
#include "SDNRoutingFrame_m.h"
#include "connection.h"
#include "contract_in.pb.h"
#include "contract_out.pb.h"
#include "global.h"

using namespace std;

enum CNRoutingTimers {
	CN_TIMER_TRACE 		= 1,
	CN_TIMER_KEEP_ALIVE = 2,
	CN_TIMER_PACKETS	= 3,
	CN_TIMER_REQUEST	= 4
};

typedef contract_in::ContractIn contractIn_t;
typedef contract_out::ContractOut contractOut_t;

class CNRouting : public VirtualRouting {
protected:
	// The .ned file's 
	int 					mode = 0;
	int 					timeToLive;
	int 					maxRetransmitions;
	double 					intervalCounter;
	double 					timeOutTrace;
	double 					timeOutKeepAlive;
	double 					timeOutResponse;
	double                  timeOutPackets;
	double 					timeOutRequest;

	// Modules
	VirtualMobilityManager	*mobilityModule;
	VirtualMac 				*macModule;
	WirelessChannel 		*wirelessModule;
	ResourceManager 		*srceRerouceModuce;
	ResourceManager 		*energy;

	// Own
	int 					identifier = 0;
	double					lastTimeUpdateSentPackets = 0.0;
	routes_t 				routingTable;
	sentPackets_t 			sentPackets;
	Connection 				connection;
	relays_t				relays;

	// Inherited
	void startup();
	void finish();
	void toApplication(contractIn_t * contractIn, string trace = "");
	void fromApplicationLayer(cPacket *, const char *);
	void fromApplication(string trace = "");
	void fromMacLayer(cPacket *, int, double, double);
	void timerFiredCallback(int);

	void addRoute(route_s route);
	string getNextHopAddressFromDestinationAddress(string destinationAddress);

	location_s getCurrentLocation();
	double getResidualEnergy();

	SDNRoutingPacket * createPacket(int SDNRoutingPacketKind, int relayId);
	SDNRoutingPacket * createPacket(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket);
	void sendPacket(SDNRoutingPacket * netPacket);
	void sendPacket(int SDNRoutingPacketKind, int relayId);
	void sendPacket(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket);
	void addSentPacket(SDNRoutingPacket * netPacket, double timeOut);
    void addPacketHeader(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket);
	void checkHasStatus(SDNRoutingPacket * netPacket);
	void readRelayStatus(SDNRoutingPacket * netPacket);
	void resetTimerPackets();
	void resetRemainingTimeSentPackets();

	void fromApplicationUpdateUser(const contract_out::ContractOut_UpdateUser &);
	void fromApplicationUpdateRoutingTable(const contract_out::ContractOut_UpdateRoutingTable &, string trace = "");
	void fromApplicationUpdateArea(const contract_out::ContractOut_UpdateArea &);
	void fromApplicationUpdateRoutingTable(SDNUpdateRoutingTable * msgUpdateRoutingTable);
	void fromApplicationUpdateArea(SDNUpdateArea * msgUpdateArea);

	// From MAC Layer functions
	void fromMacAck(SDNRoutingPacket *);
	void fromMacNotifyStatusUser(SDNRoutingPacket *);
	void fromMacPacketIn(SDNRoutingPacket *, string trace = "");
	void fromMacRefreshRegister(SDNRoutingPacket *);
	void fromMacRegister(SDNRoutingPacket *);
	void fromMacUnregister(SDNRoutingPacket *);
	void fromMacTelemetry(SDNRoutingPacket *);

	// Timer functions
	void timerTrace();
	void timerPackets();
	void timerKeepAlive();
	void timerRequest();

};
   
bool sort_sentPacket(sentPacket_s a, sentPacket_s b);

#endif
