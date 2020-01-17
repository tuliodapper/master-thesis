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
//*  	Module: sender trace creation to Evalvid				*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/ 


package wise_mnet.node.sensor.criticalitySensor;

simple VideoSensor extends CritSensor {
    
parameters:
    @class(VideoSensor);
    double aov = default(30); // angle of view
    double dov = default(45); // depth of view
    double lineOfSight = default(-1); // direction of camera in radians
    bool isSink = default (false);
    string video;
    int max_fragmented_size = default(1024);
}
