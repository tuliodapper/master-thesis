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

#ifndef __WiseDebug_h__
#define __WiseDebug_h__

#if (defined WISE_DEBUG) && (WISE_DEBUG > 0) && (WISE_DEBUG < 4)

#include <omnetpp.h>
#include <iomanip>
#include <fstream>

class WiseDebugLogger {
private: 
	static std::fstream writer;
public:
	static inline std::ostream& get_stream();
private:
	WiseDebugLogger();
	WiseDebugLogger(const WiseDebugLogger&);
	WiseDebugLogger& operator=(const WiseDebugLogger&);
};

std::ostream& WiseDebugLogger::get_stream() 
{
	return writer << "\n[" << std::setw(16) << simTime() << "] "; 
}

#if (WISE_DEBUG > 0) && (WISE_DEBUG < 2)
#define WISE_DEBUG_1(m) WiseDebugLogger::get_stream() << m
#define WISE_DEBUG_2(m)  
#define WISE_DEBUG_3(m)  
#endif

#if (WISE_DEBUG > 1) && (WISE_DEBUG < 3)
#define WISE_DEBUG_1(m) WiseDebugLogger::get_stream() << m
#define WISE_DEBUG_2(m) WISE_DEBUG_1(m)
#define WISE_DEBUG_3(m)  
#endif

#if (WISE_DEBUG > 2) && (WISE_DEBUG < 4)
#define WISE_DEBUG_1(m) WiseDebugLogger::get_stream() << m
#define WISE_DEBUG_2(m) WISE_DEBUG_1(m)
#define WISE_DEBUG_3(m) WISE_DEBUG_1(m)
#endif

#else 

#define WISE_DEBUG_1(m)  
#define WISE_DEBUG_2(m)  
#define WISE_DEBUG_3(m)  

#endif // WISE_DEBUG

#endif // __WiseDebug_h__
