#pragma once

#include <SFML/Graphics.hpp>
#include "CatanField.h"

void DrawGecs(sf::RenderWindow* , std::vector<GECS>* );
void DrawGecsNum(sf::RenderWindow*, std::vector<GECS>* );
void DrawNodes(sf::RenderWindow*, std::vector<NODE>*);
void DrawRoads(sf::RenderWindow*, std::vector<NODE>*, std::vector<ROAD>*);
void DrawCard(sf::RenderWindow*,int, int, int, float);

void DrawRoad(sf::RenderWindow* , int, int, int, float,int );
void DrawVillage(sf::RenderWindow*, int, int, int, float);
void DrawTown(sf::RenderWindow* , int, int, int, float);

void DrawResursBank(sf::RenderWindow*);
void DrawPlayer(sf::RenderWindow*);
void Draw_Step_Info(sf::RenderWindow*);

void Draw_Start(sf::RenderWindow*);
void Draw_Cubic(sf::RenderWindow*);

int Game_x(int);
int Game_y(int);

float Draw_x(int);
float Draw_y(int);
