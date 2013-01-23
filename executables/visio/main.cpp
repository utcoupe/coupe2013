#include <iostream>
#include "camManager.h"
#include "logger.h"

using namespace std;

int main(int argc, char const *argv[])
{
	camManager *cam = new camManager(0);
	if(!cam->Init())
		cam->loop();
	return 0;
}