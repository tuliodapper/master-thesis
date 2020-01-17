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

#ifndef __WiseUtils_h__
#define __WiseUtils_h__

#include <string>
#include <sstream>
#include <vector>

namespace WiseUtils {

inline std::string print_vector(const std::vector<std::string> &v)
{
	using namespace std;
	ostringstream os;
	os << "[ ";
	vector<string>::const_iterator s;
	for (s = v.begin(); s != v.end(); ++s)
		os << *s << " ";
	os << "]";
	return os.str();
}

}; // namespace WiseUtils


#endif // __WiseUtils_h__
