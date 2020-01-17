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

#ifndef __WiseGuiWorldOpencv_h__ 
#define __WiseGuiWorldOpencv_h__ 

#include <string>
#include <cxcore.h>
#include "WiseGuiWorld.h"

namespace wise {

class GuiWorldOpencv : public GuiWorld {
private:
	std::string window_name;
	cv::Mat image;
	cv::Mat image_hold;
	unsigned width;
	unsigned height;
public:
	static GuiWorld* creator(unsigned w, unsigned h, float s) 
					{ return new GuiWorldOpencv(w, h, s); };
	GuiWorldOpencv(unsigned w, unsigned h, float scale=0);
	virtual ~GuiWorldOpencv();
	virtual void wait_key();
	virtual void draw_camera(const WiseCameraInfo&);
	virtual void draw_mobile(const WiseCameraInfo&);
	virtual void draw_BS(const WiseCameraInfo&);
	virtual void draw_target(const WiseTargetInfo&);
	virtual void hold();
	virtual void clean();
	virtual void clean_cam(const WiseCameraInfo&);
};

} // namespace wise

#endif // __WiseGuiWorldOpencv_h__ 
