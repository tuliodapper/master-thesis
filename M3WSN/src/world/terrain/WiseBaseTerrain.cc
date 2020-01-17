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

#include "WiseBaseTerrain.h"
#include "WiseDebug.h"

static unsigned n_processes;

void WiseBaseTerrain::initialize() 
{
	cModule *net = getParentModule();
	if (!net)
		opp_error("Unable to obtain SN pointer");
	x_size = (double) net->par("field_x");
	y_size = (double) net->par("field_y");
	z_size = (double) net->par("field_z");
	n_processes = net->par("numPhysicalProcesses");
	const char *type = par("gui");
	gui_type = type;
	gui = (gui_type == "") ? NULL : 
	      wise::GuiWorld::Factory::create(gui_type, (unsigned)x_size, 
					      (unsigned)y_size, 5);
	gui_wait_start = par("gui_wait_start");
}

void WiseBaseTerrain::handleMessage(cMessage * msg)
{
	delete msg;
}

void WiseBaseTerrain::finish()
{
	if (gui) {
		delete gui;
		gui = NULL;
	}
}

void WiseBaseTerrain::finishSpecific()
{
}

void WiseBaseTerrain::place_camera(const WiseCameraInfo &cam)
{
	WISE_DEBUG_3("WiseBaseTerrain::place_camera() called");
	if (!gui)
		return;
	gui->draw_camera(cam);
	gui->hold();
}

void WiseBaseTerrain::hold_mobile(const WiseCameraInfo &cam)
{
	WISE_DEBUG_3("WiseBaseTerrain::place_mobile() called");

	if (!gui)
		return;
	gui->draw_mobile(cam);
	gui->hold();
}

void WiseBaseTerrain::place_mobile(const WiseCameraInfo &cam)
{
	WISE_DEBUG_3("WiseBaseTerrain::place_mobile() called");
	if (!gui)
		return;
	gui->draw_mobile(cam);
	gui->wait_key();
	//gui->hold();
}

void WiseBaseTerrain::place_BS(const WiseCameraInfo &cam)
{
	WISE_DEBUG_3("WiseBaseTerrain::place_mobile() called");
	if (!gui)
		return;
	gui->draw_BS(cam);
	gui->hold();
}

void WiseBaseTerrain::remove_camera(const WiseCameraInfo &cam)
{
	WISE_DEBUG_3("WiseBaseTerrain::remove_camera() called");
	if (!gui)
		return;
	gui->clean_cam(cam);
}

void WiseBaseTerrain::place_target(const WiseTargetInfo &tgt)
{
	static bool once = false;     // FIXME: TEMPORARY SOLUTION, I don't like
	static unsigned expected = 0; // FIXME: TEMPORARY SOLUTION, I don't like

	WISE_DEBUG_3("WiseBaseTerrain::place_target() called");
	if (!gui)
		return;
	if (expected == 0) 
		gui->clean();
	expected = (expected + 1) % n_processes;
	gui->draw_target(tgt);
	if (gui_wait_start && !once) {
		gui->wait_key();
		once = true;
	}
}

void WiseBaseTerrain::clean_mobile()
{
	WISE_DEBUG_3("WiseBaseTerrain::clean_target() called");
	if (!gui)
		return;
	gui->clean();
}

void WiseBaseTerrain::clean_target()
{
	WISE_DEBUG_3("WiseBaseTerrain::clean_target() called");
	if (!gui)
		return;
	gui->clean();
}
