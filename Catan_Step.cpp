#include <cstdlib>
#include <string.h>
#include <iostream>
#include "Catan_Step.h"
#include "CatanField.h"


//========================================================================================
//содержит класс игры
//обеспечивает пошаговый контроль хода игры, передачу хода
//разрешения на действия при ходе
//откат действий до нажания ЗАВЕРШЕНИЕ ХОДА
// и ....
//class GAME_STEP

	//Step step[5];
	//int current_step;
	//int current_step_player;
	//int start_player;
	//char Game_name[30];
	//int total_players;
GAME_STEP::GAME_STEP()
{
	step[0].Step_Set(0);
	step[1].Step_Set(1);
	step[2].Step_Set(2);
	step[3].Step_Set(3);
	step[4].Step_Set(4);
}

extern GAME_STEP Game_Step;
extern PLAYER player[5];



//===============================================================
//  по результатам брока кубика назначаем первого ходящего
//===============================================================
int SetFirstPlayer()
{
	int pl, i;

  //std::cout << "SetFirstPlayer  назначает первого ходящего " << std::endl;

  //pl = 1 так как при старте игры первый подключенный бросает кубик первым
  for (pl = 1,i = 1; i < 5; i++)
	{
	if (player[i].active == true && player[i].first_roll > player[pl].first_roll)
	    {
		Game_Step.start_player = i;
		pl = i;
	    }
    }
  Game_Step.current_step++;   //переходим на след шаг
  Game_Step.current_active_player = Game_Step.start_player;

return(Game_Step.start_player);
}

//===============================================================
//===============================================================
int GetPrevPlayer()
{
	int prev_player;

	prev_player = Game_Step.current_active_player - 1;

	int i = 0;
	while (i++ < 6)  //цикл пока не найдем активного игрока или не попадем на первого ходящего
	{
		if (prev_player < 1)  prev_player = 4;
		if (player[prev_player].active == true)   return prev_player;
		else prev_player--;
	}

	return 0;
}


//===============================================================
//===============================================================
int GetNextPlayer()
{	
int next_player;

next_player = Game_Step.current_active_player + 1;

int i = 0;
while(i++ < 6)  //цикл пока не найдем активного игрока или не попадем на первого ходящего
    {
	if (next_player > 4) next_player = 1;
	if (player[next_player].active == true)   return next_player;
	    else next_player++;
   }

return 0;
}

//===============================================================
//==============================================================
int GAME_STEP::GetCurrentStep(void)
{
	return current_step;
}


//========================================================================================
//шаг 1 - бросок 1 кубика и выбор первого игрока
//шаг 2 - установка деревни и дороги без получения ресурсов
//шаг 3 - установка деревни и дороги с получением ресурсов
//шаг 4++  - бросок 2 кубиков, возможность строительста с получением ресурсов,
//            возможность обмена ресурсов, игры карт развития ....
//class Step

	//int wait_for_start;
	//int roll_1_dice;      //бросок 1 кубика
	//int roll_2_dice;      //бросок 2 кубиков
	//int flag_setTown;     //можно ставить город с правилом примыкания
	//int flag_setVillage;  //можно ставить деревню с правилом примыкания
	//int flag_set_one_Village;  //можно ставить деревню без правил примыкания
	//int flag_set_one_Town;
	//int flag_set_Road
	//int flag_set_one_Road
	//int flag_getResurs;   //получение ресурсов от всех построек
	//int flag_flag_getResurs_one_village;

Step::Step()
{
}

void Step::Step_Set(int i)
{
	if (i == 0)  wait_for_start = 1;
	if (i == 1)  roll_1_dice = 1;
	if (i == 2) { flag_set_one_Village = 1;  flag_set_one_Road = 1; }
	if (i == 3) { flag_set_one_Village = 1;   flag_set_one_Road = 1;    flag_getResurs_one_village = 1; }
	if (i == 4)
	{
		roll_2_dice = 1;
		flag_setVillage = 1;
		flag_setTown = 1;
		flag_getResurs = 1;   //получение ресурсов от всех построек
		flag_set_Road = 1;
	}
	return;
}