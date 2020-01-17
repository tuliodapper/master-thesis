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

#ifndef RESULTINFOWRITER_H_
#define RESULTINFOWRITER_H_

#include <iostream>
#include <fstream>
using namespace std;

class ResultInfoWriter {
 private:
	static ofstream theFile;
	static string fileName;
	static string configuration;

 public:
	ResultInfoWriter(const string & fName);
	static void setResultFileName(const string & fName);
	static void setConfig(const string &);
	static ofstream & getStream(void);
	static void closeStream(void);
	static ofstream & start();
};

#endif				/*DEBUGINFOWRITER_H_ */
