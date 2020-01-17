//*******************************************************************************
//*	Copyright (c) 2013. Federal University of Para (UFPA), Brazil and 	*
//*			    University of Bern (UBern), Switzerland		*
//*	Developed by Research Group on Computer Network and Multimedia		*
//*	Communication (GERCOM) of UFPA in collaboration to Communication and 	*
//*	Distributed Systems (CDS) research group of UBern.			*
//*	All rights reserved							*
//*										*
//*	Permission to use, copy, modify, and distribute this protocol and its	*
//*	documentation for any purpose, without fee, and without written		*
//*	agreement is hereby granted, provided that the above copyright notice,	*
//*	and the author appear in all copies of this protocol.			*
//*										*
//*  	Module: Sensor application 						*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*		 Zhongliang Zhao <zhao@iam.unibe.ch>				*
//*										*
//******************************************************************************/ 

#include "WisePhysicalProcessMessage_m.h"
#include "CritSensor.h"
#include "WiseBaseApplication.h"

CritSensor::CritSensor() : criticality(0), criticalityDecreaseTimeout(0), minCaptureRate(0), 
     maxCaptureRate(0), currentCaptureRate(0), numCoverSets(0), maxNumCoverSets(0),
     energyPerSample(0), criticalityTimeout(0), takeSample(0)  {

}

CritSensor::~CritSensor() {
}

/**
 * TODO implement possibility for constant frame rate for comparison
 */
void CritSensor::computeCaptureRate() {
    #ifndef CONSTANT_FRAMERATE
    	currentCaptureRate = minCaptureRate;
    	int X = numCoverSets + 1 >= maxNumCoverSets ? maxNumCoverSets : numCoverSets + 1;
    	// Bezier behavior, see WCNC'10 paper
    	if (numCoverSets >= 0) {
            double hx = maxNumCoverSets;
            double hy = maxCaptureRate;

            if (criticality < 0)
            	criticality = 0;

            double bx = -hx * criticality + hx;
            double by = hy * criticality;

            double aX = (-bx + sqrt(bx * bx - 2 * bx * X + hx * X)) / (hx - 2 * bx);

            if ((hx - 2 * bx) == 0) {
            	currentCaptureRate = ((hy - 2 * by) / (4 * bx * bx)) * X * X + (by / bx) * X;
            } else {
            	currentCaptureRate = (hy - 2 * by) * aX * aX + 2 * by * aX;
            }

            if (currentCaptureRate < minCaptureRate)
              	currentCaptureRate = minCaptureRate;
     	}
    	emit(captureRateSignal, currentCaptureRate);
    #endif
    //trace() << "computed capture rate " << currentCaptureRate;
}

/**
 *
 */
void CritSensor::initialize() {
    CastaliaModule::initialize();

    cModule* mod = getParentModule()->getModuleByRelativePath("Application");
    app = check_and_cast<WiseBaseApplication*>(mod);

    mod = getParentModule()->getModuleByRelativePath("MobilityManager");
    mobilityModule = check_and_cast<VirtualMobilityManager*>(mod);

    mod = getParentModule()->getParentModule()->getSubmodule("wiseTerrain");
    terrain = check_and_cast<WiseBaseTerrain*>(mod);

    self = getParentModule()->getIndex();

    threshold = par("threshold");
    fov_radius = par("fov_bb_radius");
    criticality = par("criticality");
    criticalityDecreaseTimeout = par("criticalityDecreaseTimeout");
    minCaptureRate = par("minCaptureRate");
    maxCaptureRate = par("maxCaptureRate");
    currentCaptureRate = par("initialCaptureRate");
    energyPerSample = par("energyPerSample");
    maxNumCoverSets = par("maxNumCoverSets");
    fecType = par("fecType");
    fecRedundancy = par("fecRedundancy");
    pRedundancy = par("pRedundancy");
    gopSize = par("gopSize");
    captureRateSignal = registerSignal("capturerate");
    criticalitySignal = registerSignal("criticality");
    coverSetsSignal = registerSignal("coversets");
    const char *parameterStr, *token;

    parseStringParams();

    //get the type of each sensor device (just a description e.g.: light, intruder, multimedia etc.)
    sensorTypes.clear();
    parameterStr = par("sensorTypes");
    cStringTokenizer typesTokenizer(parameterStr);
    while ((token = typesTokenizer.nextToken()) != NULL) {
	string sensorType(token);
	sensorTypes.push_back(sensorType);
	trace() << "Node " << self << " equiped with " << sensorTypes[0] << " sensor";
    }

    takeSample = new cMessage("take sample");
    scheduleAt(simTime() + (1 / currentCaptureRate), takeSample); //of kind cMessage*

    if (sensorTypes[0] != "intruder"){
    	criticalityTimeout = new cMessage("criticality timeout"); //of kind cMessage*
    	scheduleAt(simTime() + criticalityDecreaseTimeout, criticalityTimeout);
    }

    nMSG = 0;
    nbReinforcementMsgRec = 0;
    maxCriticalityLevel = 1;
    processTrace = new cMessage("process trace", 11);
}


void CritSensor::parseStringParams(){
	const char *parameterStr, *token;
	simtime_t sampleInterval;

	//get the physical process index that each sensor device is monitoring
	corrPhyProcess.clear();
	parameterStr = par("corrPhyProcess");
	cStringTokenizer phyTokenizer(parameterStr);
	while ((token = phyTokenizer.nextToken()) != NULL)
		corrPhyProcess.push_back(atoi(token));

	//get the power consumption of each sensor device
	pwrConsumptionPerDevice.clear();
	parameterStr = par("pwrConsumptionPerDevice");
	cStringTokenizer pwrTokenizer(parameterStr);
	while ((token = pwrTokenizer.nextToken()) != NULL)
		pwrConsumptionPerDevice.push_back(((double)atof(token)) / 1000.0f);

	//get the samplerate for each sensor device and calculate the minSamplingIntervals 
	//(that is every how many ms to request a sample from the physical process)
	minSamplingIntervals.clear();
	parameterStr = par("maxSampleRates");
	cStringTokenizer ratesTokenizer(parameterStr);
	while ((token = ratesTokenizer.nextToken()) != NULL) {
		sampleInterval = (double)(1.0f / atof(token));
		minSamplingIntervals.push_back(sampleInterval);
	}

	//get the type of each sensor device (just a description e.g.: light, temperature etc.)
	sensorTypes.clear();
	parameterStr = par("sensorTypes");
	cStringTokenizer typesTokenizer(parameterStr);
	while ((token = typesTokenizer.nextToken()) != NULL) {
		string sensorType(token);
		sensorTypes.push_back(sensorType);
	}

	// get the bias sigmas for each sensor device
	sensorBiasSigma.clear();
	parameterStr = par("devicesBias");
	cStringTokenizer biasSigmaTokenizer(parameterStr);
	while ((token = biasSigmaTokenizer.nextToken()) != NULL)
		sensorBiasSigma.push_back((double)atof(token));

	// get the bias sigmas for each sensor device
	sensorNoiseSigma.clear();
	parameterStr = par("devicesNoise");
	cStringTokenizer noiseSigmaTokenizer(parameterStr);
	while ((token = noiseSigmaTokenizer.nextToken()) != NULL)
		sensorNoiseSigma.push_back((double)atof(token));

	sensorSensitivity.clear();
	parameterStr = par("devicesSensitivity");
	cStringTokenizer sensitivityTokenizer(parameterStr);
	while ((token = sensitivityTokenizer.nextToken()) != NULL)
		sensorSensitivity.push_back((double)atof(token));

	sensorResolution.clear();
	parameterStr = par("devicesResolution");
	cStringTokenizer resolutionTokenizer(parameterStr);
	while ((token = resolutionTokenizer.nextToken()) != NULL)
		sensorResolution.push_back((double)atof(token));

	sensorSaturation.clear();
	parameterStr = par("devicesSaturation");
	cStringTokenizer saturationTokenizer(parameterStr);
	while ((token = saturationTokenizer.nextToken()) != NULL)
		sensorSaturation.push_back((double)atof(token));

	totalSensors = par("numSensingDevices");

	int totalPhyProcesses = gateSize("toNodeContainerModule");

	//check for malformed parameter string in the omnet.ini file
	int aSz, bSz, cSz, dSz, eSz, fSz, mSz, rSz, sSz;
	aSz = (int)pwrConsumptionPerDevice.size();
	bSz = (int)minSamplingIntervals.size();
	cSz = (int)sensorTypes.size();
	dSz = (int)corrPhyProcess.size();
	eSz = (int)sensorBiasSigma.size();
	fSz = (int)sensorNoiseSigma.size();
	mSz = (int)sensorSensitivity.size();
	rSz = (int)sensorResolution.size();
	sSz = (int)sensorSaturation.size();

	if ((totalPhyProcesses < totalSensors) || (aSz != totalSensors)
	    || (bSz != totalSensors) || (cSz != totalSensors)
	    || (dSz != totalSensors) || (eSz != totalSensors)
	    || (fSz != totalSensors) || (mSz != totalSensors)
	    || (rSz != totalSensors) || (sSz != totalSensors))
		opp_error("\n[Sensor Device Manager]: The parameters of the sensor device manager are not initialized correctly in omnet.ini file.");
}

/**
 *
 */
void CritSensor::finish() {
    CastaliaModule::finish();
    cancelAndDelete(criticalityTimeout);
    cancelAndDelete(takeSample);
    cancelAndDelete(processTrace);
}

/**
 *
 */
void CritSensor::handleMessage(cMessage* msg) {
	if ( msg->isSelfMessage() ) {
		//trace() << "self msg " << msg << " kind " << msg->getKind();
		handleSelfMessage(msg);
		return;
	}

	if ( msg->isSelfMessage() ) {
		//trace() << "self msg " << msg << " kind " << msg->getKind();
        	handleSelfMessage(msg);
		return;
	}

	if ( msg->getKind() == DESTROY_NODE ) {
      	  disabled = true;
		if (sensorTypes[0] == "intruder"){
			cancelEvent(takeSample);
		} else {
			cancelEvent(criticalityTimeout);
		}
		trace() << "DESTROY_NODE " << msg << " kind " << msg->getKind();
	        delete msg;
        	return;
	}

	if ( msg->getKind() == PHYSICAL_PROCESS_SAMPLING ) {
		//trace() << "PHYSICAL_PROCESS_SAMPLING";
		handleSample(msg);
		return;
	}

	if ( msg->getKind() == PHYSICAL_PROCESS_SCALAR_DATA ) {
		//trace() << "PHYSICAL_PROCESS_SCALAR_DATA";
		handleSample(msg);
		return;
		}

	if ( msg->getKind() == MOBILE_MESSAGE ) {
		//trace() << "MOBILE_MESSAGE";
		handleSample(msg);
		return;
	}

	if ( msg->getKind() == SENSOR_READING_MESSAGE ) {
		trace() << "SENSOR_READING_MESSAGE - msg kind " << msg->getKind();
		handleSample(msg);
		return;
	}

	if ( msg->getKind() == MULTIMEDIA_REQUEST_MESSAGE ) {
		trace() << "MULTIMEDIA_REQUEST_MESSAGE - msg kind " << msg->getKind();
		handleSample(msg);
		//handleSensorMessage(msg);
		return;
	}

	WiseSensorMessage* sm = dynamic_cast<WiseSensorMessage*>(msg);
	if (sm == 0) {
		delete msg;
		return;
	}

	switch (msg->getKind()) {
		case UPDATE_COVERSETS: // if neighborhood changes
			computeCoverSets();
			emit(coverSetsSignal, numCoverSets);
			break;
		case ALERT:
			trace() << "critc sensor - dinamicIncreaseOfCriticality";
			dinamicIncreaseOfCriticality();
			break;
		default:
			break;
	}
	delete msg;
}


/**
 *
 */
void CritSensor::handleSelfMessage(cMessage* msg) {
    if ( msg == criticalityTimeout) {
	criticality = par("criticality");
        nMSG = 0;
	nbReinforcementMsgRec = 0;
        emit(criticalitySignal, criticality);
        computeCaptureRate();
	scheduleAt(simTime() + criticalityDecreaseTimeout, msg);
	//trace() << "Criticality reduced to " << criticality;
    } else if ( msg == takeSample ) {
	//trace() << "takeSample ";
	if (sensorTypes[0] == "intruder"){
            double energy = energyPerSample + uniform(0, 1) * (energyPerSample / 100);
	    //powerDrawn(energy);
	}
        int numSensors = getParentModule()->getParentModule()->par("numPhysicalProcesses");

        for ( int i = 0; i < numSensors; i++ ) {
            WisePhysicalProcessMessage* m = new WisePhysicalProcessMessage("Request sample", PHYSICAL_PROCESS_SAMPLING);
            m->setSrcID(self);
            send(m, "toNodeContainerModule", i);
        }
	//trace() << "takeSample at timmer " << 1 / currentCaptureRate;
        scheduleAt(simTime() + (1 / currentCaptureRate), takeSample);
    } else if (msg->getKind() == 10){ //SETUP_TRACE timmer
	handleSensorMessage (msg);
    } else if (msg->getKind() == 11){//PROCESS_TRACE timmer
	handleSensorMessage (msg);
    }
}

/**
 *
 */
void CritSensor::increaseCriticality() {
    criticality = maxCriticalityLevel;
    computeCaptureRate();
    cancelEvent(criticalityTimeout);
    scheduleAt(simTime() + criticalityDecreaseTimeout, criticalityTimeout);
    //trace() << "Increase the criticality to " << criticality;
}


/**
 *
 */
void CritSensor::dinamicIncreaseOfCriticality() {
    // use step increase of 0.1 and needs 2 msg to move from 0.6 to 0.7 for instance
    // moving from 0.4 to 0.5 needs 2 more msg
    // moving from 0.5 to 0.6 needs 2 more msg
    // moving from 0.6 to 0.7 needs 2 more msg
    // moving from 0.7 to 0.8 needs 2 more msg
    // moving from 0.8 to 0.9 needs 2 more msg
    nbReinforcementMsgRec++; 
    nMSG++;
    double newCriticalityLevel;
    if (nbReinforcementMsgRec >= (int) ((maxCriticalityLevel - ALERTED_NODE_CRITICALITY_LEVEL) * 20))
	newCriticalityLevel = maxCriticalityLevel;
    else
	newCriticalityLevel = criticality + (nMSG >> 1) * 0.1;
    criticality = newCriticalityLevel;

    if (nMSG == 2)
	nMSG = 0;

    computeCaptureRate();
    cancelEvent(criticalityTimeout);
    scheduleAt(simTime() + criticalityDecreaseTimeout, criticalityTimeout);
    //trace() << "dinamic increase the criticality to " << criticality << "\n";
}


