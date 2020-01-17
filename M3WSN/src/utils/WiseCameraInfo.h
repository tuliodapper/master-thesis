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

#ifndef __WiseCameraInfo_h__
#define __WiseCameraInfo_h__

class WiseCameraInfo {
public:
	typedef enum {
		NONE = 0,
		BOUNDING_BOX,
		TRIANGLE_FOV
	} fov_t;
public:
	struct fov_bb_t {
		double c_x;
		double c_y;
		double width;
		double height;
		double max_x;
		double min_x;
		double max_y;
		double min_y;
	public:
		fov_bb_t() : c_x(0), c_y(0), width(0), height(0),
			     max_x(0), min_x(0), max_y(0), min_y(0) {} ;
		fov_bb_t(double x, double y) : c_x(x), c_y(y), 
					       width(0), height(0),
			     		       max_x(0), min_x(0), 
					       max_y(0), min_y(0) {} ;
	};

	struct fov_triangle_t {
	   double a_x, a_y;
	   double b_x, b_y;
	   double c_x, c_y;

	   fov_triangle_t() : a_x(0), a_y(0), b_x(0), b_y(0), c_x(0), c_y(0) {};
	};

private: 
	double pos_x;
	double pos_y;
	double pos_z;
	fov_t fov_type;
	fov_bb_t fov_bb;
	fov_triangle_t fov_triangle;
	// Other stuff

public:
	void set_triangle_fov(const fov_triangle_t& fov) { fov_type = TRIANGLE_FOV; fov_triangle = fov; };
	fov_triangle_t get_triangle_fov() const { return fov_triangle; };

	inline void get_position(double&, double&, double&) const;
	inline fov_t get_fov_type() const;
	inline void get_fov_bb(fov_bb_t&) const;

	WiseCameraInfo() : pos_x(0), pos_y(0), pos_z(0),
			   fov_type(NONE), fov_bb(0, 0) {};

	inline void set_position(double, double, double);
	inline void set_fov_bb(double, double, double x_max=0, double y_max=0);

public:
	//friend class WiseBaseSensorManager;
	friend class WiseCameraManager;
	friend class WiseCameraApplication;
};


void WiseCameraInfo::get_position(double &x, double &y, double &z) const
{
	x = pos_x;
	y = pos_y;
	z = pos_z;
}

void WiseCameraInfo::set_position(double x, double y, double z)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
}

WiseCameraInfo::fov_t WiseCameraInfo::get_fov_type() const
{
	return fov_type;
}

void WiseCameraInfo::get_fov_bb(fov_bb_t &fov) const
{
	fov = fov_bb;
}

void WiseCameraInfo::set_fov_bb(double w, double h, double x_max, double y_max)
{
	fov_type = BOUNDING_BOX;
	fov_bb.c_x = pos_x;
	fov_bb.c_y = pos_y;

	fov_bb.width = w;
	fov_bb.height = h;
	w /= 2;
	h /= 2;
	fov_bb.min_x = pos_x - w;
	fov_bb.max_x = pos_x + w;
	fov_bb.min_y = pos_y - h;
	fov_bb.max_y = pos_y + h;
	if (fov_bb.min_x < 0)
		fov_bb.min_x = 0;
	if (fov_bb.min_y < 0)
		fov_bb.min_y = 0;
	if (x_max > 0 && fov_bb.max_x > x_max)
		 fov_bb.max_x = x_max;
	if (y_max > 0 && fov_bb.max_y > y_max)
		 fov_bb.max_y = y_max;
}

#endif // __WiseCameraInfo_h__
