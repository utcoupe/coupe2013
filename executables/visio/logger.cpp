#include <iostream>
#include "logger.h"

void Logger::log(int msg)
{
	cerr << "> [LOG] " << "Get int: " << msg << endl;
}

void Logger::log(string msg)
{
	cout << "> [LOG] " << msg << endl;
}

void Logger::err(int msg)
{
	cout << "> [ERR] " <<"Get int: " << msg << endl;
}

void Logger::err(string msg)
{
	cout << "> [ERR] " << msg << endl;
}