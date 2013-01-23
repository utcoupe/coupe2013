#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "camManager.h"

using namespace std;

/**
 * Initialize camera and test if there is a camera associated with CAMERA_N
 * @return -1 if camera not found, 0 if everything is ok
 */
int camManager::Init()
{
	this->capture = cv::VideoCapture(this->CAMERA_N);
	this->logger = new Logger("CAM", this->CAMERA_N);
	if (!capture.isOpened())
	{
		logger->err("Failed to open a video device!");
		return -1;
	}
	return 0;
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

 		if(this->display)
 			cv::imshow( "Raw", image );

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