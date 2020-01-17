#include "CNRouting.h"

Define_Module(CNRouting);

void CNRouting::startup() {

	// Read Config Parameters	
	timeToLive			= par("TIME_TO_LIVE");
	maxRetransmitions 	= par("MAX_RETRANSMITIONS");
	timeOutTrace		= par("TIMEOUT_TRACE").doubleValue();
	timeOutRequest		= par("TIMEOUT_REQUEST").doubleValue();
	timeOutPackets		= par("TIMEOUT_PACKETS").doubleValue();
	intervalCounter		= par("INTERVAL_COUNTER").doubleValue();
	timeOutKeepAlive 	= par("TIMEOUT_KEEP_ALIVE").doubleValue();

    int firstRelayNodeId  = par("firstRelayNodeId");
    int lastRelayNodeId   = par("lastRelayNodeId");

	// Who am I
	self = getParentModule()->getParentModule()->getIndex();

	// Load Modules
	wirelessModule 		= check_and_cast <WirelessChannel*>(getParentModule()->getParentModule()->getParentModule()->getSubmodule("wirelessChannel"));
	mobilityModule 		= check_and_cast <VirtualMobilityManager*>(getParentModule()->getParentModule()->getSubmodule("MobilityManager"));
	radioModule 		= check_and_cast <Radio*>(getParentModule()->getSubmodule("Radio"));
	macModule 			= check_and_cast <VirtualMac*>(getParentModule()->getSubmodule("MAC"));	
	srceRerouceModuce 	= check_and_cast <ResourceManager*>(getParentModule()->getParentModule()->getParentModule()->getSubmodule("node",1)->getSubmodule("ResourceManager"));

	connection = Connection();
	connection.start(self);
	fromApplication();

	for (int i = firstRelayNodeId; i < lastRelayNodeId; ++i)
	{
        contractIn_t contractIn;
        contractIn.set_relayid(i);
        contractIn.set_contractinpacket(contractIn_t::REGISTER_RELAY);

        contractIn_t::Register msgRegister = contractIn.msgregister();

        contractIn_t::Status status = msgRegister.status();

        contractIn_t::Location location = status.location();
        location.set_x(to_string(0.1));
        location.set_y(to_string(0.1));

        toApplication(&contractIn);

	}

	// Set Timers
	// setTimer(CN_TIMER_TRACE, 0);
	
	// IGNORED AS THERE IS NO NEED -> NO MOBILITY
	//setTimer(CN_TIMER_REQUEST, timeOutRequest);

	newTrace("CNRouting::started up!");
}

void CNRouting::finish() {
	cOwnedObject *Del=NULL;
	int OwnedSize = this->defaultListSize();
	for(int i=0; i<OwnedSize; ++i){
		Del = this->defaultListGet(0);
		this->drop(Del);
		delete Del;
	}
}

void CNRouting::fromMacLayer(cPacket *pkt, int srcMacAddress, double RSSI, double LQI){
	switch (pkt->getKind()) {
		case NETWORK_LAYER_PACKET:{
			SDNRoutingPacket *netPacket = dynamic_cast <SDNRoutingPacket*>(pkt);
			if (!netPacket)
				return;
			if (self == atoi(netPacket->getDestinationAddress())){

                newOutput(getOutputPacket(OUTPUT_PACKET_RECEIVED, netPacket));
                newTrace(getTracePacket(OUTPUT_PACKET_RECEIVED, netPacket));

                switch (netPacket->getSDNRoutingPacketKind()) {
					case SDN_ACK:{
						fromMacAck(netPacket);
						break;
					}
					case SDN_NOTIFY_STATUS_USER:{
						fromMacNotifyStatusUser(netPacket);
						break;
					}
					case SDN_PACKET_IN:{
						fromMacPacketIn(netPacket);
						break;
					}
					case SDN_REFRESH_REGISTER:{
						fromMacRefreshRegister(netPacket);
						break;
					}
					case SDN_REGISTER:{
						fromMacRegister(netPacket);
						break;
					}
					case SDN_UNREGISTER:{
						fromMacUnregister(netPacket);
						break;
					}
					case SDN_RESPONSE_KEEP_ALIVE:{
						readRelayStatus(netPacket);
						break;
					}
                    case SDN_TELEMETRY:{
                        fromMacTelemetry(netPacket);
                        break;
                    }
				}
			}
			break;
		}
	}
}

void CNRouting::fromApplicationLayer(cPacket *, const char *){

}

void CNRouting::timerFiredCallback(int index){
	switch (index) {
		case CN_TIMER_TRACE: {
			timerTrace();
			break;
		}
		case CN_TIMER_KEEP_ALIVE:{
			timerKeepAlive();
			break;
		}
		case CN_TIMER_PACKETS: {
			timerPackets();
			break;
		}
		case CN_TIMER_REQUEST: {
			timerRequest();
			break;
		}
	}
}

void CNRouting::fromMacAck(SDNRoutingPacket * netPacket){

    newTrace(std::stringstream() 	<< "CNRouting::fromMacAck -> " 
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

void CNRouting::fromMacNotifyStatusUser(SDNRoutingPacket * netPacket){

    newTrace(std::stringstream() 	<< "CNRouting::fromMacNotifyStatusUser -> " 
    								<< "relayId: " 	<< netPacket->getSourceAddress() << ", "
    								<< "userId: " 	<< netPacket->getMsgNotifyStatusUser().userId);

	checkHasStatus(netPacket);
	
	sendPacket(SDN_ACK, netPacket);

	contractIn_t contractIn;
	contractIn.set_userid(netPacket->getMsgNotifyStatusUser().userId);
	contractIn.set_relayid(atoi(netPacket->getSourceAddress()));
	contractIn.set_contractinpacket(contractIn_t::NOTIFY_STATUS_USER);

	contractIn_t::NotifyStatusUser msgNotifyStatusUser = contractIn.msgnotifystatususer();

	contractIn_t::Status status = msgNotifyStatusUser.status();

	contractIn_t::Location location = status.location();

	location.set_x(to_string(netPacket->getMsgNotifyStatusUser().status.location.x));
	location.set_y(to_string(netPacket->getMsgNotifyStatusUser().status.location.y));

	toApplication(&contractIn);
}

void CNRouting::fromMacPacketIn(SDNRoutingPacket * netPacket, string trace){

    newTrace(std::stringstream() 	<< "CNRouting::fromMacPacketIn -> " 
    								<< "relayId: " 		<< netPacket->getSourceAddress() << ", "
    								<< "destination: "	<< netPacket->getMsgPacketIn().userId);

	checkHasStatus(netPacket);

	sendPacket(SDN_ACK, netPacket);

	contractIn_t contractIn;
	contractIn.set_userid(netPacket->getMsgPacketIn().userId);
	contractIn.set_relayid(atoi(netPacket->getSourceAddress()));
	contractIn.set_contractinpacket(contractIn_t::PACKET_IN);

	toApplication(&contractIn, trace);
}

void CNRouting::fromMacRefreshRegister(SDNRoutingPacket * netPacket){

    newTrace(std::stringstream() 	<< "CNRouting::fromMacRefreshRegister -> " 
    								<< "relayId: " 	<< netPacket->getSourceAddress() << ", "
    								<< "userId: " 	<< netPacket->getMsgRegister().userId);

	checkHasStatus(netPacket);

	contractIn_t contractIn;
	contractIn.set_userid(netPacket->getMsgRegister().userId);
	contractIn.set_relayid(atoi(netPacket->getSourceAddress()));
	contractIn.set_contractinpacket(contractIn_t::REFRESH_REGISTER_USER);

	contractIn_t::Register msgRegister = contractIn.msgregister();

    contractIn_t::Status status = msgRegister.status();
	contractIn_t::Location location = status.location();
	location.set_x(to_string(netPacket->getMsgRegister().status.location.x));
	location.set_y(to_string(netPacket->getMsgRegister().status.location.y));

	contractIn_t::Load load = msgRegister.load();
	load.set_waittimepacketqueue(to_string(netPacket->getMsgRegister().load.waitTimePacketQueue));
	load.set_usernumber(netPacket->getMsgRegister().load.userNumber);
	load.set_dropnumber(netPacket->getMsgRegister().load.dropNumber);

	toApplication(&contractIn);
}

void CNRouting::fromMacRegister(SDNRoutingPacket * netPacket){

    newTrace(std::stringstream() 	<< "CNRouting::fromMacRegister -> " 
    								<< "relayId: "	<< netPacket->getSourceAddress() << ", "
    								<< "userId: " 	<< netPacket->getMsgRegister().userId);

	checkHasStatus(netPacket);

	contractIn_t contractIn;
	contractIn.set_userid(netPacket->getMsgRegister().userId);
	contractIn.set_relayid(atoi(netPacket->getSourceAddress()));
	contractIn.set_contractinpacket(contractIn_t::REGISTER_USER);

	contractIn_t::Register msgRegister = contractIn.msgregister();

    contractIn_t::Status status = msgRegister.status();
	contractIn_t::Location location = status.location();
	location.set_x(to_string(netPacket->getMsgRegister().status.location.x));
	location.set_y(to_string(netPacket->getMsgRegister().status.location.y));

	contractIn_t::Load load = msgRegister.load();
	load.set_waittimepacketqueue(to_string(netPacket->getMsgRegister().load.waitTimePacketQueue));
	load.set_usernumber(netPacket->getMsgRegister().load.userNumber);
	load.set_dropnumber(netPacket->getMsgRegister().load.dropNumber);

	toApplication(&contractIn);
}

void CNRouting::fromMacUnregister(SDNRoutingPacket * netPacket){

    newTrace(std::stringstream() 	<< "CNRouting::fromMacUnregister -> " 
    								<< "relayId: " 	<< netPacket->getSourceAddress() << ", "
    								<< "userId: " 	<< netPacket->getMsgUnregister().userId);

	checkHasStatus(netPacket);

	contractIn_t contractIn;
	contractIn.set_userid(netPacket->getMsgUnregister().userId);
	contractIn.set_relayid(atoi(netPacket->getSourceAddress()));
	contractIn.set_contractinpacket(contractIn_t::UNREGISTER_USER);

	contractIn_t::Unregister msgUnregister = contractIn.msgunregister();

	if (netPacket->getMsgUnregister().event == SDN_REQUIRED){
	    msgUnregister.set_event(contract_in::ContractIn_Event_REQUIRED);
	} else if (netPacket->getMsgUnregister().event == SDN_FORCED){
        msgUnregister.set_event(contract_in::ContractIn_Event_FORCED);
	}

	toApplication(&contractIn);
}

void CNRouting::fromMacTelemetry(SDNRoutingPacket * netPacket){

    newTrace(getTraceTelemetry(netPacket));
    newOutput(getOutputTelemetry(netPacket));

}

void CNRouting::toApplication(contractIn_t * contractIn, string trace){

	newTrace("CNRouting::toApplication");

    double sim_time = simTime().dbl();

    contractIn->set_time(to_string(sim_time));

    std::string sendMsg = contractIn->SerializeAsString();

    connection.transmit(sendMsg.c_str());

   	fromApplication(trace);

}

void CNRouting::fromApplication(string trace){

    newTrace("CNRouting::fromApplication");

    char buffer[CONST_MAX_BUFFER] = {0};
    int si = connection.receive(buffer);
    contractOut_t contractOut;
    contractOut.ParseFromArray(buffer, si);

    for (int i = 0; i < contractOut.updateuser_size(); i++) {
        fromApplicationUpdateUser(contractOut.updateuser(i));
    }
    for (int i = 0; i < contractOut.updateroutingtable_size(); i++) {
        fromApplicationUpdateRoutingTable(contractOut.updateroutingtable(i), trace);
    }
    for (int i = 0; i < contractOut.updatearea_size(); i++) {
        fromApplicationUpdateArea(contractOut.updatearea(i));
    }
}

void CNRouting::fromApplicationUpdateUser(const contract_out::ContractOut_UpdateUser & UpdateUser){

	newTrace(std::stringstream() 	<< "CNRouting::fromApplicationUpdateUser -> "
									<< "userId: " 	<< UpdateUser.userid() << ", " 
									<< "action: " 	<< UpdateUser.action() << ", " 
									<< "event: " 	<< UpdateUser.event());

	SDNRoutingPacket *msg = new SDNRoutingPacket("Node is sending an adv msg", NETWORK_LAYER_PACKET);
	msg->setDestinationAddress(to_string(UpdateUser.relayid()).c_str());
	SDNUpdateUser msgUpdateUser;
	msgUpdateUser.userId = UpdateUser.userid();
	msgUpdateUser.action = UpdateUser.action();
	msgUpdateUser.event = UpdateUser.event();

	msg->setMsgUpdateUser(msgUpdateUser);
	
	addPacketHeader(SDN_UPDATE_USER, msg);
	addSentPacket(msg, timeOutPackets);
	sendPacket(msg);
}

void CNRouting::fromApplicationUpdateRoutingTable(const contract_out::ContractOut_UpdateRoutingTable & UpdateRoutingTable, string trace){

	newTrace(std::stringstream() 	<< "CNRouting::fromApplicationUpdateRoutingTable -> "
									<< "nodeId: " 		<< UpdateRoutingTable.relayid() << ", " 
									<< "destination: " 	<< UpdateRoutingTable.route().destination() << ", " 
									<< "nextHop: " 		<< UpdateRoutingTable.route().nexthop() << ", "
									<< "drop: " 		<< UpdateRoutingTable.route().drop() << ", " 
									<< "action: " 		<< UpdateRoutingTable.action());
	
	SDNUpdateRoutingTable msgUpdateRoutingTable;
	msgUpdateRoutingTable.relayId = UpdateRoutingTable.relayid();
	msgUpdateRoutingTable.route.destination = UpdateRoutingTable.route().destination();
    msgUpdateRoutingTable.route.drop = UpdateRoutingTable.route().drop();
    msgUpdateRoutingTable.route.nextHopAddress = to_string(UpdateRoutingTable.route().nexthop()).c_str();
	msgUpdateRoutingTable.action = UpdateRoutingTable.action();
	
	if (UpdateRoutingTable.relayid() == atoi(SELF_NETWORK_ADDRESS)){
		fromApplicationUpdateRoutingTable(&msgUpdateRoutingTable);
	} else {
		SDNRoutingPacket *msg = new SDNRoutingPacket("Node is sending an adv msg", NETWORK_LAYER_PACKET);
		msg->setDestinationAddress(to_string(UpdateRoutingTable.relayid()).c_str());
		msg->setMsgUpdateRoutingTable(msgUpdateRoutingTable);
		msg->setTrace(trace.c_str());
		addPacketHeader(SDN_UPDATE_ROUTING_TABLE, msg);
		addSentPacket(msg, timeOutPackets);
		sendPacket(msg);
	}

}

void CNRouting::fromApplicationUpdateRoutingTable(SDNUpdateRoutingTable * msgUpdateRoutingTable){
	if ((msgUpdateRoutingTable->action == SDN_DELETE) || 
		(msgUpdateRoutingTable->action == SDN_UPDATE)) { 
		routingTable.erase(msgUpdateRoutingTable->route.destination);
	}
	if ((msgUpdateRoutingTable->action == SDN_ADD) || 
		(msgUpdateRoutingTable->action == SDN_UPDATE)) { 
		addRoute(msgUpdateRoutingTable->route);
	}
}

void CNRouting::addRoute(route_s route) {
    routingTable.insert(pair<int, route_s> (route.destination, route));
}

void CNRouting::fromApplicationUpdateArea(const contract_out::ContractOut_UpdateArea & UpdateArea){

	newTrace(std::stringstream() 	<< "CNRouting::fromApplicationUpdateArea -> "
									<< "nodeId: " 	<< UpdateArea.relayid() << ", " 
									<< "radius: " 	<< UpdateArea.area().radius() << ", " 
									<< "x: " 		<< UpdateArea.area().center().x() << ", "
									<< "y: " 		<< UpdateArea.area().center().y());

	SDNUpdateArea msgUpdateArea;
	msgUpdateArea.area.radius = stod(UpdateArea.area().radius());
	msgUpdateArea.area.center.x = stod(UpdateArea.area().center().x());
    msgUpdateArea.area.center.y = stod(UpdateArea.area().center().y());

	if (UpdateArea.relayid() == atoi(SELF_NETWORK_ADDRESS)){
		fromApplicationUpdateArea(&msgUpdateArea);
	} else {
		SDNRoutingPacket *msg = new SDNRoutingPacket("Node is sending an adv msg", NETWORK_LAYER_PACKET);
		msg->setDestinationAddress(to_string(UpdateArea.relayid()).c_str());
		msg->setMsgUpdateArea(msgUpdateArea);
		addPacketHeader(SDN_UPDATE_AREA, msg);
        addSentPacket(msg, timeOutPackets);
        sendPacket(msg);
	}
}

void CNRouting::fromApplicationUpdateArea(SDNUpdateArea * msgUpdateArea){
	ApplicationPacket *topology = new ApplicationPacket("setup", MOBILE_MESSAGE);
	topology->setX(msgUpdateArea->area.center.x);
	topology->setY(msgUpdateArea->area.center.y);
	toApplicationLayer(topology);
}


void CNRouting::checkHasStatus(SDNRoutingPacket * netPacket){
	if (netPacket->getHasStatus()){
		readRelayStatus(netPacket);
	}
}

void CNRouting::readRelayStatus(SDNRoutingPacket * netPacket){

	relays_i it = relays.find(atoi(netPacket->getSourceAddress()));
	it->second.counter = (int) timeOutKeepAlive/intervalCounter;
	if (it->second.status.null){
        it->second.status.location = netPacket->getMsgRelayStatus().status.location;
        it->second.status.null = false;
	}

	contractIn_t contractIn;
	contractIn.set_relayid(atoi(netPacket->getSourceAddress()));
	contractIn.set_contractinpacket(contractIn_t::NOTIFY_STATUS_RELAY);

	contractIn_t::NotifyStatusRelay msgNotifyStatusRelay = contractIn.msgnotifystatusrelay();

	contractIn_t::Status status = msgNotifyStatusRelay.status();

	contractIn_t::Location location = status.location();
	location.set_x(to_string(netPacket->getMsgRelayStatus().status.location.x));
	location.set_y(to_string(netPacket->getMsgRelayStatus().status.location.y));

	toApplication(&contractIn);
}

void CNRouting::timerTrace(){
	newOutput(getOutputLocation(getCurrentLocation()));
	newTrace(getTraceLocation(getCurrentLocation()));
    setTimer(CN_TIMER_TRACE, timeOutTrace);
}

void CNRouting::timerRequest(){

    contractIn_t::Location * location = new contractIn_t::Location;
    location->set_x(to_string(0.1));
    location->set_y(to_string(0.1));

    contractIn_t::Status * status = new contractIn_t::Status;
    status->set_allocated_location(location);

    contractIn_t::NotifyStatusController * msgNotifyStatusController = new contractIn_t::NotifyStatusController;
    msgNotifyStatusController->set_allocated_status(status);

    contractIn_t * contractIn = new contractIn_t;
    contractIn->set_contractinpacket(contractIn_t::NOTIFY_STATUS_CONTROLLER);
    contractIn->set_allocated_msgnotifystatuscontroller(msgNotifyStatusController);
    contractIn->set_relayid(1);
    contractIn->set_userid(1);

    toApplication(contractIn);

    contractIn_t contractIn2;

    contractIn2.set_contractinpacket(contractIn_t::REQUEST);

    toApplication(&contractIn2);

    setTimer(CN_TIMER_REQUEST, timeOutRequest);
}

void CNRouting::timerPackets(){
    newTrace("CNRouting::timerPackets");
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

void CNRouting::resetTimerPackets(){
	cancelTimer(CN_TIMER_PACKETS);
	if (sentPackets.size() > 0) {
		sort(sentPackets.begin(), sentPackets.end(), sort_sentPacket);
		lastTimeUpdateSentPackets = SIMTIME_DBL(simTime());
		setTimer(CN_TIMER_PACKETS, sentPackets.begin()->remainingTime);
	}
}

void CNRouting::sendPacket(SDNRoutingPacket * netPacket) {
    SDNRoutingPacket * dupPacket = netPacket->dup();
	routes_i it = routingTable.find(atoi(dupPacket->getDestinationAddress()));
	if (it != routingTable.end()) {
		if (!it->second.drop) {
			if (dupPacket->getTimeToLive() > 0) {
			    dupPacket->setNextHopAddress(it->second.nextHopAddress.c_str());
			    dupPacket->setTimeToLive(dupPacket->getTimeToLive()-1);

				newOutput(getOutputPacket(OUTPUT_PACKET_SENT, dupPacket));
				newTrace(getTracePacket(OUTPUT_PACKET_SENT, dupPacket));

				toMacLayer(dupPacket, resolveNetworkAddress(dupPacket->getNextHopAddress()));
			}
		}
	}
}	

void CNRouting::sendPacket(int SDNRoutingPacketKind, int relayId){
	sendPacket(createPacket(SDNRoutingPacketKind, relayId));
}

void CNRouting::sendPacket(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket){
	sendPacket(createPacket(SDNRoutingPacketKind, netPacket));
}

SDNRoutingPacket * CNRouting::createPacket(int SDNRoutingPacketKind, int relayId){
	SDNRoutingPacket *msg = new SDNRoutingPacket("Node is sending an adv msg", NETWORK_LAYER_PACKET);
	addPacketHeader(SDNRoutingPacketKind, msg);
	switch (SDNRoutingPacketKind) {
		case SDN_KEEP_ALIVE: {
			msg->setDestinationAddress(to_string(relayId).c_str());
			break;
		}
	}
	return msg;
}

SDNRoutingPacket * CNRouting::createPacket(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket){
	SDNRoutingPacket *msg = new SDNRoutingPacket("Node is sending an adv msg", NETWORK_LAYER_PACKET);
	// TO DO: Verificar o tamanho do pacote dependendo do tipo.
	addPacketHeader(SDNRoutingPacketKind, msg);
	switch (SDNRoutingPacketKind) {
		case SDN_ACK: {
			msg->setDestinationAddress(netPacket->getSourceAddress());
			SDNAck msgAck;
			msgAck.identifier = netPacket->getIdentifier();
			msg->setMsgAck(msgAck);
			break;
		}
	}
	return msg;
}

void CNRouting::addPacketHeader(int SDNRoutingPacketKind, SDNRoutingPacket * netPacket) {
	netPacket->setByteLength(netDataFrameOverhead);
	netPacket->setTime(SIMTIME_DBL(simTime()));
	netPacket->setIdentifier(identifier++);
	netPacket->setSourceAddress(SELF_NETWORK_ADDRESS);
	netPacket->setTimeToLive(timeToLive);
	netPacket->setSDNRoutingPacketKind(SDNRoutingPacketKind);
}

void CNRouting::addSentPacket(SDNRoutingPacket * netPacket, double timeOut) {
	resetRemainingTimeSentPackets();
	sentPacket_s sentPacket;
	sentPacket.packet = netPacket->dup();
	sentPacket.timeOut = timeOut;
	sentPacket.remainingTime = timeOut;
	sentPacket.retransmitions = maxRetransmitions;
	sentPackets.push_back(sentPacket);
    resetTimerPackets();
}

void CNRouting::resetRemainingTimeSentPackets(){
	double diff = SIMTIME_DBL(simTime()) - lastTimeUpdateSentPackets;
	for (int i = 0; i < sentPackets.size(); ++i) {
		sentPackets[i].remainingTime -= diff;
		if (sentPackets[i].remainingTime < 0){
		 	sentPackets[i].remainingTime = 0;
		}
	}
}

void CNRouting::timerKeepAlive(){
	for (relays_i it = relays.begin(); it != relays.end(); ++it) {
		it->second.counter--;
		if (it->second.counter == 0){
			sendPacket(SDN_KEEP_ALIVE, it->second.id);
		}
	}
	cancelTimer(CN_TIMER_KEEP_ALIVE);
	setTimer(CN_TIMER_KEEP_ALIVE, intervalCounter);
}

string CNRouting::getNextHopAddressFromDestinationAddress(string destinationAddress) {
	routes_i it = routingTable.find(atoi(destinationAddress.c_str()));
	return it->second.nextHopAddress.c_str();
}

location_s CNRouting::getCurrentLocation(){
	NodeLocation location;
	location = mobilityModule->getLocation();
	location_s location2;
	location2.x = location.x;
	location2.y = location.y;
	return location2;
}

double CNRouting::getResidualEnergy(){
	return resMgrModule->getRemainingEnergy();
}
