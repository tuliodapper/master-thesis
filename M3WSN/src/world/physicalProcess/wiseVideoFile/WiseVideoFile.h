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

#ifndef __WiseVideoFile_h__
#define __WiseVideoFile_h__

#include <string>
#include "WiseBasePhysicalProcess.h"
#include "WiseVideoFileMessage_m.h"

class WiseVideoFile: public WiseBasePhysicalProcess {
private:
	bool use_multi_camera;
	std::string file_name;
	double frame_rate;
protected:
	virtual void initialize();
	virtual void handleMessage(cMessage * msg);
	virtual void finishSpecific();
};

#endif // __WiseVideoFile_h__