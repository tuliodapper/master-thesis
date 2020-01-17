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

#ifndef __WiseCameraManager_h__
#define __WiseCameraManager_h__

#include <string>
#include <vector>
#include <map>
#include "WiseBaseSensorManager.h"
#include "WiseCameraMessage_m.h"
#include "WiseMovingTargetMessage_m.h"
#include "WiseCameraHandler.h"

class WiseCameraManager: public WiseBaseSensorManager {
private:
	// NOTE: for more camera per sensor this shall become a vector. Sure ?
	WiseCameraInfo cam_info;
	std::map<std::string, wise::CameraHandler*> handlers;

public:
	virtual ~WiseCameraManager();
	inline const WiseCameraInfo& get_camera_info() const {return cam_info;};
	inline unsigned get_num_processes() const { return numProcesses; };

    virtual bool covers(double x, double y);

protected:
	virtual void initialize();
	virtual void startup();
	virtual void handleSample(WisePhysicalProcessMessage *msg);
	virtual void processCommandRequest(WiseSensorManagerMessage *req);
	virtual void processSampleRequest(WiseSensorManagerMessage *req);
private:
	void init_fov_bb();
	inline void init_handlers();
	inline void delete_handlers();
	inline wise::CameraHandler *get_handler(const std::string&);
};

#endif // __WiseCameraManager_h__
