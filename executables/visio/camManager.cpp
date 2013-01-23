#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "camManager.h"

using namespace std;

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

void camManager::loop()
{
	while(1)
	{
		capture >> image;
		if (image.empty())
		{
			logger->err("image empty");
			continue;
		}
		cv::imshow( "Raw", image );

		int c = cv::waitKey( 100 );
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