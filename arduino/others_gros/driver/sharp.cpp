/**
 *Filename	:sharp.cpp
 *
 *Author	:YANG Qiao
 *Date		:09/03/2013
 */

#include "sharp.h"
#include "Arduino.h"
#include "protocole.h"
#include "message.h"

void check_sharp(){
	int sensor_value[4];

	sensor_value[0] = analogRead(PIN_SHARP_ARRIERE_DROIT);
	sensor_value[1] = analogRead(PIN_SHARP_ARRIERE_GAUCHE);
	sensor_value[2] = analogRead(PIN_SHARP_AVANT_GAUCHE);
	sensor_value[3] = analogRead(PIN_SHARP_AVANT_DROIT);

	for (int i = 0; i < 4; ++i)
	{
		if (sensor_value[i] > THRESHOLD_PROCHE)
            sendEvent(Q_SHARP_TROP_PROCHE, i+1);
	}
}

/**
 * Convert analog output into cm.
 * Not used because dependent on model.
 */
// float sharp_distance(){
// 	sharp_init();
// 	float distance=0;

// 	int sensor_value=analogRead(PIN_SHARP_ARRIERE_DROIT);

// 	distance=2020*pow(sensor_value,-1);

// 	return distance;
// }
