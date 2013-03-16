#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <cstring>
#include "camManager.h"
#include "helper.h"

#include "json/json.h"

using namespace std;

struct CommaIterator
:
public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	std::ostream *os;
	std::string comma;
	bool first;

	CommaIterator(std::ostream& os, const std::string& comma)
	:
	os(&os), comma(comma), first(true)
	{
	}

	CommaIterator& operator++() { return *this; }
	CommaIterator& operator++(int) { return *this; }
	CommaIterator& operator*() { return *this; }
  template <class T>
	CommaIterator& operator=(const T& t) {
		if(first)
			first = false;
		else
			*os << comma;
		*os << "(" << t.x << " " << t.y << ")";
		return *this;
	}
};


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
 	this->loadSets();
 	if (!capture.isOpened())
 	{
 		logger->err("Failed to open a video device!");
 		return -1;
 	}
 	return 0;
 }

 vector<cv::Point> camManager::findObjects(const cv::Mat &src, cv::Mat &original, const COLOR color = BLUE)
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

 			cv::Scalar paint_color;
 			switch(color){
 				case RED:
 				paint_color = cv::Scalar(0, 0, 255);
 				break;
 				case BLUE:
 				paint_color = cv::Scalar(255, 0, 0);
 				break;
 				case WHITE:
 				paint_color = cv::Scalar(0, 255, 0);
 				break;
 				default:
 				logger->err("Unkown color!");
 			}

 			if(this->display){
 				cv::circle(drawing, minRect.center, 3, paint_color, -1, 200, 0);
 				char coord[20];
 				sprintf(coord, "(%.3lf, %.3lf)", minRect.center.x, minRect.center.y);
 				cv::putText(drawing, coord, minRect.center, cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255));
 				if(color == WHITE)
 					cv::ellipse(drawing, minRect, paint_color, -1);
 				else{
 					cv::Point2f rect_points[4]; 
 					minRect.points( rect_points );
 					/// 2.b. Creating rectangles
					rectangle( drawing,
					           rect_points[0],
					           rect_points[2],
					           paint_color,
					           -1,
					           8 );

 					// for( int j = 0; j < 4; j++ )
      //     				line( drawing, rect_points[j], rect_points[(j+1)%4], paint_color, 1, 8 );
 				}
 			}
 		}
 	}
 	return result;
 }

 bool camManager::EliminatedContour(const cv::RotatedRect &minRect)
 {
	cout<<"size = "<<minRect.size.area()<<endl;
 	if(minRect.size.area() < 400)
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

 cv::Mat & camManager::binaryFiltering(cv::Mat &img, const COLOR color){

 	cv::Mat img_hsv;
 	cv::cvtColor(img, img_hsv, CV_BGR2HSV);

 	switch (color){
 		case RED:
 		cv::inRange(img_hsv, 
 			cv::Scalar(colorSets.red.h_lower, colorSets.red.s_lower, colorSets.red.v_lower), 
 			cv::Scalar(colorSets.red.h_higher, colorSets.red.s_higher, colorSets.red.v_higher), img);
 		break;
 		case BLUE:
 		cv::inRange(img_hsv, 
 			cv::Scalar(colorSets.blue.h_lower, colorSets.blue.s_lower, colorSets.blue.v_lower), 
 			cv::Scalar(colorSets.blue.h_higher, colorSets.blue.s_higher, colorSets.blue.v_higher), img);
 		break;
 		case WHITE:
 		cv::inRange(img_hsv, 
 			cv::Scalar(colorSets.white.h_lower, colorSets.white.s_lower, colorSets.white.v_lower), 
 			cv::Scalar(colorSets.white.h_higher, colorSets.white.s_higher, colorSets.white.v_higher), img);
 		break;
 		default:
 		this->logger->log("Unknown color!");
 	}
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
		this->loaded = true;
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
 Json::Value camManager::DisplayLoopWithColorMatching()
 {
 	while(1)
 	{
 		capture >> image;
 		// cv::normalize(image, image, 0, 256, cv::NORM_MINMAX);
 		blur( image, image, cv::Size(3,3) );
	 	if(display)
		 	drawing = cv::Mat::zeros( image.size(), CV_8UC3 );

 		if (image.empty())
 		{
 			logger->err("image empty");
 			continue;
 		}

 		Json::Value result;
 		vector<cv::Point> v1, v2, v3;

 		cv::Mat binaryImg = image;
 		binaryImg = binaryFiltering(binaryImg, BLUE);
 		v1 = findObjects(binaryImg, image, BLUE);
 		// Convert vector to string
 		std::ostringstream oss;
 		std::copy(v1.begin(), v1.end(), CommaIterator(oss, ","));
 		result["data"]["blue"] = oss.str();
		  // cout << "BLUE: " + res << endl;

 		binaryImg = image;
 		binaryImg = binaryFiltering(binaryImg, RED);
 		v2 = findObjects(binaryImg, image, RED);
 		// Convert vector to string
 		oss.str("");
 		std::copy(v2.begin(), v2.end(), CommaIterator(oss, ","));
 		result["data"]["red"] = oss.str();
		  // cout << "RED: " + res << endl;

 		binaryImg = image;
 		binaryImg = binaryFiltering(binaryImg, WHITE);
 		v3 = findObjects(binaryImg, image, WHITE);
 		// Convert vector to string
 		oss.str("");
 		std::copy(v3.begin(), v3.end(), CommaIterator(oss, ","));
 		result["data"]["green"] = oss.str();
		  // cout << "RED: " + res << endl;

 		if(display){
 			cv::imshow( "object tracing test", image );
 			cv::imshow("Drawing", drawing);
 		}

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
 			return result;

 			case 'r':
 			return result;

 			default:
 			if (c != -1)
 				logger->err(c);
 		}
 	}
 }


 Json::Value camManager::DisplayWithColorMatching()
 {
 	for (int i = 0; i < 4; ++i)
 	{
 		capture.grab();
 	}
 	capture >> image;
 	blur( image, image, cv::Size(3,3) );
 	if(display)
 		drawing = cv::Mat::zeros( image.size(), CV_8UC3 );

 	Json::Value result;
 	vector<cv::Point> v1, v2, v3;

 	cv::Mat binaryImg = image;
 	binaryImg = binaryFiltering(binaryImg, BLUE);
 	v1 = findObjects(binaryImg, image, BLUE);
 		// Convert vector to string
 	std::ostringstream oss;
 	std::copy(v1.begin(), v1.end(), CommaIterator(oss, ","));
 	result["data"]["blue"] = oss.str();
		  // cout << "BLUE: " + res << endl;

 	binaryImg = image;
 	binaryImg = binaryFiltering(binaryImg, RED);
 	v2 = findObjects(binaryImg, image, RED);
 		// Convert vector to string
 	oss.str("");
 	std::copy(v2.begin(), v2.end(), CommaIterator(oss, ","));
 	result["data"]["red"] = oss.str();
		  // cout << "RED: " + res << endl;

	binaryImg = image;
	binaryImg = binaryFiltering(binaryImg, WHITE);
	v3 = findObjects(binaryImg, image, WHITE);
	// Convert vector to string
	oss.str("");
	std::copy(v3.begin(), v3.end(), CommaIterator(oss, ","));
	result["data"]["white"] = oss.str();
		//   // cout << "WHITE: " + res << endl;

 	if(display)
 		cv::imshow( "object tracing test", image );

 	return result;
 }

 /**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
 void camManager::MatchingMethod(const COLOR color, char *buffer, const double threshold)
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
 	cv::threshold(result, result, threshold, 1., CV_THRESH_TOZERO);

 	while (true) 
 	{
 		double minval, maxval;
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

	Json::Value camManager::testCase(const double threshold = THRESHOLD)
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
				MatchingMethod(RED, buffer, threshold);
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
				MatchingMethod(BLUE, buffer, threshold);
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
				MatchingMethod(WHITE, buffer, threshold);
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

 			char buffer[200];
 			MatchingMethod(RED, buffer, THRESHOLD);
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
