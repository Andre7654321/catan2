#pragma once
#ifndef GAME_STEP_H
#define GAME_STEP_H


int GetNextPlayer();
int GetPrevPlayer();
int SetFirstPlayer();

//========================================================================================
//��� 1 - ������ 1 ������ � ����� ������� ������
//��� 2 - ��������� ������� � ������ ��� ��������� ��������
//��� 3 - ��������� ������� � ������ � ���������� ��������
//��� 4++  - ������ 2 �������, ����������� ������������ � ���������� ��������,
//            ����������� ������ ��������, ���� ���� �������� ....
class Step
{
public:
	int wait_for_start = 1;
	int roll_1_dice = 0;      //������ 1 ������
	int roll_2_dice = 0;      //������ 2 �������
	int flag_setTown = 0;     //����� ������� ����� � �������� ����������
	int flag_setVillage = 0;  //����� ������� ������� � �������� ����������
	int flag_set_one_Village = 0;  //����� ������� ������� ��� ������ ����������
	int flag_set_one_Town = 0;
	int flag_set_Road = 0;
	int flag_set_one_Road = 0;
	int flag_getResurs = 0;   //��������� �������� �� ���� ��������
	int flag_getResurs_one_village = 0;
	int flag_bandit = 0;

	Step();
	void Step_Set(int);
	int GetCurrentStep(void);
};

//========================================================================================
//�������� ����� ����
//������������ ��������� �������� ���� ����, �������� ����
//���������� �� �������� ��� ����
//����� �������� �� ������� ���������� ����
// � ....
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
