#pragma once
#ifndef GAME_STEP_H
#define GAME_STEP_H


int GetNextPlayer();
int GetPrevPlayer();
int SetFirstPlayer();

//========================================================================================
//шаг 1 - бросок 1 кубика и выбор первого игрока
//шаг 2 - установка деревни и дороги без получения ресурсов
//шаг 3 - установка деревни и дороги с получением ресурсов
//шаг 4++  - бросок 2 кубиков, возможность строительста с получением ресурсов,
//            возможность обмена ресурсов, игры карт развития ....
class Step
{
public:
	int wait_for_start = 1;
	int roll_1_dice = 0;      //бросок 1 кубика
	int roll_2_dice = 0;      //бросок 2 кубиков
	int flag_setTown = 0;     //можно ставить город с правилом примыкания
	int flag_setVillage = 0;  //можно ставить деревню с правилом примыкания
	int flag_set_one_Village = 0;  //можно ставить деревню без правил примыкания
	int flag_set_one_Town = 0;
	int flag_set_Road = 0;
	int flag_set_one_Road = 0;
	int flag_getResurs = 0;   //получение ресурсов от всех построек
	int flag_getResurs_one_village = 0;
	int flag_bandit = 0;

	Step();
	void Step_Set(int);
	int GetCurrentStep(void);
};

//========================================================================================
//содержит класс игры
//обеспечивает пошаговый контроль хода игры, передачу хода
//разрешения на действия при ходе
//откат действий до нажания ЗАВЕРШЕНИЕ ХОДА
// и ....
class GAME_STEP
{
public:
	Step step[5];
	int current_step = 0;
	int current_active_player = 0;
	int start_player = 1;
	char Game_name[30] = "noname.gme";
	int total_players = 0;

	GAME_STEP();
	int GetCurrentStep(void);

};

#endif
