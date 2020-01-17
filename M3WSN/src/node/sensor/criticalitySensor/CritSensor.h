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

#ifndef CRITICALITYSENSOR_H
#define CRITICALITYSENSOR_H

#include <set>
#include "Point.h"
#include "Triangle.h"
#include "ResourceManager.h"
#include "Radio.h"
#include "VirtualMobilityManager.h"
#include "WiseSensorMessage_m.h"
#include "WiseApplicationPacket_m.h"
#include "WiseBaseTerrain.h"
#include "reed-solomon.h"

using namespace std;

class WiseBaseApplication;

#define MAX_COVERSET_TABLESIZE 10
#define ALERTED_NODE_CRITICALITY_LEVEL 0.6

class CritSensor: public CastaliaModule {

public:
    CritSensor();
    virtual ~CritSensor();

    virtual bool covers(double x, double y) = 0;
    bool isDisabled() { return disabled; };
    virtual int getGopSize(){return gopSize;};
    virtual bool getIsSink(){return isSink;};

protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage* msg);
    virtual void computeCaptureRate();
    virtual void computeCoverSets() = 0;
    virtual void handleSample(cMessage* msg) = 0;
    virtual void handleSensorMessage(cMessage *) {};
    void handleSelfMessage(cMessage* msg);
    bool isSink;
    void parseStringParams();

    void dinamicIncreaseOfCriticality();
    void increaseCriticality();

    double criticality, criticalityDecreaseTimeout;
    double minCaptureRate, maxCaptureRate, currentCaptureRate;
    int numCoverSets, maxNumCoverSets;
    double energyPerSample;
    double fov_radius;

    cMessage* criticalityTimeout, *takeSample;

    simsignal_t captureRateSignal, criticalitySignal, coverSetsSignal;

    WiseBaseApplication* app;
    ResourceManager* resMgrModule; //a pointer to the object of the Resource Manager Module
    VirtualMobilityManager* mobilityModule; //a pointer to the mobilityModule object
    bool disabled;
    double cpuClockDrift;

    double maxCriticalityLevel;
    int nbReinforcementMsgRec;
    int nMSG;
    vector<string> sensorTypes;

	vector<int> corrPhyProcess;
	vector<double> pwrConsumptionPerDevice;
	vector<simtime_t> minSamplingIntervals;
	vector<double> sensorBiasSigma;
	vector<double> sensorNoiseSigma;
	vector<double> sensorSensitivity;
	vector<double> sensorResolution;
	vector<double> sensorSaturation;
	int totalSensors;
	vector<simtime_t> sensorlastSampleTime;
	vector<double> sensorLastValue;
	vector<double> sensorBias;

    WiseBaseTerrain *terrain;
    WiseCameraInfo caminfo;
    ReedSolomonFec reed;

    int self; // node ID
    double threshold;

    /**
      * Stores information on a video stream
    */
	
    struct tracerec {
	simtime_t trec_time; /* inter-packet time (usec) */
	u_int32_t trec_size; /* frame size (bytes) */
	char trec_type; /* packet type */
	u_int32_t trec_prio; /* packet priority */
	u_int32_t trec_id; /* maximun fragmented size (bytes) */
    };

    TraceInfo dataTracer;
    unsigned int max, ndx, a_, fullPkt, restPkt, i, nrec, id, contFrm, contSd, nLin, nLT;
    ofstream sdTrace;
    ofstream rdTrace;
    const char *fileIn;
    int fecType;
    double fecRedundancy;
    double pRedundancy;
    int gopSize;

    struct tracerec *tracer;
    struct tracerec trec_;
    cMessage* processTrace;
    int last_rand_id;
    int pos_frame;
    int uid;
};

#endif /* CRITICALITYSENSOR_H_ */
