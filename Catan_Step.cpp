#include <cstdlib>
#include <string.h>
#include <iostream>
#include "Catan_Step.h"
#include "CatanField.h"


//========================================================================================
//�������� ����� ����
//������������ ��������� �������� ���� ����, �������� ����
//���������� �� �������� ��� ����
//����� �������� �� ������� ���������� ����
// � ....
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
//  �� ����������� ����� ������ ��������� ������� ��������
//===============================================================
int SetFirstPlayer()
{
	int pl, i;

  //std::cout << "SetFirstPlayer  ��������� ������� �������� " << std::endl;

  //pl = 1 ��� ��� ��� ������ ���� ������ ������������ ������� ����� ������
  for (pl = 1,i = 1; i < 5; i++)
	{
	if (player[i].active == true && player[i].first_roll > player[pl].first_roll)
	    {
		Game_Step.start_player = i;
		pl = i;
	    }
    }
  Game_Step.current_step++;   //��������� �� ���� ���
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
	while (i++ < 6)  //���� ���� �� ������ ��������� ������ ��� �� ������� �� ������� ��������
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
while(i++ < 6)  //���� ���� �� ������ ��������� ������ ��� �� ������� �� ������� ��������
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
//��� 1 - ������ 1 ������ � ����� ������� ������
//��� 2 - ��������� ������� � ������ ��� ��������� ��������
//��� 3 - ��������� ������� � ������ � ���������� ��������
//��� 4++  - ������ 2 �������, ����������� ������������ � ���������� ��������,
//            ����������� ������ ��������, ���� ���� �������� ....
//class Step

	//int wait_for_start;
	//int roll_1_dice;      //������ 1 ������
	//int roll_2_dice;      //������ 2 �������
	//int flag_setTown;     //����� ������� ����� � �������� ����������
	//int flag_setVillage;  //����� ������� ������� � �������� ����������
	//int flag_set_one_Village;  //����� ������� ������� ��� ������ ����������
	//int flag_set_one_Town;
	//int flag_set_Road
	//int flag_set_one_Road
	//int flag_getResurs;   //��������� �������� �� ���� ��������
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
		flag_getResurs = 1;   //��������� �������� �� ���� ��������
		flag_set_Road = 1;
	}
	return;
}