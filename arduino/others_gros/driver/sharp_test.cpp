#define PIN_SHARP_AVANT_GAUCHE 2
#define PIN_SHARP_AVANT_DROIT 3
#define PIN_SHARP_ARRIERE_GAUCHE 1
#define PIN_SHARP_ARRIERE_DROIT 0

#define THRESHOLD_PROCHE 300

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
}

void loop(){
	int sensor_value[4];

	sensor_value[0] = analogRead(PIN_SHARP_ARRIERE_DROIT);
	sensor_value[1] = analogRead(PIN_SHARP_ARRIERE_GAUCHE);
	sensor_value[2] = analogRead(PIN_SHARP_AVANT_GAUCHE);
	sensor_value[3] = analogRead(PIN_SHARP_AVANT_DROIT);

	for (int i = 0; i < 4; ++i)
	{
		if (sensor_value[i] > THRESHOLD_PROCHE){
  
                  Serial.println("Trop proche!");
                  Serial.println(i);
                  Serial.println("Avec:");
                  Serial.println(sensor_value[2]);
                }
 //           sendEvent(Q_SHARP_TROP_PROCHE, i+1);
	}
}
