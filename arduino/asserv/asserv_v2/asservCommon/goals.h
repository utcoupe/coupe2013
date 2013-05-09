/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 08/04/13			*
 ****************************************/
#ifndef GOALS_H
#define GOALS_H

#include "parameters.h"

#define TYPE_POS 1
#define TYPE_ANG 2
#define TYPE_PWM 3
#define TYPE_SPD 4
#define NO_GOAL -1

struct goal{
	int type;
	double data_1; //	x	angle	pwmL	spdL
	double data_2; //	y	.	pwmR	spdR
	double data_3; //	.	.	duree	duree
	int ID;
	bool isReached;
	bool isPaused;
	struct goal *next; //Chaque goal contient un pointeur vers le goal suivant;
};
	
class Fifo{
	public:
	int pushGoal(int ID, int p_type, double p_data_1 = 0, double p_data_2 = 0, double p_data_3 = 0);
	void killGoal(int ID);
	void clearGoals();
	struct goal gotoNext(); //Va au goal suivant et le renvoit
	void pushIsReached(); //Set le flag isReached du current goal

	struct goal getCurrentGoal(); //Renvoie le current goal
	int getRemainingGoals(); //Renvoie le nombre de goals restant

	void pause();//met en pause
	void resume();//arrete la pause

	Fifo(); //Constructeur
	private:
	struct goal *current_goal;
	int nbrGoals;
	int maxGoals;
};

extern Fifo fifo;

#endif
