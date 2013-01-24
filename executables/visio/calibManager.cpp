#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "calibManager.h"
#include <iostream>

/*
	Calibration class. This is used to calibrate color matching schemes and save them to file
 */
calibManager::calibManager(){
	this->logger = new Logger("Calibration", 0);
	this->colorsetPath = "./yml/colorSet.yml";

	this->calibBlue = false;
	this->calibRed = false;
	this->calibWhite = false;

	this->loadSets();
}

/*
	This one takes the current schema settings and save it into yml file.
 */
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

/*
	loads color matching schema from the yml file. This file is human readable
 */
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



/**
 * Creat trackbar for each color
 * @param  id    0 for blue, 1 for red, 2 for white
 * @param  image the image to be binary filtered
 * @return       return the image binary filtered
 */
cv::Mat *calibManager::calib(int id, cv::Mat *image, camManager *cam)
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
		image = cam->binaryFiltering(image, this->colorSets.blue);
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
		image = cam->binaryFiltering(image, this->colorSets.red);
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
		image = cam->binaryFiltering(image, this->colorSets.white);
		return image;
		break;

		default:
		logger->err("calibManager::calib doesn't return properly!");
		return NULL;
	}
}


/**
 * set yml file for color matching schema, usable for three colors by default
 * @param cam the camera to use. ATTENTION: NEED TO CHANGE the colorschema file path if you want multiple schemes, by 
 * default, like the case for 2013, we tend to use the same schema for both cameras.
 */
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
			image = *(this->calib(0, &image, cam));

		if(this->calibRed)
			image = *(this->calib(1, &image, cam));

		if(this->calibWhite)
			image = *(this->calib(2, &image, cam));

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