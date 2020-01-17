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

#include "VirtualMac.h"

void VirtualMac::initialize()
{
	macBufferSize = par("macBufferSize");
	macFrameOverhead = par("macPacketOverhead");
	macMaxFrameSize = par("macMaxPacketSize");
	dropType = par("dropType").stringValue();
	w1 = par("w1");
	w2 = par("w2");
	w3 = par("w3");
	delayLimit = par("delayLimit");
	self = getParentModule()->getParentModule()->getIndex();

	/* Get a valid references to the Resources Manager module and the
	 * Radio module, so that we can make direct calls to their public methods
	 */
	radioModule = check_and_cast <Radio*>(getParentModule()->getSubmodule("Radio"));
	resMgrModule = check_and_cast <ResourceManager*>(getParentModule()->getParentModule()->getSubmodule("ResourceManager"));

	if (!resMgrModule || !radioModule)
		opp_error("\n Virtual Routing init: Error in geting a valid reference module(s).");

	setTimerDrift(resMgrModule->getCPUClockDrift());
	pktHistory.clear();
	disabled = true;
	currentSequenceNumber = 1;
}

int VirtualMac::handleControlCommand(cMessage * msg)
{
	// // trace() << "WARNING: handleControlCommand not defined in this module";
	return 0;
}

int VirtualMac::handleRadioControlMessage(cMessage * msg)
{
	toNetworkLayer(msg);
	return 1;
}

int VirtualMac::bufferPacket(cPacket * rcvFrame){
	if ((int)TXBuffer.size() >= macBufferSize) {
        macPacketDrops++;
		dropAlgorithm(rcvFrame);
		// send a control message to the upper layer
		MacControlMessage *fullBuffMsg = new MacControlMessage("MAC buffer full", MAC_CONTROL_MESSAGE);
		fullBuffMsg->setMacControlMessageKind(MAC_BUFFER_FULL);
		send(fullBuffMsg, "toNetworkModule");
        newOutput("drp;mac");
        trace() << "Packet DROPPED at MAC layer!";
		return 0;
	} else {

	    newTrace(getTraceTelemetry(getBufferSize(), 0, 0, 0));
	    newOutput(getOutputTelemetry(getBufferSize(), 0, 0, 0));

		TXBuffer.push(rcvFrame);
		trace() << "Packet buffered at MAC layer. Buffer: " << TXBuffer.size() << "/" << macBufferSize;
		return 1;
	}
}

int VirtualMac::bufferControlPacket(cPacket * rcvFrame){
	if ((int)TXbeaconBuffer.size() >= macBufferSize) {
		cancelAndDelete(rcvFrame);
		// send a control message to the upper layer
		MacControlMessage *fullBuffMsg = new MacControlMessage("MAC buffer full", MAC_CONTROL_MESSAGE);
		fullBuffMsg->setMacControlMessageKind(MAC_BUFFER_FULL);
		send(fullBuffMsg, "toNetworkModule");
		return 0;
	} else {
		TXbeaconBuffer.push(rcvFrame);
		// // trace() << "Control pkt buffered from network layer, buffer state: " << TXbeaconBuffer.size() << "/" << macBufferSize;
		return 1;
	}
}

//================================================================
//    dropAlgorithm
//================================================================
void VirtualMac::dropAlgorithm(cPacket * rcvFrame){
	// output() << "dropType " << dropType << " w1 " << w1 << " w2 " << w2 << " w3 " << w3;
	if (dropType == "dropTail"){
		// send a control message to routing layer for statitics reasons

// Commented by Antonio and Tulio
	    MacPacket *packet = dynamic_cast <MacPacket*>(rcvFrame);
	    MacPacket *dupPkt = packet->dup();
	    RoutingPacket *temp = check_and_cast <RoutingPacket*>(dupPkt->decapsulate());
	    RoutingPacket *pktTrace = new RoutingPacket("dropped packet", MAC_STATISTICS_MESSAGE);
	    pktTrace->setIdVideo(temp->getIdVideo());
	    pktTrace->setSource(temp->getSource());
	    toNetworkLayer(pktTrace);

		cancelAndDelete(rcvFrame);
		// output() << " drop using drop tail";
	//--------------------------------------QoE-aware--------------------------------------
	} else if (dropType == "QoE-aware"){
		// output() << " drop using QoE-aware";
		TXBuffer.push(rcvFrame);
		while (!TXBuffer.empty()){
			MacPacket *packet = dynamic_cast <MacPacket*>(TXBuffer.front());
			MacPacket *macPkt = packet->dup();
			RoutingPacket *netPkt = check_and_cast <RoutingPacket*>(decapsulatePacket(macPkt));
			ApplicationPacket *appPkt = check_and_cast <ApplicationPacket*>(netPkt->decapsulate());

			queueList temp;
			temp.dropProbability = qoeDrop(appPkt); // calcular o DP //done
			temp.packets.push(packet->dup());
			bufferTemp.push_back(temp);
			TXBuffer.pop();

//			// // trace() << " - SN_" << netPkt->getSource() << " location: (" << appPkt->getX() << "," << appPkt->getY() << ") relevance: " << appPkt->getRelevance() << " frame type: " << appPkt->getInfo().frameType << " frame number: " << appPkt->getFrame() << " GoP: " << appPkt->getInfo().gop << " dropProbability " << temp.dropProbability;
		}
		//// // trace() << "---copy from TXBuffer to bufferTemp---";
		int index = 0;
		int maior = 0;
		for(int i=0; i<bufferTemp.size(); i++){
			if(bufferTemp[i].dropProbability>=maior){
				maior = bufferTemp[i].dropProbability;
				index = i;
			}
		}
		// send a control message to routing layer for statitics reasons

// Commented by Antonio and Tulio
//		MacPacket *packet = dynamic_cast <MacPacket*>(bufferTemp[index].packets.front());
//		MacPacket *dupPkt = packet->dup();
//		RoutingPacket *temp = check_and_cast <RoutingPacket*>(dupPkt->decapsulate());
//		RoutingPacket *pkt// trace = new RoutingPacket("dropped packet", MAC_STATISTICS_MESSAGE);
//		pkt// trace->setIdVideo(temp->getIdVideo());
//		pkt// trace->setSource(temp->getSource());
//		toNetworkLayer(pkt// trace);

		bufferTemp.erase(bufferTemp.begin()+index); //apaga o elemento

		//// // trace() << "---copy from bufferTemp to TXBuffer---";
		for(int i=0;i<bufferTemp.size(); i++){
			TXBuffer.push(bufferTemp[i].packets.front());
		}
		bufferTemp.clear();
	//--------------------------------------exception--------------------------------------
	} else{
		// send a control message to routing layer for statitics reasons

// Commented by Antonio and Tulio
//		MacPacket *packet = dynamic_cast <MacPacket*>(rcvFrame);
//		MacPacket *dupPkt = packet->dup();
//		RoutingPacket *temp = check_and_cast <RoutingPacket*>(dupPkt->decapsulate());
//		RoutingPacket *pkt// trace = new RoutingPacket("dropped packet", MAC_STATISTICS_MESSAGE);
//		pkt// trace->setIdVideo(temp->getIdVideo());
//		pkt// trace->setSource(temp->getSource());
//		toNetworkLayer(pkt// trace);

		cancelAndDelete(rcvFrame);
		// output() << "Exception - drop using drop tail";
	}
}

double VirtualMac::qoeDrop(ApplicationPacket *appPkt){
	// output() << "\n\n----------------"<< appPkt->getInfo().frameType << "-Frame " << appPkt->getFrame();
	//Cálculo da Relevância em Base Exponencial
	double relevanceInput = appPkt->getRelevance();
	double relevance = (double)1/(1+std::exp(-0.5*(relevanceInput - 5)));
	// output() << "Relevânciainput:  " << relevanceInput;
	// output() <<"Relevance: " << relevance<<"\n";

	//Cálculo de Delay em Base Exponencial
	double delayInput = (simTime().dbl() - appPkt->getAppNetInfoExchange().timestamp.dbl())*1000;
	if (delayInput < 0)
		delayInput = 0;
	double delayMedium = delayLimit/2;
	double delay = (double) 1/(1+std::exp(-0.5*(delayInput - delayMedium)));

	// output() << "TimeStamp atual: " << simTime();
	// output() << "TimeStamp pacote: " << appPkt->getAppNetInfoExchange().timestamp;
	// output() << "DelayInput: " << delayInput;
	// output() << "DelayMedium: " << delayMedium;
	// output() << "DelayLimit: " << delayLimit;
	// output() << "Delay: " << delay << "\n";

	//Cálculo de frame importance em Base Exponencial
	int gopmax = (appPkt->getInfo().gop - 1)* 18;
	int frame = appPkt->getFrame();
	int frameInput = (appPkt->getFrame() - gopmax) - 1;
	switch (appPkt->getInfo().frameType){
		case 'B':{
			frameInput = 19;
			break;
		}
	}
	double frameImportance = (double) 1/(1+std::exp(-0.5*(frameInput - 9)));
	// output() << "GoP: " << appPkt->getInfo().gop - 1;
	// output() << "Frame position within the gop " << frameInput;
	// output() << "frameImportance " << frameImportance << "\n";

	double dropPercentage = (w1*frameImportance) + (w2*delay) + (w3*relevance);

	// output() << "Drop probability: " << dropPercentage;
	return dropPercentage;
}

void VirtualMac::handleMessage(cMessage * msg)
{

	int msgKind = (int)msg->getKind();

	if (disabled && msgKind != NODE_STARTUP) {
		delete msg;
		return;
	}

	switch (msgKind) {

		case NODE_STARTUP:{
			disabled = false;
			send(new cMessage("MAC --> Radio startup message", NODE_STARTUP), "toRadioModule");
			startup();
			break;
		}

		case NETWORK_LAYER_PACKET:{

			RoutingPacket *pkt = check_and_cast <RoutingPacket*>(msg);
			if (macMaxFrameSize > 0 && macMaxFrameSize < pkt->getByteLength() + macFrameOverhead) {
				// // trace() << "Oversized packet dropped. Size:" << pkt->getByteLength() << ", MAC layer overhead:" << macFrameOverhead << ", max MAC frame size:" << macMaxFrameSize;
				break;
			}
			/* Control is now passed to a specific MAC protocol by calling fromNetworkLayer()
			 * Notice that after the call we RETURN (not BREAK) so that the packet is not deleted.
			 * This is done since the packet will most likely be encapsulated and forwarded to the
			 * Radio layer. If the protocol specific function wants to discard the packet is has
			 * to delete it.
			 */
			fromNetworkLayer(pkt, pkt->getNetMacInfoExchange().nextHop);
			return;
		}

		case MAC_LAYER_PACKET:{
			MacPacket *pkt = check_and_cast <MacPacket*>(msg);
			/* Control is now passed to a specific routing protocol by calling fromRadioLayer()
			 * Notice that after the call we BREAK so that the MAC packet gets deleted.
			 * This will not delete the encapsulated NET packet if it gets decapsulated
			 * by fromMacLayer(), i.e., the normal/expected action.
			 */
			fromRadioLayer(pkt, pkt->getMacRadioInfoExchange().RSSI,
								pkt->getMacRadioInfoExchange().LQI);
			break;
		}

		case TIMER_SERVICE:{
			handleTimerMessage(msg);
			break;
		}

		case MAC_CONTROL_COMMAND:{
			if (handleControlCommand(msg))
				return;
			break;
		}

		case RADIO_CONTROL_COMMAND:{
			toRadioLayer(msg);
			return; // do not delete msg
		}

		case RADIO_CONTROL_MESSAGE:{
			if (handleRadioControlMessage(msg))
				return;
			break;
		}

		case OUT_OF_ENERGY:{
			disabled = true;
			break;
		}

		case DESTROY_NODE:{
			disabled = true;
			break;
		}

		default:{
			opp_error("MAC module received message of unknown kind %i", msgKind);
		}
	}

	delete msg;
}

void VirtualMac::finish()
{
	cOwnedObject *Del=NULL;
	int OwnedSize = this->defaultListSize();
	for(int i=0; i<OwnedSize; i++){
		Del = this->defaultListGet(0);
		this->drop(Del);
		delete Del;
	}

	CastaliaModule::finish();
	while (!TXBuffer.empty()) {
		cancelAndDelete(TXBuffer.front());
		TXBuffer.pop();
	}
	while (!TXbeaconBuffer.empty()) {
		cancelAndDelete(TXbeaconBuffer.front());
		TXbeaconBuffer.pop();
	}
}

void VirtualMac::toNetworkLayer(cMessage * macMsg)
{
	// // trace() << "Delivering [" << macMsg->getName() << "] to Network layer";
	send(macMsg, "toNetworkModule");
}

void VirtualMac::toRadioLayer(cMessage * macMsg)
{
	send(macMsg, "toRadioModule");
}

void VirtualMac::encapsulatePacket(cPacket * pkt, cPacket * netPkt)
{
	MacPacket *macPkt = check_and_cast <MacPacket*>(pkt);
	macPkt->setByteLength(macFrameOverhead);
	macPkt->setKind(MAC_LAYER_PACKET);
	macPkt->setSequenceNumber(currentSequenceNumber++);
	macPkt->setSource(SELF_MAC_ADDRESS);
	// by default the packet created has its generic destination address to broadcast
	// a specific protocol can change this, and/or set more specific dest addresses
	macPkt->setDestination(BROADCAST_MAC_ADDRESS);
	macPkt->encapsulate(netPkt);
}

cPacket *VirtualMac::decapsulatePacket(cPacket * pkt)
{
	MacPacket *macPkt = check_and_cast <MacPacket*>(pkt);
	RoutingPacket *netPkt = check_and_cast <RoutingPacket*>(macPkt->decapsulate());
	netPkt->getNetMacInfoExchange().RSSI = macPkt->getMacRadioInfoExchange().RSSI;
	netPkt->getNetMacInfoExchange().LQI = macPkt->getMacRadioInfoExchange().LQI;
	// The lastHop field has valid information only if the specific MAC protocol
	// updates the generic'source' field in the MacPacket.
	netPkt->getNetMacInfoExchange().lastHop = macPkt->getSource();
	return netPkt;
}

bool VirtualMac::isNotDuplicatePacket(cPacket * pkt)
{
	//extract source address and sequence number from the packet
	MacPacket *macPkt = check_and_cast <MacPacket*>(pkt);
	int src = macPkt->getSource();
	unsigned int sn = macPkt->getSequenceNumber();

	//resize packet history vector if necessary
	if (src >= (int)pktHistory.size())
		pktHistory.resize(src+1,0);

	//if recorded sequence number is less than new
	//then packet is new (i.e. not duplicate)
	if (pktHistory[src] < sn) {
		pktHistory[src] = sn;
		return true;
	}

	return false;
}

int VirtualMac::getControlBufferSize(){
	return (int)TXbeaconBuffer.size();
}

int VirtualMac::getBufferSize(){
	return (int)TXBuffer.size();
}

int VirtualMac::getPacketDrops(){
    return macPacketDrops;
}

int VirtualMac::getDelayMax(){
	return delayLimit;
}

string VirtualMac::getAdaptationMechanism(){
	return dropType;
}

double VirtualMac::getW1(){
	return w1;
}

double VirtualMac::getW2(){
	return w2;
}

double VirtualMac::getW3(){
	return w3;
}

void VirtualMac::newOutput(std::string str){
    output() << simTime() << ";" << self << ";" << str;
}

void VirtualMac::newTrace(std::string str){
    trace() << simTime() << ";" << self << ";" << str;
}

std::string VirtualMac::getOutputTelemetry(int lenQueueMac, int lenQueueNet, int packetDropMac, int packetDropNet){
    std::stringstream ss;
    ss  << "tel;"
        << lenQueueMac << ";"
        << lenQueueNet << ";"
        << packetDropMac << ";"
        << packetDropNet;
    return ss.str();
}

std::string VirtualMac::getTraceTelemetry(int lenQueueMac, int lenQueueNet, int packetDropMac, int packetDropNet){
    std::stringstream ss;
    ss  << "telemetry -> "
        << "lenQueueMac: "      << lenQueueMac << " | "
        << "lenQueueNet: "      << lenQueueNet << " | "
        << "packetDropMac: "    << packetDropMac << " | "
        << "packetDropNet: "    << packetDropNet;
    return ss.str();
}
