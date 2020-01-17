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

#ifndef _MOBILITYMODULE_H_
#define _MOBILITYMODULE_H_

#include <fstream>
#include <list>
#include <iterator>
#include "MobilityManagerMessage_m.h"
#include "VirtualMobilityManager.h"
#include "ResourceManager.h"
#include "CastaliaModule.h"

using namespace std;

enum mobilityTimers {
	TARGET_POSITION	= 1,
	MOVE		= 2,
};

class BonnMotionMobilityManager: public VirtualMobilityManager {
 private:
	/*--- The .ned file's parameters ---*/
	bool isMobile;
	double updateInterval;
	double updateIntervalTargetPoint;

	double current_x, current_y, current_z;
	double dest_x, dest_y, dest_z;
	double temp_x, temp_y, temp_z;
	double incr_x, incr_y, incr_z;
	double distance;
	double startTime, endTime, difTime;
	double vx, vy, vz; //vectorial velocity //Lari
	bool vecSpeed; // if it will use it or not //L
    //struct velocity{
     //       double x;
       //     double y;
         //   double z;
       // }; //LARI

	/*--- boonMotion struct ---*/
	struct BonnMotionMovements {
	    double startTime;
	    double endTime;
	    double posX;
	    double posY;
	    double posZ;
	};

	const char *mobilityFile;
	BonnMotionMovements auxMovements;
	list<BonnMotionMovements> nodePosition;
	list<BonnMotionMovements>::iterator it;

 public: //Lari

    double getVX();
    double getVY();
    double getVZ();
    double getdifTime();
    NodeLocation getDestination();

 protected:
	void initialize();
	void finish();
	void handleMessage(cMessage * msg);
	void fomApplicationLayer(cMessage *);
	NodeLocation nodeDestination;

	/*--- BonnMotion ---*/
	/* identification, archive of movements, list created to each movements */
	void getMovements( int, const char *, list<BonnMotionMovements> &);

	cMessage* traceMobility;
};

#endif
