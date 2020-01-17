#include "global.h"

bool sort_sentPacket(sentPacket_s a, sentPacket_s b){
	return (a.remainingTime < b.remainingTime);
}


std::string getSDNRoutingPacketKindName(int SDNRoutingPacketKind)
{
    std::string kind = "UNDEFINED";

    switch (SDNRoutingPacketKind) {
        case SDN_HELLO:                 { kind = "SDN_HELLO"; break; }
        case SDN_CLOSE:                 { kind = "SDN_CLOSE"; break; }
        case SDN_REFRESH:               { kind = "SDN_REFRESH"; break; }
        case SDN_KEEP_ALIVE:            { kind = "SDN_KEEP_ALIVE"; break; }
        case SDN_IS_CONNECTED:          { kind = "SDN_IS_CONNECTED"; break; }
        case SDN_CONFIRMATION_REGISTRY: { kind = "SDN_CONFIRMATION_REGISTRY"; break; }
        case SDN_RESPONSE_CLOSE:        { kind = "SDN_RESPONSE_CLOSE"; break; }
        case SDN_RESPONSE_IS_CONNECTED: { kind = "SDN_RESPONSE_IS_CONNECTED"; break; }
        case SDN_RESPONSE_KEEP_ALIVE:   { kind = "SDN_RESPONSE_KEEP_ALIVE"; break; }
        case SDN_REGISTER:              { kind = "SDN_REGISTER"; break; }
        case SDN_UNREGISTER:            { kind = "SDN_UNREGISTER"; break; }
        case SDN_REFRESH_REGISTER:      { kind = "SDN_REFRESH_REGISTER"; break; }
        case SDN_PACKET_IN:             { kind = "SDN_PACKET_IN"; break; }
        case SDN_NOTIFY_STATUS_USER:    { kind = "SDN_NOTIFY_STATUS_USER"; break; }
        case SDN_UPDATE_USER:           { kind = "SDN_UPDATE_USER"; break; }
        case SDN_UPDATE_ROUTING_TABLE:  { kind = "SDN_UPDATE_ROUTING_TABLE"; break; }
        case SDN_UPDATE_AREA:           { kind = "SDN_UPDATE_AREA"; break; }
        case SDN_ACK:                   { kind = "SDN_ACK"; break; }
        case SDN_DATA:                  { kind = "SDN_DATA"; break; }
        case SDN_TELEMETRY:             { kind = "SDN_TELEMETRY"; break; }
    }

    return kind;
}

std::string getContractInPacketName(enum contract_in::ContractIn_ContractInPacket contractInPacket)
{
    std::string kind = "UNDEFINED";

    switch (contractInPacket) {
        case contract_in::ContractIn::UNREGISTER_USER:          { kind = "UNREGISTER_USER"; break; }
        case contract_in::ContractIn::NOTIFY_STATUS_RELAY:      { kind = "NOTIFY_STATUS_RELAY"; break; }
        case contract_in::ContractIn::REGISTER_RELAY:           { kind = "REGISTER_RELAY"; break; }
        case contract_in::ContractIn::NOTIFY_STATUS_USER:       { kind = "NOTIFY_STATUS_USER"; break; }
        case contract_in::ContractIn::REGISTER_USER:            { kind = "REGISTER_USER"; break; }
        case contract_in::ContractIn::REFRESH_REGISTER_USER:    { kind = "REFRESH_REGISTER_USER"; break; }
        case contract_in::ContractIn::NOTIFY_STATUS_CONTROLLER: { kind = "NOTIFY_STATUS_CONTROLLER"; break; }
        case contract_in::ContractIn::PACKET_IN:                { kind = "PACKET_IN"; break; }
        case contract_in::ContractIn::REQUEST:                  { kind = "REQUEST"; break; }
    }

    return kind;
}

std::string getOutputPacket(std::string action, SDNRoutingPacket * netPacket){
    std::stringstream ss;
    ss  << "pkt;"
        << action << ";"
        << getSDNRoutingPacketKindName(netPacket->getSDNRoutingPacketKind()) << ";"
        << netPacket->getMessageType() << ";"
        << netPacket->getIdTransmission() << ";"
        << netPacket->getIdentifier() << ";"
        << netPacket->getSourceAddress() << ";"
        << netPacket->getNextHopAddress() << ";"
        << netPacket->getDestinationAddress() << ";"
        << netPacket->getTimeToLive() << ";"
        << netPacket->getTrace() << ";";
    return ss.str();
}

std::string getTracePacket(std::string action, SDNRoutingPacket * netPacket){
    std::stringstream ss;
    ss  << "packet -> "
        << "act: " << action << " | "
        << "knd: " << getSDNRoutingPacketKindName(netPacket->getSDNRoutingPacketKind()) << " | "
        << "typ: " << netPacket->getMessageType() << " | "
        << "idt: " << netPacket->getIdTransmission() << " | "
        << "idm: " << netPacket->getIdentifier() << " | "
        << "src: " <<netPacket->getSourceAddress() << " | "
        << "nxt: " <<netPacket->getNextHopAddress() << " | "
        << "dst: " <<netPacket->getDestinationAddress() << " | "
        << "ttl: " <<netPacket->getTimeToLive() << " | "
        << "trc: " <<netPacket->getTrace() << " |";
    return ss.str();
}

std::string getOutputLocation(location_s location){
    std::stringstream ss;
    ss  << "loc;"
        << location.x << ";" 
        << location.y << ";";
    return ss.str();
}

std::string getTraceLocation(location_s location){
    std::stringstream ss;
    ss  << "location -> "
        << "x: " << location.x << " | " 
        << "y: " << location.y << " | ";
    return ss.str();
}

std::string getOutputTelemetry(int lenQueueMac, int lenQueueNet, int packetDropMac, int packetDropNet){
    std::stringstream ss;
    ss  << "tel;" 
        << lenQueueMac << ";"
        << lenQueueNet << ";"
        << packetDropMac << ";"
        << packetDropNet;
    return ss.str();
}

std::string getOutputTelemetry(SDNRoutingPacket * netPacket){
    std::stringstream ss;
    ss  << "tel;"
        << netPacket->getMsgTelemetry().source << ";"
        << netPacket->getMsgTelemetry().destination << ";"
        << netPacket->getMsgTelemetry().messageType << ";"
        << netPacket->getMsgTelemetry().size << ";";
    for (int i=0; i < netPacket->getMsgTelemetry().lenMsgTelemetryByHop; ++i) {
        ss  << "byhop;"
            << netPacket->getMsgTelemetry().msgTelemetryByHop[i].relayId << ";"
            << netPacket->getMsgTelemetry().msgTelemetryByHop[i].timeReceived << ";"
            << netPacket->getMsgTelemetry().msgTelemetryByHop[i].lenQueueMac << ";"
            << netPacket->getMsgTelemetry().msgTelemetryByHop[i].lenQueueNet << ";"
            << netPacket->getMsgTelemetry().msgTelemetryByHop[i].packetDropMac << ";"
            << netPacket->getMsgTelemetry().msgTelemetryByHop[i].packetDropNet << ";"
            << netPacket->getMsgTelemetry().msgTelemetryByHop[i].rssi << ";";
    }
    ss  << "end";
    return ss.str();
}

std::string getTraceTelemetry(int lenQueueMac, int lenQueueNet, int packetDropMac, int packetDropNet){
    std::stringstream ss;
    ss  << "telemetry -> " 
        << "lenQueueMac: "      << lenQueueMac << " | "
        << "lenQueueNet: "      << lenQueueNet << " | "
        << "packetDropMac: "    << packetDropMac << " | "
        << "packetDropNet: "    << packetDropNet;
    return ss.str();
}

std::string getTraceTelemetry(SDNRoutingPacket * netPacket){
    std::stringstream ss;
    ss  << "telemetry -> "
        << "source: "       << netPacket->getMsgTelemetry().source << " | "
        << "destination: "  << netPacket->getMsgTelemetry().destination << " | "
        << "messageType: "  << netPacket->getMsgTelemetry().messageType << " | "
        << "size: "         << netPacket->getMsgTelemetry().size << "\n";
    for (int i=0; i < netPacket->getMsgTelemetry().lenMsgTelemetryByHop; ++i) {
        ss  << "byhop -> "
            << "relayId: "          << netPacket->getMsgTelemetry().msgTelemetryByHop[i].relayId << " | "
            << "timeReceived: "     << netPacket->getMsgTelemetry().msgTelemetryByHop[i].timeReceived << " | "
            << "lenQueueMac: "      << netPacket->getMsgTelemetry().msgTelemetryByHop[i].lenQueueMac << " | "
            << "lenQueueNet: "      << netPacket->getMsgTelemetry().msgTelemetryByHop[i].lenQueueNet << " | "
            << "packetDropMac: "    << netPacket->getMsgTelemetry().msgTelemetryByHop[i].packetDropMac << " | "
            << "packetDropNet: "    << netPacket->getMsgTelemetry().msgTelemetryByHop[i].packetDropNet << " | "
            << "rssi: "             << netPacket->getMsgTelemetry().msgTelemetryByHop[i].rssi << "\n";
    }
    return ss.str();
}