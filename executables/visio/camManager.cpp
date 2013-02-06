#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <cstring>
#include "camManager.h"
#include "helper.h"

#include "json/json.h"

using namespace std;

camManager::camManager(const int id, const int display)
{ 
	NB_OF_OBJECTS_TO_DETECT = 4;
	
	CAMERA_N = id; 
	this->display = display; 
	logger = new Logger("CAM", this->CAMERA_N);

	colorsetPath = "./yml/colorSet.yml";
	sprintf(redTemplPath, "./yml/redTemplPath_%d.yml", CAMERA_N);
	sprintf(blueTemplPath, "./yml/blueTemplPath_%d.yml", CAMERA_N);
	sprintf(whiteTemplPath, "./yml/whiteTemplPath_%d.yml", CAMERA_N);
	loaded = false;
}

/**
 * Initialize camera and test if there is a camera associated with CAMERA_N
 * @return -1 if camera not found, 0 if everything is ok
 */
int camManager::Init()
{
	capture = cv::VideoCapture(CAMERA_N);

	if (!capture.isOpened())
	{
		logger->err("Failed to open a video device!");
		return -1;
	}
	return 0;
}

vector<cv::Point> camManager::findObjects(const cv::Mat &src, cv::Mat &original)
{
	vector<cv::Point> result;
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;

    cv::findContours( src, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

    for (unsigned int i=0; i < contours.size(); i++)
    {
        cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contours[i]));
        if (not EliminatedContour(minRect))
        {
        	result.push_back(minRect.center);
        	cv::circle(original, minRect.center, 3, cv::Scalar(255,0,0), -1, 200, 0);
        	cv::Point2f corners[4];
        	minRect.points(corners);
        	cv::rectangle(original, corners[0], corners[3], cv::Scalar(255,0,0));
        }
    }
    return result;
}

bool camManager::EliminatedContour(const cv::RotatedRect &minRect)
{
	// cout<<"size = "<<minRect->size.area()<<endl;
	if(minRect.size.area() < 20)
		return true;
	return false;
}



/**
 * binary filtering
 * @param  img the image to be filtered
 * @param  set Color Set to use, this is fetched from trackbars and passed in according to color id
 * @return     processed image	
 */
cv::Mat & camManager::binaryFiltering(cv::Mat &img, ColorSet set)
{
	cv::Mat img_hsv;
	cv::cvtColor(img, img_hsv, CV_BGR2HSV);
	set.h_lower = cv::getTrackbarPos("h_lower", "Calib");
	set.s_lower = cv::getTrackbarPos("s_lower", "Calib");
	set.v_lower = cv::getTrackbarPos("v_lower", "Calib");
	set.h_higher = cv::getTrackbarPos("h_higher", "Calib");
	set.s_higher = cv::getTrackbarPos("s_higher", "Calib");
	set.v_higher = cv::getTrackbarPos("v_higher", "Calib");

	cv::inRange(img_hsv, 
		cv::Scalar(set.h_lower, set.s_lower, set.v_lower), 
		cv::Scalar(set.h_higher, set.s_higher, set.v_higher), 
		img);

	return img;
}

/*
	loads color matching schema from the yml file. This file is human readable
 */
void camManager::loadSets()
{
	cv::FileStorage ColorSetFile(colorsetPath, cv::FileStorage::READ);

	colorSets.blue.h_lower = ColorSetFile["blue_h_lower"];
	colorSets.blue.s_lower = ColorSetFile["blue_s_lower"];
	colorSets.blue.v_lower = ColorSetFile["blue_v_lower"];
	colorSets.blue.h_higher = ColorSetFile["blue_h_higher"];
	colorSets.blue.s_higher = ColorSetFile["blue_s_higher"];
	colorSets.blue.v_higher = ColorSetFile["blue_v_higher"];

	colorSets.red.h_lower = ColorSetFile["red_h_lower"];
	colorSets.red.s_lower = ColorSetFile["red_s_lower"];
	colorSets.red.v_lower = ColorSetFile["red_v_lower"];
	colorSets.red.h_higher = ColorSetFile["red_h_higher"];
	colorSets.red.s_higher = ColorSetFile["red_s_higher"];
	colorSets.red.v_higher = ColorSetFile["red_v_higher"];
	
	colorSets.white.h_lower = ColorSetFile["white_h_lower"];
	colorSets.white.s_lower = ColorSetFile["white_s_lower"];
	colorSets.white.v_lower = ColorSetFile["white_v_lower"];
	colorSets.white.h_higher = ColorSetFile["white_h_higher"];
	colorSets.white.s_higher = ColorSetFile["white_s_higher"];
	colorSets.white.v_higher = ColorSetFile["white_v_higher"];

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
	if(display)
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
 		capture >> image;

 		if (image.empty())
 		{
 			logger->err("image empty");
 			continue;
 		}

		cv::Mat binaryImg = image;
		binaryImg = binaryFiltering(binaryImg, colorSets.blue);
		findObjects(binaryImg, image);

 		if(display)
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
void camManager::MatchingMethod(const COLOR color, char *buffer)
 {
 	extern cv::Mat img;
 	extern cv::Mat roiImg;
 	extern cv::Mat result;
 	extern cv::Rect rect;
 	extern int select_flag;
	
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
			double minval, maxval, threshold = 0.8;
			cv::Point minloc, maxloc;
			cv::minMaxLoc(result, &minval, &maxval, &minloc, &maxloc);

			if (maxval >= threshold)
			{
				if(color == 1 && display)
					cv::rectangle(
						img, 
						maxloc, 
						cv::Point(maxloc.x + roiImg.cols, maxloc.y + roiImg.rows), 
						CV_RGB(255,0,0), 2
						);
				else if(color == 2 && display)
					cv::rectangle(
						img, 
						maxloc, 
						cv::Point(maxloc.x + roiImg.cols, maxloc.y + roiImg.rows), 
						CV_RGB(0,0,255), 2
						);
				else if (color == 3 && display)
					cv::rectangle(
						img, 
						maxloc, 
						cv::Point(maxloc.x + roiImg.cols, maxloc.y + roiImg.rows), 
						CV_RGB(255,255,255), 2
						);


				cv::floodFill(result, maxloc, cv::Scalar(0), 0, cv::Scalar(.1), cv::Scalar(1.));
				matchLoc = maxloc;
				if(color == 1){
					sprintf(buffer, "%s(%d %d) ", buffer, matchLoc.x, matchLoc.y);
						// (*response)["data"] = buffer;
						// (*response)["error"] = "";
						// logger->log(buffer);
				}
				else if(color == 2){
					sprintf(buffer, "%s(%d %d) ", buffer, matchLoc.x, matchLoc.y);
						// (*response)["data"] = buffer;
						// (*response)["error"] = "";
						// logger->log(buffer);
				}
				else if(color == 3){
					sprintf(buffer, "%s(%d %d) ", buffer, matchLoc.x, matchLoc.y);
						// (*response)["data"] = buffer;
						// (*response)["error"] = "";
						// logger->log(buffer);
				}
			}
			else
				return;
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
	}

	Json::Value camManager::testCase()
	{
		extern cv::Mat roiImg;
		extern cv::Mat img;
			
		Json::Value res;

		/*
			Une facon pour vider le ring buffer....4-6 pour linux, 2 pour windows, et pas besoin pour mac osx.
			Ceci est du au creation d'un buffer au niveau de camera, donc c'est peut-etre pas gerable autrement... 
		 */
 		for (int i = 0; i < 4; ++i)
 		{
 			capture.grab();
 		}
		capture >> img;

		cv::FileStorage redTemplFile(redTemplPath, cv::FileStorage::READ);
		cv::FileStorage blueTemplFile(blueTemplPath, cv::FileStorage::READ);
		cv::FileStorage whiteTemplFile(whiteTemplPath, cv::FileStorage::READ);

		redTemplFile["red_roi_img"] >> roiImg;
		if(!roiImg.empty())
		{
			char buffer[355] = "";
			if(display)
				cv::imshow("ROI_red", roiImg); /* show the image bounded by the box */
			logger->log("Test for red pattern search...");
			// sprintf(buffer, "R:");
			MatchingMethod(RED, buffer);
			res["data"]["red"] = buffer;
			logger->log(buffer);
		}

		blueTemplFile["blue_roi_img"] >> roiImg;
		if(!roiImg.empty())
		{
			char buffer[355] = "";
			if(display)
				cv::imshow("ROI_blue", roiImg); /* show the image bounded by the box */
			logger->log("Test for blue pattern search...");
			// strcat(buffer, "B:");
			MatchingMethod(BLUE, buffer);
			res["data"]["blue"] = buffer;
			logger->log(buffer);
		}

		whiteTemplFile["white_roi_img"] >> roiImg;
		if(!roiImg.empty())
		{
			char buffer[355] = "";
			if(display)
				cv::imshow("ROI_white", roiImg); /* show the image bounded by the box */
			logger->log("Test for white pattern search...");
			// strcat(buffer, "W:");
			MatchingMethod(WHITE, buffer);
			res["data"]["white"] = buffer;
			logger->log(buffer);
		}
		redTemplFile.release();
		blueTemplFile.release();
		whiteTemplFile.release();


		if(display)
		{
 			cv::imshow( "Snapshot With Pattern Matching", img );
 			cv::waitKey(30);
		}
		return res;
	}


/**
 * @TODO, find a way to get rid of global variables...Callback has to be static member function
 *
 * Used to save Rigion of Interest template. No longer used to detect objects.
 */
 void camManager::LocatingWithPatternMatching()
 {
 	extern cv::Mat img;
 	extern cv::Mat roiImg;
 	extern cv::Rect rect;
 	extern bool select_flag;
 	vector<cv::Point> result;

 	capture >> img; // Move this out of the loop for SNAPSHOT mode.

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
            /*
             * Uncomment this to detect RED object. Only for debug.
             */
            char buffer[200];
            MatchingMethod(RED, buffer);
            logger->log(buffer);
 			select_flag = 0; //Uncomment this for SNAPSHOT mode. 
 		}

 		cv::rectangle(img, rect, CV_RGB(0, 0, 0), 1, 8, 0);

 		// if(display)
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
 			capture >> img;
 			if(!roiImg.empty())
 				select_flag = 1;
 			break;

 			// t for test (three colors)
 			case 't':
 			testCase();
 			break;

 			// type sr for save red sb for blue, etc. 
 			case 's':
 			if(!roiImg.empty())
 			{
 				int color = cv::waitKey(250);
 				if (color == 'r')
 				{
 					cv::FileStorage redTemplFile(redTemplPath, cv::FileStorage::WRITE);
 					redTemplFile << "red_roi_img" << roiImg;
 					redTemplFile.release();
 					logger->log("Red roi img saved");
 				}
 				else if (color == 'b')
 				{
 					cv::FileStorage blueTemplFile(blueTemplPath, cv::FileStorage::WRITE);
 					blueTemplFile << "blue_roi_img" << roiImg;
 					blueTemplFile.release();
 					logger->log("Blue roi img saved");
 				}
 				else if (color == 'w')
 				{
 					cv::FileStorage whiteTemplFile(whiteTemplPath, cv::FileStorage::WRITE);
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
