#ifndef CAMMANAGER_H
#define CAMMANAGER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include "logger.h"
#include "colorsets.h"
#include "helper.h"

#include "json/json.h"
#define	THRESHOLD 0.7

extern void mouseHandler(int event, int x, int y, int flags, void* param);

class camManager
{

private:
	const int CAMERA_N;
	const int display;

	string colorsetPath;
	char redTemplPath[80], blueTemplPath[80], whiteTemplPath[80];

	ColorSets colorSets;
	cv::Mat image;
	cv::Mat drawing;
	bool loaded;
	
	Logger *logger;

	static cv::Point POS_BOUGIE[20];
	static cv::Point POS_GATEAU;

	bool EliminatedContour(const cv::RotatedRect &minRect);
	string vec2flags(vector<cv::Point> v[], const cv::Point &, int);
	void checkTennis(int i, vector<cv::Point> & v, const cv::Point & mostLeft);
	void traitementPremierRangee(vector<cv::Point> v[], const bool see[]);

public:
	static int flags[20];
	int NB_OF_OBJECTS_TO_DETECT;
	enum COLOR {RED = 1, BLUE, WHITE};
	
	cv::VideoCapture capture;
	
	camManager(int id, int display = 1);
	~camManager() { delete logger; }

	int Init();

	void loadSets();

	Json::Value DisplayLoopWithColorMatching();
	Json::Value DisplayWithColorMatching(int, int, int);

	void LocatingWithPatternMatching();

	Json::Value testCase(const double);

	void MatchingMethod(COLOR color, char *buffer, const double);

	cv::Mat SnapShot();

	cv::Mat & binaryFiltering(cv::Mat &img, ColorSet set);
	cv::Mat & binaryFiltering(cv::Mat &img, const COLOR color);

	vector<cv::Point> findObjects(const cv::Mat &src, cv::Mat &original, const COLOR color);


};

#endif
