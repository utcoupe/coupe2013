#ifndef CAMMANAGER_H
#define CAMMANAGER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "logger.h"
#include "colorsets.h"
#include "helper.h"

#include "json/json.h"

extern void mouseHandler(int event, int x, int y, int flags, void* param);

class camManager
{
private:
	int CAMERA_N;
	int display;

	string colorsetPath;
	char redTemplPath[80], blueTemplPath[80], whiteTemplPath[80];

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
	void MatchingMethod(int color, Json::Value *response);
	void testCase(Json::Value *response);
	cv::Mat SnapShot();
	cv::Mat *binaryFiltering(cv::Mat *img, ColorSet set);
	vector<cv::Point> findObjects(cv::Mat *src, cv::Mat *original);


};

#endif