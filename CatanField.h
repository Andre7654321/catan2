#pragma once

#ifndef CATANFIELD_H
#define CATANFIELD_H

#include <vector>

bool isVillageNear(int);
bool isRoadNear(int, int);
void Get_Resurs(int, int);
void Step_Resurs(int);
int CountScore(int );

//---------------------------------------------------------------------------------
//������������ ����� ��������
enum class RESURS
{
	EMPTY,
	WOOD,
	BRICKS,
	BREAD,
	STONE,
	OVCA,
	PIRATE,
	FISH1,
	FISH2,
	FISH3
};

#define VILLAGE 1
#define TOWN    2

//��������� ����� �������� - ��� �������� �������� ��������
//����� ������������ � ����� ����� ���� � ����� �������
struct bank
{
public:
	int resurs[10];
};

//============================= ����� PLAYER =====================================
class PLAYER
{
public:
	int village = 5;
	int town = 4;
	int road = 15;
	int resurs[10];
	bool active = false;
	int first_roll = 0;
	int last_village_node = -1;
	PLAYER();
};   //============================= PLAYER   end =============================================

//----------------------------------------------------------------------------

//============================= ����� GECS =====================================
class GECS
{
public:
	int x = -1;
	int y = -1;
	RESURS  type = RESURS::EMPTY;
	int gecs_game_number = -1;       //����� ��� ��������� � ������� ������  - ������������� � ������ ����

public:
	//����������� �� ��������� 
	GECS();
	GECS(int,int);
	int getGecs_x(void);
	int getGecs_y(void);

};   //============================= GECS   end =============================================

//============================================================================
//�������� �������� ������ ���� �����
class NODE
{
public:
	int n_x = 0;
	int n_y = 0;
	int owner = -1;      //� ������ ��������� ����    ==  -1  - ��� ��������� 
	int object = -1;     //��� ��������� � ����       ==  -1  - ��� ���������� 1 - village 2 town
	int number = -1;     //���������� ����� ����

	NODE();
	NODE(int, int);

	bool isNode_free(int);
	bool isNode_infocus(int, int);
	int getNode_number();
	int getNode_x(void);
	int getNode_y(void);

};  //=============================== end NODE ==============================

//============================= ����� ������ =====================================
class ROAD
{
public:
	int Node_num_start = -1;
	int Node_num_end = -1;
	int owner = -1;      //== 0  - ��� ���������
	int type  =  0;      // � ������� ����� �� ���� ����� ���� ��� ������ ��� ����

	ROAD();
	ROAD(int, int);
	bool isRoad_infocus(int, int, std::vector<NODE>*);
	bool isYourNodeNear(int, std::vector<NODE>*);
	bool isYourLastVillageNear(int);
	bool isYourRoadNear(int,int);

	int getRoad_start();
	int getRoad_end();
	int getRoad_owner();
	~ROAD();

};   //============================= GAME_ROADS   end =============================================

int Init_CATAN_Field(std::vector<GECS>*, std::vector<NODE>*, std::vector<ROAD>*);
#endif