#include <iostream>
#include "camManager.h"
#include "logger.h"
#include "calibManager.h"

using namespace std;

int main(int argc, char const *argv[])
{
	camManager *cam0 = new camManager(0);
	// calibManager *calib = new calibManager();
	cam0->loadSets();
	if(!cam0->Init())
	{
		cam0->LocatingWithPatternMatching();
		// cam0->SnapShot();
		// calib->yamlSetter(cam0);
		


	}
		
		

	return 0;
}