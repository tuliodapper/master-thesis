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

#include "WiseCameraManager.h"
#include "WiseDebug.h"

using namespace std;
using namespace wise;

Define_Module(WiseCameraManager);

void WiseCameraManager::init_handlers() {
    std::map<std::string, wise::CameraHandler*>::iterator i = handlers.begin();
    for (; i != handlers.end(); ++i)
        i->second->initialize();
}

void WiseCameraManager::delete_handlers() {
    std::map<std::string, wise::CameraHandler*>::iterator i = handlers.begin();
    for (; i != handlers.end(); ++i)
        delete i->second;
    handlers.clear();
}

void WiseCameraManager::processCommandRequest(WiseSensorManagerMessage *req) {
}

void WiseCameraManager::processSampleRequest(WiseSensorManagerMessage *req) {
}

bool WiseCameraManager::covers(double x, double y) {
    return x >= cam_info.fov_bb.min_x && x <= cam_info.fov_bb.max_x &&
            y >= cam_info.fov_bb.min_y && y <= cam_info.fov_bb.max_y;
}

wise::CameraHandler *WiseCameraManager::get_handler(const string &n) {
    if (handlers.find(n) == handlers.end()) {
        handlers[n] = wise::CameraHandler::Factory::create(n, *this);
        handlers[n]->initialize();
    }
    return handlers[n];
}

WiseCameraManager::~WiseCameraManager() {
    delete_handlers();
}

void WiseCameraManager::initialize() {
    WiseBaseSensorManager::initialize();

    WISE_DEBUG_3("WiseCameraManager::initialize()");
    NodeLocation loc = get_node_location();
    cam_info.set_position(loc.x, loc.y, loc.z);
    string s = par("fov_type");
    WISE_DEBUG_3("\tfov_type = " << s);
    if (s == "bounding_box")
        init_fov_bb();
    WISE_DEBUG_3("\tPlacing Camera on Terrain");
    if (numProcesses < 0)
        opp_error("WiseCameraManager::initialize\n"
                "parameter numProcess CANNOT be negative");
    terrain->place_camera(cam_info);
    init_handlers();
}

void WiseCameraManager::startup() {
    WISE_DEBUG_3("WiseCameraManager::startup()");
}

//void WiseCameraManager::processSampleRequest(WiseSensorManagerMessage *req)
//{
//	WiseBaseSensorManager::processSampleRequest(req);
//}

void WiseCameraManager::handleSample(WisePhysicalProcessMessage *msg) {
    WISE_DEBUG_3("WiseCameraManager::handleSample()");
    const char *type = msg->getClassName();
    WiseCameraMessage *smp = get_handler(type)->process(msg);
    if (smp)
        send(smp, "toApplicationModule");
}

void WiseCameraManager::init_fov_bb() {
    double fov_w = par("fov_bb_width");
    double fov_h = par("fov_bb_height");
    WISE_DEBUG_3("\tfov_bb_w = " << fov_w); WISE_DEBUG_3("\tfov_bb_h = " << fov_h);
    cam_info.set_fov_bb(fov_w, fov_h, terrain->get_x_size(), terrain->get_y_size());
}
