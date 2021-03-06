
#include <iostream>
#include <string>
#include <ctime>

#include "json/json.h"
#include "../../lib/zerobot/cpp/service.hpp"

#include "camManager.h"
#include "logger.h"
#include "calibManager.h"
#include "camException.h"

using namespace std;

class Visio : public Service
{
public:
	Visio(const int camID, const int graphic_mode, const bool debug_mode, const string & identity, const string & addr, CONNECTION_TYPE type=CONNECT)
	: Service(identity, addr, type) {
		
		debug = debug_mode;
		
		cam_0 = new camManager(camID, graphic_mode);
		cam_0->Init();

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
				"CAM_ID = 0 pour la webcam interne, 1 pour la webcam usb. \n "
				"-> ping(): pong \n "
				"-> calib(CAM_ID): calibrer la camera par-rapport-a couleur. \n"
				"\t tapez r, b, w pour filtrer les couleurs en binaire, vous pouvez tapez r, regler les config, puis v pour valider, puis b pour bleu, puis v .... etc \n "
				"\t necessite aussi activer la camera. \n "
				"-> get_by_pattern(CAM_ID), permet de retourner les positions des patterns de trois couleurs. \n"
				"-> set_ROI(CAM_ID), permet de selectionner la zone d'interet.\n"
				"-> get_by_color(int x, int y, int angle), permet de trouver les objets par couleurs. (x,y,angle), position actuelle du robot. \n"
				);
			sendResponse(remote_id, request, data);
		}
			// "calib(0)" or "calib(1)", this is used for colormatching, which is not recommended for 2013.
		else if (request["fct"] == "calib") {
			calibManager *calib = new calibManager();
			int camId = request["args"][0].asInt();
			if(camId == 0)
				calib->yamlSetter(* cam_0);
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
		else if (request["fct"] == "get_by_pattern"){
			int camId = request["args"][0].asInt();
			double threshold = request["args"][1].asDouble();
			Json::Value res;

			switch(camId){
				case 0:
				res = this->cam_0->testCase(threshold);
				sendResponse(remote_id, request, res);
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
		else if (request["fct"] == "set_ROI"){
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

		else if (request["fct"] == "get_by_color"){
			if(this->debug){
				Json::Value res = this->cam_0->DisplayLoopWithColorMatching();
				sendResponse(remote_id, request, res);
			}
			else{
				try{
					Json::Value res = this->cam_0->DisplayWithColorMatching(request["args"][0].asInt(), request["args"][1].asInt(), request["args"][2].asInt());
					sendResponse(remote_id, request, res);
				}
				catch (camException ex){
					// Retry If we didn't have exacte number of candles detected.
					cerr << "Didn't get enough candles. Retrying..." << endl;
					for (int i = 0; i < 20; ++i)
						this->cam_0->flags[i] = -1;
					try{
					Json::Value res = this->cam_0->DisplayWithColorMatching(request["args"][0].asInt(), request["args"][1].asInt(), request["args"][2].asInt());
					sendResponse(remote_id, request, res);
					}
					catch(camException ex){
						for (int i = 0; i < 20; ++i)
							this->cam_0->flags[i] = -1;
						this->sendError(remote_id, request, "Visio doesn't work, move and retry!", "");
					}
				}
			}
		}

		else {
			this->sendError(remote_id, request, "Unknown function", "");
		}
	}

private:
	camManager *cam_0;
	bool debug;
	// camManager *cam_1;

};

int main(int argc, char *argv[]) {

	if(argc != 4)
		cout << "Help -> \"visio camID graphic_mode debug_mode\"\n"
			<< "\t1 for external cam, 0 for internal cam\n"
			<< "\t0 for no graphic_mode, 1 for graphic_mode\n" 
			<< "\t0 for match_mode, 1 for debug_mode"<< endl;

	Visio visio(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), "visio", "tcp://*:5001", Service::CONNECT);
	cout << "connect on port 5001" << endl;

	while (!s_interrupted) {
		visio.read();
	}

	return 0;
}


