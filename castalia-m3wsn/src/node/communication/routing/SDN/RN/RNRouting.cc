#include "RNRouting.h"

Define_Module(RNRouting);

void RNRouting::startup() {

	// Read Config Parameters
	timeToLive			= par("TIME_TO_LIVE");
	maxRetransmitions 	= par("MAX_RETRANSMITIONS");
	positionRangeUser	= par("POSITION_RANGE_USER").doubleValue();
	intervalCounter		= par("INTERVAL_COUNTER").doubleValue();
	timeOutTrace 		= par("TIMEOUT_TRACE").doubleValue();
	timeOutKeepAlive 	= par("TIMEOUT_KEEP_ALIVE").doubleValue();
	timeOutPacketIn 	= par("TIMEOUT_PACKET_IN").doubleValue();
	timeOutResponse		= par("TIMEOUT_RESPONSE").doubleValue();
	timeOutNotifyUser 	= par("TIMEOUT_NOTIFY_USER").doubleValue();
	timeOutStatus 		= par("TIMEOUT_STATUS").doubleValue();
	timeOutTelemetry 	= par("TIMEOUT_TELEMETRY").doubleValue();

	// Who am I
	self = getParentModule()->getParentModule()->getIndex();

	// Load Modules
	wirelessModule = check_and_cast <WirelessChannel*>(getParentModule()->getParentModule()->getParentModule()->getSubmodule("wirelessChannel"));
	mobilityModule = check_and_cast <VirtualMobilityManager*>(getParentModule()->getParentModule()->getSubmodule("MobilityManager"));
	radioModule = check_and_cast <Radio*>(getParentModule()->getSubmodule("Radio"));
	macModule = check_and_cast <VirtualMac*>(getParentModule()->getSubmodule("MAC"));	
	srceRerouceModuce = check_and_cast <ResourceManager*>(getParentModule()->getParentModule()->getParentModule()->getSubmodule("node",1)->getSubmodule("ResourceManager"));

	connection = Connection();
	connection.start(self);
	fromApplication();

	// Set Timers
	// setTimer(RN_TIMER_TRACE, 0);

	// IGNORED AS THERE IS NO NEED -> NO MOBILITY
	//setTimer(RN_TIMER_STATUS, timeOutStatus);
	//setTimer(RN_TIMER_KEEP_ALIVE, intervalCounter);
	//setTimer(RN_TIMER_TELEMETRY, 0);

    newTrace("RNRouting::started up!");
}

void RNRouting::finish() {
	cOwnedObject *Del=NULL;
	int OwnedSize = this->defaultListSize();
	for(int i=0; i<OwnedSize; ++i){
		Del = this->defaultListGet(0);
		this->drop(Del);
		delete Del;
	}
}

void RNRouting::fromMacLayer(cPacket *pkt, int srcMacAddress, double RSSI, double LQI){
	switch (pkt->getKind()) {
		case NETWORK_LAYER_PACKET:{
			SDNRoutingPacket *netPacket = dynamic_cast <SDNRoutingPacket*>(pkt);
			if (!netPacket)
				return;
			if (self == atoi(netPacket->getDestinationAddress())) {

                newOutput(getOutputPacket(OUTPUT_PACKET_RECEIVED, netPacket));
                newTrace(getTracePacket(OUTPUT_PACKET_RECEIVED, netPacket));

                switch (netPacket->getSDNRoutingPacketKind()) {
					case SDN_UPDATE_USER:{
						fromMacUpdateUser(netPacket);
						break;
					}
					case SDN_UPDATE_ROUTING_TABLE:{
						fromMacUpdateRoutingTable(netPacket);
						break;
					}
					case SDN_UPDATE_AREA:{
						fromMacUpdateArea(netPacket);
						break;
					}
					case SDN_ACK:{
						fromMacAck(netPacket);
						break;
					}
				}
			} else if (atoi(BROADCAST_NETWORK_ADDRESS) == atoi(netPacket->getNextHopAddress())) {

                newOutput(getOutputPacket(OUTPUT_PACKET_RECEIVED, netPacket));
                newTrace(getTracePacket(OUTPUT_PACKET_RECEIVED, netPacket));

				switch (netPacket->getSDNRoutingPacketKind()) {
					case SDN_HELLO:{
						fromMacHello(netPacket);
						break;
					}
					case SDN_REFRESH:{
						fromMacRefresh(netPacket);
						break;
					}
				}

				if (users.find(atoi(netPacket->getSourceAddress())) != users.end()) {
					switch (netPacket->getSDNRoutingPacketKind()) {
						case SDN_RESPONSE_KEEP_ALIVE:{
							readUserStatus(netPacket);
							break;
						}
						case SDN_CLOSE:{
							fromMacClose(netPacket);
							break;
						}
						case SDN_DATA:{
							fromMacData(netPacket, RSSI);
							break;
						}
					}
				}

			} else if (self == atoi(netPacket->getNextHopAddress())) {
		        newOutput(getOutputPacket(OUTPUT_PACKET_RECEIVED_TO_FOWARD, netPacket));
                parserTelemetry(netPacket, RSSI);
                deparserTelemetry(netPacket);
				sendPacket(netPacket);
			}
			break;
		}
	}
}

void RNRouting::fromApplicationLayer(cPacket *, const char *){

}

void RNRouting::fromApplication(){

    newTrace("RNRouting::fromApplication");

    char buffer[CONST_MAX_BUFFER] = {0};
    int si = connection.receive(buffer);
    contractOut_t contractOut;
    contractOut.ParseFromArray(buffer, si);

    for (int i = 0; i < contractOut.updateroutingtable_size(); i++) {
        fromApplicationUpdateRoutingTable(contractOut.updateroutingtable(i));
    }

    for (int i = 0; i < contractOut.updateuser_size(); i++) {
        fromApplicationUpdateUser(contractOut.updateuser(i));
    }
}

void RNRouting::fromApplicationUpdateRoutingTable(const contract_out::ContractOut_UpdateRoutingTable & UpdateRoutingTable){

	newTrace(std::stringstream() 	<< "RNRouting::fromApplicationUpdateRoutingTable -> "
									<< "nodeId: " 		<< UpdateRoutingTable.relayid() << ", " 
									<< "destination: " 	<< UpdateRoutingTable.route().destination() << ", " 
									<< "nextHop: " 		<< UpdateRoutingTable.route().nexthop() << ", "
									<< "drop: " 		<< UpdateRoutingTable.route().drop() << ", " 
									<< "action: " 		<< UpdateRoutingTable.action());

	route_s route;
	route.destination = UpdateRoutingTable.route().destination();
    route.nextHopAddress = to_string(UpdateRoutingTable.route().nexthop()).c_str();
    route.drop = UpdateRoutingTable.route().drop();

    addRoute(route);
}

void RNRouting::fromApplicationUpdateUser(const contract_out::ContractOut_UpdateUser & UpdateUser){

	newTrace(std::stringstream() 	<< "RNRouting::fromApplicationUser -> "
									<< "relayId: " 		<< UpdateUser.relayid() << ", " 
									<< "userId: " 		<< UpdateUser.userid() << ", " 
									<< "action: " 		<< UpdateUser.action() << ", "
									<< "event: " 		<< UpdateUser.event());

	user_s user;
	user.id = UpdateUser.userid();
	user.status.null = true;
	user.counter = (int) timeOutKeepAlive/intervalCounter;

	addUser(user);
}

void RNRouting::timerFiredCallback(int index){
	switch (index) {
		case RN_TIMER_TRACE:{
			timerTrace();
			break;
		}
		case RN_TIMER_KEEP_ALIVE:{
			timerKeepAlive();
			break;
		}
		case RN_TIMER_PACKETS:{
			timerPackets();
			break;
		}
        case RN_TIMER_TELEMETRY:{
            timerTelemetry();
            break;
        }
	}
}

void RNRouting::fromMacData(SDNRoutingPacket * netPacket, double RSSI){
	newTrace("RNRouting::fromMacData");
	if (netPacket->getHasStatus()) {
		readUserStatus(netPacket);
	}
	if (sendTelemetry){
		addTelemetry(netPacket);
		addTelemetryByHop(netPacket, RSSI);
		sendTelemetry = false;
		setTimer(RN_TIMER_TELEMETRY, timeOutTelemetry);
	}
	sendPacket(netPacket);
}

void RNRouting::readUserStatus(SDNRoutingPacket * netPacket){
	users_i it = users.find(atoi(netPacket->getSourceAddress()));
	it->second.counter = (int) timeOutKeepAlive/intervalCounter;
	if (it->second.status.null){
        it->second.status.location = netPacket->getMsgUserStatus().status.location;
        it->second.status.null = false;
	}
	if (isOutOfRange(getDistance(it->second.status.location, netPacket->getMsgUserStatus().status.location))){
	    it->second.status.location = netPacket->getMsgUserStatus().status.location;
		sendPacket(SDN_NOTIFY_STATUS_USER, netPacket);
	}
}

void RNRouting::fromMacHello(SDNRoutingPacket * netPacket){

	newTrace(std::stringstream() 	<< "RNRouting::fromMacHello -> "
									<< "userId: " << netPacket->getSourceAddress());

	if (users.find(atoi(netPacket->getSourceAddress())) != users.end()) {
		sendPacket(SDN_CONFIRMATION_REGISTRY, netPacket);
	} else {
		sendPacket(SDN_REGISTER, netPacket);
	}
}

void RNRouting::fromMacClose(SDNRoutingPacket * netPacket){
	sendPacket(SDN_UNREGISTER, netPacket);
}

void RNRouting::fromMacRefresh(SDNRoutingPacket * netPacket){
	sendPacket(SDN_REFRESH_REGISTER, netPacket);
}

void RNRouting::fromMacUpdateUser(SDNRoutingPacket * netPacket){

	newTrace(std::stringstream() 	<< "RNRouting::fromMacUpdateUser -> "
									<< "userId: " 	<< netPacket->getMsgUpdateUser().userId << ", "
									<< "action: " 	<< netPacket->getMsgUpdateUser().action << ", "
									<< "event: " 	<< netPacket->getMsgUpdateUser().event);

	sendPacket(SDN_ACK, netPacket);

	if (netPacket->getMsgUpdateUser().action == SDN_ADD){
		user_s user;
		user.id = netPacket->getMsgUpdateUser().userId;
		user.status.null = true;
		user.counter = (int) timeOutKeepAlive/intervalCounter;
		addUser(user);
		route_s route;
		route.destination = netPacket->getMsgUpdateUser().userId;
	    route.nextHopAddress = to_string(netPacket->getMsgUpdateUser().userId);
	    route.drop = false;
		addRoute(route);
		if (netPacket->getMsgUpdateUser().event == SDN_REQUIRED) {
			sendPacket(SDN_CONFIRMATION_REGISTRY, netPacket);
		}
	} else {
		users.erase(netPacket->getMsgUpdateUser().userId);
		if (netPacket->getMsgUpdateUser().event == SDN_REQUIRED) {
			sendPacket(SDN_RESPONSE_CLOSE, netPacket);
		}
	}
}

void RNRouting::fromMacUpdateRoutingTable(SDNRoutingPacket * netPacket){

	newTrace(std::stringstream() 	<< "RNRouting::fromMacUpdateRoutingTable -> "
									<< "nextHop: " 	<< netPacket->getMsgUpdateRoutingTable().route.nextHopAddress << ", "
									<< "dest: " 	<< netPacket->getMsgUpdateRoutingTable().route.destination << ", "
									<< "action: " 	<< netPacket->getMsgUpdateRoutingTable().action);
    
	sendPacket(SDN_ACK, netPacket);

	if ((netPacket->getMsgUpdateRoutingTable().action == SDN_DELETE) || 
		(netPacket->getMsgUpdateRoutingTable().action == SDN_UPDATE)) { 
		routingTable.erase(netPacket->getMsgUpdateRoutingTable().route.destination);
	}
	if ((netPacket->getMsgUpdateRoutingTable().action == SDN_ADD) || 
		(netPacket->getMsgUpdateRoutingTable().action == SDN_UPDATE)) { 
		addRoute(netPacket->getMsgUpdateRoutingTable().route);
	}

	queue< cPacket* > bufferTemp;
	SDNRoutingPacket* currPkt;
	while (!TXBuffer.empty()) {
		currPkt = dynamic_cast <SDNRoutingPacket*>(TXBuffer.front());
		if(currPkt && atoi(currPkt->getDestinationAddress()) == netPacket->getMsgUpdateRoutingTable().route.destination) {
			sendPacket(currPkt);
		} else {
			bufferTemp.push(TXBuffer.front());
		}
		TXBuffer.pop();
	}
	while (!bufferTemp.empty()) {
		TXBuffer.push(bufferTemp.front());
		bufferTemp.pop();
	}
}

void RNRouting::fromMacUpdateArea(SDNRoutingPacket * netPacket) {

	newTrace(std::stringstream() 	<< "RNRouting::fromMacUpdateArea -> "
									<< "x: " << area.center.x << ", "
									<< "y: " << area.center.y);

	sendPacket(SDN_ACK, netPacket);

	area = netPacket->getMsgUpdateArea().area;
	ApplicationPacket *topology = new ApplicationPacket("setup", MOBILE_MESSAGE);
	topology->setX(area.center.x);
	topology->setY(area.center.y);
	toApplicationLayer(topology);
}

void RNRouting::fromMacAck(SDNRoutingPacket * netPacket){

    newTrace(std::stringstream() 	<< "RNRouting::fromMacAck -> " 
    								<< "identifier: " << netPacket->getMsgAck().identifier);

	for (sentPackets_i it = sentPackets.begin(); it != sentPackets.end();) {
		if (it->packet->getIdentifier() == netPacket->getMsgAck().identifier) {
            it = sentPackets.erase(it);
        } else {
            ++it;
        }
	}
	resetRemainingTimeSentPackets();
	resetTimerPackets();
}

void RNRouting::timerTrace(){

	// Location
	// newOutput(getOutputLocation(getCurrentLocation()));
	// newTrace(getTraceLocation(getCurrentLocation()));

    newTrace(getTraceTelemetry(macModule->getBufferSize(), (int)TXBuffer.size(), macModule->getPacketDrops(), netPacketDrops));
    newOutput(getOutputTelemetry(macModule->getBufferSize(), (int)TXBuffer.size(), macModule->getPacketDrops(), netPacketDrops));

	setTimer(RN_TIMER_TRACE, timeOutTrace);
}

void RNRouting::timerStatus(){
	sendStatus = true;
	setTimer(RN_TIMER_STATUS, timeOutStatus);
}

void RNRouting::timerKeepAlive(){
	for (users_i it = users.begin(); it != users.end(); ++it) {
		it->second.counter--;
		if (it->second.counter == 0){
			sendPacket(SDN_KEEP_ALIVE, it->second.id);
		}
		if (it->second.counter == -(int)timeOutResponse/intervalCounter) {
			sendPacket(SDN_NOTIFY_STATUS_USER, it->second.id);
			sendPacket(SDN_UNREGISTER, it->second.id);
		}
	}
	cancelTimer(RN_TIMER_KEEP_ALIVE);
	setTimer(RN_TIMER_KEEP_ALIVE, intervalCounter);
}

void RNRouting::timerPackets(){
	newTrace("RNRouting::timerPackets");
	SDNRoutingPacket *netPacket = sentPackets.begin()->packet;
	sendPacket(netPacket);
	resetRemainingTimeSentPackets();
	sentPackets.begin()->remainingTime = sentPackets.begin()->timeOut;
	sentPackets.begin()->retransmitions--;
    if (sentPackets.begin()->retransmitions < 0){
    	sentPackets.erase(sentPackets.begin());
    }
    resetTimerPackets();
}

void RNRouting::timerTelemetry(){
	sendTelemetry = true;
	setTimer(RN_TIMER_TELEMETRY, timeOutTelemetry);
}

void RNRouting::resetTimerPackets(){
	cancelTimer(RN_TIMER_PACKETS);
	if (sentPackets.size() > 0) {
		sort(sentPackets.begin(), sentPackets.end(), sort_sentPacket);
		lastTimeUpdateSentPackets = SIMTIME_DBL(simTime());
		setTimer(RN_TIMER_PACKETS, sentPackets.begin()->remainingTime);
	}
}

void RNRouting::addSentPacket(SDNRoutingPacket * netPacket, double timeOut) {
    resetRemainingTimeSentPackets();
	sentPacket_s sentPacket;
	sentPacket.packet = netPacket->dup();
	sentPacket.timeOut = timeOut;
	sentPacket.remainingTime = timeOut;
	sentPacket.retransmitions = maxRetransmitions;
	sentPackets.push_back(sentPacket);
	resetTimerPackets();
}

void RNRouting::resetRemainingTimeSentPackets(){
	double diff = SIMTIME_DBL(simTime()) - lastTimeUpdateSentPackets;
	for (int i = 0; i < sentPackets.size(); ++i) {
		sentPackets[i].remainingTime -= diff;
		if (sentPackets[i].remainingTime < 0){
		 	sentPackets[i].remainingTime = 0;
		 }
	}
}

SDNRoutingPacket * RNRouting::createPacket(int SDNRoutingPacketKind, int userId){
	SDNRoutingPacket *msg = new SDNRoutingPacket("Node is sending an adv msg", NETWORK_LAYER_PACKET);
	addPacketHeader(SDNRoutingPacketKind, msg);
	switch (SDNRoutingPacketKind) {
		case SDN_KEEP_ALIVE: {
			msg->setDestinationAddress(to_string(userId).c_str());
			break;
		}
		case SDN_NOTIFY_STATUS_USER: {
			msg->setDestinationAddress(CONTROLLER_NETWORK_ADDRESS);
			SDNNotifyStatusUser msgNotifyStatusUser;
			users_i it = users.find(userId);
			msgNotifyStatusUser.status = it->second.status;
			msg->setMsgNotifyStatusUser(msgNotifyStatusUser);
			break;
		}
		case SDN_UNREGISTER: {
			msg->setDestinationAddress(CONTROLLER_NETWORK_ADDRESS);
			SDNUnregister msgUnregister;
			msgUnregister.userId 	= userId;
			msgUnregister.event = SDN_FORCED;
			msg->setMsgUnregister(msgUnregister);
			break;
		}
	}
	return msg;
}

SDNRoutingPacket * RNRouting::createPacket(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket){
	SDNRoutingPacket *msg = new SDNRoutingPacket("Node is sending an adv msg", NETWORK_LAYER_PACKET);
	addPacketHeader(SDNRoutingPacketKind, msg);
	// TO DO: Verificar o tamanho do pacote dependendo do tipo.
	switch (SDNRoutingPacketKind) {
		case SDN_PACKET_IN: {
			msg->setDestinationAddress(CONTROLLER_NETWORK_ADDRESS);
			msg->setTrace(netPacket->getTrace());
			SDNPacketIn msgPacketIn;
			msgPacketIn.userId = atoi(netPacket->getDestinationAddress());
			msg->setMsgPacketIn(msgPacketIn);
			addSentPacket(msg, timeOutPacketIn);
			break;
		}
		case SDN_NOTIFY_STATUS_USER: {
			msg->setDestinationAddress(CONTROLLER_NETWORK_ADDRESS);
			SDNNotifyStatusUser msgNotifyStatusUser;
			msgNotifyStatusUser.userId = atoi(netPacket->getSourceAddress());
			msgNotifyStatusUser.status = netPacket->getMsgUserStatus().status;
			msg->setMsgNotifyStatusUser(msgNotifyStatusUser);
			addSentPacket(msg, timeOutNotifyUser);
			break;
		}
		case SDN_CONFIRMATION_REGISTRY: {
			if (netPacket->getSDNRoutingPacketKind() == SDN_UPDATE_USER) {
				msg->setDestinationAddress(to_string(netPacket->getMsgUpdateUser().userId).c_str());
			} else {
				msg->setDestinationAddress(netPacket->getSourceAddress());
				msg->setNextHopAddress(netPacket->getSourceAddress());
			}
			break;
		}
		case SDN_REGISTER:
		case SDN_REFRESH_REGISTER: {
			msg->setDestinationAddress(CONTROLLER_NETWORK_ADDRESS);
			SDNRegister msgRegister;
			msgRegister.userId 	= atoi(netPacket->getSourceAddress());
			msgRegister.status 	= netPacket->getMsgUserStatus().status;
			msgRegister.load 	= load;
			msg->setMsgRegister(msgRegister);
			break;
		}
		case SDN_UNREGISTER: {
			msg->setDestinationAddress(CONTROLLER_NETWORK_ADDRESS);
			SDNUnregister msgUnregister;
			msgUnregister.userId = atoi(netPacket->getSourceAddress());
			msgUnregister.event = SDN_REQUIRED;
			msg->setMsgUnregister(msgUnregister);
			break;
		}
		case SDN_RESPONSE_CLOSE: {
			msg->setDestinationAddress(to_string(netPacket->getMsgUpdateUser().userId).c_str());
			break;
		}
		case SDN_ACK: {
			msg->setDestinationAddress(CONTROLLER_NETWORK_ADDRESS);
			msg->setTrace(netPacket->getTrace());
			SDNAck msgAck;
			msgAck.identifier = netPacket->getIdentifier();
			msg->setMsgAck(msgAck);
			break;
		}
		case SDN_TELEMETRY: {
			msg->setDestinationAddress(CONTROLLER_NETWORK_ADDRESS);
            SDNTelemetry msgTelemetry;
            msgTelemetry = netPacket->getMsgTelemetry();
			msg->setMsgTelemetry(msgTelemetry);
			break;
		}
	}
	return msg;
}

void RNRouting::sendPacket(SDNRoutingPacket * netPacket) {

    SDNRoutingPacket * dupPacket = netPacket->dup();
	routes_i it = routingTable.find(atoi(dupPacket->getDestinationAddress()));
	if (it != routingTable.end()) {
		if (!it->second.drop) {
			if (dupPacket->getTimeToLive() > 0) {
			    dupPacket->setNextHopAddress(it->second.nextHopAddress.c_str());
			    dupPacket->setTimeToLive(dupPacket->getTimeToLive()-1);
				if ((atoi(dupPacket->getDestinationAddress()) == atoi(CONTROLLER_NETWORK_ADDRESS)) && (sendStatus)){
					sendStatus = false;
					SDNRelayStatus msgRelayStatus;
					msgRelayStatus.status.location = getCurrentLocation();
					dupPacket->setMsgRelayStatus(msgRelayStatus);
					dupPacket->setHasStatus(true);
				}

				if (atoi(netPacket->getSourceAddress()) == self){
					newOutput(getOutputPacket(OUTPUT_PACKET_SENT, dupPacket));
					newTrace(getTracePacket(OUTPUT_PACKET_SENT, dupPacket));
				} else {
					newOutput(getOutputPacket(OUTPUT_PACKET_FOWARDED, dupPacket));
					newTrace(getTracePacket(OUTPUT_PACKET_FOWARDED, dupPacket));
				}

				toMacLayer(dupPacket, resolveNetworkAddress(dupPacket->getNextHopAddress()));
			}
		}
	} else {
		bufferPacket(netPacket->dup());
		sendPacket(SDN_PACKET_IN, netPacket);
	}
}

void RNRouting::parserTelemetry(SDNRoutingPacket * netPacket, double RSSI){
	newTrace("RNRouting::parserTelemetry");
	if (netPacket->getSDNRoutingPacketKind() == SDN_DATA){
		if (netPacket->getHasTelemetry()){
			addTelemetryByHop(netPacket, RSSI);
		}
	}
}

void RNRouting::deparserTelemetry(SDNRoutingPacket * netPacket) {
	newTrace("RNRouting::deparserTelemetry");
	if (netPacket->getSDNRoutingPacketKind() == SDN_DATA){
		if (users.find(atoi(netPacket->getDestinationAddress())) != users.end()) {
			if (netPacket->getHasTelemetry()){
				sendPacket(SDN_TELEMETRY, netPacket);
			}
		}
	}
}

void RNRouting::addTelemetry(SDNRoutingPacket * netPacket) {
	newTrace("RNRouting::addTelemetry");
	SDNTelemetry msgTelemetry;
	msgTelemetry.source 		= atoi(netPacket->getSourceAddress());
	msgTelemetry.destination 	= atoi(netPacket->getDestinationAddress());
	msgTelemetry.messageType	= 0;
	msgTelemetry.size 			= 0;
	netPacket->setMsgTelemetry(msgTelemetry);
	netPacket->setHasTelemetry(true);
}

void RNRouting::addTelemetryByHop(SDNRoutingPacket * netPacket, double RSSI) {
	newTrace("RNRouting::addTelemetryByHop");
	SDNTelemetryByHop msgTelemetryByHop;
	msgTelemetryByHop.relayId = self;
	msgTelemetryByHop.timeReceived = SIMTIME_DBL(simTime());
	msgTelemetryByHop.lenQueueMac = macModule->getBufferSize();
	msgTelemetryByHop.lenQueueNet = (int)TXBuffer.size();
	msgTelemetryByHop.packetDropMac = macModule->getPacketDrops();
	msgTelemetryByHop.packetDropNet = netPacketDrops;
	msgTelemetryByHop.rssi = RSSI;
    netPacket->getMsgTelemetry().msgTelemetryByHop[netPacket->getMsgTelemetry().lenMsgTelemetryByHop] = msgTelemetryByHop;
    netPacket->getMsgTelemetry().lenMsgTelemetryByHop = netPacket->getMsgTelemetry().lenMsgTelemetryByHop+1;
}


void RNRouting::sendPacket(int SDNRoutingPacketKind, int userId) {
	sendPacket(createPacket(SDNRoutingPacketKind, userId));
}

void RNRouting::sendPacket(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket) {
	sendPacket(createPacket(SDNRoutingPacketKind, netPacket));
}

void RNRouting::addPacketHeader(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket) {
	netPacket->setByteLength(netDataFrameOverhead);
	netPacket->setTime(SIMTIME_DBL(simTime()));
	netPacket->setIdentifier(identifier++);
	netPacket->setSourceAddress(SELF_NETWORK_ADDRESS);
	netPacket->setTimeToLive(timeToLive);
	netPacket->setSDNRoutingPacketKind(SDNRoutingPacketKind);
}

void RNRouting::addRoute(route_s route) {
    routingTable.insert(pair<int, route_s> (route.destination, route));
}

void RNRouting::addUser(user_s user) {
	users.insert(pair<int, user_s> (user.id, user));
}

double RNRouting::isOutOfRange(double distance){
	return (distance > positionRangeUser);
}

double RNRouting::getDistance(location_s location1, location_s location2){
	return sqrt(pow(location2.x - location1.x, 2) + pow(location2.y - location1.y, 2));
}

location_s RNRouting::getCurrentLocation(){
	NodeLocation location;
	location = mobilityModule->getLocation();
	location_s location2;
	location2.x = location.x;
	location2.y = location.y;
	return location2;
}

double RNRouting::getResidualEnergy(){
	return resMgrModule->getRemainingEnergy();
}
