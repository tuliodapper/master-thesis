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

package wise_mnet.world.terrain.wiseEmptyTerrain;

simple WiseEmptyTerrain like wise_mnet.world.terrain.WiseBaseTerrain {

parameters:
	bool collectTraceInfo = default (false);
	string gui = default ("");
	bool gui_wait_start = default(false);
gates:
	input update @ directIn; // WiseBaseTerrain Update
}
