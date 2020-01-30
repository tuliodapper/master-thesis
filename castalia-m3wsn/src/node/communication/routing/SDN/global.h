#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <cmath>
#include <queue>
#include <vector>
#include <omnetpp.h>
#include <algorithm>
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
#include "contract_in.pb.h"

#define OUTPUT_PACKET_SENT "SENT"
#define OUTPUT_PACKET_RECEIVED "RECV"
#define OUTPUT_PACKET_FOWARDED "FWRD"
#define OUTPUT_PACKET_RECEIVED_TO_FOWARD "RCFW"

struct relay_s {
    int 		id;
    status_s 	status;
    int 		counter;
};

struct user_s {
    int 		id;
    status_s 	status;
    int 		counter;
};

struct sentDataPacket_s {
	SDNRoutingPacket*	packet;
};

struct sentPacket_s {
	double 				timeOut;
	double 				remainingTime;
	SDNRoutingPacket*	packet;
	int 				retransmitions;
};

typedef map<int,route_s> routes_t; 	// key: destination
typedef routes_t::iterator routes_i;
typedef map<int,user_s> users_t; 	// key: id
typedef users_t::iterator users_i;    // key: id
typedef map<int,relay_s> relays_t; 	// key: id
typedef relays_t::iterator relays_i;    // key: id
typedef vector<sentPacket_s> sentPackets_t;
typedef sentPackets_t::iterator sentPackets_i;
   
bool sort_sentPacket(sentPacket_s a, sentPacket_s b);
std::string getSDNRoutingPacketKindName(int SDNRoutingPacketKind);
std::string getContractInPacketName(enum contract_in::ContractIn_ContractInPacket contractInPacket);
std::string getOutputPacket(std::string action, SDNRoutingPacket * netPacket);
std::string getTracePacket(std::string action, SDNRoutingPacket * netPacket);
std::string getOutputLocation(location_s location);
std::string getTraceLocation(location_s location);
std::string getOutputTelemetry(SDNRoutingPacket * netPacket);
std::string getTraceTelemetry(SDNRoutingPacket * netPacket);
std::string getOutputTelemetry(int lenQueueMac, int lenQueueNet, int packetDropMac, int packetDropNet);
std::string getTraceTelemetry(int lenQueueMac, int lenQueueNet, int packetDropMac, int packetDropNet);
double getRandomDelay();

#endif
