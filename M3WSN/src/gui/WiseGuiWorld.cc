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

#include "WiseGuiWorld.h"
#include "WiseException.h"

using namespace std;
using namespace wise;

map<string, GuiWorld::Factory::creator_t> *GuiWorld::Factory::registry = NULL;

GuiWorld::Factory::Factory(const string &name, GuiWorld::Factory::creator_t fun)
{ 
	if (!registry)
		registry = new map<string, creator_t>();
        if (registry->find(name) != registry->end()) {
		string s = "\nwcns::GuiWorld::Factory : "
			   "ERROR calling Factory(...) :\n"
			   "Attempt to register a concrete GuiWorld creator "
			   "with a name already in the registry (name = '";
		s += name + "').\n"; 
                throw Exception(s);
	}
        registry->insert(make_pair(name, fun));
}

GuiWorld* GuiWorld::Factory::create(const string &name, unsigned w, unsigned h,
				    float scale)
{
	if (!registry) {
		string s = "\nwise::GuiWorld::Factory : "
			   "ERROR calling create(...) :\n"
			   "Attempt to create a concrete GuiWorld but registry"
			   " is empty (name = '";
		s += name + "').\n";
                throw WiseException(s);
	}
	if (registry->find(name) == registry->end()) {
		string s = "\nwise::GuiWorld::Factory : "
			   "ERROR calling create(...) :\n"
			   "Attempt to create a concrete GuiWorld with name "
			   "not in the registry (name = '";
		s += name + "').\n";
                throw WiseException(s);
	}
	return registry->at(name)(w, h, scale);
}

GuiWorld::GuiWorld(unsigned w, unsigned h, float s)
{
	scaling_factor = (s == 0) ?  320.f / w : s;
}
