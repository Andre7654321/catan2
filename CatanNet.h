#pragma once

#include "CatanField.h"

#ifndef NET_PART_H
#define NET_PART_H

#include <winsock2.h>

int Start_Server_CATAN(void);
int Init_Client_CATAN(void);
int  init_WSA(void);
void Check_Connections(void);
int  ClientChart();
void ClientHandler(int);
void ServerClientStreamFunc(int);

void Ask_Send_Arrays();
int Count_Num_players();

void Say_Start();
void Say_Move_Over();
void Say_Roll_1Dice();
void Say_Roll_2Dice();

void Send_To_All(char* , int);
void Send_To_All_Info_Resurs();


#endif
