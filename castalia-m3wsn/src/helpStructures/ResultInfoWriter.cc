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

#include "ResultInfoWriter.h"

ofstream ResultInfoWriter::theFile;
string ResultInfoWriter::fileName;
string ResultInfoWriter::configuration;

ResultInfoWriter::ResultInfoWriter(const string & fName){
	fileName = fName;
}

void ResultInfoWriter::setResultFileName(const string & fName){
	fileName = fName;
}

void ResultInfoWriter::setConfig(const string & name){
	configuration = name;
}

ofstream &  ResultInfoWriter::start(){
	if (!theFile.is_open()){
		theFile.open(fileName.c_str(), ios::out);
    		theFile.close();
    		theFile.clear();
		theFile.open(fileName.c_str(), ios::app);

		char buff0[256];
		sprintf(buff0, "Castalia| what:%s (1)\n", configuration.c_str());
		string lin1;
		lin1.assign(buff0);

		//string lin1 = "Castalia| what:General (1)\n";

		time_t rawtime;
		struct tm * timeinfo;
		time (&rawtime);
  		timeinfo = localtime (&rawtime);
		char buff1[256], buff2[256], buff3[256], buff4[256];
		string month, day, hour, min;
		//--------------month
		if (timeinfo->tm_mon +1 > 9)
			sprintf(buff1, "%i", timeinfo->tm_mon +1);
		else
			sprintf(buff1, "0%i", timeinfo->tm_mon +1);
		month.assign(buff1);
		//--------------day
		if (timeinfo->tm_mday > 9)
			sprintf(buff2, "%i", timeinfo->tm_mday);
		else
			sprintf(buff2, "0%i", timeinfo->tm_mday);
		day.assign(buff2);
		//--------------hour
		if (timeinfo->tm_hour > 9)
			sprintf(buff3, "%i", timeinfo->tm_hour);
		else
			sprintf(buff3, "0%i", timeinfo->tm_hour);
		hour.assign(buff3);
		//--------------minutes
		if (timeinfo->tm_min > 9)
			sprintf(buff4, "%i", timeinfo->tm_min);
		else
			sprintf(buff4, "0%i", timeinfo->tm_min);
		min.assign(buff4);

		string lin2;
		char buff[256];
		sprintf(buff, "Castalia| when:%i-%s-%s %s:%s\n", timeinfo->tm_year + 1900, month.c_str(), day.c_str(), hour.c_str(), min.c_str());
		lin2.assign(buff);
		char buff5[256];
		sprintf(buff5, "Castalia| label:%s\n", configuration.c_str());
		string lin3;
		lin3.assign(buff5);

		theFile.write(lin1.c_str(),lin1.size());
		theFile.write(lin2.c_str(),lin2.size());
		theFile.write(lin3.c_str(),lin3.size());
	}
	return theFile;
}

ofstream & ResultInfoWriter::getStream(void){
	if (!theFile.is_open()){
		theFile.open(fileName.c_str(), ios::app);
	}
	theFile.setf(ios::left);
	return theFile;
}

void ResultInfoWriter::closeStream(void){
	if (theFile.is_open())
		theFile.close();
}
