/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis                 *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *  
 ****************************************************************************/

#ifndef OUTPUTINFOWRITER_H_
#define OUTPUTINFOWRITER_H_

#include <iostream>
#include <fstream>
using namespace std;

class OutputInfoWriter {
 private:
	static ofstream theFile;
	static string fileName;

 public:
	OutputInfoWriter(const string & fName);
	static void setOutputFileName(const string & fName);
	static ofstream & getStream(void);
	static void closeStream(void);
};

#endif				/*DEBUGINFOWRITER_H_ */
