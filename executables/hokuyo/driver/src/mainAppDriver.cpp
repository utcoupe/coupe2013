


#include "mainAppDriver.h"
#include "mainAppException.h"
#include "global.h"


/***********************************************************************
 * <h1>Constructeur</h1>
 * 
 * @param arg - parametres du main à passer
 **********************************************************************/
MainAppDriver::MainAppDriver(int argc, char *argv[])
{
	if(argc < 3){
		cerr << "Execution type: ./hokuyo -color 0\n";
		cerr << "\t 0 pour red, 1 pour bleu\n";
		throw mainAppException(this, 50);
	}

	initArgManager(argc,argv);
	initUrgDriver();
}

/***********************************************************************
 * 
 **********************************************************************/
void MainAppDriver::initArgManager(int argc, char *argv[])
{
	//! --- Récupération des paramétres ---
	am = new ArgManager();
	// am->addArg(new Argument<string>(TAG_PORTCOM));	// Le port com
	am->addArg(new Argument<int>(TAG_COLOR)); 		// La couleur 
	am->analyse(argc,argv);
}



/***********************************************************************
 * 
 **********************************************************************/
void MainAppDriver::initUrgDriver()
{
		//! --- Start Hokuyo ---
		ud = UrgDriver::getUrgDriver();
		
			// On récupére le port com
		Argument<string>* portcom = am->getArg(TAG_PORTCOM,ArgManager::GET_STRING);
		if(portcom->isValid()){
			ud->setComPort(portcom->getValue());
		}
		else{
			ud->setComPort(ud->hokuyoFindPortCom(10));
		}
			// On récupére la couleur
		Argument<int>* argColor = am->getArg(TAG_COLOR,ArgManager::GET_INT);
		if(argColor->isValid()){
			ud->updateParamWithColor(argColor->getValue());
		}
		else{
			// ud->updateParamWithColor(ud->hokuyoFindColor());
			throw mainAppException(this, mainAppException::Err_argException_react_tropDerr);
		}

			// On test la connection
		ud->refInit();
		#if DEBUG
			ud->toString();
		#endif

		ud->start();

}

/***********************************************************************
 * 
 **********************************************************************/
void MainAppDriver::waitHere()
{	
	ud->waitHere();    
	// cm->waitHere();
}
