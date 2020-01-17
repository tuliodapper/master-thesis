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
//*  	Module: Bonnmotion: a mobility scenario generation and analysis tool	*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Claudio Silva <claudio@ufpa.br>				*
//*		 Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/

package castalia.node.mobilityManager.platoonMobilityManager;

simple PlatoonMobilityManager like castalia.node.mobilityManager.iMobilityManager {
 parameters:
	bool collectTraceInfo = default (false);
	bool collectOutputInfo = default (false);
	bool isMobile = default (false);
	double updateInterval = default (1.0);
	double energyToFly = default (0.0);
	string mobilityFile = default ("");
	//double ganho = default(2);
 gates:
 	output toApplicationModule;
	input fromApplicationModule;
}
