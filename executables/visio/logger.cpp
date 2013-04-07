#include <iostream>
#include "logger.h"

void Logger::log(int msg)
{
	cerr << "> [ DEBUG | " << this->name << ":" << this->id << " ] " << msg << endl;
}

void Logger::log(string msg)
{
	cout << "> [ DEBUG | " << this->name << ":" << this->id << " ] " << msg << endl;
}

void Logger::err(int msg)
{
	cout << "> [ ERR | " << this->name << ":" << this->id << " ] " <<"Get int: " << msg << endl;
}

void Logger::err(string msg)
{
	cout << "> [ ERR | " << this->name << ":" << this->id << " ] " << msg << endl;
}