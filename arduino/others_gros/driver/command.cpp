#include "Arduino.h"
#include "command.h"
#include "message.h"

#define CHECK_ARGS(nbr) if (size < nbr) { sendError(id, E_INVALID_PARAMETERS_NUMBERS); } else

/**
 * Analyse le message et effectue les actions associees
 *
 * @param id : l'identifiant associe au message
 * @param header : le type de message (en-tete)
 * @param args : le tableau d'entier contenant les arguments
 * */
void cmd(int16_t id, int8_t cmd, int8_t size, int16_t *args){
    char error;

    /* On analyse le message en fonction de son type */
    switch(cmd){

    case Q_ID: /* Identification */
        {
            sendResponse(id, (char *)ID_OTHERS);
            break;
        }

    case Q_PING:
        {
            CHECK_ARGS(1)
                {
                    sendResponse(id, args[0]+42);
                }
            break;
        }

    case Q_GOTO_AX12:
        {
            CHECK_ARGS(3)
                {
                    tourner(id, args[0], args[1], args[2]);
                }
			break;
        }
    case Q_POS_AX12:
        {
            CHECK_ARGS(1)
                {
                    int resp = get_position(args[0], &error);
                    //if (error == 0)
                        sendResponse(id, resp);
                        //else
                        //sendError(id, error);
                }
			break;
        }
    case Q_READ_AX12:
        {
            CHECK_ARGS(2)
                {
                    int resp = lire(args[1], args[0], &error);
					sendResponse(id, resp);
                }
			break;
        }

    case Q_WRITE_AX12:
        {
            CHECK_ARGS(3)
                {
					if (args[0] >= NB_MOTEURS) {
						sendError(id, E_INVALID_PARAMETERS_VALUE);
					} else {
						motor[args[0]].writeInfo(args[1], args[2]);
						sendResponse(id, motor[args[0]].status_error);
					}
                }
			break;
        }
	case Q_DETECT_AX12:
		{
			CHECK_ARGS(0)
				{
					cherche_moteurs();
					sendResponse(id, 18, reverse);
				}
            break;
		}
	case Q_BALLON:
		{
			digitalWrite(PIN_BALLON, HIGH);
			delay(2000);
			digitalWrite(PIN_BALLON, LOW);
		}
	break;
    default:
        {
            sendError(id,E_INVALID_CMD);
            break;
        }
    }
}
