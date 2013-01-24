#ifndef CAMMANAGER_H
#define CAMMANAGER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "logger.h"
#include "colorsets.h"

#ifndef _CALIBRATION
#define _CALIBRATION 0
#endif

#ifndef _CAMERA
#define _CAMERA 1
#endif

class camManager
{
private:
	int CAMERA_N;
	int display;

	string colorsetPath;
	ColorSets colorSets;
	cv::Mat image;
	
	Logger *logger;

	bool EliminatedContour(cv::RotatedRect *minRect);


public:
	cv::VideoCapture capture;
	
	camManager(int id, int display = 1);
	~camManager() { delete logger; }

	int Init();
	void loadSets();
	void DisplayLoop();
	cv::Mat SnapShot();
	cv::Mat *binaryFiltering(cv::Mat *img, ColorSet set, int calledBy = _CALIBRATION);
	vector<cv::Point> findObjects(cv::Mat *src, cv::Mat *original);


};

#endif