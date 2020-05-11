#pragma once

#include <SFML/Graphics.hpp>
#include "CatanField.h"

void DrawCATAN19(sf::RenderWindow*);
void DrawGecs(sf::RenderWindow* , std::vector<GECS>* );
void DrawGecsNum(sf::RenderWindow*, std::vector<GECS>* );
void DrawNodes(sf::RenderWindow*, std::vector<NODE>*);
void DrawNodesInfo(sf::RenderWindow*, std::vector<NODE>*);
void DrawRoads(sf::RenderWindow*, std::vector<NODE>*, std::vector<ROAD>*);
void DrawRoadsNet(sf::RenderWindow*, std::vector<NODE>*, std::vector<ROAD>*);
void DrawCard(sf::RenderWindow*,int, int, int, float);
void DrawDevelopCard(sf::RenderWindow*, int, int, int, float,int);
void Draw_Develop_Cards_Field(sf::RenderWindow*);
int getNumDevelopCARD(IMP_TYPE, int);

void DrawRoad(sf::RenderWindow* , int, int, int, float,int );
void DrawVillage(sf::RenderWindow*, int, int, int, float);
void DrawTown(sf::RenderWindow* , int, int, int, float);

void DrawResursBank(sf::RenderWindow*);
void DrawChangeBank(sf::RenderWindow* win,int,int,int);
void DrawPlayer(sf::RenderWindow*);
void Draw_Step_Info(sf::RenderWindow*);

void Draw_Start(sf::RenderWindow*);
void Draw_Cubic(sf::RenderWindow*);
void DrawBandit(sf::RenderWindow* , std::vector<GECS>*);
void DrawBanditOnCoord(sf::RenderWindow* , int, int, float);
void Draw_MaxWay(sf::RenderWindow*, int, int);

int Game_x(int);
int Game_y(int);

float Draw_x(int);
float Draw_y(int);
