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

#ifndef __WiseGuiWorld_h__ 
#define __WiseGuiWorld_h__ 

#include <string>
#include <map>
#include "WiseCameraInfo.h"
#include "WiseTargetInfo.h"

namespace wise {

class GuiWorld {
public:
	class Factory {
	private: 
		typedef GuiWorld* (*creator_t)(unsigned, unsigned, float);
	private:
		static std::map<std::string, creator_t> *registry;
	public:
		Factory(const std::string&, creator_t);
		static GuiWorld *create(const std::string&, unsigned, unsigned,
					float scaling_factor=0);
	};

protected:
	bool show_gui;
	float scaling_factor;

public:
	GuiWorld(unsigned w, unsigned h, float scale=0);
	virtual ~GuiWorld() {} ;
	virtual void wait_key() = 0;
	virtual void draw_camera(const WiseCameraInfo&) = 0;
	virtual void draw_mobile(const WiseCameraInfo&) = 0;
	virtual void draw_BS(const WiseCameraInfo&) = 0;
	virtual void draw_target(const WiseTargetInfo&) = 0;
	virtual void hold() = 0;
	virtual void clean() = 0;
	virtual void clean_cam(const WiseCameraInfo&) = 0;

protected:
	inline unsigned scale(unsigned);
	inline double scale(double);
};

inline unsigned GuiWorld::scale(unsigned c)
{
	return (unsigned) ((float)c * scaling_factor);
}

inline double GuiWorld::scale(double c)
{
	return  c * scaling_factor;
}

} // namespace wise 

#endif // __WiseGuiWorld_h__ 
