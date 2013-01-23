#ifndef CAMMANAGER_H
#define CAMMANAGER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "logger.h"

class camManager
{
private:
	int CAMERA_N;
	int display;

	cv::Mat image;
	
	Logger *logger;

public:
	cv::VideoCapture capture;
	
	camManager(int id, int display = 1) { this->CAMERA_N = id; this->display = display; }
	~camManager() { delete logger; }

	int Init();

	void DisplayLoop();
	cv::Mat SnapShot();
};

#endif