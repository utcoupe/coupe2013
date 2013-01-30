#include "Arduino.h"

#include "command.h"

unsigned long index = 0;
unsigned long timeStart = 0;

void setup(){
	/*Active la liaison serie*/
	initSerialLink();

    /* Détection des ax12 */
    cherche_moteurs();

    /* Active le jack et le bouton d'arrêt*/
    initJackDetection();
    initEmergencyStopDetection();

	// LED qui n'en est pas une
	pinMode(16,OUTPUT);
}

void loop(){
	/* on note le temps de debut */
	timeStart = micros();

	/* La del est allumee pendant le traitement */
	digitalWrite(16, HIGH);

	/* zone programmation libre */
	
	/*lecture des ordres*/
	readIncomingData(cmd);

    /*on regarde l'état du jack et du bouton d'arrêt*/
    jackCheck();
    if (emergencyCheck()) {
      //STOP ALL THE STUFF !
    }
	
	/* fin zone de programmation libre */
	
	/* On eteint la del */
	digitalWrite(16, LOW);
	
	/* On attend le temps qu'il faut pour boucler */
	long udelay = DUREE_CYCLE*1000-(micros()-timeStart);
	if(udelay<0)
		Serial.println("ouch : mainloop trop longue");
	else
		 delayMicroseconds(udelay);
}


