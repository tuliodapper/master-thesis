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

#ifndef __WiseCameraApplication_h__
#define __WiseCameraApplication_h__

#include "WiseBaseApplication.h"
#include "WiseCameraInfo.h"
#include "WiseCameraMessage_m.h"

/** 
* @brief Base class for camera-based application
* 
* This class is meant to be used as a base class for camera-based application,
* in particular when a WiseCameraManager is connected to the application module.
*
* The class hides the interaction with the WiseCameraManager and exports an
* overload (specialized) version of handleSensorReading callback.
*
* The class provides a mechanism to calculate the camera-node neighborhoud 
* based on the overlapping field-of-view of adjacent cameras.
*/
class WiseCameraApplication : public WiseBaseApplication {
protected:
	struct neighbour_cam_t {
		std::string node_id;
		WiseCameraInfo cam_info;
	};
protected:
	std::vector<neighbour_cam_t> overlapping_fov_cameras;
	WiseCameraInfo camera_info;

protected:
	virtual void startup(); 
	virtual void finishSpecific();

	void requestSensorReading(int index = 0);

	virtual void handleSensorReading(WiseSensorManagerMessage *);
	virtual void handleNetworkControlMessage(cMessage *) {};
	virtual void handleMacControlMessage(cMessage *) {};
	virtual void handleRadioControlMessage(RadioControlMessage *) {};

	virtual void handleSensorReading(WiseCameraMessage *) = 0;

private:
	void calculate_overlapping_fovs(const WiseCameraInfo&);
};

#endif //__WiseCameraApplication_h__
