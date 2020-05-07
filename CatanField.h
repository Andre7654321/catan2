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
//перечисление видов ресурсов
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

//структура банка ресурсов - где хранятся карточки ресурсов
//будет использована в общем банке игры и банки игроков
struct bank
{
public:
	int resurs[10];
};

//============================= класс PLAYER =====================================
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

//============================= класс GECS =====================================
class GECS
{
public:
	int x = -1;
	int y = -1;
	RESURS  type = RESURS::EMPTY;
	int gecs_game_number = -1;       //номер для сравнения с броском кубика  - присваивается в начале игры

public:
	//конструктор по умолчанию 
	GECS();
	GECS(int,int);
	int getGecs_x(void);
	int getGecs_y(void);
	bool isGecs_infocus(int, int);

};   //============================= GECS   end =============================================
//============================================================================
//содержит описание одного узла карты
class NODE
{
public:
	int n_x = 0;
	int n_y = 0;
	int owner = -1;      //№ игрока владельца узла    ==  -1  - нет владельца 
	int object = -1;     //тип постройки в узле       ==  -1  - нет посторойки 1 - village 2 town
	int number = -1;     //уникальный номер узла
	int type = -1;       //-1 = обычный узел
	                     // 0 = обмен ресурса 3 к 1
	                     // 1...9 - обмен ресурса 2 к 1 по номеру ресурса

	NODE();
	NODE(int, int);

	bool isNode_free(int);
	bool isNode_infocus(int, int);
	int getNode_number();
	int getNode_x(void);
	int getNode_y(void);

};  //=============================== end NODE ==============================

//============================= класс дороги =====================================
class ROAD
{
public:
	int Node_num_start = -1;
	int Node_num_end = -1;
	int owner = -1;      //== 0  - нет владельца
	int type  =  0;      // в будущих играх на пути может быть или дорога или мост

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
