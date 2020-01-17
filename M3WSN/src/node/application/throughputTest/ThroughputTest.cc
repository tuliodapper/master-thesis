/*
 * ThroughputTest.cpp
 *
 *  Created on: Mar 8, 2012
 *      Author: bm
 */

#include "ThroughputTest.h"
#include "WiseDebug.h"

Define_Module(ThroughputTest);

ThroughputTest::ThroughputTest() {
}

ThroughputTest::~ThroughputTest() {
}

void ThroughputTest::initialize() {
	WiseBaseApplication::initialize();
	isSink = par("isSink");
	packet_rate = par("packet_rate");
	//recipientAddress = par("nextRecipient").stringValue();
	startupDelay = par("startupDelay");
	primaryPath = "-1";

	packet_spacing = packet_rate > 0 ? 1 / float (packet_rate) : -1;
	dataSN = 0;
	round = 0;
	nPktPrimary = 0;
	nPktBroadcast = 0;
	nRecPkt = 0;

	if (packet_spacing > 0){
		setTimer(SEND_PACKET, packet_spacing + startupDelay);
	}
	declareOutput("Packets received per node");

	if (self == 0) { //sink
		setTimer(RECEIVER_SIDE, 309.1);
	} else if (self == 1) { //sender
		setTimer(SENDER_SIDE, 309.5);
	}
}

void ThroughputTest::finish() {
    WiseBaseApplication::finish();
    cOwnedObject *Del=NULL;
    int OwnedSize = this->defaultListSize();
    for(int i=0; i<OwnedSize; i++){
	Del = this->defaultListGet(0);
	this->drop(Del);
	delete Del;
    }
}

void ThroughputTest::timerFiredCallback(int index){
	switch (index) {
		case SEND_PACKET:{
			toNetworkLayer(createGenericDataPacket(0, dataSN), primaryPath.c_str());
			//trace() << "Sending packet #" << dataSN;
			//trace() << "primaryPath " << primaryPath << "\n";
			dataSN++;

			if (atoi(primaryPath.c_str()) == -1 )
				nPktBroadcast ++;
			else if (atoi(primaryPath.c_str()) != -1)
				nPktPrimary ++;

			bool included = false;
			for (int i=0; i<statisticsSender.size(); i++){
				if (statisticsSender[i].round == round){
					included = true;
					statisticsSender[i].primaryPath = nPktPrimary;
					statisticsSender[i].broadcast = nPktBroadcast;
					statisticsSender[i].total = dataSN;
					/*trace() << "Sender update!";
					trace() << "round " << statisticsSender[i].round;
					trace() << "primaryPath " << statisticsSender[i].primaryPath;
					trace() << "broadcast " << statisticsSender[i].broadcast << "\n";*/
					break;
				}
			}
			if (included == false){
				statisticsSource temp;
				temp.round = round;
				temp.primaryPath = nPktPrimary;
				temp.broadcast = nPktBroadcast;
				statisticsSender.push_back(temp);
				/*trace() << "Sender include!";
				trace() << "round " << temp.round;
				trace() << "primaryPath " << temp.primaryPath;
				trace() << "broadcast " << temp.broadcast << "\n";*/
			}

			if (simTime() < 300.0)
				setTimer(SEND_PACKET, packet_spacing);
			break;
		}
		case RECEIVER_SIDE:{
			output() << "\nStatistics Receiver";
			for (int i=0; i< statisticsReceiver.size(); i++)
				output() << statisticsReceiver[i].round << " " << statisticsReceiver[i].recPkts << " " << statisticsReceiver[i].hops << " " << statisticsReceiver[i].total;
	    		break;
		}
		case SENDER_SIDE:{
			output() << "\nStatistics Sender";
			for (int i=0; i< statisticsSender.size(); i++)
				output() << statisticsSender[i].round << " " << statisticsSender[i].primaryPath << " " << statisticsSender[i].broadcast << " " << statisticsSender[i].total;
			break;
		}
	}
}

void ThroughputTest::handleMobilityControlMessage(MobilityManagerMessage* pkt) {
	WiseApplicationPacket *pktTrace = new WiseApplicationPacket("SENDING SCALAR DATA", TOPOLOGY_CONTROL);
	pktTrace->setSource(self);
	pktTrace->setX(pkt->getXCoorDestination());
	pktTrace->setY(pkt->getYCoorDestination());
	toNetworkLayer(pktTrace, BROADCAST_NETWORK_ADDRESS);
	trace() << "TOPOLOGY_CONTROL " << pktTrace->getX() << " " << pktTrace->getY();
}

void ThroughputTest::fromNetworkLayer(cPacket* msg, const char* src, double rssi, double lqi) {
	switch (msg->getKind()) {
		case NETWORK_LAYER_PACKET:{
			RoutingPacket *pkt = check_and_cast<RoutingPacket*>(msg);
			if (self == 1){
				if (round != pkt->getRound()){
					round = pkt->getRound();
					nPktPrimary = 0;
					nPktBroadcast = 0;
					//trace() << "sender increase round to " << round << "\n";
				}
				primaryPath = pkt->getPrimaryPath();
				//trace() << "receive a message from network layer with next hop";
				//trace() << "primary path " << primaryPath << "\n";
			} else if (self == 0 && atoi(pkt->getPrimaryPath()) != -1){
				nRecPkt ++;
				bool included = false;
				for (int i=0; i<statisticsReceiver.size(); i++){
					if (statisticsReceiver[i].round == pkt->getRound()){
						included = true;
						statisticsReceiver[i].recPkts ++;
						statisticsReceiver[i].hops = pkt->getHopCount();
						statisticsReceiver[i].total = nRecPkt;
						/*trace() << "update!";
						trace() << "round " << statisticsReceiver[i].round;
						trace() << "Hop count " << statisticsReceiver[i].hops;
						trace() << "recPkts " << statisticsReceiver[i].recPkts << "\n";*/
						break;
					}
				}
				if (included == false){
					statisticsSink temp;
					temp.round = pkt->getRound();
					temp.hops = pkt->getHopCount();
					temp.recPkts = 1;
					temp.total = nRecPkt;
					statisticsReceiver.push_back(temp);
					/*trace() << "include!";
					trace() << "round " << temp.round;
					trace() << "Hop count " << temp.hops;
					trace() << "recPkts " << temp.recPkts << "\n";*/
				}
			}
			break;
		}
		case APPLICATION_PACKET:{
			WiseApplicationPacket *rcvPacket = check_and_cast <WiseApplicationPacket*>(msg);
			int sequenceNumber = rcvPacket->getSequenceNumber();
			//trace() << "Received packet #" << sequenceNumber << " from node " << rcvPacket->getSource();
			collectOutput("Packets received per node", atoi(src));
			break;
		}
	}
}


void ThroughputTest::handleSensorReading(WiseSensorMessage* msg) {

}


void ThroughputTest::handleDirectApplicationMessage(WiseApplicationPacket* pkt) {
     //trace() << "direct app msg";
}
