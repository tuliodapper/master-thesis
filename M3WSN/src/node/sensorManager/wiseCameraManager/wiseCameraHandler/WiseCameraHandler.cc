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

#include "WiseCameraHandler.h"
#include "WiseCameraManager.h"
#include "WiseException.h"

using namespace std;
using namespace wise;

map<string, CameraHandler::Factory::creator_t> 
				*CameraHandler::Factory::registry = NULL;

CameraHandler::Factory::Factory(const string &name, 
				CameraHandler::Factory::creator_t func)
{
	if (!registry)
		registry = new map<string, creator_t>();
        if (registry->find(name) != registry->end()) {
		string s = "\nwcns::CameraHandler::Factory : "
			   "ERROR calling Factory(...) :\nAttempt to register a"
			   " concrete CameraHandler creator "
			   "with a name already in the registry (name = '";
		s += name + "').\n"; 
                throw Exception(s);
	}
        registry->insert(make_pair(name, func));
}

CameraHandler* CameraHandler::Factory::create(const string &name, 
					      const WiseCameraManager &m)
{
	if (!registry) {
		string s = "\nwise::CameraHandler::Factory : "
			   "ERROR calling create(...) :\n"
			   "Attempt to create a concrete CameraHandler but "
			   "registry is empty (name = '";
		s += name + "').\n";
                throw WiseException(s);
	}
	if (registry->find(name) == registry->end()) {
		string s = "\nwise::CameraHandler::Factory : "
			   "ERROR calling create(...) :\n"
			   "Attempt to create a concrete CameraHandler with "
			   "name not in the registry (name = '";
		s += name + "').\n";
                throw WiseException(s);
	}
	return registry->at(name)(m);
}

