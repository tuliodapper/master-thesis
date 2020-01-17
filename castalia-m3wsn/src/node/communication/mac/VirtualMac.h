/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2011                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Yuriy Tselishchev, Athanassios Boulis                        *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *
 ****************************************************************************/

#ifndef VIRTUALMACMODULE
#define VIRTUALMACMODULE

#define SELF_MAC_ADDRESS self

#include <queue>
#include <vector>
#include <omnetpp.h>

#include <cmath>
#include <stdlib.h>
#include <stdio.h> //adicionado por Larissa

#include "ApplicationPacket_m.h"
//#include "LinGORoutingFrame_m.h"
//#include "XLinGORoutingFrame_m.h"
//#include "SDUAVNetRoutingFrame_m.h"
//#include "XLinGOMobiFANETRoutingFrame_m.h"
//#include "XLinGOVANETRoutingFrame_m.h"
//#include "QoEXLinGORoutingFrame_m.h"
//#include "XLinGORoutingFrame_m.h"
#include "RoutingPacket_m.h"
#include "MacPacket_m.h"
#include "RadioControlMessage_m.h"

#include "ResourceManager.h"
#include "Radio.h"
#include "TimerService.h"
#include "CastaliaModule.h"

using namespace std;

struct queueList{
	double dropProbability;
	queue <cPacket *> packets;
};

class VirtualMac: public CastaliaModule, public TimerService {
 protected:
	int self;		// the node's ID
	bool disabled;
	int macMaxFrameSize;
	int macFrameOverhead;
	unsigned int currentSequenceNumber;

	string dropType;
	double w1;
	double w2;
	double w3;
	double delayLimit;

	//a pointer to the object of the Radio Module (used for direct method calls)
	Radio *radioModule;

	//a pointer to the object of the Resource Manager Module (used for direct method calls)
	ResourceManager *resMgrModule;

    int macPacketDrops = 0;
	queue< cPacket* > TXBuffer;
	queue< cPacket* > TXbeaconBuffer;
	vector< unsigned int > pktHistory;
	vector <queueList> bufferTemp;

	void createAndSendRadioControlCommand(RadioControlCommand_type, double, const char *, BasicState_type);
	void initialize();
	void handleMessage(cMessage * msg);
	void finish();
	virtual void finishSpecific() {}
	virtual void startup() {}

	void toNetworkLayer(cMessage *);
	void toRadioLayer(cMessage *);
	virtual void fromNetworkLayer(cPacket *, int) = 0;
	virtual void fromRadioLayer(cPacket *, double, double) = 0;
	int bufferPacket(cPacket *);
	int bufferControlPacket(cPacket *);
	bool isNotDuplicatePacket(cPacket *);

	virtual int handleControlCommand(cMessage * msg);
	virtual int handleRadioControlMessage(cMessage * msg);

	void encapsulatePacket(cPacket *, cPacket *);
	cPacket *decapsulatePacket(cPacket *);

	void dropAlgorithm(cPacket *);
	double qoeDrop(ApplicationPacket *); //adicionado por Larissa

 public:
	virtual int getBufferSize();
	virtual int getControlBufferSize();
	virtual int getDelayMax();
	virtual string getAdaptationMechanism();
	virtual double getW1();
	virtual double getW2();
	virtual double getW3();
    void newOutput(std::string str);
    void newTrace(std::string str);
    int getPacketDrops();
	int macBufferSize;
	int dataRate_in;
	int dataRate_out;
	int avgBufferSize;
	std::string getOutputTelemetry(int lenQueueMac, int lenQueueNet, int packetDropMac, int packetDropNet);
	std::string getTraceTelemetry(int lenQueueMac, int lenQueueNet, int packetDropMac, int packetDropNet);

};

#endif
