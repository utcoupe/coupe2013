#ifndef CALIBMANAGER_H
#define CALIBMANAGER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "camManager.h"
#include "logger.h"
#include "colorsets.h"

class calibManager
{
	private:
		string colorsetPath;

		ColorSets colorSets;

		bool calibBlue; 
		bool calibRed; 
		bool calibWhite; 

		Logger *logger;
		camManager *cam;

	public:
		calibManager();
		~calibManager() {delete this->logger;}

		void yamlSetter(camManager *cam);
		void loadSets();
		void writeSets();
		cv::Mat *calib(int id, cv::Mat *image, camManager *cam);
};

#endif