
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
			Json::Value response;
			response["uid"] = request["uid"];
			// "ping"
			if (request["fct"] == "ping") {
				response["data"] = "pong";
				response["error"] = "";
				// response["error"]["error"] = "";
				// response["error"]["tb"] = "";
				s_sendmore(_socket, remote_id);
			}
			// "who"
			else if (request["fct"] == "who") {
				response["data"] = "visio";
				response["error"] = "";
				// response["error"]["error"] = "";
				// response["error"]["tb"] = "";
				s_sendmore(_socket, remote_id);
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
				response["data"] = "";
				response["error"] = "";
				// response["error"]["error"] = "";
				// response["error"]["tb"] = "";
				s_sendmore(_socket, remote_id);
			}
			// "getcandles 0" or "getcandles 1" to get candles' positions. positions are returned by red, blue and white.
			// It should return positions only for ones with tennis ball on top.
			else if (request["fct"] == "getcandles"){
				int camId = request["args"][0].asInt();

				switch(camId){
					case 0:
						this->cam_0->testCase(&response);
					break;

					case 1:
						// this->cam_1->testCase(&response);
					break;
					default:
					char buffer[80];
					sprintf(buffer, "Unkown camId at %s:%s line: %d", __FILE__, __FUNCTION__, __LINE__);
					response["data"] = "";
					response["error"]["error"] = buffer;
					response["error"]["tb"] = "";
				}

				s_sendmore(_socket, remote_id);
			}
			// "setROI(0)" or "setROI(1)"
			else if (request["fct"] == "setROI"){
				int camId = request["args"][0].asInt();

				switch(camId){
					case 0:
						this->cam_0->LocatingWithPatternMatching();
						response["error"] = "";
					break;

					case 1:
						// this->cam_1->LocatingWithPatternMatching();
						// response["error"] = "";
					break;
					default:		
					char buffer[80];
					sprintf(buffer, "Unkown camId at %s:%s line: %d", __FILE__, __FUNCTION__, __LINE__);
					response["data"] = "";
					response["error"]["error"] = buffer;
					response["error"]["tb"] = "";
				}

			}
			else {
				response["data"] = "";
				response["error"]["error"] = "unknown function";
				response["error"]["tb"] = "";
			}
			string packed_response = _writer.write( response );
			s_send(_socket, packed_response);
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

