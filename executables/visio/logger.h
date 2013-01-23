// logger.h
#ifndef LOGGER_H
#define LOGGER_H

using namespace std;

class Logger
{
private:
	int id;
	string name;

public:
	Logger(string name, int id) {this->id = id; this->name = name;}
	void err(string msg);
	void err(int msg);
	void log(string msg);
	void log(int c);
};

#endif