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
void Ask_Send_Resurs();
void Ask_Send_Objects();
int Count_Num_players();
void InitChange_BANK();
bool AskChangeWithBank(RESURS);
bool AskBuyImproveCARD();
void AskSendCardsToBank();
void Info_Change_Bank();
void AskTakeRandomCardFromPlayer(int);
void AskPlayDevelopCard(IMP_TYPE);

void Say_Start();
void Say_Move_Over();
void Say_Roll_1Dice();
void Say_Roll_2Dice();
void Say_Move_Banditos();

void Send_To_All(char* , int);
void Send_To_All_Info_Resurs();
void Send_To_All_Info_Nodes();
void Send_To_All_Info_Change_Area(int);
void Send_To_All_Info_Max_Way();
void Send_To_All_Develop_CARDS(int);
void Send_To_All_Improve_CARDS();

void Send_roads();
void Send_nodes();


#endif
