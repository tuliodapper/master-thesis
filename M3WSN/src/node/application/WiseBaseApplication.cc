// *****************************************************************************
//  Copyright (C): Christian Nastasi, 2011                                    
//  Author(s): Christian Nastasi                                              
//  Developed at the:                                                         
//  - Retis LAB, TeCIP Institute, Scuola Superiore Sant'Anna (Pisa)           
//  - School of Elec Eng and Computer Science, Queen Mary University (London) 
//  This file is distributed under the terms in the attached LICENSE_2 file.  
//  If you do not find this file, copies can be found by writing to:          
//  - c.nastasi@sssup.it                                                      
//  - nastasichr@gmail.com                                                    
//  - andrea.cavallaro@eecs.qmul.ac.uk                                        
// *****************************************************************************

#include "WiseBaseApplication.h"
#include "WiseDebug.h"

void WiseBaseApplication::initialize() {
    cMessage* m = 0;
    readParameter();

    /* Send the STARTUP message to:
     * 1) Sensor_Manager;
     * 2) Commmunication module;
     * 3) APP (self message) so that the node starts to operate.
     * We are using a random delay IF we must not have node synchronized
     */
    double startup_delay = simTime().dbl();
    if (!synchronizedStartup)
        startup_delay += genk_dblrand(0) * 0.05 * cpuClockDrift;
    m = new cMessage("Communication [STARTUP]", NODE_STARTUP);
    sendDelayed(m, startup_delay, "toCommunicationModule");
    m = new cMessage("Sensor Dev Mgr [STARTUP]", NODE_STARTUP);
    sendDelayed(m, startup_delay, "toSensorDeviceManager");
    m = new cMessage("Resource Manager [STARTUP]", NODE_STARTUP);
    sendDirect(m, startup_delay, 0, resMgrModule, "powerConsumption");
    m = new cMessage("App [STARTUP]", NODE_STARTUP);
    scheduleAt(startup_delay, m);

    /* Latency measurement is optional. An application can not define the
     * following two parameters. If they are not defined then the
     * declareHistogram and collectHistogram statement are not called.
     */
    latencyMax = hasPar("latencyHistogramMax") ? par("latencyHistogramMax") : 0;
    latencyBuckets = hasPar("latencyHistogramBuckets") ? par("latencyHistogramBuckets") : 0;
    if (latencyMax > 0 && latencyBuckets > 0)
        declareHistogram("Application level latency, in ms", 0, latencyMax, latencyBuckets);
    last_packet_latency = -1;
}

void WiseBaseApplication::handleMessage(cMessage* msg) {
    int msgKind = msg->getKind();
    WISE_DEBUG_3("WiseBaseApplication::handleMessage()");

    if (disabled && msgKind != NODE_STARTUP) {
	//trace() << "wisebase return" ;
        delete msg;
        return;
    }

    switch (msgKind) {
        case NODE_STARTUP:
            disabled = 0;
            startup();
            break;
        case APPLICATION_PACKET:
            WISE_DEBUG_3("\tApplication Packet");
	    //trace() << "Application Packet";
            WiseApplicationPacket *appPacket;
            appPacket = check_and_cast<WiseApplicationPacket*>(msg);
            processAppPacket(appPacket);
            break;
        case MULTIMEDIA_PACKET:
            WISE_DEBUG_3("\tApplication Packet");
            WiseApplicationPacket *multPacket;
            multPacket = check_and_cast<WiseApplicationPacket*>(msg);
	    multPacket->setKind(MULTIMEDIA_PACKET);
            processAppPacket(multPacket);
            break;

	case APPLICATION_PLATOON:
            WISE_DEBUG_3("\tApplication Packet");
            trace() << "wisebase app";
            WiseApplicationPacket *platoonMsg;
            platoonMsg = check_and_cast<WiseApplicationPacket*>(msg);
	    platoonMsg->setKind(APPLICATION_MESSAGE);
            processAppPacket(platoonMsg);
            break;

	case APPLICATION_MESSAGE:
            WISE_DEBUG_3("\tApplication Message");
            trace() << "wisebase app";

            ApplicationPacket *applicationPacket;
            applicationPacket = check_and_cast<ApplicationPacket*>(msg);
            fromNetworkLayer(applicationPacket, "1", 0, 0);

            //WiseApplicationPacket *beaconMsg;
            //beaconMsg = check_and_cast<WiseApplicationPacket*>(msg);
            //beaconMsg->setKind(APPLICATION_MESSAGE);
            //processAppPacket(beaconMsg);
            
            break;

        case SENSING_RELEVANCE_MESSAGE:
            WISE_DEBUG_3("\tApplication Packet");
            WiseApplicationPacket *srPacket;
            srPacket = check_and_cast<WiseApplicationPacket*>(msg);
	    srPacket->setKind(SENSING_RELEVANCE_MESSAGE);
            processAppPacket(srPacket);
            break;

        case SCALAR_PACKET:
            WISE_DEBUG_3("\tApplication Packet");
            WiseApplicationPacket *scalarPacket;
            scalarPacket = check_and_cast<WiseApplicationPacket*>(msg);
	    scalarPacket->setKind(SCALAR_PACKET);
            processAppPacket(scalarPacket);
            break;
        case TIMER_SERVICE:
            handleTimerMessage(msg);
            break;
        case SENSOR_READING:
            WISE_DEBUG_3("\tSENSOR_READING_MESSAGE");
            WiseSensorMessage *sensMsg;
            sensMsg = check_and_cast<WiseSensorMessage*>(msg);
            resMgrModule->consumeEnergy(1);
            handleSensorReading(sensMsg);
            break;
        case MULTIMEDIA:
            WISE_DEBUG_3("\tSENSOR_READING_MESSAGE");
            WiseSensorMessage *multMsg;
            multMsg = check_and_cast<WiseSensorMessage*>(msg);
            resMgrModule->consumeEnergy(1);
            handleSensorReading(multMsg);
            break;
        case SENSING_RELEVANCE:
            WISE_DEBUG_3("\tSENSOR_READING_MESSAGE");
            WiseSensorMessage *srMsg;
            srMsg = check_and_cast<WiseSensorMessage*>(msg);
            resMgrModule->consumeEnergy(1);
            handleSensorReading(srMsg);
            break;
        case OUT_OF_ENERGY:
            disabled = true;
            break;
        case DESTROY_NODE:
            disabled = true;
            break;
        case NETWORK_CONTROL_MESSAGE:
            WISE_DEBUG_3("\tNetwork Control");
	    WiseApplicationPacket *ntwMsg;
            ntwMsg = check_and_cast<WiseApplicationPacket*>(msg);
	    ntwMsg->setKind(NETWORK_CONTROL_MESSAGE);
            processAppPacket(ntwMsg);
            break;
        case NETWORK_EVENT:
            WISE_DEBUG_3("\tNetwork Event");
	    WiseApplicationPacket *evetMsg;
            evetMsg = check_and_cast<WiseApplicationPacket*>(msg);
	    evetMsg->setKind(NETWORK_EVENT);
            processAppPacket(evetMsg);
            break;
	case HANDOFF_MSG:{
            WISE_DEBUG_3("\tHANDOFF");
	    WiseApplicationPacket *handoffMsg;
            handoffMsg = check_and_cast<WiseApplicationPacket*>(msg);
	    handoffMsg->setKind(HANDOFF_MSG);
            processAppPacket(handoffMsg);
	    break;
	}
	case MOBILE_WAKEUP:{
            WISE_DEBUG_3("\tMOBILE WAKEUP");
	    WiseApplicationPacket *wakeUpMsg;
            wakeUpMsg = check_and_cast<WiseApplicationPacket*>(msg);
	    wakeUpMsg->setKind(MOBILE_WAKEUP);
            processAppPacket(wakeUpMsg);
	    break;
	}
	case NETWORK_SETUP:{
	    WiseApplicationPacket *setup;
	    setup = new WiseApplicationPacket("Setup msg", NETWORK_SETUP);
	    setup->setKind(NETWORK_SETUP);
	    setup->setSource(self);
            processAppPacket(setup);
	    delete setup;
	    break;
	}
        case MAC_CONTROL_MESSAGE:{
            WISE_DEBUG_3("\tMac Control" << endl);
            handleMacControlMessage(msg);
            break;
	}
        case RADIO_CONTROL_MESSAGE:{
            WISE_DEBUG_3("\tRadio Control" << endl);
            RadioControlMessage *radioMsg;
            radioMsg = check_and_cast<RadioControlMessage*>(msg);
            handleRadioControlMessage(radioMsg);
            break;
	}
	case NETWORK_LAYER_PACKET:{
	    //trace() << "wisebase NETWORK_LAYER_PACKET";
            WISE_DEBUG_3("\tRouting Packet");
	    RoutingPacket *pkt = check_and_cast<RoutingPacket*>(msg);
	    NetMacInfoExchange_type ctl =pkt->getNetMacInfoExchange();
	    fromNetworkLayer(pkt, pkt->getSourceAddress(), ctl.RSSI, ctl.LQI);
	    break;
	}
	case TOPOLOGY_CONTROL:{
	    MobilityManagerMessage *pkt = check_and_cast<MobilityManagerMessage*>(msg);
	    handleMobilityControlMessage(pkt);
	    break;
	}
        case MOBILE_MESSAGE:{
            WISE_DEBUG_3("\tApplication Packet");
	    //trace() << "Application Packet";
            ApplicationPacket *applicationPacket;
            applicationPacket = check_and_cast<ApplicationPacket*>(msg);
            fromNetworkLayer(applicationPacket, "1", 0, 0);
            break;
	}
        default:
            throw cRuntimeError("Application module received unexpected message");
    }
    delete msg;
}

void WiseBaseApplication::finish() {
    CastaliaModule::finish();
    DebugInfoWriter::closeStream();
    OutputInfoWriter::closeStream();
    ResultInfoWriter::closeStream();
}

void WiseBaseApplication::requestSensorReading() {
    WISE_DEBUG_3("WiseBaseApplication::requestSensorReading()\n");
    send(new WiseSensorMessage("Sample request", SAMPLE), "toSensorDeviceManager");
}

// A function used to send control messages
void WiseBaseApplication::toMobilityManager(cMessage * msg)
{
	if (msg->getKind() == APPLICATION_PACKET)
		opp_error("toNetworkLayer() function used incorrectly to send APPLICATION_PACKET without destination Network address");
	send(msg, "toMobilityManager");
}

void WiseBaseApplication::toNetworkLayer(cMessage * msg) {
	if (msg->getKind() == APPLICATION_PACKET)
        	throw cRuntimeError("toNetworkLayer() function used incorrectly to send "
        			"APPLICATION_PACKET without destination Network address");
	send(msg, "toCommunicationModule");
}

void WiseBaseApplication::toNetworkLayer(cPacket* pkt, const char* dst, double delay) {
	WiseApplicationPacket* appPkt = check_and_cast<WiseApplicationPacket*>(pkt);
	appPkt->getAppNetInfoExchange().destination = string(dst);
	appPkt->getAppNetInfoExchange().source = string(SELF_NETWORK_ADDRESS);
	appPkt->setApplicationID(applicationID.c_str());
	appPkt->getAppNetInfoExchange().timestamp = simTime();
	//trace() << "teste2 " << appPkt->getByteLength();
	if (delay == 0)
		send(appPkt, "toCommunicationModule");
	else 
		sendDelayed(appPkt, delay, "toCommunicationModule");
}

void WiseBaseApplication::sendDirectApplicationMessage(WiseApplicationPacket *pkt, const char *dst, unsigned int type) {
    int idx = atoi(dst);
    AppNetInfoExchange_type &ctrl = pkt->getAppNetInfoExchange();
    ctrl.source = string(SELF_NETWORK_ADDRESS);
    ctrl.destination = string(dst);
    ctrl.timestamp = simTime();
    pkt->setType(type);
    trace() << "SEND_DIRECT_APP: to node=" << idx;
    if (idx >= 0) {
        if (idx == self) {
            // Cannot send to myself
            delete pkt;
            return;
        }
        send(pkt, "toApplicationDirect", idx);
        return;
    }
    // To all application modules
    for (int i = 0; i < gateSize("toApplicationDirect"); i++) {
        if (i == self) // DON'T send to myself
            continue;
        send(pkt->dup(), "toApplicationDirect", i);
    }
    delete pkt;
}

void WiseBaseApplication::processAppPacket(WiseApplicationPacket* pkt) {
    switch (pkt->getType()) {
        case WISE_APP_NORMAL:{
	    //trace() << "normalAppPacket";
            normalAppPacket(pkt);
            break;
	}
        case WISE_APP_DIRECT_COMMUNICATION:{
	    //trace() << "handleDirectApplicationMessage";
            handleDirectApplicationMessage(pkt);
            break;
	}
        default:{
            trace() << "Uknown WiseApplicationPacketType = " << pkt->getType();
            break;
	}
    }
}

void WiseBaseApplication::normalAppPacket(WiseApplicationPacket *pkt) {
    AppNetInfoExchange_type ctl = pkt->getAppNetInfoExchange();
    last_packet_latency = SIMTIME_DBL(simTime() - ctl.timestamp);
    if (latencyMax > 0 && latencyBuckets > 0) {
        double l = 1000 * last_packet_latency; // in [ms]
        collectHistogram("Application level latency, in ms", l);
    }
    fromNetworkLayer(pkt, ctl.source.c_str(), ctl.RSSI, ctl.LQI);
}

WiseApplicationPacket* WiseBaseApplication::createGenericDataPacket(double data, unsigned int seqNum, int size)
{
	WiseApplicationPacket *newPacket = new WiseApplicationPacket("App generic packet", APPLICATION_PACKET);
	newPacket->setSource(self);
	newPacket->setData(data);
	newPacket->setSequenceNumber(seqNum);
	if (size > 0) 
		newPacket->setByteLength(size);
	return newPacket;
}

void WiseBaseApplication::readParameter() {
    /* Get a valid references to the objects of the Resources Manager module
     * the Mobility module and the radio module, so that we can make direct
     * calls to their public methods.*/
    cModule *parent = getParentModule();

    if (parent->findSubmodule("ResourceManager") != -1) {
        resMgrModule = check_and_cast<ResourceManager*>(parent->getSubmodule("ResourceManager"));
    } else {
        throw cRuntimeError("Error in geting a valid reference to ResourceManager "
        "for direct method calls.");
    }

    if (parent->findSubmodule("MobilityManager") != -1) {
        mobilityModule = check_and_cast<VirtualMobilityManager*>(parent->getSubmodule("MobilityManager"));
    } else {
        throw cRuntimeError("Error in geting a valid reference to MobilityManager "
        "for direct method calls.");
    }
    if (parent->findSubmodule("SensorManager") != -1) {
        sensor = check_and_cast<CritSensor*>(parent->getSubmodule("SensorManager"));
    } else {
        throw cRuntimeError("Error in geting a valid reference to MobilityManager "
        "for direct method calls.");
    }
    // we make no checks here
    radioModule = check_and_cast<Radio*>(parent->getSubmodule("Communication")->getSubmodule("Radio"));

    self = parent->getIndex();
    if (self < 0)
        throw cRuntimeError("Negative node index, very bad!");
    n_nodes = parent->getParentModule()->par("numNodes");
    cpuClockDrift = resMgrModule->getCPUClockDrift();
    setTimerDrift(cpuClockDrift);
    disabled = 1;

    applicationID = par("applicationID").stringValue();
    priority = par("priority");
    synchronizedStartup = par("synchronizedStartup");
    packetHeaderOverhead = par("packetHeaderOverhead");
    constantDataPayload = par("constantDataPayload");
    isSink = hasPar("isSink") ? par("isSink") : false;
    stringstream out;
    out << self;
    selfAddress = out.str();
}

int WiseBaseApplication::getGopSize() {
	 return sensor->getGopSize();
}
