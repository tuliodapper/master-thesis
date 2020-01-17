//********************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2011                             *
//*  Developed at the ATP lab, Networked Systems research theme                  *
//*  Author(s): Yuriy Tselishchev, Athanassios Boulis                            *
//*  This file is distributed under the terms in the attached LICENSE file.      *
//*  If you do not find this file, copies can be found by writing to:            *
//*                                                                              *
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia                 *
//*      Attention:  License Inquiry.                                            *
//*                                                                              *
//*******************************************************************************/

// We need to pass information between app and routing layer which is external to
// the packet i.e. not carried by a real packet (e.g., which is the destination,
// or what was the RSSI for the packet received) but this information is related
// to the specific packet. Since information is passed between modules with
// messages/packets, we decided to encode this kind of external info as a 
// separate structure in the packet. The fields there are handled by the
// virtualApp and virtualRouting code, setting a framework of interaction.

enum AplicationMessageType {
    APP_ADV		= 2;
    APP_JOIN	= 3;
    APP_ACK		= 4;
    APP_HEAD	= 5;
    APP_ADV_NOT	= 6;
};

struct TraceInfo {
    int id;		// added by Pedro Cumino
	char type;	// added by Pedro Cumino
  	int idFrame;
	int byteLength;
	int nodeId;
	int seqNum;
	int idTransmission;
	char frameType;
	int gop;
	int k;
};

struct AppNetInfoExchange_type {
	double RSSI;	// the RSSI of the received packet
	double LQI;		// the LQI of the received packet
	string source;	// the routing layer source of the received packet
	string destination;	// the routing layer dest of the packet to be sent
	simtime_t timestamp;	// creation timestamp of the received packet 
} 

// A generic application packet. If defining your own packet you have to extend
// from this packet. You do not have to use the fields already defined, and you
// can always define your own size.

packet ApplicationPacket {
    
    int AppPacketKind enum (AplicationMessageType);
	AppNetInfoExchange_type appNetInfoExchange;
	string applicationID;		// virtual app uses application ID to filter packet delivery.
	unsigned int sequenceNumber;// a field to distinguish between packets
	double data;				// a simple type to carry some data

	//to evalvid
	TraceInfo info;
	int idNode;
	int idFrame;
	int idVideo;
	int frame;
	double iLost;
	double pLost;
	double bLost;
	double numberRecFrames;

	//to game theory
	double x;
	double y;
	double startVideo;
	double x_final;
	int relevance;
	int source;
	int destination;
	int nextHop;
	int eventType;


	// Added by Antonio e Tulio
	bool connected;
	int messageType;
	int idTransmission;

}

