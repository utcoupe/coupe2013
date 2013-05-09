#ifndef COMMAND_H_
#define COMMAND_H_

#include "message.h"
#include "protocole.h"
#include "jack.h"
#include "emergency.h"
#include "controle_ax12.h"

#define DUREE_CYCLE 10
#define PIN_BALLON 11

void cmd(int16_t id, int8_t cmd, int8_t size,int16_t args[]); // msg_uid, id_cmd, sizeArgs, *args

#endif /* COMMAND_H_ */
