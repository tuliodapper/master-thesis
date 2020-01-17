/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Yuriy Tselishchev                                            *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *  
 ****************************************************************************/

#ifndef CASTALIA_TIMER
#define CASTALIA_TIMER

#define TIMER_MIN_SIZE 8
#define TIMER_MAX_SIZE 256

#include <omnetpp.h>
#include <vector>
#include "CastaliaMessages.h"
#include "TimerServiceMessage_m.h"

class TimerService: public virtual cSimpleModule {
 private:
	double timerDrift;
 protected:
	std::vector<TimerServiceMessage*> timerMessages;
	std::vector<TimerServiceMessage*> multipleTimerMessages;

	simtime_t getClock();
	void setTimerDrift(double new_drift);
	double getTimerDrift();
	void handleTimerMessage(cMessage *);
	void setTimer(int index, simtime_t time);
	void cancelTimer(int index);
	simtime_t getTimer(int index);
	void setMultipleTimer(int, int, simtime_t);
	void cancelMultipleTimer(int, int);
	simtime_t getMultipleTimer(int timerIndex, int nodeId);

	virtual void timerFiredCallback(int index);
	virtual void multipleTimerFiredCallback(int index, int nodeId);
};

#endif
