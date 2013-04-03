
#include <iostream>
#include <string>
#include <ctime>

#include "json/json.h"
#include "../../../../lib/zerobot/cpp/service.hpp"
#include "mainAppDriver.h"
#include "mainAppException.h"
#include "urgDriver.h"
#include "urgException.h"
// #include "controlFunction.h"
#include "global.h"

using namespace std;


//! Instance global
MainAppDriver* application;


class Hokuyo : public Service
{
public:
	Hokuyo(int argc, char *argv[], const string & identity, const string & addr, CONNECTION_TYPE type=CONNECT)
	: Service(identity, addr, type) {

		application = new MainAppDriver(argc,argv);
		// application->waitHere();
	};
	
protected:
	virtual void process_request(const string & remote_id, const Json::Value & request) {
			// "ping"
		if (request["fct"] == "ping") {
			Json::Value data("pong");
			sendResponse(remote_id, request, data);
		}
		else if (request["fct"] == "help") {
			Json::Value data(
				"-> ping(): pong \n "
				"-> send_pos(): recuperer les positions des robots.\n"
				"-> setRed(): le red c'est la maison!\n"
				"-> setBlue(): le blue c'est la maison!\n"
				);
			sendResponse(remote_id, request, data);
		}

		else if (request["fct"] == "setRed") {
			
			application->ud->stop();
			application->ud->updateParamWithColor(ROUGE);
			application->ud->start();
			cout << "hokuyo cote red\n";

			Json::Value data("hokuyo cote red");
			sendResponse(remote_id, request, data);
				// response["error"]["error"] = "";
				// response["error"]["tb"] = "";
		}
		else if (request["fct"] == "setBlue"){
			application->ud->stop();
			application->ud->updateParamWithColor(BLEU);
			application->ud->start();
			cout << "hokuyo cote blue\n";

			Json::Value data("hokuyo cote blue");
			sendResponse(remote_id, request, data);
		}
			// On recuprer les positions.
		else if (request["fct"] == "send_pos"){
			Json::Value res;
			application->ud->sendInfos(res);
			sendResponse(remote_id, request, res);
		}

		else {
			this->sendError(remote_id, request, "Unknown function", "");
		}
	}

private:

};


int main(int argc, char *argv[]) {
	
	Hokuyo hokuyo(argc, argv, "hokuyo", "tcp://*:5001", Service::CONNECT);
	cout << "connect on port 5001" << endl;

	while (!s_interrupted) {
		hokuyo.read();
	}

	return 0;
}


