#include "UNRouting.h"

Define_Module(UNRouting);

void UNRouting::startup() {
	// Read Config Parameters
	timeToLive		= par("TIME_TO_LIVE");
	timeOutHello	= par("TIMEOUT_HELLO").doubleValue();
	timeOutCheck	= par("TIMEOUT_CHECK").doubleValue();
	timeOutRefresh	= par("TIMEOUT_REFRESH").doubleValue();
	timeOutStatus	= par("TIMEOUT_STATUS").doubleValue();
	timeOutTrace	= par("TIMEOUT_TRACE").doubleValue();

	// Who am I
	self = getParentModule()->getParentModule()->getIndex();

	// Load Modules
	wirelessModule = check_and_cast <WirelessChannel*>(getParentModule()->getParentModule()->getParentModule()->getSubmodule("wirelessChannel"));
	mobilityModule = check_and_cast <VirtualMobilityManager*>(getParentModule()->getParentModule()->getSubmodule("MobilityManager"));
    radioModule = check_and_cast <Radio*>(getParentModule()->getSubmodule("Radio"));
	macModule = check_and_cast <VirtualMac*>(getParentModule()->getSubmodule("MAC"));	
	srceRerouceModuce = check_and_cast <ResourceManager*>(getParentModule()->getParentModule()->getParentModule()->getSubmodule("node",1)->getSubmodule("ResourceManager"));

	// Set Timers
    // setTimer(UN_TIMER_TRACE, 0);
    setTimer(UN_TIMER_CONNECT, getRandomDelay());

    newTrace(std::stringstream() << "UNRouting::started up!");
}

void UNRouting::finish() {
	cOwnedObject *Del=NULL;
	int OwnedSize = this->defaultListSize();
	for(int i=0; i<OwnedSize; ++i){
		Del = this->defaultListGet(0);
		this->drop(Del);
		delete Del;
	}
}

void UNRouting::fromMacLayer(cPacket *pkt, int srcMacAddress, double RSSI, double LQI){
	switch (pkt->getKind()) {
		case NETWORK_LAYER_PACKET:{
			SDNRoutingPacket *netPacket = dynamic_cast <SDNRoutingPacket*>(pkt);
			if (!netPacket)
				return;
			if (self == atoi(netPacket->getDestinationAddress()) &&
				self == atoi(netPacket->getNextHopAddress())){

                newOutput(getOutputPacket(OUTPUT_PACKET_RECEIVED, netPacket));
                newTrace(getTracePacket(OUTPUT_PACKET_RECEIVED, netPacket));

				switch (netPacket->getSDNRoutingPacketKind()) {
					case SDN_DATA:{
						fromMacData(netPacket, pkt);
						break;
					}
					case SDN_CONFIRMATION_REGISTRY:{
						fromMacConfirmationRegistry(netPacket);
						break;
					}
					case SDN_KEEP_ALIVE:{
						fromMacKeepAlive(netPacket);
						break;
					}
					case SDN_RESPONSE_CLOSE:{
						fromMacResponseClose(netPacket);
						break;
					}
				}
			}
			break;
		}
	}
}

void UNRouting::fromApplicationLayer(cPacket *pkt, const char *dstAddr){
	switch (pkt->getKind()) {
		case APPLICATION_MESSAGE:{
			ApplicationPacket *internalMsg = new ApplicationPacket("setup", APPLICATION_MESSAGE);
			internalMsg->setConnected(connected);
			toApplicationLayer(internalMsg);
			break;
		}
		case INTERNAL_CLOSE:{
			sendPacket(SDN_CLOSE);
			break;
		}
		case APPLICATION_PACKET:{

			ApplicationPacket *appPkt = check_and_cast <ApplicationPacket*>(pkt);

			SDNRoutingPacket *msg = new SDNRoutingPacket("SN is sending multimedia pkt", NETWORK_LAYER_PACKET);
			addPacketHeader(SDN_DATA, msg);
			msg->setDestinationAddress(dstAddr);
			msg->setHasStatus(sendStatus);
			msg->setIdVideo(appPkt->getIdVideo());
			msg->setMessageType(appPkt->getMessageType());
			msg->setIdTransmission(appPkt->getIdTransmission());

			if (sendStatus){
				SDNUserStatus msgUserStatus;
				msgUserStatus.status.location = getCurrentLocation();
				msg->setMsgUserStatus(msgUserStatus);
				sendStatus = false;
			}

			stringstream ss;
			ss 	<< self << "-"
				<< msg->getIdentifier() << "-"
				<< atoi(msg->getDestinationAddress());
			msg->setTrace(ss.str().c_str());

			encapsulatePacket(msg, pkt);

    		sendPacket(msg);
		}
	}
}

void UNRouting::timerFiredCallback(int index){
	switch (index) {
		case UN_TIMER_TRACE:{
			timerTrace();
			break;
		}
		case UN_TIMER_CONNECT:{
			timerConnect();
			break;
		}
		case UN_TIMER_CHECK:{
			timerCheck();
			break;
		}
		case UN_TIMER_STATUS:{
			timerStatus();
			break;
		}
	}
}

void UNRouting::fromMacConfirmationRegistry(SDNRoutingPacket * netPacket){
	
    newTrace(std::stringstream() << "UNRouting::fromMacConfirmationRegistry");

	cancelTimer(UN_TIMER_CONNECT);

	// IGNORED AS THERE IS NO NEED -> NO MOBILITY
	// setTimer(UN_TIMER_STATUS, timeOutStatus);
	// setTimer(UN_TIMER_CHECK, timeOutCheck);

	connected = true;
}

void UNRouting::fromMacKeepAlive(SDNRoutingPacket * netPacket){

	// IGNORED AS THERE IS NO NEED -> NO MOBILITY
	// cancelTimer(UN_TIMER_CHECK);
	// setTimer(UN_TIMER_CHECK, timeOutCheck);

	sendStatus = false;
	sendPacket(SDN_RESPONSE_KEEP_ALIVE);
}

void UNRouting::fromMacData(SDNRoutingPacket * netPacket, cPacket *pkt){
	// IGNORED AS THERE IS NO NEED -> NO MOBILITY
	// cancelTimer(UN_TIMER_CHECK);
	// setTimer(UN_TIMER_CHECK, timeOutCheck);
    // cout << "UNRouting::fromMacData --> MessageType = " << netPacket->getMessageType() << endl;
	if (netPacket->getMessageType() == MESSAGE_TYPE_VIDEO){
		processMessageTypeVideo(pkt);
	}
}

void UNRouting::fromMacResponseClose(SDNRoutingPacket * netPacket){
	connected = false;
}

void UNRouting::timerTrace(){

	newOutput(getOutputLocation(getCurrentLocation()));
	newTrace(getTraceLocation(getCurrentLocation()));

    setTimer(UN_TIMER_TRACE, timeOutTrace);
}

void UNRouting::timerConnect(){
    newTrace(std::stringstream() << "UNRouting::timerConnect");
	if (connected){
		// connected = false;
		sendPacket(SDN_REFRESH);
		setTimer(UN_TIMER_CONNECT, timeOutRefresh);
	} else {
		sendPacket(SDN_HELLO);
		setTimer(UN_TIMER_CONNECT, timeOutHello);
	}
}

void UNRouting::timerCheck(){
	setTimer(UN_TIMER_CONNECT, 0);
}

void UNRouting::timerStatus(){
	sendStatus = true;
	setTimer(UN_TIMER_STATUS, timeOutStatus);
}

void UNRouting::sendPacket(int SDNRoutingPacketKind){
	SDNRoutingPacket *msg = new SDNRoutingPacket("Node is sending an adv msg", NETWORK_LAYER_PACKET);
	// TO DO: Verificar o tamanho do pacote dependendo do tipo.
	// cout << "User Node: create Packet" << endl;
	addPacketHeader(SDNRoutingPacketKind, msg);
	// cout << "User Node: add header Packet" << endl;
	msg->setDestinationAddress(BROADCAST_NETWORK_ADDRESS);
	// cout << "User Node: set destination Packet" << endl;
    switch (SDNRoutingPacketKind) {
        case SDN_RESPONSE_KEEP_ALIVE:
        case SDN_REFRESH:
        case SDN_HELLO:{
            // cout << "User Node: if send packet" << endl;
            SDNUserStatus msgUserStatus;
            msgUserStatus.status.location = getCurrentLocation();
            msg->setMsgUserStatus(msgUserStatus);
            break;
        }
    }
    sendPacket(msg);
}

void UNRouting::sendPacket(SDNRoutingPacket * netPacket){
	newOutput(getOutputPacket(OUTPUT_PACKET_SENT, netPacket));
	newTrace(getTracePacket(OUTPUT_PACKET_SENT, netPacket));
	toMacLayer(netPacket, BROADCAST_MAC_ADDRESS);
}

void UNRouting::addPacketHeader(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket) {
	netPacket->setByteLength(netDataFrameOverhead);
	netPacket->setTime(SIMTIME_DBL(simTime()));
	netPacket->setIdentifier(identifier++);
	netPacket->setSourceAddress(SELF_NETWORK_ADDRESS);
	netPacket->setTimeToLive(timeToLive);
	netPacket->setSDNRoutingPacketKind(SDNRoutingPacketKind);
	netPacket->setNextHopAddress(BROADCAST_NETWORK_ADDRESS);
}

double UNRouting::getRandomDelay(){
	int tmp = rand()%100;
	double tmpTime = tmp / 100.0;
	return 0.15 + tmpTime;
}

location_s UNRouting::getCurrentLocation(){
	NodeLocation location;
	location = mobilityModule->getLocation();
	location_s location2;
	location2.x = location.x;
	location2.y = location.y;
	return location2;
}

double UNRouting::getResidualEnergy(){
	return resMgrModule->getRemainingEnergy();
}

void UNRouting::processMessageTypeVideo(cPacket *pkt){
	SDNRoutingPacket *netPacket = dynamic_cast <SDNRoutingPacket*>(pkt);

	bool notDuplicate = isNotDuplicatePacket(pkt);
	if (notDuplicate){
		SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(pkt);
		SDNRoutingPacket *netPkt = packet->dup();
		ApplicationPacket *appPkt = check_and_cast <ApplicationPacket*>(decapsulatePacket(netPkt));
		int index = -1;
		
		//cout << "Init idVideo: " << netPacket->getIdVideo() << endl;
		//cout << "Init idTransmission: " << netPacket->getIdTransmission() << endl;
		//cout << "Size: " << route.size() << endl;
		//cout << "Source: " << netPacket->getSourceAddress() << endl;
		//cout << "Destination: " << netPacket->getDestinationAddress() << endl;
		//cout << "SequenceNumber: " << netPacket->getSequenceNumber() << endl;

		for (int i = 0; i<route.size(); i++){
			if (	(route[i].source == atoi(netPacket->getSourceAddress())) && 
					(route[i].destination == atoi(netPacket->getDestinationAddress())) &&
					(route[i].idTransmission == netPacket->getIdTransmission())){
				//cout << "Entrou no if Já existe no buffer" << endl;
				index = i;
				break;
			}
		}
		//cout << "Linha 1" << endl;
		if (index == -1){
		    //cout << "Criando transmissão no buffer" << endl;
			routeCInfo table;
			table.source = atoi(netPacket->getSourceAddress());
			table.destination = atoi(netPacket->getDestinationAddress());
			//cout << "idTransmission: " << netPacket->getIdVideo() << endl;
			table.idTransmission = netPacket->getIdTransmission();
			table.lastReceivedPacket = -1;
			route.push_back(table);
			index = route.size() - 1;
		}
		//cout << "Linha 2" << endl;
		double delay = (simTime().dbl() - appPkt->getAppNetInfoExchange().timestamp.dbl())*1000;
		//cout << "Linha 3" << endl;
		if (index != -1){
		    //cout << "Linha 4" << endl;
			if((netPacket->getSequenceNumber() == 0 && route[index].lastReceivedPacket == -1) || netPacket->getSequenceNumber() == route[index].lastReceivedPacket + 1 || route[index].lastReceivedPacket == -1){
				if (delay > macModule->getDelayMax()){ //drop due to packet deadline
				    //cout << "Linha 5" << endl;
				    trace() << "Drop packet - Delay (" << delay << ")higher than the playout deadline(" << macModule->getDelayMax() << ")";
					route[index].lastReceivedPacket = netPacket->getSequenceNumber();
					//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationLinGO()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  XLINGOTC_STATISTICS_DROP_DELAY);
				} else { //send packet to application layer
				    //cout << "Linha 6" << endl;
					trace() << "receive a subsequent pkt number " << netPacket->getSequenceNumber() << " and send it to application";
					toApplicationLayer(decapsulatePacket(pkt));
					route[index].lastReceivedPacket = netPacket->getSequenceNumber();
					//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationAddress()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  SDUAVNet_STATISTICS_RECEIVED_PACKET);
					if (buffer.size() > 0 && searchBuffer(atoi(netPacket->getSourceAddress()), netPacket->getIdTransmission())){
					    //cout << "Linha 7" << endl;
					    sort(buffer.begin(), buffer.end(), SDUAVNet_sort_buffer);
					    //cout << "Linha 8" << endl;
						bool tmp = true;
						while(buffer.size() > 0 && searchBuffer(atoi(netPacket->getSourceAddress()), netPacket->getIdTransmission()) && tmp){
							tmp = false;
							//cout << "Linha 9" << endl;
							for (int i=0; i<buffer.size(); i++){
								SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(buffer[i].bufferPkt.front());
								if (atoi(packet->getSourceAddress()) == atoi(netPacket->getSourceAddress()) && buffer[i].seqNumber == route[index].lastReceivedPacket + 1){
									trace() << "Send pkt number " << buffer[i].seqNumber << " to application";
									toApplicationLayer(decapsulatePacket(buffer[i].bufferPkt.front()));
									route[index].lastReceivedPacket = buffer[i].seqNumber;
									buffer.erase(buffer.begin()+i);
									tmp = true;
									break;
								}
							}
						}
					}
				}
			}else {
			    //cout << "Linha 10" << endl;
				if (route[index].lastReceivedPacket < netPacket->getSequenceNumber()){
				    //cout << "Linha 11" << endl;
					if ( delay > macModule->getDelayMax()){ //drop due to packet deadline
						trace() << "Drop packet - Delay (" << delay << ")higher than the playout deadline(" << macModule->getDelayMax() << ")";
						//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationLinGO()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  XLINGOTC_STATISTICS_DROP_DELAY);
					} else { //buffer the packet
					    //cout << "Linha 12" << endl;
						pktList temp;
						//cout << "Linha 12.1" << endl;
						temp.seqNumber = netPacket->getSequenceNumber();
						//cout << "Linha 12.2" << endl;
						temp.bufferPkt.push(netPacket->dup());
						//cout << "Linha 12.3" << endl;
						buffer.push_back(temp);
						//cout << "Linha 12.4" << endl;
						//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationAddress()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  SDUAVNet_STATISTICS_RECEIVED_PACKET);
						setMultipleTimer(UN_TIMER_CLEAN_BUFFER, netPacket->getIdTransmission(), 1);
						//cout << "Linha 12.5" << endl;
						trace() << "Buffered pkt number " << netPacket->getSequenceNumber() << " buffer state: " << buffer.size() << "/" << netBufferSize;
					}
				}  else{
				    //cout << "Linha 13" << endl;
					trace() << "Drop packet number " << netPacket->getSequenceNumber() << ", due to it is old. Last Packet sent to App Layer";
					trace() << "last rec pkt " << route[index].lastReceivedPacket << " for source " << route[index].source;
					//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationLinGO()), netPacket->getIdVideo(), 0, 0, XLINGOTC_STATISTICS_DROP_NTW);
				}
				if (buffer.size() == netBufferSize){ //send buffered packets to application layer
				    //cout << "Linha 14" << endl;
					sort(buffer.begin(), buffer.end(), SDUAVNet_sort_buffer);
					while(buffer.size() > (netBufferSize/2.0)){
						SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(buffer[0].bufferPkt.front());
						trace() << "Send pkt number " << buffer[0].seqNumber << " to application from source " << packet->getSourceAddress();
						//int tmpIndex = getNextHopIndex(packet->getSourceAddress(), netPacket->getDestinationAddress());
						int tmpIndex = index;
						toApplicationLayer(decapsulatePacket(buffer[0].bufferPkt.front()));
						route[tmpIndex].lastReceivedPacket = buffer[0].seqNumber;
						buffer.erase(buffer.begin()+0);
					}
					//cout << "Linha 15" << endl;
					bool tmp = true;
					while(buffer.size() > 0 && searchBuffer(atoi(netPacket->getSourceAddress()), netPacket->getIdTransmission()) && tmp){
						tmp = false;
						for (int i=0; i<buffer.size(); i++){
							SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(buffer[i].bufferPkt.front());
							if (atoi(packet->getSourceAddress()) == atoi(netPacket->getSourceAddress()) && buffer[i].seqNumber == route[index].lastReceivedPacket + 1){
								trace() << "Send pkt number " << buffer[i].seqNumber << " to application from source " << packet->getSourceAddress();
								toApplicationLayer(decapsulatePacket(buffer[i].bufferPkt.front()));
								//int tmpIndex = getNextHopIndex(packet->getSourceAddress(), netPacket->getDestinationAddress());
								int tmpIndex = index;
								route[tmpIndex].lastReceivedPacket = buffer[i].seqNumber;
								buffer.erase(buffer.begin()+i);
								tmp = true;
								break;
							}
						}
					}
					//cout << "Linha 16" << endl;
					if (buffer.size() == 0)
						cancelMultipleTimer(UN_TIMER_CLEAN_BUFFER, netPacket->getIdTransmission());
				}
				//cout << "Linha 17" << endl;
			}
			//cout << "Linha 18" << endl;
		}

	} else if (!notDuplicate){
		///statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationLinGO()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  XLINGOTC_STATISTICS_DUPLICATED_PACKET);
		trace() << "DN received a duplicated packet";
	}
	//cout << "Linha 20" << endl;
	trace() << "Routing buffer state: " << buffer.size() << "/" << netBufferSize;
	trace() << "MAC control buffer state: " << macModule->getControlBufferSize() << "/" << macModule->macBufferSize;
	trace() << "MAC buffer state: " << macModule->getBufferSize() << "/" << macModule->macBufferSize << "\n";

}

// void UNRouting::processMessageTypeVideo(cPacket *pkt){
// 	toApplicationLayer(decapsulatePacket(pkt));
// }

// void UNRouting::processMessageTypeVideo(cPacket *pkt){
//     cout << "UNRouting::processMessageTypeVideo" << endl;
//     SDNRoutingPacket *netPacket = dynamic_cast <SDNRoutingPacket*>(pkt);
// 	//trace() << "DN received multimedia pkt number " << netPacket->getSequenceNumber() << " from SN_" << netPacket->getSourceAddress() << " to DN_" << netPacket->getDestinationAddress() << "routed by F_" << netPacket->getNextHop() << " with " << netPacket->getHopCount() + 1 << " hops";
//     cout << "begin:isNotDuplicatePacket" << endl;
// 	bool notDuplicate = isNotDuplicatePacket(pkt);
// 	cout << "end:isNotDuplicatePacket" << endl;
// 	if (notDuplicate){
// 	    cout << "begin:notDuplicate" << endl;
// 	    SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(pkt);
// 	    SDNRoutingPacket *netPkt = packet->dup();
// 		ApplicationPacket *appPkt = check_and_cast <ApplicationPacket*>(decapsulatePacket(netPkt));
// 		cout << "begin:buffer_i" << endl;
// 		buffer_i buffer_it = buffer.find(atoi(netPacket->getSourceAddress()));
// 		cout << "end:buffer_i" << endl;
// 		if (buffer_it == buffer.end()) {
// 		    cout << "begin:if buffer_it" << endl;
// 			transmission_t transmission;
// 			buffer.insert(pair<int, transmission_t> (atoi(netPacket->getSourceAddress()), transmission));
// 			buffer_it = buffer.find(atoi(netPacket->getSourceAddress()));
// 			cout << "end:if buffer_it" << endl;
// 		}
// 		cout << "begin:transmission_i" << endl;
// 		transmission_i transmission_it = buffer_it->second.find(netPacket->getIdVideo());
// 		cout << "end:transmission_i" << endl;
// 		if (transmission_it == buffer_it->second.end()) {
// 		    cout << "begin:if transmission_it" << endl;
// 			transmission_s transmission;
// 			transmission.id = idTransmission++;
// 			cout << "1:if transmission_it" << endl;
// 			buffer_it->second.insert(pair<int, transmission_s> (netPacket->getIdVideo(), transmission));
// 			transmission_it = buffer_it->second.find(netPacket->getIdVideo());
// 			cout << "end:if transmission_it" << endl;
// 		}

// 		double delay = (simTime().dbl() - appPkt->getAppNetInfoExchange().timestamp.dbl())*1000;
// 			if((netPacket->getSequenceNumber() == 0 && transmission_it->second.lastReceivedPacket == -1) || 
// 				netPacket->getSequenceNumber() == transmission_it->second.lastReceivedPacket + 1 || 
// 				transmission_it->second.lastReceivedPacket == -1){
// 				if (delay > macModule->getDelayMax()){ //drop due to packet deadline
// 					trace() << "Drop packet - Delay (" << delay << ")higher than the playout deadline(" << macModule->getDelayMax() << ")";
// 					transmission_it->second.lastReceivedPacket = netPacket->getSequenceNumber();
// 					//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationLinGO()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  XLINGOTC_STATISTICS_DROP_DELAY);
// 				} else { //send packet to application layer
// 					trace() << "receive a subsequent pkt number " << netPacket->getSequenceNumber() << " and send it to application";
// 					toApplicationLayer(decapsulatePacket(pkt));
// 					transmission_it->second.lastReceivedPacket = netPacket->getSequenceNumber();
// 					//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationAddress()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  SDUAVNet_STATISTICS_RECEIVED_PACKET);
// 					if (transmission_it->second.buffer.size() > 0){
// 						sort(transmission_it->second.buffer.begin(), transmission_it->second.buffer.end(), SDUAVNet_sort_buffer);
// 						bool tmp = true;
// 						while((transmission_it->second.buffer.size() > 0) && tmp){
// 							tmp = false;
// 							for (int i=0; i<transmission_it->second.buffer.size(); i++){
// 								SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(transmission_it->second.buffer[i].bufferPkt.front());
// 								if (atoi(packet->getSourceAddress()) == atoi(netPacket->getSourceAddress()) &&
// 									transmission_it->second.buffer[i].seqNumber == transmission_it->second.lastReceivedPacket + 1){
// 									trace() << "Send pkt number " << transmission_it->second.buffer[i].seqNumber << " to application";
// 									toApplicationLayer(decapsulatePacket(transmission_it->second.buffer[i].bufferPkt.front()));
// 									transmission_it->second.lastReceivedPacket = transmission_it->second.buffer[i].seqNumber;
// 									transmission_it->second.buffer.erase(transmission_it->second.buffer.begin()+i);
// 									tmp = true;
// 									break;
// 								}
// 							}
// 						}
// 					}
// 				}
// 			}else {
// 				if (transmission_it->second.lastReceivedPacket < netPacket->getSequenceNumber()){
// 					if ( delay > macModule->getDelayMax()){ //drop due to packet deadline
// 						trace() << "Drop packet - Delay (" << delay << ")higher than the playout deadline(" << macModule->getDelayMax() << ")";
// 						//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationLinGO()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  XLINGOTC_STATISTICS_DROP_DELAY);
// 					} else { //buffer the packet
// 						pktList temp;
// 						temp.seqNumber = netPacket->getSequenceNumber();
// 						temp.bufferPkt.push(netPacket->dup());
// 						transmission_it->second.buffer.push_back(temp);
// 						//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationAddress()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  SDUAVNet_STATISTICS_RECEIVED_PACKET);

// 						setMultipleTimer(UN_TIMER_CLEAN_BUFFER, transmission_it->second.id, 1);
// 						trace() << "Buffered pkt number " << netPacket->getSequenceNumber() << " buffer state: " << transmission_it->second.buffer.size() << "/" << netBufferSize;
// 					}
// 				}  else{
// 					trace() << "Drop packet number " << netPacket->getSequenceNumber() << ", due to it is old. Last Packet sent to App Layer";
// 					//trace() << "last rec pkt " << transmission_it->second.lastReceivedPacket << " for source " << transmission_it->second.source;
// 					//statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationLinGO()), netPacket->getIdVideo(), 0, 0, XLINGOTC_STATISTICS_DROP_NTW);
// 				}
// 				if (transmission_it->second.buffer.size() == netBufferSize){ //send buffered packets to application layer
// 					sort(transmission_it->second.buffer.begin(), transmission_it->second.buffer.end(), SDUAVNet_sort_buffer);
// 					while(buffer.size() > (netBufferSize/2.0)){
// 						SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(transmission_it->second.buffer[0].bufferPkt.front());
// 						trace() << "Send pkt number " << transmission_it->second.buffer[0].seqNumber << " to application from source " << packet->getSourceAddress();
// 						//int tmpIndex = getNextHopIndex(packet->getSourceAddress(), netPacket->getDestinationAddress());
// 						//int tmpIndex = index;
// 						toApplicationLayer(decapsulatePacket(transmission_it->second.buffer[0].bufferPkt.front()));
// 						transmission_it->second.lastReceivedPacket = transmission_it->second.buffer[0].seqNumber;
// 						transmission_it->second.buffer.erase(transmission_it->second.buffer.begin()+0);
// 					}
// 					bool tmp = true;
// 					while(transmission_it->second.buffer.size() > 0 && tmp){
// 						tmp = false;
// 						for (int i=0; i<transmission_it->second.buffer.size(); i++){
// 							SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(transmission_it->second.buffer[i].bufferPkt.front());
// 							if (atoi(packet->getSourceAddress()) == atoi(netPacket->getSourceAddress()) && transmission_it->second.buffer[i].seqNumber == transmission_it->second.lastReceivedPacket + 1){
// 								trace() << "Send pkt number " << transmission_it->second.buffer[i].seqNumber << " to application from source " << packet->getSourceAddress();
// 								toApplicationLayer(decapsulatePacket(transmission_it->second.buffer[i].bufferPkt.front()));
// 								//int tmpIndex = getNextHopIndex(packet->getSourceAddress(), netPacket->getDestinationAddress());
// 								//int tmpIndex = index;
// 								transmission_it->second.lastReceivedPacket = transmission_it->second.buffer[i].seqNumber;
// 								transmission_it->second.buffer.erase(transmission_it->second.buffer.begin()+i);
// 								tmp = true;
// 								break;
// 							}
// 						}
// 					}
// 					//TO DO: Túlio verificar depois isso daqui, tá ok!!
// 					if (transmission_it->second.buffer.size() == 0)
// 					    cancelMultipleTimer(UN_TIMER_CLEAN_BUFFER, transmission_it->second.id);
// 				}
// 			}

// 	} else if (!notDuplicate){
// 		///statistics(atoi(netPacket->getSourceAddress()), atoi(netPacket->getDestinationLinGO()), netPacket->getIdVideo(), netPacket->getHopCount() + 1, 0,  XLINGOTC_STATISTICS_DUPLICATED_PACKET);
// 		trace() << "DN received a duplicated packet";
// 	}
// 	trace() << "Routing buffer state: " << buffer.size() << "/" << netBufferSize;
// 	trace() << "MAC control buffer state: " << macModule->getControlBufferSize() << "/" << macModule->macBufferSize;
// 	trace() << "MAC buffer state: " << macModule->getBufferSize() << "/" << macModule->macBufferSize << "\n";
// }

void UNRouting::multipleTimerFiredCallback(int index, int id){
	switch (index) {

		case UN_TIMER_CLEAN_BUFFER:{
		    //cout << "id: " << id << endl;
		    //cout << "Iniciou timer" << endl;
			sort(buffer.begin(), buffer.end(), SDUAVNet_sort_buffer);
			//cout << "saiu do sort" << endl;
            bool repeat = true;
			while(repeat){
	            repeat = false;
			    //cout << "while size: " << buffer.size() << endl;
				for (int i=0; i<buffer.size(); i++){
				    //cout << "For: " << i << endl;
					SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(buffer[i].bufferPkt.front());
					//cout << "idTransmission: " << packet->getIdTransmission() << endl;
					//cout << "SequenceNumber: " << packet->getSequenceNumber() << endl;
					if (packet->getIdTransmission() == id){
					    //cout << "entrou no IF" << endl;
						trace() << "Send pkt number " << buffer[i].seqNumber << " to application";
						toApplicationLayer(decapsulatePacket(buffer[i].bufferPkt.front()));
						//cout << "Linha 1" << endl;
						buffer.erase(buffer.begin()+i);
						//cout << "Linha 2" << endl;
						repeat = true;
						break;
					}
				}

			}
			trace() << "SDUAVNet - End: SDUAVNet_CLEAN_BUFFER of Node " << self << " at time = " << simTime() << "\n";
			break;
		}

		// case UN_TIMER_CLEAN_BUFFER:{
		// 	trace() << "SDUAVNet - case: UN_TIMER_CLEAN_BUFFER of Node " << self << " for SN_" << id << " at time = " << simTime();

		// 	transmission_i transmission_it;
		// 	bool found = false;
		// 	for (buffer_i buffer_it = buffer.begin(); buffer_it != buffer.end(); ++buffer_it){
		// 		for (transmission_it = buffer_it->second.begin(); transmission_it != buffer_it->second.end(); ++transmission_it){
		// 			if (transmission_it->second.id == id){
		// 				found = true;
		// 				break;
		// 			}
		// 		}
		// 		if (found) {break;};
		// 	}

		// 	sort(transmission_it->second.buffer.begin(), transmission_it->second.buffer.end(), SDUAVNet_sort_buffer);
		// 	while(transmission_it->second.buffer.size() > 0){
		// 		SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(transmission_it->second.buffer[0].bufferPkt.front());
		// 		trace() << "Send pkt number " << transmission_it->second.buffer[0].seqNumber << " to application";
		// 		toApplicationLayer(decapsulatePacket(transmission_it->second.buffer[0].bufferPkt.front()));
		// 		transmission_it->second.buffer.erase(transmission_it->second.buffer.begin());
		// 	}
		// 	trace() << "SDUAVNet - End: UN_TIMER_CLEAN_BUFFER of Node " << self << " at time = " << simTime() << "\n";
		// 	break;
		// }
	}
}

bool UNRouting::searchBuffer(int source, int idTransmission){
	for (int i=0; i<buffer.size(); i++){
		SDNRoutingPacket *packet = dynamic_cast <SDNRoutingPacket*>(buffer[i].bufferPkt.front());
		if((atoi(packet->getSourceAddress()) == source) && (packet->getIdTransmission() == idTransmission)){
			return true;
		}
	}
	return false;
}

bool SDUAVNet_sort_buffer(pktList a, pktList b){
	return (a.seqNumber < b.seqNumber);
}
