/**
 * \file 	argManager.cpp
 * \author 	Xavier RODRIGUEZ
 * \date	08/03/2012
 * 
 * */

#include "argManager.h"

#include <stdlib.h>

const string ArgManager::GET_STRING = "";

//! Constructor
ArgManager::ArgManager()
{
	this->nbOfArg = 0;
}
	
//! 
void ArgManager::addArg(absArgument* arg)
{
	arguments.push_back(arg);
	incArgNumber();
}		

//! 
absArgument* ArgManager::getArg(string tag)
{
	for ( it=arguments.begin() ; it!=arguments.end() ; it++ )
	{
		#if DEBUG
		cout << "getTag() = " << (*it)->getTag() << " tag = " << tag << endl;
		#endif

		if(!(*it)->getTag().compare(tag))
		{
			return (*it);
		}
	}
	return NULL;
}


//! 
void ArgManager::analyse(int argc, char *argv[])
{
	// Lecture des paramétres
	int nbParam = (argc-1)/2;
	int acParam = 0;
	while(acParam<nbParam && nbParam>0)
	{
		acParam++;
		string argTag = string(argv[acParam]);	
		absArgument* arg = getArg(argTag);
		if(arg==NULL){
			cerr << "Erreur argument " << argTag << " : tag non valide" << endl;
			stopProcess();
		}
		
		Argument<string>* strArg = dynamic_cast<Argument<string>* >(arg);
		Argument<int>* intArg = dynamic_cast<Argument<int>* >(arg);
		if(strArg)
		{	
			#if DEBUG
			cout << "if(strArg) ok" << " Value = " << string(argv[acParam+1]);
			#endif
			strArg->validArg();
			strArg->setValue(string(argv[acParam+1]));
		}
		else if(intArg)
		{
			#if DEBUG
			cout << "acParam = " << acParam << "argc = "<< argc << endl;
			cout << "if(intArg) ok" << " Value = " << string(argv[acParam+1]) << endl;
			#endif
			intArg->validArg();
			string strInt = string(argv[acParam+1]);
			intArg->setValue(atoi(strInt.c_str()));
		}
		else
		{
			cerr << "Erreur argument " << argTag << " : type non valide" << endl;
			stopProcess();
		}
		
	}
}


//!
void ArgManager::stopProcess()
{
	#if DEBUG
	cerr << "Hokuyo Should stop here\n";
	#endif
}


//!
void ArgManager::incArgNumber()
{
	this->nbOfArg++;
}
