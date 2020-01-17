//****************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2010                         *
//*  Developed at the ATP lab, Networked Systems research theme              *
//*  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
//*  This file is distributed under the terms in the attached LICENSE file.  *
//*  If you do not find this file, copies can be found by writing to:        *
//*                                                                          *
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
//*      Attention:  License Inquiry.                                        *
//*                                                                          *  
//****************************************************************************/

enum MobilityManagerMessageType {
	MOBILITY_PERIODIC = 1;
	MOVEMENT	  = 2;
	COLIGATE 	  = 3;
	NOT_COLIGATE 	  = 4;
	PRE_COLIGATE 	  = 5;
}

message MobilityManagerMessage {
    int MobilePacketKind enum (MobilityManagerMessageType);
	double updateInterval;
	double xCoorDestination;
	double yCoorDestination;
	double zCoorDestination;
	double speed;
	double speed_final;
	double time_final;
	double time_final_final;
	double tamanho_mapa;
	double startVideo;
	int source;
	int appType; //Created by WELL
}
