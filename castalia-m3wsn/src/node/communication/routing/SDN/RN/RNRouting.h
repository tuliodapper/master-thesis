#ifndef _RN_ROUTING_H_
#define _RN_ROUTING_H_

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
#include <math.h>
#include "WirelessChannel.h"
#include "Radio.h"
#include "VirtualMac.h"
#include "VirtualRouting.h"
#include "VirtualMobilityManager.h"
#include "SDNRoutingFrame_m.h"
#include "global.h"
#include "contract_out.pb.h"
#include "connection.h"

using namespace std;

typedef contract_out::ContractOut contractOut_t;

enum RNRoutingTimers {
	RN_TIMER_TRACE 			= 1,
	RN_TIMER_KEEP_ALIVE 	= 2,
	RN_TIMER_PACKETS		= 3,
	RN_TIMER_STATUS 		= 4,
	RN_TIMER_TELEMETRY 		= 5
};

class RNRouting : public VirtualRouting {
protected:
	// The .ned file's parameters
	int 					timeToLive;
	int 					maxRetransmitions;
	double 					positionRangeUser;
	double 					intervalCounter;
	double 					timeOutTrace;
	double 					timeOutKeepAlive;
	double 					timeOutPacketIn;
	double 					timeOutResponse;
	double 					timeOutNotifyUser;
	double 					timeOutStatus;
    double                  timeOutTelemetry;

	// Modules
	VirtualMobilityManager	*mobilityModule;
	VirtualMac 				*macModule;
	WirelessChannel 		*wirelessModule;
	ResourceManager 		*srceRerouceModuce;
	ResourceManager 		*energy;

	// Own
    bool                    sendTelemetry   = false;
	int 					mode = 0;
	int 					identifier = 0;
	bool 					sendStatus 	= false;
	routes_t 				routingTable;
	users_t					users;
	area_s 					area;
	double					lastTimeUpdateSentPackets = 0.0;
	sentPackets_t 			sentPackets;
	load_s					load;
	Connection 				connection;

	// Inherited
	void startup();
	void finish();
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void timerFiredCallback(int);

	void addTelemetry(SDNRoutingPacket * netPacket);
	void addTelemetryByHop(SDNRoutingPacket * netPacket, double RSSI);
	void parserTelemetry(SDNRoutingPacket * netPacket, double RSSI);
	void deparserTelemetry(SDNRoutingPacket * netPacket);

	void addSentPacket(SDNRoutingPacket * netPacket, double timeOut);
	void addUser(user_s user);
	void addRoute(route_s route);
	double isOutOfRange(double distance);
	double getDistance(location_s location1, location_s location2);

	location_s getCurrentLocation();
	double getResidualEnergy();
	SDNRoutingPacket * createPacket(int SDNRoutingPacketKind, int userId);
	SDNRoutingPacket * createPacket(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket);
	void sendPacket(int SDNRoutingPacketKind, int userId);
	void sendPacket(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket);
	void sendPacket(SDNRoutingPacket * netPacket);
    void addPacketHeader(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket);
	void resetTimerPackets();
	void resetRemainingTimeSentPackets();
	
	// From MAC Layer functions
	void readUserStatus(SDNRoutingPacket *);
	void fromMacData(SDNRoutingPacket * netPacket, double RSSI);
	void fromMacHello(SDNRoutingPacket *);
	void fromMacClose(SDNRoutingPacket *);
	void fromMacRefresh(SDNRoutingPacket *);
	void fromMacUpdateUser(SDNRoutingPacket *);
	void fromMacUpdateRoutingTable(SDNRoutingPacket *);
	void fromMacUpdateArea(SDNRoutingPacket *);
	void fromMacAck(SDNRoutingPacket *);

	void fromApplication();
	void fromApplicationUpdateRoutingTable(const contract_out::ContractOut_UpdateRoutingTable & UpdateRoutingTable);
	void fromApplicationUpdateUser(const contract_out::ContractOut_UpdateUser & UpdateUser);

	// Timer functions
	void timerTrace();
	void timerKeepAlive();
	void timerPackets();
	void timerStatus();
	void timerTelemetry();

};

#endif
