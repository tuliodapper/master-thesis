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

#include "WiseGuiWorldOpencv.h"
#include "WiseDebug.h"

// #include <highgui.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace wise;

static GuiWorld::Factory registrat("opencv", &GuiWorldOpencv::creator);

GuiWorldOpencv::GuiWorldOpencv(unsigned w, unsigned h, float s) 
: GuiWorld(w, h, s), image(cv::Size(scale(w), scale(h)), CV_8UC3), 
  image_hold(cv::Size(scale(w), scale(h)), CV_8UC3) 
{
	window_name = "World";
	//cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
	cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	width = scale(w);
	height = scale(h);
	cv::imshow(window_name, image);
	cvWaitKey(5);
}

GuiWorldOpencv::~GuiWorldOpencv()
{
	cvDestroyWindow(window_name.c_str());
}

void GuiWorldOpencv::wait_key() 
{
	cvWaitKey(5);
}

void GuiWorldOpencv::draw_camera(const WiseCameraInfo &cam)
{
	double c_x, c_y, c_z;

	cam.get_position(c_x, c_y, c_z);
	c_x = scale(c_x);
	c_y = scale(c_y);
	c_z = scale(c_z);
	WISE_DEBUG_3("WiseGuiWorld::draw_camera() : " <<
		     "x=" << c_x << " y=" << c_y << " z=" << c_z);

	cv::circle(image, cv::Point(c_x, c_y), 3, cv::Scalar(100, 0, 200));

	if (cam.get_fov_type() == WiseCameraInfo::BOUNDING_BOX) {
		WiseCameraInfo::fov_bb_t fov;
		cam.get_fov_bb(fov);
		double tl_x = scale(fov.c_x) - scale(fov.width / 2);
		double br_x = scale(fov.c_x) + scale(fov.width / 2);
		double tl_y = scale(fov.c_y) - scale(fov.height / 2);
		double br_y = scale(fov.c_y) + scale(fov.height / 2);
		cv::rectangle(image, cv::Point(tl_x, tl_y),
			      cv::Point(br_x, br_y), cv::Scalar(0, 200, 220));
	} else if  (cam.get_fov_type() == WiseCameraInfo::TRIANGLE_FOV) {
	    WiseCameraInfo::fov_triangle_t fov = cam.get_triangle_fov();
	    double sa_x = scale(fov.a_x);
	    double sa_y = scale(fov.a_y);

	    double sb_x = scale(fov.b_x);
	    double sb_y = scale(fov.b_y);

	    double sc_x = scale(fov.c_x);
	    double sc_y = scale(fov.c_y);

	    // draw point g (center of mass)
	    cv::Point g((sa_x + sb_x + sc_x) / 3, (sa_y + sb_y + sc_y) / 3);
	    cv::circle(image, g, 1, cv::Scalar(255, 255, 0));

	    // draw FoV
            cv::Scalar green(0, 255, 00);
	    cv::line(image, cv::Point(sa_x, sa_y), cv::Point(sb_x, sb_y), green);
	    cv::line(image, cv::Point(sa_x, sa_y), cv::Point(sc_x, sc_y), green);
	    cv::line(image, cv::Point(sb_x, sb_y), cv::Point(sc_x, sc_y), green);
	}

	cv::imshow(window_name, image);
	cvWaitKey(5);
}

void GuiWorldOpencv::draw_BS(const WiseCameraInfo &cam)
{
	double c_x, c_y, c_z;

	cam.get_position(c_x, c_y, c_z);
	c_x = scale(c_x);
	c_y = scale(c_y);
	c_z = scale(c_z);
	WISE_DEBUG_3("WiseGuiWorld::draw_BS() : " <<
		     "x=" << c_x << " y=" << c_y << " z=" << c_z);

	cv::circle(image, cv::Point(c_x, c_y), 10, cv::Scalar(150, 0, 200));

	cv::imshow(window_name, image);
	cvWaitKey(5);
}

void GuiWorldOpencv::draw_mobile(const WiseCameraInfo &cam)
{
	double c_x, c_y, c_z;

	cam.get_position(c_x, c_y, c_z);
	c_x = scale(c_x);
	c_y = scale(c_y);
	c_z = scale(c_z);
	WISE_DEBUG_3("WiseGuiWorld::draw_mobile() : " <<
		     "x=" << c_x << " y=" << c_y << " z=" << c_z);

	cv::circle(image, cv::Point(c_x, c_y), 3, cv::Scalar(255, 165, 0));

	if (cam.get_fov_type() == WiseCameraInfo::BOUNDING_BOX) {
		WiseCameraInfo::fov_bb_t fov;
		cam.get_fov_bb(fov);
		double tl_x = scale(fov.c_x) - scale(fov.width / 2);
		double br_x = scale(fov.c_x) + scale(fov.width / 2);
		double tl_y = scale(fov.c_y) - scale(fov.height / 2);
		double br_y = scale(fov.c_y) + scale(fov.height / 2);
		cv::rectangle(image, cv::Point(tl_x, tl_y),
			      cv::Point(br_x, br_y), cv::Scalar(0, 200, 220));
	} else if  (cam.get_fov_type() == WiseCameraInfo::TRIANGLE_FOV) {
	    WiseCameraInfo::fov_triangle_t fov = cam.get_triangle_fov();
	    double sa_x = scale(fov.a_x);
	    double sa_y = scale(fov.a_y);

	    double sb_x = scale(fov.b_x);
	    double sb_y = scale(fov.b_y);

	    double sc_x = scale(fov.c_x);
	    double sc_y = scale(fov.c_y);

	    // draw point g (center of mass)
	    cv::Point g((sa_x + sb_x + sc_x) / 3, (sa_y + sb_y + sc_y) / 3);
	    cv::circle(image, g, 1, cv::Scalar(255, 255, 0));

	    // draw FoV
            cv::Scalar green(153, 204, 50);
	    cv::line(image, cv::Point(sa_x, sa_y), cv::Point(sb_x, sb_y), green);
	    cv::line(image, cv::Point(sa_x, sa_y), cv::Point(sc_x, sc_y), green);
	    cv::line(image, cv::Point(sb_x, sb_y), cv::Point(sc_x, sc_y), green);
	}

	cv::imshow(window_name, image);
	cvWaitKey(5);
}
	
void GuiWorldOpencv::draw_target(const WiseTargetInfo &target)
{
	double c_x, c_y, c_z;

	target.get_position(c_x, c_y, c_z);
	c_x = scale(c_x);
	c_y = scale(c_y);
	c_z = scale(c_z);
	WISE_DEBUG_3("WiseGuiWorld::draw_target() : " <<
		     "x=" << c_x << "y=" << c_y << "z=" << c_z);

	cv::circle(image, cv::Point(c_x, c_y), 2, cv::Scalar(0, 255, 0));

	double bb_w;
	double bb_h;
	target.get_bb(bb_w, bb_h);
	bb_w = scale(bb_w);
	bb_h = scale(bb_h);
	double tl_x = c_x - bb_w / 2;
	double br_x = c_x + bb_w / 2;
	double tl_y = c_y - bb_h / 2;
	double br_y = c_y + bb_h / 2;
	cv::rectangle(image, cv::Point(tl_x, tl_y), cv::Point(br_x, br_y), cv::Scalar(0, 255, 0));
	cv::imshow(window_name, image);
	cvWaitKey(5);
}

void GuiWorldOpencv::hold()
{
	image.copyTo(image_hold);
}

void GuiWorldOpencv::clean_cam(const WiseCameraInfo &cam)
{
	double c_x, c_y, c_z;

	cam.get_position(c_x, c_y, c_z);
	c_x = scale(c_x);
	c_y = scale(c_y);
	c_z = scale(c_z);
	WISE_DEBUG_3("WiseGuiWorld::draw_camera() : " <<
		     "x=" << c_x << " y=" << c_y << " z=" << c_z);

	cv::circle(image, cv::Point(c_x, c_y), 3, cv::Scalar(0, 0, 0));

	if (cam.get_fov_type() == WiseCameraInfo::BOUNDING_BOX) {
		WiseCameraInfo::fov_bb_t fov;
		cam.get_fov_bb(fov);
		double tl_x = scale(fov.c_x) - scale(fov.width / 2);
		double br_x = scale(fov.c_x) + scale(fov.width / 2);
		double tl_y = scale(fov.c_y) - scale(fov.height / 2);
		double br_y = scale(fov.c_y) + scale(fov.height / 2);
		cv::rectangle(image, cv::Point(tl_x, tl_y),
			      cv::Point(br_x, br_y), cv::Scalar(0, 0, 0));
	} else if  (cam.get_fov_type() == WiseCameraInfo::TRIANGLE_FOV) {
	    WiseCameraInfo::fov_triangle_t fov = cam.get_triangle_fov();
	    double sa_x = scale(fov.a_x);
	    double sa_y = scale(fov.a_y);

	    double sb_x = scale(fov.b_x);
	    double sb_y = scale(fov.b_y);

	    double sc_x = scale(fov.c_x);
	    double sc_y = scale(fov.c_y);

	    // draw point g (center of mass)
	    cv::Point g((sa_x + sb_x + sc_x) / 3, (sa_y + sb_y + sc_y) / 3);
	    cv::circle(image, g, 1, cv::Scalar(0, 0, 0));

	    // draw FoV
            cv::Scalar green(0, 0, 0);
	    cv::line(image, cv::Point(sa_x, sa_y), cv::Point(sb_x, sb_y), green);
	    cv::line(image, cv::Point(sa_x, sa_y), cv::Point(sc_x, sc_y), green);
	    cv::line(image, cv::Point(sb_x, sb_y), cv::Point(sc_x, sc_y), green);
	}

	cv::imshow(window_name, image);
	cvWaitKey(5);
}

void GuiWorldOpencv::clean()
{
//	cv::rectangle(image, cv::Point(0, 0), cv::Point(height, width),
//		      cv::Scalar(0, 0, 0));
	//image = image_hold;
	image_hold.copyTo(image);
}
