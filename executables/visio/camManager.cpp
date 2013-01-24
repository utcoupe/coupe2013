#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include "camManager.h"

#ifndef _CALIBRATION
#define _CALIBRATION 0
#endif

#ifndef _CAMERA
#define _CAMERA 1
#endif

using namespace std;


camManager::camManager(int id, int display)
{ 
	this->CAMERA_N = id; 
	this->display = display; 
	this->logger = new Logger("CAM", this->CAMERA_N);
	this->colorsetPath = "./yml/colorSet.yml";
}

/**
 * Initialize camera and test if there is a camera associated with CAMERA_N
 * @return -1 if camera not found, 0 if everything is ok
 */
int camManager::Init()
{
	this->capture = cv::VideoCapture(this->CAMERA_N);
	this->loadSets();
	if (!capture.isOpened())
	{
		logger->err("Failed to open a video device!");
		return -1;
	}
	return 0;
}


/**
 * Based on binary Mat src, we find all possible contours, then keep the right ones and draw its contours+centers
 */
vector<cv::Point> camManager::findObjects(cv::Mat *src, cv::Mat *original)
{
	vector<cv::Point> result;
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;

    cv::findContours( *src, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

    for (unsigned int i=0; i < contours.size(); i++)
    {
        cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contours[i]));
        if (not this->EliminatedContour(&minRect))
        {
        	result.push_back(minRect.center);
        	cv::circle(*original, minRect.center, 2, cv::Scalar(255,0,0), -1, 200, 0);
        	cv::Point2f corners[4];
        	minRect.points(corners);
        	cv::line(*original, corners[0], corners[1], cv::Scalar(128,0,0), 3);
        	cv::line(*original, corners[1], corners[2], cv::Scalar(128,0,0), 3);
        	cv::line(*original, corners[2], corners[3], cv::Scalar(128,0,0), 3);
        	cv::line(*original, corners[3], corners[0], cv::Scalar(128,0,0), 3);
        }
    }
    return result;
}


/**
 * Determinate if we eliminate a contour
 * @param  minRect the minimal area rectangle found
 * @return         true if we eliminate it, false if not
 * @TODO	Better the algorithm to adapt to match (keep only 4 for each snapshot for ex.)
 * 			Convert the selection into mm instead of px
 */
bool camManager::EliminatedContour(cv::RotatedRect *minRect)
{
	// cout<<"size = "<<minRect->size.area()<<endl;
	if(minRect->size.area() < 3000)
		return true;
	return false;
}



/**
 * binary filtering
 * @param  img the image to be filtered
 * @param  set Color Set to use, this is fetched from trackbars and passed in according to color id
 * @return     processed image	
 */
cv::Mat *camManager::binaryFiltering(cv::Mat *img, ColorSet set, int calledBy)
{
	cv::Mat img_hsv;
	cv::cvtColor(*img, img_hsv, CV_BGR2HSV);
	if(calledBy == _CALIBRATION)
	{
		set.h_lower = cv::getTrackbarPos("h_lower", "Calib");
		set.s_lower = cv::getTrackbarPos("s_lower", "Calib");
		set.v_lower = cv::getTrackbarPos("v_lower", "Calib");
		set.h_higher = cv::getTrackbarPos("h_higher", "Calib");
		set.s_higher = cv::getTrackbarPos("s_higher", "Calib");
		set.v_higher = cv::getTrackbarPos("v_higher", "Calib");
	}

	cv::inRange(img_hsv, 
		cv::Scalar(set.h_lower, set.s_lower, set.v_lower), 
		cv::Scalar(set.h_higher, set.s_higher, set.v_higher), 
		*img);

	return img;
}

/*
	loads color matching schema from the yml file. This file is human readable
 */
void camManager::loadSets()
{
	cv::FileStorage ColorSetFile(this->colorsetPath, cv::FileStorage::READ);

	this->colorSets.blue.h_lower = ColorSetFile["blue_h_lower"];
	this->colorSets.blue.s_lower = ColorSetFile["blue_s_lower"];
	this->colorSets.blue.v_lower = ColorSetFile["blue_v_lower"];
	this->colorSets.blue.h_higher = ColorSetFile["blue_h_higher"];
	this->colorSets.blue.s_higher = ColorSetFile["blue_s_higher"];
	this->colorSets.blue.v_higher = ColorSetFile["blue_v_higher"];

	this->colorSets.red.h_lower = ColorSetFile["red_h_lower"];
	this->colorSets.red.s_lower = ColorSetFile["red_s_lower"];
	this->colorSets.red.v_lower = ColorSetFile["red_v_lower"];
	this->colorSets.red.h_higher = ColorSetFile["red_h_higher"];
	this->colorSets.red.s_higher = ColorSetFile["red_s_higher"];
	this->colorSets.red.v_higher = ColorSetFile["red_v_higher"];
	
	this->colorSets.white.h_lower = ColorSetFile["white_h_lower"];
	this->colorSets.white.s_lower = ColorSetFile["white_s_lower"];
	this->colorSets.white.v_lower = ColorSetFile["white_v_lower"];
	this->colorSets.white.h_higher = ColorSetFile["white_h_higher"];
	this->colorSets.white.s_higher = ColorSetFile["white_s_higher"];
	this->colorSets.white.v_higher = ColorSetFile["white_v_higher"];

	logger->log("Successfully load file");

	ColorSetFile.release();
}


/**
 * Take one instant picture, used to detect static position, doesn't show the image unless display = 1
 * @return return a cv::Mat image
 */
cv::Mat camManager::SnapShot()
{
	capture >> image;
	if(this->display)
	{
		cv::imshow("SnapShot", image);
		int c = 0;
		while( c != 'q' && c != 27 )
		{
			c = cvWaitKey( 250 );
		}
	}
	return image;
}


/**
 * Display raw image in a loop from camera. 
 * For debug purpose
 */
 void camManager::DisplayLoop()
 {
 	while(1)
 	{
 		this->capture >> image;

 		if (image.empty())
 		{
 			logger->err("image empty");
 			continue;
 		}

		cv::Mat binaryImg = image.clone();
		cv::Mat *pBinaryImg = &binaryImg;
		binaryImg = *(this->binaryFiltering(pBinaryImg, this->colorSets.blue, _CAMERA));
		this->findObjects(pBinaryImg, &image);

 		if(this->display)
 			cv::imshow( "object tracing test", image );

 		int c = cv::waitKey( 50 );
 		switch (c)
 		{
			// p for pause, press p or esc for unpause
 			case 'p':
 			c = 0;
 			while( c != 'p' && c != 27 )
 			{
 				c = cvWaitKey( 250 );
 			}
 			break;

			// q for quit
 			case 'q':
 			return;

 			default:
 			if (c != -1)
 				logger->err(c);
 		}
 	}
 }