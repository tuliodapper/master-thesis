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

#ifndef __WiseBaseTerrain_h__
#define __WiseBaseTerrain_h__

#include "CastaliaModule.h"
#include "WiseGuiWorld.h"
#include "WiseCameraInfo.h"
#include "WiseTargetInfo.h"
#include <string>

class WiseBaseTerrain: public CastaliaModule {
private:

protected:
	/*--- The .ned file's parameters ---*/
	std::string gui_type;
	bool gui_wait_start;
	/*--- Custom class parameters ---*/
	double x_size;
	double y_size;
	double z_size;
	wise::GuiWorld *gui;

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage * msg);
	virtual void finish();
	virtual void finishSpecific();

public:
	WiseBaseTerrain() : gui(NULL) {};
	inline double get_x_size() const { return x_size; };
	inline double get_y_size() const { return y_size; };
	inline double get_z_size() const { return z_size; };

	virtual void place_camera(const WiseCameraInfo&);
	virtual void place_mobile(const WiseCameraInfo&);
	virtual void hold_mobile(const WiseCameraInfo&);
	virtual void place_BS(const WiseCameraInfo&);
	virtual void remove_camera(const WiseCameraInfo&);
	virtual void place_target(const WiseTargetInfo&);
	virtual void clean_target();
	virtual void clean_mobile();

	virtual inline bool free_point(double x, double y, double z) const 
								{return true;};
	virtual inline bool free_box(double tx, double ty, double tz, 
				     double bl, double bw, double h) const 
								{return true;};
};

#endif // __WiseBaseTerrain_h__
