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

#ifndef __WiseMovingTarget_h__
#define __WiseMovingTarget_h__

#define SIMTIME_STEP 0.01

#include <fstream>
#include "WiseBasePhysicalProcess.h"
#include "WiseMovingTargetMessage_m.h"

using namespace std;

enum phyProcessType {
	DIRECT_NODE_VALUES = 0,
	SCENARIO_BASED = 1,
	TRACE_FILE = 2
};

typedef struct {
	simtime_t time;
	double x;
	double y;
	double value;
} sourceSnapshot;

class WiseMovingTarget: public WiseBasePhysicalProcess {
private:
	/*--- The .ned file's parameters ---*/
	double x_init;
	double y_init;
	double z_init;
	double target_width;
	double target_height;
	double target_depth;
	double update_time;
	double move_min_x;
	double move_max_x;
	double move_min_y;
	double move_max_y;
	double move_later;
	bool log_enabled;
	double noise;

	/*temperature sensor*/
	double a;
	double k;
	sourceSnapshot *curr_source_state;
	int *source_index;
	int numSources;
	sourceSnapshot **sources_snapshots;	// N by M array, where N is numSources and, M is the 
										// maximum number of source snapshots. A source snapshot 
										// is a tuple (time, x, y, value)
	simtime_t time;
	int inputType;
	int numNodes;
	const char *description;
	int max_num_snapshots;
	double sigma;
	double *valuesTable;
	double defaultValue;

	/*--- Custom class member variables ---*/
	double x_curr;
	double y_curr;
	double z_curr;
	WiseTargetInfo target_info;
	double lin_x_step;
	double lin_y_step;
	double diagonal;
	double radius;
	double rotation_angle;
	double rotation_step;
	void (WiseMovingTarget::*move_func)(void);

	static std::ofstream *writer;

public: 
	virtual ~WiseMovingTarget();

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage * msg);
	virtual void finishSpecific();
	void readIniFileParameters();
	void initHelpStructures();
	void readScenariosFromIniFile();

private:
	void load_parameters();
	void log_information();
	void calculate_bounding_box(WiseTargetBoundingBox &bb);
	void move_linear_noise();
	void move_linear_random();
	void move_circular_noise();
	void move_circular_noise(double r);
	void move_lincirc_noise();
	void move_random_walk();
	void move_random_walk_8();
	double calculateSensedValue (const double &x_coo, const double &y_coo, const simtime_t & stime);
};

#endif // __WiseMovingTarget_h__
