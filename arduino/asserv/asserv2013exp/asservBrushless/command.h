#ifndef COMMAND_H_
#define COMMAND_H_

#include "protocole.h"

void cmd(int16_t id, int8_t cmd, int8_t size,int16_t args[]); // msg_uid, id_cmd, sizeArgs, *args

#endif /* COMMAND_H_ */
