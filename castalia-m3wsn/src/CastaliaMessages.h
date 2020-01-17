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

#ifndef CASTALIA_MESSAGES
#define CASTALIA_MESSAGES

#define BROADCAST_MAC_ADDRESS -1
#define CONTROLLER_NETWORK_ADDRESS "0"
#define BROADCAST_NETWORK_ADDRESS "-1"
#define SINK_NETWORK_ADDRESS "SINK"
#define PARENT_NETWORK_ADDRESS "PARENT"

enum CastaliaMessageTypes {
	NODE_STARTUP 			= 1,
	TIMER_SERVICE 			= 2,
	MULTIPLE_TIMER_SERVICE		= 3,

	OUT_OF_ENERGY 			= 4,
	DESTROY_NODE 			= 5,

	SENSOR_READING_MESSAGE 		= 6,
	MULTIMEDIA_REQUEST_MESSAGE	= 7,
	RESOURCE_MANAGER_DRAW_POWER 	= 8,
	PHYSICAL_PROCESS_SAMPLING 	= 9,
	PHYSICAL_PROCESS_SCALAR_DATA	= 10,

	WC_SIGNAL_START 		= 11,
	WC_SIGNAL_END 			= 12,
	WC_NODE_MOVEMENT 		= 13,

	RADIO_CONTROL_MESSAGE 		= 21,
	RADIO_CONTROL_COMMAND 		= 22,
	RADIO_ENTER_STATE 		= 23,
	RADIO_CONTINUE_TX 		= 24,

	MAC_LAYER_PACKET 		= 30,
	MAC_CONTROL_MESSAGE 		= 31,
	MAC_CONTROL_COMMAND 		= 32,
	MAC_STATISTICS_MESSAGE 		= 33,

	NETWORK_LAYER_PACKET 		= 40,
	NETWORK_CONTROL_MESSAGE 	= 41,
	NETWORK_CONTROL_COMMAND 	= 42,
	NETWORK_SETUP			= 43,
	NETWORK_EVENT			= 44,
	NETWORK_STATISTICS_MESSAGE 	= 45,

	APPLICATION_MESSAGE		= 50,
	APPLICATION_PACKET 		= 51,
	MULTIMEDIA_PACKET 		= 52,
	SCALAR_PACKET 			= 53,
	APPLICATION_STATISTICS_MESSAGE 	= 54,
	SENSING_RELEVANCE_MESSAGE	= 55,
	APPLICATION_PLATOON		= 56,

	MOBILE_MESSAGE			= 60,
	HANDOFF_MSG			= 61,
	MOBILE_WAKEUP			= 62,

	TOPOLOGY_CONTROL		= 70,

	INTERNAL_ISCONNECTED		= 71,
	INTERNAL_RESPONSE_ISCONNECTED	= 72,
	INTERNAL_CLOSE 			= 73

};

#endif
