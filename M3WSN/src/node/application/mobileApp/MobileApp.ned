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
//*  	Module: Application to camera nodes with node mobility and QoE-aware FEC*
//*										*
//*	Ref.:	Z. Zhao, T. Braun, D. Rosário, E. Cerqueira, R. Immich, and 	*
//*		M. Curado, “QoE-aware FEC mechanism for intrusion detection in 	*
//*		multi-tier wireless multimedia sensor networks,” 		*
//*		in Proceedings of the 1st International Workshop on Wireless 	*
//*		Multimedia Sensor Networks (WiMob’12 WSWMSN), Barcelona, Spain,	*
//*		oct. 2012, pp. 697–704.						*
//*										*
//*  	Version: 2.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/ 

package wise_mnet.node.application.mobileApp;

simple MobileApp like 
    wise_mnet.node.application.wiseCameraApplication.WiseCameraApplication {

parameters:
    @class(MobileApp);
    bool synchronizedStartup = default (true);
    bool neighbourDiscover = default (true);
    bool showCamImage = default (true);
    string applicationID = default ("CriticalityApp");
    bool collectTraceInfo = default (false);
    bool collectOutputInfo = default (false);
    int priority = default (1);
    int maxAppPacketSize = default (30);    // in bytes
    int packetHeaderOverhead = default (8); // in bytes
    int constantDataPayload = default (2);  // in bytes
    double msgInterval = default (0.5);
    int dimension = default(200);

    bool checkConnected         = default (false);
    bool sendDataPackets        = default (false);
    bool moveRandomPositions    = default (false);
    string loadPath             = default ("");
    double packet_rate          = default (0);  // packets per second, by default we transmit no packets
    double startupDelay         = default (0);  // delay in seconds before the app stars producing packets
    int firstUserNodeId         = default(1);
    int numUserNodes            = default(0);
    double simTimeLimit         = default(0.0);
    
gates:
    output toApplicationDirect[];
    input fromApplicationDirect[];
    output toCommunicationModule;
    output toSensorDeviceManager;
    output toMobilityManager;
    input fromMobilityManager;
    input fromCommunicationModule;
    input fromSensorDeviceManager;
    input fromResourceManager;
}
