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

	bool EliminatedContour(const cv::RotatedRect &minRect);


public:
	int NB_OF_OBJECTS_TO_DETECT;
	enum COLOR {RED = 1, BLUE, WHITE};
	
	cv::VideoCapture capture;
	
	camManager(int id, int display = 1);
	~camManager() { delete logger; }

	int Init();

	void loadSets();

	void DisplayLoopWithColorMatching();

	void LocatingWithPatternMatching();

	Json::Value testCase();

	void MatchingMethod(COLOR color, char *buffer);

	cv::Mat SnapShot();

	cv::Mat & binaryFiltering(cv::Mat &img, ColorSet set);
	
	vector<cv::Point> findObjects(const cv::Mat &src, cv::Mat &original);


};

#endif
