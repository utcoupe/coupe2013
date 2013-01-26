#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include "camManager.h"
#include "helper.h"

using namespace std;

const int NB_OF_OBJECTS_TO_DETECT = 4;
const int RED = 0;
const int BLUE = 1;
const int WHITE = 2;

camManager::camManager(int id, int display)
{ 
	this->CAMERA_N = id; 
	this->display = display; 
	this->logger = new Logger("CAM", this->CAMERA_N);

	this->colorsetPath = "./yml/colorSet.yml";
	this->redTemplPath = "./yml/redTemplPath.yml";
	this->blueTemplPath = "./yml/blueTemplPath.yml";
	this->whiteTemplPath = "./yml/whiteTemplPath.yml";

	this->loaded = false;
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
	// cout<<"size = "<<minRect->size.area()<<endl;
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
 void camManager::MatchingMethod( int color )
 {
 	extern cv::Mat img;
 	extern cv::Mat roiImg;
 	extern cv::Mat result;
 	extern cv::Rect rect;
 	extern int select_flag;
	
	char buffer[50];
	cv::Point matchLoc;

	// *
	//  * First methode: take all points which is similar and draw a cercle for each of these points...
	//  * float threshold = 0.08;
	//  * cv::Mat thresholdedImage = result < threshold;
	//  * Print a cercle for each non zero pixel 
	//  *
	//  * 	
		result = cv::Mat(img.rows-roiImg.rows+1, img.cols-roiImg.cols+1, CV_32FC1);
		cv::matchTemplate(img, roiImg, result, CV_TM_CCOEFF_NORMED);
		cv::threshold(result, result, 0.8, 1., CV_THRESH_TOZERO);

	    while (true) 
	    {
	        double minval, maxval, threshold = 0.88;
	        cv::Point minloc, maxloc;
	        cv::minMaxLoc(result, &minval, &maxval, &minloc, &maxloc);

	        if (maxval >= threshold)
	        {
	        	if(color == RED)
		            cv::rectangle(
		                img, 
		                maxloc, 
		                cv::Point(maxloc.x + roiImg.cols, maxloc.y + roiImg.rows), 
		                CV_RGB(255,0,0), 2
		            );
		        else if(color == BLUE)
		        	cv::rectangle(
		                img, 
		                maxloc, 
		                cv::Point(maxloc.x + roiImg.cols, maxloc.y + roiImg.rows), 
		                CV_RGB(0,0,255), 2
		            );
		        else
		        	cv::rectangle(
		                img, 
		                maxloc, 
		                cv::Point(maxloc.x + roiImg.cols, maxloc.y + roiImg.rows), 
		                CV_RGB(255,255,255), 2
		            );
	            cv::floodFill(result, maxloc, cv::Scalar(0), 0, cv::Scalar(.1), cv::Scalar(1.));
	            matchLoc = maxloc;
	        	sprintf(buffer, "matchLoc: (%d, %d): %lf", matchLoc.x, matchLoc.y, result.at<double>(matchLoc));
				logger->log(buffer);
	        }
	        else
	            break;
	    }
	 

	/**
	 * Second method: Erase the most similar region at each loop!
	 */

	// Localizing the best match with minMaxLoc
	// double minVal, maxVal; 
	// cv::Point minLoc, maxLoc, matchLoc;



	// for (int i = 0; i < NB_OF_OBJECTS_TO_DETECT; ++i)
	// {
	// 	// Create the result matrix
	// 	result.create( img.cols - roiImg.cols + 1, img.rows - roiImg.rows + 1, CV_32FC1 );
	// 	// Do the Matching and Normalize
	// 	cv::matchTemplate( img, roiImg, result, cv::TM_SQDIFF );
	// 	cv::normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
	// 	cv::imshow("result", result);
	// 	cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
	// 	matchLoc = minLoc; 
	// 	cv::rectangle( 	img, 
	// 					matchLoc, 
	// 					cv::Point( matchLoc.x + roiImg.cols, matchLoc.y + roiImg.rows ), 
	// 					CV_RGB(255, 0, 0), 
	// 					3, 8, 0 );



	// 	cv::Rect regionToEliminate = cv::Rect(matchLoc.x, matchLoc.y, roiImg.cols, roiImg.rows);
	// 	cv::Mat temp = img(regionToEliminate);
	// 	temp = temp.ones(temp.rows, temp.cols, CV_32FC1);
		

		// The lower the demonimater, the easier tracker will track different objects. As we eliminate larger area.
		// cv::Rect regionToEliminate = cv::Rect(matchLoc.x - roiImg.cols/8, matchLoc.y-roiImg.rows/8, roiImg.cols/4, roiImg.rows/4);
		// cv::Rect wholeArea = cv::Rect(cv::Point(0,0), cv::Point(result.cols, result.rows));
		// regionToEliminate = regionToEliminate & wholeArea;

		// cv::Mat temp = result(regionToEliminate);
		// temp = temp.ones(temp.rows, temp.cols, CV_32FC1);

	// 	sprintf(buffer, "matchLoc: (%d, %d): %lf", matchLoc.x, matchLoc.y, result.at<double>(matchLoc));
	// 	logger->log(buffer);
	// }
	return;

}

void camManager::testCase()
{
	extern cv::Mat roiImg;
	extern cv::Mat img;

	this->capture >> img;
	cv::FileStorage redTemplFile(this->redTemplPath, cv::FileStorage::READ);
	cv::FileStorage blueTemplFile(this->blueTemplPath, cv::FileStorage::READ);
	cv::FileStorage whiteTemplFile(this->whiteTemplPath, cv::FileStorage::READ);
	
	redTemplFile["red_roi_img"] >> roiImg;
	if(!roiImg.empty())
	{
		cv::imshow("ROI_red", roiImg); /* show the image bounded by the box */
		logger->log("Test for red pattern search...");
		this->MatchingMethod(RED);
	}

	blueTemplFile["blue_roi_img"] >> roiImg;
	if(!roiImg.empty())
	{
		cv::imshow("ROI_blue", roiImg); /* show the image bounded by the box */
		logger->log("Test for blue pattern search...");
		this->MatchingMethod(BLUE);
	}

	whiteTemplFile["white_roi_img"] >> roiImg;
	if(!roiImg.empty())
	{
		cv::imshow("ROI_white", roiImg); /* show the image bounded by the box */
		logger->log("Test for white pattern search...");
		this->MatchingMethod(WHITE);
	}
}


/**
 * @TODO, find a way to get rid of global variables...Callback has to be static member function
 */
 void camManager::LocatingWithPatternMatching()
 {
 	extern cv::Mat img;
 	extern cv::Mat roiImg;
 	extern cv::Rect rect;
 	extern bool select_flag;
 	vector<cv::Point> result;

 	this->capture >> img; // Move this out of the loop for SNAPSHOT mode.

 	while(1)
 	{

 		if (img.empty())
 		{
 			logger->err("image empty");
 			continue;
 		}

 		cv::setMouseCallback("Display Loop With Pattern Matching", mouseHandler, NULL);

 		if (select_flag == 1){
            cv::imshow("ROI", roiImg); /* show the image bounded by the box */
 			this->MatchingMethod(RED);
 			select_flag = 0; //Uncomment this for SNAPSHOT mode. 
 		}

 		cv::rectangle(img, rect, CV_RGB(0, 0, 0), 1, 8, 0);

 		if(this->display)
 			cv::imshow( "Display Loop With Pattern Matching", img );

 		int c = cv::waitKey(50);
 		switch (c)
 		{
			// p for pause, press p or esc for unpause
 			case 'p':
 			c = 0;
 			while( c != 'p' && c != 27 )
 			{
 				c = cv::waitKey( 250 );
 			}
 			break;

			// q for quit
 			case 'q':
 			return;

 			// r for reload from camera
 			case 'r':
 			this->capture >> img;
 			if(!roiImg.empty())
 				select_flag = 1;
 			break;

 			// t for test (three colors)
 			case 't':
	 			this->testCase();
 			break;

 			// type sr for save red sb for blue, etc. 
 			case 's':
 			if(!roiImg.empty())
 			{
 				int color = cv::waitKey(250);
 				if (color == 'r')
 				{
 					cv::FileStorage redTemplFile(this->redTemplPath, cv::FileStorage::WRITE);
 					redTemplFile << "red_roi_img" << roiImg;
 					redTemplFile.release();
 					logger->log("Red roi img saved");
 				}
 				else if (color == 'b')
 				{
 					cv::FileStorage blueTemplFile(this->blueTemplPath, cv::FileStorage::WRITE);
 					blueTemplFile << "blue_roi_img" << roiImg;
 					blueTemplFile.release();
 					logger->log("Blue roi img saved");
 				}
 				else if (color == 'w')
 				{
 					cv::FileStorage whiteTemplFile(this->whiteTemplPath, cv::FileStorage::WRITE);
 					whiteTemplFile << "white_roi_img" << roiImg;
 					whiteTemplFile.release();
 					logger->log("White roi img saved");
 				}
 				else
 					logger->err("Save operation timed out!");
 			}
 			else
 				logger->err("roiImg empty!");
 			break;

 			default:
 			if (c != -1)
 				logger->err(c);
 		}
 	}
 }