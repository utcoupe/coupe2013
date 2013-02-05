
#include <iostream>
#include <string>
#include <ctime>

#include "json/json.h"
#include "zhelpers.hpp"
#include "service.hpp"

#include "camManager.h"
#include "logger.h"
#include "calibManager.h"

using namespace std;

class Visio : public Service
{
	public:
		Visio(const string & identity, const string & addr, CONNECTION_TYPE type=CONNECT)
			: Service(identity, addr, type) {
				// Set both cam to display = 0, no graphic mode.
				cam_0 = new camManager(0);
				cam_0->Init();
				// cam_0->SnapShot();
				// cam_1 = new camManager(1, 0);



				/**
				 * It takes 3secs to grab a picture from the camera (if cam is not initialized). It seems better to grab a picture at launch so as to intialize the camera, then, taking a picture is much faster
				 */

				// this->cam_1->Init()
			};
	
	protected:
		virtual void process_request(const string & remote_id, const Json::Value & request) {
			// "ping"
			if (request["fct"] == "ping") {
			    Json::Value data("pong");
			    sendResponse(remote_id, request, data);
			}
			// "who"
			else if (request["fct"] == "who") {
			    Json::Value data("visio");
			    sendResponse(remote_id, request, data);
			}
			// "calib(0)" or "calib(1)", this is used for colormatching, which is not recommended for 2013.
			else if (request["fct"] == "calib") {
				calibManager *calib = new calibManager();
				int camId = request["args"][0].asInt();
				if(camId == 0)
					calib->yamlSetter(this->cam_0);
				// else if(camId == 1)
				// 	calib->yamlSetter(this->cam_1);
				cout << "Type b for blue, r for red, w for white. Use trackbars to adjust to the best calibration, then type v to save it.";
				
			    Json::Value data("");
			    sendResponse(remote_id, request, data);
				// response["error"]["error"] = "";
				// response["error"]["tb"] = "";
			}
			// "getcandles 0" or "getcandles 1" to get candles' positions. positions are returned by red, blue and white.
			// It should return positions only for ones with tennis ball on top.
			else if (request["fct"] == "getcandles"){
				int camId = request["args"][0].asInt();
				string result;
				Json::Value response;

				switch(camId){
					case 0:
						result = this->cam_0->testCase();
        			    response = new Json::Value (result);
						sendResponse(remote_id, request, response);
					break;

					case 1:
						// this->cam_1->testCase(&response);
					break;
					default:
					char buffer[80];
					sprintf(buffer, "At %s:%s line: %d", __FILE__, __FUNCTION__, __LINE__);
					sendError(remote_id, request, "Unknown camID", buffer);
				}

			}
			// "setROI(0)" or "setROI(1)"
			else if (request["fct"] == "setROI"){
				int camId = request["args"][0].asInt();

				switch(camId){
					case 0:
						this->cam_0->LocatingWithPatternMatching();
					break;

					case 1:
						// this->cam_1->LocatingWithPatternMatching();
						// response["error"] = "";
					break;
					default:
					char buffer[80];
					sprintf(buffer, "At %s:%s line: %d", __FILE__, __FUNCTION__, __LINE__);
					sendError(remote_id, request, "Unkown camId", buffer);
				}

			}
			else {
				this->sendError(remote_id, request, "Unknow function", "");
			}
		}

	private:
		camManager *cam_0;
		// camManager *cam_1;

};

int main() {
	
	

	Visio visio("visio", "tcp://*:5001", Service::CONNECT);
	cout << "connect on port 5001" << endl;
	
	while (!s_interrupted) {
		visio.read();
	}

	return 0;
}


