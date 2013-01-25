#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include "camManager.h"
#include "helper.h"

using namespace std;

const int NB_OF_OBJECTS_TO_DETECT = 4;

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

	if (!capture.isOpened())
	{
		logger->err("Failed to open a video device!");
		return -1;
	}
	return 0;
}

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
        	cv::circle(*original, minRect.center, 3, cv::Scalar(255,0,0), -1, 200, 0);
        	cv::Point2f corners[4];
        	minRect.points(corners);
        	cv::rectangle(*original, corners[0], corners[3], cv::Scalar(255,0,0));
        }
    }
    return result;
}

bool camManager::EliminatedContour(cv::RotatedRect *minRect)
{
	cout<<"size = "<<minRect->size.area()<<endl;
	if(minRect->size.area() < 20)
		return true;
	return false;
}



/**
 * binary filtering
 * @param  img the image to be filtered
 * @param  set Color Set to use, this is fetched from trackbars and passed in according to color id
 * @return     processed image	
 */
cv::Mat *camManager::binaryFiltering(cv::Mat *img, ColorSet set)
{
	cv::Mat img_hsv;
	cv::cvtColor(*img, img_hsv, CV_BGR2HSV);
	set.h_lower = cv::getTrackbarPos("h_lower", "Calib");
	set.s_lower = cv::getTrackbarPos("s_lower", "Calib");
	set.v_lower = cv::getTrackbarPos("v_lower", "Calib");
	set.h_higher = cv::getTrackbarPos("h_higher", "Calib");
	set.s_higher = cv::getTrackbarPos("s_higher", "Calib");
	set.v_higher = cv::getTrackbarPos("v_higher", "Calib");

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
 void camManager::DisplayLoopWithColorMatching()
 {
 	while(1)
 	{
 		this->capture >> image;

 		if (image.empty())
 		{
 			logger->err("image empty");
 			continue;
 		}

		cv::Mat binaryImg = image;
		binaryImg = *(this->binaryFiltering(&binaryImg, this->colorSets.blue));
		this->findObjects(&binaryImg, &image);

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


 /**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
 void camManager::MatchingMethod( int, void* )
 {
 	extern cv::Mat img;
 	extern cv::Mat roiImg;
 	extern cv::Mat result;
 	extern cv::Rect rect;
 	extern int select_flag;

/**
 * First methode: take all points which is similar and draw a cercle for each of these points...
 * float threshold = 0.08;
 * cv::Mat thresholdedImage = result < threshold;
 * Print a cercle for each non zero pixel 
 */




/**
 * Second method: Erase the most similar region at each loop!
 */

  // Localizing the best match with minMaxLoc
 double minVal, maxVal; 
 cv::Point minLoc, maxLoc, matchLoc;

 for (int i = 0; i < NB_OF_OBJECTS_TO_DETECT; ++i)
 {
 	// Create the result matrix
 	int result_cols =  img.cols - roiImg.cols + 1;
 	int result_rows = img.rows - roiImg.rows + 1;

 	result.create( result_cols, result_rows, CV_32FC1 );

  	// Do the Matching and Normalize
 	cv::matchTemplate( img, roiImg, result, CV_TM_SQDIFF );
 	cv::normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
 	cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
 	matchLoc = minLoc; 
 	cv::rectangle( img, matchLoc, cv::Point( matchLoc.x + roiImg.cols, matchLoc.y + roiImg.rows ), cv::Scalar::all(0), 2, 8, 0 );

 	cv::Rect regionToEliminate = cv::Rect(matchLoc.x, matchLoc.y, roiImg.cols, roiImg.rows);
 	cv::Mat temp = img(regionToEliminate);
 	temp = temp.zeros(temp.rows, temp.cols, CV_32FC1);
 }
 return;
}


/**
 * @TODO, find a way to get rid of global variables...Callback has to be static member function
 */
 void camManager::DisplayLoopWithPatternMatching()
 {
 	extern cv::Mat img;
 	extern cv::Mat roiImg;
 	extern cv::Rect rect;
 	extern int select_flag;

 	while(1)
 	{
 		this->capture >> img; // Move this out of the loop for SNAPSHOT mode.

 		if (img.empty())
 		{
 			logger->err("image empty");
 			continue;
 		}

 		cv::setMouseCallback("Display Loop With Pattern Matching", mouseHandler, NULL);

 		if (select_flag == 1){
            cv::imshow("ROI", roiImg); /* show the image bounded by the box */
 			this->MatchingMethod(0, 0);
 			// select_flag = 0; Un comment this for SNAPSHOT mode. 
 		}

 		cv::rectangle(img, rect, CV_RGB(255, 0, 0), 1, 8, 0);

 		if(this->display)
 			cv::imshow( "Display Loop With Pattern Matching", img );

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