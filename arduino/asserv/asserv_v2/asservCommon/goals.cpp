/****************************************
 * Author : Quentin C			*
 * Mail : quentin.chateau@gmail.com	*
 * Date : 08/04/13      		*
 ****************************************/
#include "goals.h"

Fifo fifo;

/********************************************************
 * 							*
 * 		      CLASSE FIFO			*
 *							*
 ********************************************************/
Fifo::Fifo(){
	nbrGoals = 0;
	current_goal = 0;
	maxGoals = MAX_GOALS;
}

int Fifo::pushGoal(int ID, int p_type, double p_data_1, double p_data_2, double p_data_3){
	if(nbrGoals >= maxGoals)
		return 1; //si on est deja au max de goals, return 1

	if(current_goal != 0){
		struct goal *last_goal = current_goal;

		while(last_goal->next != 0)
			last_goal = last_goal->next; //va chercher le dernier goal

		last_goal->next = new struct goal;
		if(last_goal->next != 0){//Si l'allocation réussit
			last_goal = last_goal->next;//last_goal devient notr enouveau goal
			//definition du nouveau goal
			last_goal->type = p_type;
			last_goal->data_1 = p_data_1;
			last_goal->data_2 = p_data_2;
			last_goal->data_3 = p_data_3;
			last_goal->ID = ID;
			last_goal->isReached = false;
			last_goal->next = 0;

			nbrGoals++;//Un goal de plus !
			return 0;
		}
		else
			return 1;//Si echec de l'allocation, renvoie 1
	}
	else{
		current_goal = new struct goal;
		if(current_goal != 0){
			current_goal->type = p_type;
			current_goal->data_1 = p_data_1;
			current_goal->data_2 = p_data_2;
			current_goal->data_3 = p_data_3;
			current_goal->ID = ID;
			current_goal->isReached = false;
			current_goal->next = 0;
			nbrGoals++;//Un goal de plus !
			return 0;
		}
		else
			return 1;
	}
}

struct goal Fifo::gotoNext(){
	if(current_goal != 0){
		struct goal *deleted_goal = current_goal;
		current_goal = current_goal->next;
		delete deleted_goal;
		nbrGoals--;
	}
	return Fifo::getCurrentGoal();
}

void Fifo::killGoal(int ID){
	if(current_goal != 0){
		struct goal *seek_goal, *deleted_goal;

		seek_goal = current_goal;

		while(seek_goal->next != 0){ //on passe tout les goals a partir du deuxieme en revue
			if(seek_goal->next->ID == ID){ //et on supprime les bons ID
				if(seek_goal->next->next == 0){
					delete seek_goal->next;
					seek_goal->next = 0;
				}
				else{
					deleted_goal = seek_goal->next;
					seek_goal->next = deleted_goal->next;
					delete deleted_goal;
				}
			}
			else
				seek_goal = seek_goal->next;
		}
		
		if(current_goal->ID == ID) //puis on s'occupe du premier
			gotoNext();
	}
}


void Fifo::clearGoals(){
	while(current_goal != 0)
		gotoNext();
}

void Fifo::pushIsReached(){
	current_goal->isReached = true;
}

int Fifo::getRemainingGoals(){
	return nbrGoals;
}

struct goal Fifo::getCurrentGoal(){
	if(current_goal == 0){
		struct goal no_goal;
		no_goal.type = NO_GOAL;
		return no_goal;
	}
	else
		return *current_goal;
}

void Fifo::pause(){
	current_goal->isPaused = true;
}

void Fifo::resume(){
	current_goal->isPaused = false;
}
