#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "calibManager.h"
#include <iostream>


calibManager::calibManager(){
	this->logger = new Logger("Calibration", 0);
	this->colorsetPath = "./yml/colorSet.yml";

	this->calibBlue = false;
	this->calibRed = false;
	this->calibWhite = false;

	this->loadSets();
}

void calibManager::writeSets()
{
	cv::FileStorage ColorSetFile(this->colorsetPath, cv::FileStorage::WRITE);
	if(ColorSetFile.isOpened())
	{
		ColorSetFile << "blue_h_lower" << this->colorSets.blue.h_lower;
		ColorSetFile << "blue_s_lower" << this->colorSets.blue.s_lower;
		ColorSetFile << "blue_v_lower" << this->colorSets.blue.v_lower;
		ColorSetFile << "blue_h_higher" << this->colorSets.blue.h_higher;
		ColorSetFile << "blue_s_higher" << this->colorSets.blue.s_higher;
		ColorSetFile << "blue_v_higher" << this->colorSets.blue.v_higher;

		ColorSetFile << "red_h_lower" << this->colorSets.red.h_lower;
		ColorSetFile << "red_s_lower" << this->colorSets.red.s_lower;
		ColorSetFile << "red_v_lower" << this->colorSets.red.v_lower;
		ColorSetFile << "red_h_higher" << this->colorSets.red.h_higher;
		ColorSetFile << "red_s_higher" << this->colorSets.red.s_higher;
		ColorSetFile << "red_v_higher" << this->colorSets.red.v_higher;

		ColorSetFile << "white_h_lower" << this->colorSets.white.h_lower;
		ColorSetFile << "white_s_lower" << this->colorSets.white.s_lower;
		ColorSetFile << "white_v_lower" << this->colorSets.white.v_lower;
		ColorSetFile << "white_h_higher" << this->colorSets.white.h_higher;
		ColorSetFile << "white_s_higher" << this->colorSets.white.s_higher;
		ColorSetFile << "white_v_higher" << this->colorSets.white.v_higher;

		logger->log("Successfully write to file");
	}
	else
		logger->err("Failed to write to color set file");

	ColorSetFile.release();
}

void calibManager::loadSets()
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

cv::Mat *calibManager::binaryFiltering(cv::Mat *img, ColorSet set)
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

cv::Mat *calibManager::calib(int id, cv::Mat *image)
{
	switch (id){
		case 0:
		cv::namedWindow( "Calib" , CV_WINDOW_AUTOSIZE);
		cv::createTrackbar( "h_lower", "Calib", &(this->colorSets.blue.h_lower), 255 );
		cv::createTrackbar( "s_lower", "Calib", &(this->colorSets.blue.s_lower), 255 );
		cv::createTrackbar( "v_lower", "Calib", &(this->colorSets.blue.v_lower), 255 );
		cv::createTrackbar( "h_higher", "Calib", &(this->colorSets.blue.h_higher), 255 );
		cv::createTrackbar( "s_higher", "Calib", &(this->colorSets.blue.s_higher), 255 );
		cv::createTrackbar( "v_higher", "Calib", &(this->colorSets.blue.v_higher), 255 );
		image = this->binaryFiltering(image, this->colorSets.blue);
		return image;
		break;

		case 1:
		cv::namedWindow( "Calib" , CV_WINDOW_AUTOSIZE);
		cv::createTrackbar( "h_lower", "Calib", &(this->colorSets.red.h_lower), 255 );
		cv::createTrackbar( "s_lower", "Calib", &(this->colorSets.red.s_lower), 255 );
		cv::createTrackbar( "v_lower", "Calib", &(this->colorSets.red.v_lower), 255 );
		cv::createTrackbar( "h_higher", "Calib", &(this->colorSets.red.h_higher), 255 );
		cv::createTrackbar( "s_higher", "Calib", &(this->colorSets.red.s_higher), 255 );
		cv::createTrackbar( "v_higher", "Calib", &(this->colorSets.red.v_higher), 255 );
		image = this->binaryFiltering(image, this->colorSets.red);
		return image;
		break;

		case 2:
		cv::namedWindow( "Calib" , CV_WINDOW_AUTOSIZE);
		cv::createTrackbar( "h_lower", "Calib", &(this->colorSets.white.h_lower), 255 );
		cv::createTrackbar( "s_lower", "Calib", &(this->colorSets.white.s_lower), 255 );
		cv::createTrackbar( "v_lower", "Calib", &(this->colorSets.white.v_lower), 255 );
		cv::createTrackbar( "h_higher", "Calib", &(this->colorSets.white.h_higher), 255 );
		cv::createTrackbar( "s_higher", "Calib", &(this->colorSets.white.s_higher), 255 );
		cv::createTrackbar( "v_higher", "Calib", &(this->colorSets.white.v_higher), 255 );
		image = this->binaryFiltering(image, this->colorSets.white);
		return image;
		break;

		default:
		logger->err("calibManager::calib doesn't return properly!");
		return NULL;
	}
}

void calibManager::yamlSetter(camManager *cam)
{
	cv::Mat image;

	while(1)
	{
		cam->capture >> image;

		if (image.empty())
		{
			logger->err("image empty");
			continue;
		}

		if(this->calibBlue)
			image = *(this->calib(0, &image));

		if(this->calibRed)
			image = *(this->calib(1, &image));

		if(this->calibWhite)
			image = *(this->calib(2, &image));

		cv::imshow( "Calibration", image );

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

			case 'b':
			this->calibBlue = true;
			this->calibRed = false;
			this->calibWhite = false;
			break;

			case 'r':
			this->calibBlue = false;
			this->calibRed = true;
			this->calibWhite = false;
			break;

			case 'w':
			this->calibBlue = false;
			this->calibRed = false;
			this->calibWhite = true;
			break;

			case 'v':
			this->writeSets();
			break;

			default:
			if (c != -1)
				this->logger->err(c);
		}
	}
}