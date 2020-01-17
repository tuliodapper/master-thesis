cplusplus {{
#include "RoutingPacket_m.h"
#include <cmath>
#include <queue>
#include <vector>
#include <map>
#include <omnetpp.h>
#include <algorithm>
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
}}

class RoutingPacket;

enum SDNRouting_FrameType{
	SDN_HELLO					= 0001;
	SDN_CLOSE					= 0002;
	SDN_REFRESH					= 0003;
	SDN_KEEP_ALIVE				= 0004;
	SDN_IS_CONNECTED			= 0005;
	SDN_CONFIRMATION_REGISTRY	= 0006;
	SDN_RESPONSE_CLOSE			= 0007;
	SDN_RESPONSE_IS_CONNECTED	= 0010;
	SDN_RESPONSE_KEEP_ALIVE		= 0011;
	SDN_REGISTER				= 0012;
	SDN_UNREGISTER				= 0013;
	SDN_REFRESH_REGISTER		= 0014;
	SDN_PACKET_IN				= 0015;
	SDN_NOTIFY_STATUS_USER		= 0016;
	SDN_UPDATE_USER				= 0017;
	SDN_UPDATE_ROUTING_TABLE	= 0020;
	SDN_UPDATE_AREA				= 0021;
	SDN_ACK						= 0022;
	SDN_DATA					= 0023;
	SDN_TELEMETRY 				= 0024;
};

enum SDNEvent{
	SDN_FORCED = 1;
	SDN_REQUIRED = 2;
};

enum SDNAction{
	SDN_ADD 	= 1;
	SDN_DELETE 	= 2;
	SDN_UPDATE 	= 3;
};

struct location_s {
	double x;
	double y;
};

struct status_s {
	bool 		null;
	location_s 	location;
};

struct route_s {
    int 		destination;
    bool 		drop;
    string 		nextHopAddress;
};

struct area_s {
    location_s 	center;
    double 		radius;
};

struct load_s {
	double 		waitTimePacketQueue;
	int 		userNumber;
	int 		dropNumber;
};

struct SDNUserStatus {
	status_s status;
}

struct SDNRelayStatus {
	status_s status;
}

struct SDNNotifyStatusUser {
	int userId;
	status_s status;
}

struct SDNRegister {
	int userId;
	status_s status;
	load_s load;
}

struct SDNUnregister {
	int userId;
	int event;
}

struct SDNPacketIn {
	int userId;
}

struct SDNAck {
	int identifier;
}

struct SDNUpdateUser {
	int userId;
	int action;
	int event;
}

struct SDNUpdateRoutingTable {
	int relayId;
	route_s route;
	int action;
}

struct SDNUpdateArea {
	area_s area;
}

struct SDNTelemetryByHop {
	int relayId;
	double timeReceived;
	int lenQueueMac;
	int lenQueueNet;
	int packetDropMac;
	int packetDropNet;
	double rssi;
}

struct SDNTelemetry {

	int source;
	int destination;
	int messageType;
	int size;

	int lenMsgTelemetryByHop = 0;
	SDNTelemetryByHop msgTelemetryByHop[50];

}

packet SDNRoutingPacket extends RoutingPacket{

	int SDNRoutingPacketKind enum (SDNRouting_FrameType);

	SDNUserStatus msgUserStatus;

	SDNRelayStatus msgRelayStatus;

	SDNNotifyStatusUser msgNotifyStatusUser;
	
	SDNRegister msgRegister;

	SDNUnregister msgUnregister;

	SDNPacketIn msgPacketIn;

	SDNAck msgAck;

	SDNUpdateUser msgUpdateUser;

	SDNUpdateRoutingTable msgUpdateRoutingTable;

	SDNUpdateArea msgUpdateArea;

	SDNTelemetry msgTelemetry;

	bool hasStatus = false;
	
	bool hasTelemetry = false;
};
