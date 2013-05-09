#include "Arduino.h"
#include "command.h"
#include "parameters.h"
#include "message.h"
#include "control.h"

#define CHECK_ARGS(nbr) if (size < nbr) { sendResponse(id, E_INVALID_PARAMETERS_NUMBERS); } else

/**
 * Analyse le message et effectue les actions associees
 *
 * @param id : l'identifiant associe au message
 * @param header : le type de message (en-tete)
 * @param args : le tableau d'entier contenant les arguments
 * */
void cmd(int16_t id, int8_t cmd, int8_t size, int16_t *args){
                        
	/* On analyse le message en fonction de son type */
	switch(cmd){

		case Q_ID: /* Identification */
		{
			sendResponse(id, (char *)ID_ASSERV);
			break;
		}

		case Q_PING:
		{
			sendResponse(id, (char*)"Pong");
			break;
		}

		case Q_SET_PID_ANG:
		{
			CHECK_ARGS(3){
				control.setPID_angle(args[0],args[1],args[2]);
			}
			break;
		}
		case Q_SET_PID_DIS:
		{
			CHECK_ARGS(3){
				control.setPID_distance(args[0],args[1],args[2]);
			}
			break;
		}
		case Q_SET_PID_SPD:
		{
			CHECK_ARGS(3){
				control.setPID_speed(args[0],args[1],args[2]);
			}
			break;
		}
		case Q_SET_PWM_MIN:
		{
			CHECK_ARGS(1){
				control.setPwmMin(args[0]);
			}
			break;
		}

		case Q_GOTO:
		{
			CHECK_ARGS(2)
			{
				control.pushGoal(id,TYPE_POS, args[0], args[1]);
				//sendResponse(id, 1);
			}
			break;
		}

		case Q_GOTOR:
		{
			CHECK_ARGS(2)
			{
				pos current = control.getPos();
				double co = cos(current.angle);
				double si = sin(current.angle);

				control.pushGoal(id,TYPE_POS, current.x + (args[0]*co - args[1]*si), current.y + (args[0]*si + args[1]*co));
				//sendResponse(id, 1);
			}
			break;
		}

		case Q_TURN:
		{
			CHECK_ARGS(1)
			{
				double angle = moduloTwoPI(((double)args[0]) * DEG_TO_RAD);
				//sendMessage(-1, (int)(angle*100.0));
				control.pushGoal(id,TYPE_ANG,angle);
				//sendResponse(id, 1);
			}
			break;
		}

		case Q_TURNR:
		{
			CHECK_ARGS(2)
			{
				double angle = moduloTwoPI(((double)args[0]) * DEG_TO_RAD + control.getPos().angle);
				control.pushGoal(id,TYPE_ANG,angle);
				//sendResponse(id, 1);
			}
			break;
		}

		case Q_POS:
		{
			pos current = control.getPos();
			int tab[] = {(int)(current.x),(int)(current.y),(int)(current.angle * RAD_TO_DEG)};
			sendResponse(id, 3, tab);
	        	break;
		}

		case Q_SPD:
		{
			CHECK_ARGS(3)
			{
				control.pushGoal(id, TYPE_SPD, args[0], args[1], args[2]);
			}
			break;
		}

		case Q_SET_POS:
		{
			CHECK_ARGS(2)
			{
				pos n_pos;
				n_pos.x = args[0];
				n_pos.y = args[1];
				n_pos.angle = args[2] * DEG_TO_RAD;
				control.pushPos(n_pos);
				sendResponse(id, 0);
			}
			break;
		}

		case Q_PWM:
		{
			CHECK_ARGS(3)
			{
				control.pushGoal(id,TYPE_PWM,args[0],args[1],args[2]);
				//sendResponse(id, 1);
			}
			break;
		}

		case Q_CANCEL: /* comme stop */
		{
			control.clearGoals();
			sendResponse(id, 0);
			break;
		}

		case Q_PAUSE: /* comme pause */
		{
			control.pause();	
			sendResponse(id, 0);
			break;
		}

		case Q_RESUME: /* comme resume */
		{
			control.resume();
			sendResponse(id, 0);
			break;
		}

		case Q_RESET:
		{
			control.reset();
			sendResponse(id, 0);
			break;
		}

		case Q_GETENC:
		{
			int tab[2] = {(int)control.getEncL(),(int)control.getEncR()};
			sendResponse(id, 2, tab);
			break;
		}

		case Q_GETSPD:
		{
			spd motorSpd = control.getMotorSpd();
			Serial.print("§speed right: ");Serial.print(motorSpd.R, DEC);
			Serial.print("§speed left: ");Serial.print(motorSpd.L, DEC);
			break;
		}

		case Q_DEBUG :
		{
			pos current = control.getPos();
			spd currentSpd = control.getMotorSpd();
			Serial.print("?,_________________§");
			Serial.print("uptime: ");Serial.print(millis());
			Serial.print("§angle: ");Serial.print(current.angle, DEC);
			Serial.print("§speed right: ");Serial.print(currentSpd.R, DEC);
			Serial.print("§speed left: ");Serial.print(currentSpd.L, DEC);
			Serial.print("§x: ");Serial.print(current.x, DEC);
			Serial.print("§y: ");Serial.print(current.y, DEC);
			Serial.print("§encL: ");Serial.print(control.getEncL());
			Serial.print("§encR: ");Serial.println(control.getEncR());
			break;
		}
		

		default:
		{
			sendResponse(id,E_INVALID_CMD);
			break;
		}
	}
}
