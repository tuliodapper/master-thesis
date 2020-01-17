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

package wise_mnet.node.sensor.criticalitySensor;

simple CritSensor like wise_mnet.node.sensorManager.WiseBaseSensorManager {
    
parameters:
    bool collectTraceInfo = default(false);
    bool collectOutputInfo = default (false);
    int numSensingDevices = default (1);
    string maxSampleRates = default ("1");
    double fov_bb_width = default (16.0);
    double fov_bb_height = default (12.0);
    double fov_bb_radius = default (1.0);
    double criticality = default(0.5);
    double criticalityDecreaseTimeout @unit(s) = default(20s);
    double initialCaptureRate = default(1);
    double maxCaptureRate = default(10);
    double minCaptureRate = default(0.1);
    double energyPerSample = default(0.2); // in Joule
    int maxNumCoverSets = default(10);
    string sensorTypes = default ("multimedia");
    double threshold = default(40);
    int fecType = default(0); //0 - without FEC 
			      //1 - standard FEC
			      //2 - QoE-aware FEC
    double fecRedundancy = default(1); 
    double pRedundancy = default(1); 
    int gopSize = default(30);

    string corrPhyProcess = default ("0");		//holds the indexes of the corresponding phy processes for
												//each sensor (usually it should be : "0 1 2 3")
	string pwrConsumptionPerDevice = default ("0.02");
	string devicesBias = default ("0.1");		//If the output signal is not zero when the measured property is zero
	string devicesNoise = default ("0.1");		//random deviation of the signal that varies in time
	string devicesSensitivity = default ("0");	//holds the minimum value which can be sensed by each sensing device.
	string devicesResolution = default ("0.001");	//holds the sensing resolution for each device 
													//(the returned value will always be a multiple of 
													//number, given here)
	string devicesSaturation = default ("1000");	//holds the saturation value for each sensing device
    
    @statistic[capturerate](title="Sensor capture rate"; record=vector);
    @statistic[criticality](title="Criticality"; record=vector);
    @statistic[coversets](title="Number of cover sets"; record=vector);

gates:
    output toApplicationModule;
    output toNodeContainerModule[];
    input fromApplicationModule;
    input fromNodeContainerModule[];
    input fromResourceManager;
}
