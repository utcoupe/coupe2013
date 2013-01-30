#include "Arduino.h"
#include "command.h"
#include "message.h"

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
      }

		default:
		{
			sendResponse(id,E_INVALID_CMD);
			break;
		}
	}
}
