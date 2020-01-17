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

#ifndef __WiseCameraHandler_h__
#define __WiseCameraHandler_h__

#include <map>
#include <string>
#include "WisePhysicalProcessMessage_m.h"
#include "WiseCameraMessage_m.h"

class WiseCameraManager;

namespace wise {

class CameraHandler {
public:
    virtual ~CameraHandler() {};

	class Factory {
	public:
		typedef CameraHandler* (*creator_t)(const WiseCameraManager&);
	private:
		static std::map<std::string, creator_t> *registry; 
	public:
		Factory(const std::string&, creator_t);
		static CameraHandler* create(const std::string&, 
					     const WiseCameraManager&);
	};
protected:
	const WiseCameraManager &manager;
protected:
	CameraHandler(const WiseCameraManager& m) : manager(m) {}
public:
	virtual void initialize() = 0;
	virtual WiseCameraMessage *process(WisePhysicalProcessMessage*) = 0;
};

} // namespace wise

#endif // __WiseCameraHandler_h__
