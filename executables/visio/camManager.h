#ifndef CAMMANAGER_H
#define CAMMANAGER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "logger.h"

class camManager
{
private:
	int CAMERA_N;
	cv::Mat image;
	cv::VideoCapture capture;
	Logger *logger;

public:
	camManager(int id) { this->CAMERA_N = id; }
	~camManager() { delete logger; }
	int Init();
	void loop();
};

#endif