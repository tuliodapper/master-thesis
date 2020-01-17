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
	TARGET_POSITION	 = 1,
	MOVE		     = 2,
	COLLIGATE        = 3,
	NOTCOLLIGATE     = 4,
	PRE_COLLIGATE    = 5,
	WINGO_STATISTICS = 7,

};

struct conctedData{
    int idNodo;
    int good;
    int conected;
    int lastTime;
};

class PlatoonMobilityManager: public VirtualMobilityManager {
 private:
	/*--- The .ned file's parameters ---*/
	bool isMobile;
	double updateInterval;
	double updateIntervalTargetPoint;
	//bool coligar = false;
	int coligar;
	int incrementador_coligacao_x, incrementador_coligacao_y, incrementador_coligacao_z;
	float atraso, atrasoTotal; //variavel contadora para o atraso
	double tempIni, posXIni, posYIni, posZIni, tempSub, disSub, velIni; //Variavel para guardar valores anteriores

	double last_speed, acceleration;
	double co2emission, totalCO2Emission; //Variaveis de emissao de CO2
	double fuel, totalFuel;

	double current_x, current_y, current_z;
	double dest_x, dest_y, dest_z;
	double temp_x, temp_y, temp_z;
	double incr_x, incr_y, incr_z;
	double distance;
	double startTime, endTime, difTime;
	double tempo_atual_simulacao;
    double x_final, y_final, endTime_final;

	double atraso_viajem;

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
	BonnMotionMovements auxMovements2;
	list<BonnMotionMovements> nodePosition;
	list<BonnMotionMovements>::iterator it;
	list<BonnMotionMovements>::iterator last_it;

	VirtualMobilityManager* location;
	vector <conctedData> statistics;
	int idTimer;

 protected:
	void initialize();
	void finish();
	void handleMessage(cMessage * msg);
	void fromApplicationLayer(cMessage * msg) ;
	/*--- BonnMotion ---*/
	/* identification, archive of movements, list created to each movements */
	void getMovements( int, const char *, list<BonnMotionMovements> &);

	double calculateCO2emission(double speed, double acceleration) const; //Calcula a emisao de CO2
	double calculateFuelRate(double speed, double acceleration) const; //Calcula o gasto de combustivel

	cMessage* traceMobility;
	cMessage* targetPosition;
	cMessage* targetColigation;
};

#endif
