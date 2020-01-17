// *****************************************************************************
//  Copyright (C): Christian Nastasi, 2011									
//  Author(s): Christian Nastasi											  
//  Developed at the:														 
//  - Retis LAB, TeCIP Institute, Scuola Superiore Sant'Anna (Pisa)		   
//  - School of Elec Eng and Computer Science, Queen Mary University (London) 
//  This file is distributed under the terms in the attached LICENSE_2 file.  
//  If you do not find this file, copies can be found by writing to:		  
//  - c.nastasi@sssup.it													  
//  - nastasichr@gmail.com													
//  - andrea.cavallaro@eecs.qmul.ac.uk										
// *****************************************************************************

cplusplus {{
        #include "ApplicationPacket_m.h"
	#include "Node.h"
}};

struct TraceInfo;

enum WiseSensorMessageType {
	SAMPLE = 110;
	UPDATE_COVERSETS = 111;
	ALERT = 112;
	SENSOR_READING = 114;
	MULTIMEDIA = 115;
	SENSING_RELEVANCE = 116;
};


message WiseSensorMessage {
	double payload;
	double sensedValue;
	int byteLength;
	string sensorType;
	int sensorIndex;
	TraceInfo info;
	int idFrame;

	double x;
	double y;
	int fecPkt [];
	int frame;
	int relevance;
}
