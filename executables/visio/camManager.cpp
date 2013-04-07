#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <limits>
#include "camException.h"
#include "camManager.h"
#include "helper.h"

#include "json/json.h"

using namespace std;

cv::Point camManager::POS_BOUGIE[20];
cv::Point camManager::POS_GATEAU;
int camManager::flags[20];

struct CommaIterator
:
public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	ostream *os;
	string comma;
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


camManager::camManager(const int id, const int display):CAMERA_N(id), display(display)
{ 
	logger = new Logger("CAM", this->CAMERA_N);

	colorsetPath = "./yml/colorSet.yml";
	sprintf(redTemplPath, "./yml/redTemplPath_%d.yml", CAMERA_N);
	sprintf(blueTemplPath, "./yml/blueTemplPath_%d.yml", CAMERA_N);
	sprintf(whiteTemplPath, "./yml/whiteTemplPath_%d.yml", CAMERA_N);
	loaded = false;

		// Initialize all candles to not detected (-1). 
	for (int i = 0; i < 20; ++i)
	{
		flags[i] = -1;
	}

	POS_GATEAU = cv::Point(1500, 0);

 	// c'est moche... mais j'arrive pas a faire marcher le initialisation list, ce qui est une nouveaute de g++ c11p
	POS_BOUGIE[7]=(cv::Point(1157, 68));
	POS_BOUGIE[6]=(cv::Point(1209, 194));
	POS_BOUGIE[5]=(cv::Point(1306, 291));
	POS_BOUGIE[4]=(cv::Point(1432, 343));
	POS_BOUGIE[3]=(cv::Point(1568, 343));
	POS_BOUGIE[2]=(cv::Point(1694, 291));
	POS_BOUGIE[1]=(cv::Point(1791, 194));
	POS_BOUGIE[0]=(cv::Point(1843, 68));

	POS_BOUGIE[19]=(cv::Point(1054, 59));
	POS_BOUGIE[18]=(cv::Point(1084, 172));
	POS_BOUGIE[17]=(cv::Point(1143, 274));
	POS_BOUGIE[16]=(cv::Point(1226, 357));
	POS_BOUGIE[15]=(cv::Point(1328, 416));
	POS_BOUGIE[14]=(cv::Point(1441, 446));
	POS_BOUGIE[13]=(cv::Point(1559, 446));
	POS_BOUGIE[12]=(cv::Point(1672, 416));
	POS_BOUGIE[11]=(cv::Point(1774, 357));
	POS_BOUGIE[10]=(cv::Point(1857, 274));
	POS_BOUGIE[9]=(cv::Point(1916, 172));
	POS_BOUGIE[8]=(cv::Point(1946, 59));
}
/**
1 * Initialize camera and test if there is a camera associated with CAMERA_N
 * @return -1 if camera not found, 0 if everything is ok
1 */
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

 	vector<vector<cv::Point> > contours_poly( contours.size() );
 	vector<cv::RotatedRect> boundRect( contours.size() );
 	vector<cv::Point2f>center( contours.size() );
 	vector<float>radius( contours.size() );

 	for (unsigned int i=0; i < contours.size(); i++)
 	{
 		double area = cv::contourArea(contours[i]);
 		cout << "area = "<< area << endl;
 		if (area > 400 && area < 3000)
 		{
 			cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
 			cv::Scalar paint_color;
 			switch(color){
 				case RED:
 				paint_color = cv::Scalar(0, 0, 255);
 				boundRect[i] = cv::minAreaRect(cv::Mat(contours[i]));
 				center[i] = boundRect[i].center;
 				break;
 				case BLUE:
 				paint_color = cv::Scalar(255, 0, 0);
 				boundRect[i] = cv::minAreaRect(cv::Mat(contours[i]));
 				center[i] = boundRect[i].center;
 				break;
 				case WHITE:
 				continue;
 				paint_color = cv::Scalar(0, 255, 0);
 				cv::minEnclosingCircle( (cv::Mat)contours_poly[i], center[i], radius[i] );
 				break;
 				default:
 				logger->err("Unkown color!");
 			}
 			result.push_back(center[i]);

 			if(this->display){
 				if(color == WHITE){
 					// circle( drawing, center[i], (int)radius[i], paint_color, -1, 8, 0 );
 				}
 				else{
 					rectangle( drawing, boundRect[i].boundingRect().tl(), boundRect[i].boundingRect().br(), paint_color, -1, 8, 0 );
 				}
 				cv::circle(drawing, center[i], 3, cv::Scalar::all(0), -1, 200, 0);
 				char coord[20];
 				sprintf(coord, "(%.2lf,%.2lf)", center[i].x, center[i].y);
 				cv::putText(drawing, coord, center[i], cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar::all(255));
 			}
 		}
 	}
 	return result;
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

 		// binaryImg = image;
 		// binaryImg = binaryFiltering(binaryImg, WHITE);
 		// v3 = findObjects(binaryImg, image, WHITE);
 		// // Convert vector to string
 		// oss.str("");
 		// std::copy(v3.begin(), v3.end(), CommaIterator(oss, ","));
 		// result["data"]["tennis"] = oss.str();
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


 Json::Value camManager::DisplayWithColorMatching(int x, int y, int angle)
 {

 	for (int i = 0; i < 20; ++i)
 	{
 		capture.grab();
 	}
 	capture >> image;
 	blur( image, image, cv::Size(4,4) );
 	if(display)
 		drawing = cv::Mat::zeros( image.size(), CV_8UC3 );

 	Json::Value result;
 	vector<cv::Point> v[2];

 	cv::Mat binaryImg = image;
 	binaryImg = binaryFiltering(binaryImg, BLUE);
 	v[0] = findObjects(binaryImg, image, BLUE);
 		// Convert vector to string
 	std::ostringstream oss;
 	std::copy(v[0].begin(), v[0].end(), CommaIterator(oss, ","));
 	result["data"]["blue"] = oss.str();
		  // cout << "BLUE: " + res << endl;

 	binaryImg = image;
 	binaryImg = binaryFiltering(binaryImg, RED);
 	v[1] = findObjects(binaryImg, image, RED);
 		// Convert vector to string
 	oss.str("");
 	std::copy(v[1].begin(), v[1].end(), CommaIterator(oss, ","));
 	result["data"]["red"] = oss.str();
		  // cout << "RED: " + res << endl;

 // 	binaryImg = image;
 // 	binaryImg = binaryFiltering(binaryImg, WHITE);
 // 	v[2] = findObjects(binaryImg, image, WHITE);
	// // Convert vector to string
 // 	oss.str("");
 // 	std::copy(v[2].begin(), v[2].end(), CommaIterator(oss, ","));
 // 	result["data"]["tennis"] = oss.str();
		//   // cout << "WHITE: " + res << endl;
 	cv::Point robot_pos(x, y);

 	result["data"]["flags"] = vec2flags(v, robot_pos, angle);

 	if(display)
 		cv::imshow( "object tracing test", image );

 	return result;
 }

 bool PointCmpX(cv::Point p1, cv::Point p2){
 	return p1.x > p2.x;
 }

 bool PointCmpY(cv::Point p1, cv::Point p2){
 	return p1.y < p2.y;
 }

 double sumY(vector<cv::Point> v){

 	double sum = 0;
 	for(std::vector<cv::Point>::iterator it = v.begin(); it != v.end(); ++it) {
 		sum += it->y;
 	}
 	return sum;
 }

 void camManager::checkTennis(int i, vector<cv::Point> & v, const cv::Point & mostLeft){
 	std::vector<cv::Point>::iterator it = v.begin();

 	while( it != v.end()) {
 		if(it->x > mostLeft.x - 20 && it->x < mostLeft.x + 20
 			&& it->y > mostLeft.y + 10 && it->y < mostLeft.y + 80){
				// THere is a tennis!
 			flags[i] += 1;
 		v.erase(it);
 		break;
 	}
 	++it;
 }
}

void camManager::traitementPremierRangee(vector<cv::Point> v2[], const bool see[]){

	sort(v2[0].begin(), v2[0].end(), PointCmpX);
	sort(v2[1].begin(), v2[1].end(), PointCmpX);

	// for (int i = 0; i < v2[0].size(); ++i)
 // 	{
 // 		cerr << "v2[0][" <<i<<"] = (" << v2[0][i].x << "," << v2[0][i].y << ") ";
 // 	}
 // 	cerr << endl;
 // 	for (int i = 0; i < v2[1].size(); ++i)
 // 	{
 // 		cerr << "v2[1][" <<i<<"] = (" << v2[1][i].x << "," << v2[1][i].y << ") ";
 // 	}

	for (int i = 8; i < 20; ++i)
	{

		if(see[i] && flags[i] == -1){
			cerr << "Je traite " << i << endl;
		// Traiter 1er rangee
			if (v2[0].empty() && v2[1].empty())
			{
				throw camException("Il manque des bougies au premiere rangee!");
			}
			else if (!v2[0].empty() && v2[0].back().x < v2[1].back().x)
 			{
 				cv::Point mostLeft = v2[0].back();
 					v2[0].pop_back();
 					flags[i] = 11;
					// checkTennis(i, v2[2], mostLeft);
 			} else {
 				cv::Point mostLeft = v2[1].back();
 					v2[1].pop_back();
 					flags[i] = 21;
					// checkTennis(i, v2[2], mostLeft);
 			}
 			cerr << "v2[0].size()=" <<v2[0].size() << " v2[1].size()=" << v2[1].size() << endl;
		}
	}
}






/**
 * This function converts an array of vecteurs to into flags, which is a String representing the color and position of each candle.
 * value of flags: -1, not detected; 11 blue with tennis, 10 blue without tennis, 21 red with tennis, 20 red without tennis.
 * 
 * @param  v   an array of vecteurs of points, BLUE=v[0], RED, Tennis. 
 * @param  pos current position of robot. angle, current angle of robot
 * @return     a string in form of "-1, -1, 10, 11, 20, 11, -1, 21, etc...", there will be 20 entry.
 * We are counting candles in the following order:
 * 										  GATEAU, front view
 * From 2nd level, blue: 				  0,1,2,3,  4,5,6,7,               2nd level, red
 * 		1st level, blue: 	 	  8,9,10,11,12,13,  14,15,16,17,18,19     1st level, red
 */
 string camManager::vec2flags(vector<cv::Point> v[], const cv::Point & pos, int angle){
	// Angle of view in degree
 	const int ANGLE_VU = 30;
	// Angle of inclinaition of the camera
 	const int ANGLE_INCLINAISON = 30;
	// Height of robot in mm
 	const int HAUTEUR_ROBOT = 200;
	// Distance entre cam et robot en mm
 	const int DIS_CAM_ROBOT = 100;

 	const cv::Point CAMERA_POS = pos + cv::Point(cos(angle) * DIS_CAM_ROBOT, sin(angle) * DIS_CAM_ROBOT);

 	const int SIZE = v[0].size() + v[1].size();

 	const cv::Point POS_GATEAU(1500, 0);

 	const double DIS_CAM_GATEAU = norm(pos - POS_GATEAU);

 	// Distance entre la bougie en face et la bougie la plus loin, 440, 771 sont des donnee mesures.
 	const double DIS_TOLERENCE = 440 * (DIS_CAM_GATEAU / 771);

 	cerr << "Converting " << SIZE << " bougies positions into flags" << endl;

 	int seeCount = 0;
 	int maxIndex = -1;
 	
 	double max = 0;
 	double minDis = 1000;

	// Find the nearest candle. Mark it as "en face"
 	for (int i = 0; i < 20; ++i)
 	{
 		cv::Point vec1 = POS_BOUGIE[i] - CAMERA_POS;
 		cv::Point vec2 = pos - CAMERA_POS; 
 		if(norm(vec1) < minDis){
 			minDis = norm(vec1);
 			maxIndex = i;
 			cerr << "Je pense la Bougie en face: " << i << endl;
 			max = abs(vec1.ddot(vec2)/(norm(vec1)*norm(vec2)) > cos(ANGLE_VU));
 		}
 	}
 	
 	

 	// refresh detected flag. =1 if robot see it, 0 otherwise.
 	bool see[20] = {0};

 	// Beginning from the one marked "en face", test the linearity, and take the one most aligned with the robot.
 	for (int i = 0; i < 20; ++i)
 	{
 		cv::Point vec1 = POS_BOUGIE[i] - CAMERA_POS;
 		cv::Point vec2 = pos - CAMERA_POS; 


 		// test if in view angle
 		// maxIndex is the candle most in line with cam and robot. Probably the one that the robot is facing!
 		if(double arccosine = abs(vec1.ddot(vec2)/(norm(vec1)*norm(vec2)) > cos(ANGLE_VU))){
 			cv::Point vec3 = POS_BOUGIE[i] - POS_BOUGIE[maxIndex];

 			// We eliminate the possibility of setting candles on the farside as "en face". 300 is the demi rayon. This should be large enough as filter
 			if(arccosine > max && norm(vec1)<(minDis+300)){
 				max = arccosine;
 				maxIndex = i;
 			}

 			// Begining from left to right, if there is a candle more "en face", we would have already set that one as "en face". We assume that the robot will see at most three candles to the right of the candle "en face". 295 is the distance of 2.5 candles
 			cerr << "norm(vec3) = "<< norm(vec3) << " Pour " << i << endl;


 			// On ne compte pas les blancs.
 			if(norm(vec3) < DIS_TOLERENCE && (i < 12 || i > 15)){
 				seeCount++;
 				see[i] = true;
 			}
 		}
 	}

 	cerr << "I believe I have seen " << seeCount << " bougies" << endl;
 	cerr << "Ils sont : ";
 	for (int i = 0; i < 20; ++i)
 	{
 		if(see[i])
 			cout << i << " ";
 	}
 	cerr << endl;

 	sort(v[0].begin(), v[0].end(), PointCmpX);
 	sort(v[1].begin(), v[1].end(), PointCmpX);

	// sort(v[2].begin(), v[2].end(), PointCmp);
	// 
	// 
 	for (int i = 0; i < v[0].size(); ++i)
 	{
 		cerr << "v[0][" <<i<<"] = (" << v[0][i].x << "," << v[0][i].y << ") ";
 	}
 	cerr << endl;
 	for (int i = 0; i < v[1].size(); ++i)
 	{
 		cerr << "v[1][" <<i<<"] = (" << v[1][i].x << "," << v[1][i].y << ") ";
 	}
 	cerr << endl;
 	double middleY = (sumY(v[0]) + sumY(v[1])) / SIZE;

 	// Vecteur qui reccueil les bougies de premiere rangee.
 	vector<cv::Point> v2[2];

 	// cerr << "middleY = " << middleY << endl;
 	// cerr << "v[0].back() = (" << v[0].back().x << ", " << v[0].back().y << ")"<< endl;


	// Traiter 2eme rangee
 	for (int i = 0; i < 20; ++i)
 	{
 		if(see[i] && flags[i] == -1){
 			cerr << "Je traite " << i << endl;
 			
 			if (!v[0].empty() && v[0].back().x < v[1].back().x)
 			{
 				cv::Point mostLeft = v[0].back();
 				if(mostLeft.y < middleY){
 					v[0].pop_back();
 					flags[i] = 11;
					// checkTennis(i, v[2], mostLeft);
 					seeCount--;
 				}
 				else{
 					--i;
 					v2[0].push_back(v[0].back());
 					v[0].pop_back();
 				}
 			} else {
 				cv::Point mostLeft = v[1].back();
 				if(mostLeft.y < middleY){
 					v[1].pop_back();
 					flags[i] = 21;
					// checkTennis(i, v[2], mostLeft);
 					seeCount--;
 				}
 				else{
 					--i;
 					v2[1].push_back(v[1].back());
 					v[1].pop_back();
 				}
 			}
	 			cerr << "v[0].size()=" <<v[0].size() << " v[1].size()=" << v[1].size() << endl;
	 			cerr << "v2[0].size()=" <<v2[0].size() << " v2[1].size()=" << v2[1].size() << endl;
 		}
 		else if(see[i] && flags[i] != -1 && i < 8)
 			seeCount--;

 		if(v[0].size()==0 && v[1].size()==0){
 			traitementPremierRangee(v2, see);
 					cerr << "/**\n* Liste des bougies et couleurs\n*/\n";
 			for (int j = 0; j < 20; ++j)
 			{
 				string c = flags[j] == 21 ? "rouge" : "bleu";
 				if(flags[j] == -1 )
 					c = "-1";
 				cout << "flags[" << j << "]: " << c << endl;
 			}

 			string res(""+flags[0]);
 			for (int j = 1; j < 20; ++j)
 			{
 				res += "," + flags[j];
 			}
			cerr << "liste a renvoie:" << res;
 			return res;
 		}
 		
 	}
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
 			}
 			else if(color == 2){
 				sprintf(buffer, "%s(%d %d) ", buffer, matchLoc.x, matchLoc.y);
 			}
 			else if(color == 3){
 				sprintf(buffer, "%s(%d %d) ", buffer, matchLoc.x, matchLoc.y);
 			}
 		}
 		else
 			return;
 	}
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
