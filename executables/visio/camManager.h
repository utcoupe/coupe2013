#ifndef CAMMANAGER_H
#define CAMMANAGER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "logger.h"
#include "colorsets.h"
#include "helper.h"

extern void mouseHandler(int event, int x, int y, int flags, void* param);

class camManager
{
private:
	int CAMERA_N;
	int display;

	string colorsetPath;
	string redTemplPath, blueTemplPath, whiteTemplPath;

	ColorSets colorSets;
	cv::Mat image;
	bool loaded;
	
	Logger *logger;

	bool EliminatedContour(cv::RotatedRect *minRect);


public:
	cv::VideoCapture capture;
	
	camManager(int id, int display = 1);
	~camManager() { delete logger; }

	int Init();
	void loadSets();
	void DisplayLoopWithColorMatching();
	void LocatingWithPatternMatching();
	void MatchingMethod(int color);
	void testCase();
	cv::Mat SnapShot();
	cv::Mat *binaryFiltering(cv::Mat *img, ColorSet set);
	vector<cv::Point> findObjects(cv::Mat *src, cv::Mat *original);


};

#endif