/**
 * \file 	urgDriver.h
 * \author 	Xavier RODRIGUEZ
 * \date	08/03/2012
 * 
 * */
#ifndef URGDRIVER_H
#define URGDRIVER_H
  
#include "global.h"

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include "json/json.h"
#include "delay.h" 
#include "UrgCtrl.h"

using std::list;  
using std::vector;
using std::string;
using namespace qrk;  
  
/**
 * \class UrgDriver
 * 
 * <h1>class UrgDriver</h1>
 * <p>classe de gestion du driver</p>
 * */ 
class UrgDriver
{
	
private:

	static UrgDriver* driverObj;
	void* loop();
	bool askValue;

	//
	short 		color;
	string 		comPort;	
	bool 		isConnected;
	list<coord> robot;
	//
	UrgCtrl 	urg;
	int 		scanMsec;
	//
	double 		radMin;
	double 		radMax;
	//
		
	long 		indexMax;
	long 		indexMin;
	//
	long* 		distanceMax;
	static const long minLength=20;
	static const long maxLength=3000;
	
	// Décalage de la balise par rapport aux (0,0)
	// Attention delta est négatif !
	int 		deltaX;
	int 		deltaY;
	
	double 		deg1;
	double 		deg2;
	
	//
	pthread_t thr;
	pthread_mutex_t  mutex;
	
	//
	UrgDriver();
	~UrgDriver();



	
public:

	//
	static UrgDriver* getUrgDriver();
	
	// On peut utiliser directement la fonction dans un thread perso
	static void* helpfct(void* arg);
	// Ou lancer le automatiquement le thread du manager
	void start();
	void stop();
	void waitHere();
	//
	pthread_mutex_t getMutex(){return this->mutex;};
	void setMutex(pthread_mutex_t m){this->mutex=m;};
	//
	void setComPort(string cp){comPort=cp;};
	void sendInfos(Json::Value &);
	//
	void toString();
	
	
	///! Fonction helpful
	//
	void connectHokuyo();
	void deconnectHokuyo();
	//
	long getData(vector<long>& data, long* timestamp);
	//
	string hokuyoFindPortCom(int nbOfPortToTest, int firstPortToTest=0);
	short  hokuyoFindColor();
	//
	void refInit();
	void UpdateRange();
	void updateParamWithColor(short color);
	void defineRange(double minDeg,double maxDeg);
	void defineRangeIndex(double minDeg,double maxDeg);
	//
	void setDelta(bool autoSearch, int dX=35, int dY=36);
	void calculLangleScanne();
	
	//! Fonctions compute
	bool checkPointBot(coord p1, coord p2);
	coord computeBotLocation(const list<coord> & bot);
	void interpretData(const vector<long> & data, int n);
	long distanceAuBord(coord c);
	bool inGateau(coord c);
};

 
 
 
#endif  // URGDRIVER_H
