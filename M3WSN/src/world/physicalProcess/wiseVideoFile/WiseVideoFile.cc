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

#include "WiseVideoFile.h"
#include "WiseDebug.h"
#include <sstream>

#define perr_app(m) opp_error("\n[WiseVideoFile]:\n ")
#define perr(m) opp_error(m)

using namespace std;

Define_Module(WiseVideoFile);

void WiseVideoFile::initialize()
{
	WiseBasePhysicalProcess::initialize();
	WISE_DEBUG_3("WiseVideoFile::initialize()");

	const char *str = par("file_name");
	file_name = str;
	frame_rate = par("frame_rate");
	use_multi_camera = par("allow_multiple_camera");
}

void WiseVideoFile::handleMessage(cMessage * msg)
{
	WISE_DEBUG_3("WiseVideoFile::handleMessage()");

	WiseVideoFileMessage *f_msg;
	switch (msg->getKind()) {
	case PHYSICAL_PROCESS_SAMPLING: 
		// NOTE: substitute the req message with a specific one
		WisePhysicalProcessMessage *s_msg;
		s_msg = check_and_cast <WisePhysicalProcessMessage*>(msg);
		WISE_DEBUG_2("WiseVideoFile: PHYSICAL_PROCESS_SAMPLING:");
		WISE_DEBUG_2("\tsrc_node = " << s_msg->getSrcID()); 
		WISE_DEBUG_2("\tsrc_sensor = " << s_msg->getSensorIndex());
		WISE_DEBUG_2("\tphyProc ID = " << self); 
		f_msg = new WiseVideoFileMessage("VideoFile Message", 
						 PHYSICAL_PROCESS_SAMPLING);
		if (s_msg->getSrcID() != self && !use_multi_camera) {
			f_msg->setValid(false);
			f_msg->setFileName("");
			f_msg->setFrameRate(0);
		} else {
			f_msg->setValid(true);
			f_msg->setFileName(file_name.c_str());
			f_msg->setFrameRate(frame_rate);
		}
		// Send reply back to the node which made the request
		send(f_msg, "toNode", s_msg->getSrcID());
		delete s_msg;
		s_msg = f_msg;
		break;
	default:
		perr_app("Received a message other than "
		         "PHYSICAL_PROCESS_SAMPLING");
		delete msg;
		break;
	}
}

void WiseVideoFile::finishSpecific()
{
	WISE_DEBUG_3("WiseVideoFile::finishSpecific()");
}
