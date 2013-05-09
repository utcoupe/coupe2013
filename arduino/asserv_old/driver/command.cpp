#include "Arduino.h"
#include "command.h"
#include "parameters.h"
#include "robotstate.h"
#include "fifo.h"
#include "message.h"
#include "encoder.h"
#include "pwm.h"

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
			sendResponse(id, (char*)"pong");
			break;
		}

		case Q_SETPID:
		{
			//ENC_CENTER_DIST_MM = args[0]/10.0;
			break;
		}

		case Q_GOTO:
		{
			CHECK_ARGS(3)
			{
				pushGoalPosition(id,(double)args[0]*ENC_MM_TO_TICKS, (double)args[1]*ENC_MM_TO_TICKS, (double)args[2]);
				//sendResponse(id, 1);
			}
			break;
		}

		case Q_GOTOR:
		{
			CHECK_ARGS(3)
			{
				double co = cos(robot_get_angle());
				double si = sin(robot_get_angle());

				pushGoalPosition(id,((double)args[0]*co-(double)args[1]*si)*18+robot_get_x()*0.01, ((double)args[0]*si+(double)args[1]*co)*18+robot_get_y()*0.01, (double)args[2]);
				//sendResponse(id, 1);
			}
			break;
		}

		case Q_TURN:
		{
			CHECK_ARGS(2)
			{
				double angle = moduloPI(((double)args[0]) * DEG_TO_RAD);
				//sendMessage(-1, (int)(angle*100.0));
				pushGoalOrientation(id,angle,args[1]);
				//sendResponse(id, 1);
			}
			break;
		}

		case Q_TURNR:
		{
			CHECK_ARGS(2)
			{
				double angle = moduloPI(((double)args[0]) * DEG_TO_RAD + robot_get_angle());
				pushGoalOrientation(id,angle,args[1]);
				//sendResponse(id, 1);
			}
			break;
		}

		case Q_POS:
		{
			int x_mm = robot_get_x()*0.01*ENC_TICKS_TO_MM;
			int y_mm = robot_get_y()*0.01*ENC_TICKS_TO_MM;
			int a_deg = robot_get_angle()*RAD_TO_DEG;
			int tab[] = {x_mm,y_mm,a_deg};
			sendResponse(id, 3, tab);
	        break;
		}

		case Q_SET_POS:
		{
			CHECK_ARGS(2)
			{
				robot_set_mm_x(args[0]);
				robot_set_mm_y(args[1]);
				robot_set_deg_angle(args[2]);
				value_left_enc = 0;
				value_right_enc = 0;
				sendResponse(id, 0);
			}
			break;
		}

		case Q_PWM:
		{
			CHECK_ARGS(2)
			{
				pushGoalPwm(id,args[0],args[1],args[2]);
				//sendResponse(id, 1);
			}
			break;
		}
/*
		case Q_MODIF_GOAL_ABS:
		{
			CHECK_ARGS(3)
			{
				current_goal.type = TYPE_POSITION;
				current_goal.isReached = false;
				current_goal.x = (double)args[0]*ENC_MM_TO_TICKS;
				current_goal.y = (double)args[1]*ENC_MM_TO_TICKS;
				current_goal.speed = args[2];
				sendResponse(id, 1);
			}
			break;
		}

		case Q_MODIF_GOAL_REL:
		{
			CHECK_ARGS(3)
			{
				double co = cos(robot_state.angle);
				double si = sin(robot_state.angle);

				current_goal.type = TYPE_POSITION;
				current_goal.isReached = false;
				current_goal.x = (args[0]*co-args[1]*si)*18+robot_state.x;
				current_goal.y = (args[0]*si+args[1]*co)*18+robot_state.y;
				current_goal.speed = args[2];
				sendResponse(id, 1);
			}
			break;
		}
		*/
		case Q_CANCEL: /* comme stop */
		{
			clearGoals();
			current_goal.isCanceled = true;
			sendResponse(id, 0);
			break;
		}

		case Q_STOP: /* comme pause */
		{
			current_goal.isPaused = true;
			sendResponse(id, 0);
			break;
		}

		case Q_RESUME: /* comme resume */
		{
			current_goal.isPaused = false;
			sendResponse(id, 0);
			break;
		}

		case Q_RESET:
		{
			clearGoals();
			current_goal.isCanceled = true;
			current_goal.isPaused = false;
			value_left_enc = 0;
			value_right_enc = 0;
			robot_set_rad_angle(0.0);
			robot_set_x(0);
			robot_set_y(0);
			sendResponse(id, 0);
			break;
		}

		case Q_GETSENS:
		{
			if (value_pwm_right > 0)
				sendResponse(id, AVANT);
			else if (value_pwm_right < 0)
				sendResponse(id, ARRIERE);
			else
				sendResponse(id, ARRET);
			break;
		}

		case Q_GETENC:
		{
			int tab[2] = {value_left_enc,value_right_enc};
			sendResponse(id, 2, tab);
			break;
		}

		case Q_DEBUG : //TODO a degager quand tout marche
		{
			/*Serial.print("?,_________________§");
			Serial.print("uptime: ");Serial.print(millis());
			Serial.print("§angle: ");Serial.print(robot_state.angle, DEC);
			Serial.print("§speed: ");Serial.print(robot_state.speed*ENC_TICKS_TO_MM, DEC);
			Serial.print("§speedt: ");Serial.print(robot_state.speed, DEC);
			Serial.print("§x: ");Serial.print(robot_state.x*ENC_TICKS_TO_MM, DEC);
			Serial.print("§xt: ");Serial.print(robot_state.x, DEC);
			Serial.print("§y: ");Serial.print(robot_state.y*ENC_TICKS_TO_MM, DEC);
			Serial.print("§yt: ");Serial.print(robot_state.y, DEC);
			Serial.print("§encL: ");Serial.print(value_left_enc);
			Serial.print("§encR: ");Serial.println(value_right_enc);*/
			break;
		}
		

		default:
		{
			sendResponse(id,E_INVALID_CMD);
			break;
		}
	}
}
