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

#include "WiseCameraApplication.h"
#include "WiseCameraManager.h"
#include "WiseDebug.h"

void WiseCameraApplication::startup() 
{
	WISE_DEBUG_3("WiseCameraApplication::startup()"); 

	cModule *m = getParentModule()->getSubmodule("SensorManager");
	WiseCameraManager *c = check_and_cast<WiseCameraManager*>(m);
	camera_info = c->get_camera_info();
	calculate_overlapping_fovs(camera_info);
	// todo: Generalize to non-bounding-box FOV type
	WiseCameraInfo::fov_bb_t fov;
	c->get_camera_info().get_fov_bb(fov);
}

void WiseCameraApplication::finishSpecific() 
{
	WISE_DEBUG_3("WiseCameraApplication::finishSpecific()"); 
}

void WiseCameraApplication::requestSensorReading(int index)
{
	WISE_DEBUG_3("WiseCameraApplication::requestSensorReading()");

	WiseCameraMessage *reqMsg;
	reqMsg = new WiseCameraMessage("application-to-WiseCameraManager"
				      " (sample request)", 
				      SENSOR_READING_MESSAGE);
	reqMsg->setSensorIndex(index);
	send(reqMsg, "toSensorDeviceManager");
}

void WiseCameraApplication::handleSensorReading(WiseSensorManagerMessage *msg) 
{
	WISE_DEBUG_3("WiseCameraApplication:handleSensorReading"
		     "(WiseSensorManagerMessage *msg)");
	WiseCameraMessage *cam_msg = check_and_cast<WiseCameraMessage*>(msg);
	handleSensorReading(cam_msg);
}


void WiseCameraApplication::calculate_overlapping_fovs(const WiseCameraInfo &a)
{
	// todo: Generalize to non-bounding-box FOV type
	WiseCameraInfo::fov_bb_t mine;
	a.get_fov_bb(mine);

	overlapping_fov_cameras.clear();
	// Access other nodes information (through Omnet's modules)
	for (int i = 0; i < n_nodes; i++) {
		if (i == self)	
			continue;
        	cModule *m = getParentModule()->getParentModule(); // m=SN
		m = m->getSubmodule("node", i)->getSubmodule("SensorManager");
		WiseCameraManager *c = check_and_cast<WiseCameraManager*>(m);
		WiseCameraInfo::fov_bb_t other;
		c->get_camera_info().get_fov_bb(other);
		// check overlapping condition (todo: generalize to non-box)
		if (other.min_x > mine.max_x || other.max_x < mine.min_x)
			continue;
		if (other.min_y > mine.max_y || other.max_y < mine.min_y)
			continue;
		ostringstream os;
		os << i;
		neighbour_cam_t i_info;
		i_info.node_id = os.str();
		i_info.cam_info = c->get_camera_info();
		overlapping_fov_cameras.push_back(i_info);
	}
}
