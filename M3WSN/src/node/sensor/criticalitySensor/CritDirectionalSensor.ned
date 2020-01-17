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
//*  	Module: receiver trace creation to QoE-aware FEC mechanism		*
//*										*
//*	Ref.:	Z. Zhao, T. Braun, D. Ros´ario, E. Cerqueira, R. Immich, and 	*
//*		M. Curado, “QoE-aware FEC mechanism for intrusion detection in 	*
//*		multi-tier wireless multimedia sensor networks,” 		*
//*		in Proceedings of the 1st International Workshop on Wireless 	*
//*		Multimedia Sensor Networks (WiMob’12 WSWMSN), Barcelona, Spain,	*
//*		oct. 2012, pp. 697–704.						*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*		 Zhongliang Zhao <zhao@iam.unibe.ch>				*
//*										*
//******************************************************************************/ 


package wise_mnet.node.sensor.criticalitySensor;

simple CritDirectionalSensor extends CritSensor {
    
parameters:
    @class(CritDirectionalSensor);
    double aov = default(0.314159); // angle of view
    double dov = default(15); // depth of view
    double lineOfSight = default(-1); // direction of camera in radians
    bool isSink = default (false);
    string video;
    int max_fragmented_size = default(1024);
}
