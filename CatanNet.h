#pragma once

#include "CatanField.h"

#ifndef NET_PART_H
#define NET_PART_H

#include <winsock2.h>

//server  send  ====================================================
void Set_Player_Number(int num);     //new style
void Send_Field_GECS(int pl);        //new style
void Send_Bandit_GECS(int pl);       //new style
void Send_Connected_Players(int pl); //new style
void Set_Game_Step(int pl);          //new style
void Set_New_Move(int pl);           //new style
void Send_Info_Nodes(int pl);        //new style
void Send_Info_Roads(int pl);        //new style
void Send_Bank_Resurs(int pl);       //new style
void Send_Player_CARDS(int pl, int nplayer);   //new style
void Send_Player_Objects(int pl, int nplayer); //new style
void Info_Dise_7(int pl, int npl);             //new style
void Info_Play_Develop_Card(int pl, int npl, int type);      //new style
void Info_Player_Last_Dice(int pl, int npl, int dice);       //new style
void Send_To_All_Info_MaxWayArmy(int pl);                    //new style
void Send_Info_Change_Area(int pl, int npl);          //new style
void Send_Develop_CARDS(int pl, int npl);      //new style
void Send_Improve_CARDS(int pl);               //new style
void Send_To_All_Info_Resurs();                //new style
void Send_Info_Change(int pl, int s);          //new style

//====================================================================

void Ask_Send_Arrays();             //new style
void Send_roads();                  //new style
void Send_nodes();                  //new style
void Info_Main_Bank();              //new style
void Player_Objects_To_Server();    //new style
void Info_Player_Resurs();          //new style 
void Say_Roll_Start_Dice();         //new style 
void Say_Roll_2Dice();              //new style
void Say_Move_Over();               //new style
void Ask_Send_Resurs();             //new style
void Say_Move_Banditos();           //new style

//--------------------------------------------------------------------
//  use  old  packets
void Ask_Reset_Game();
void Test_Game();

void Ask_Send_Objects();
int  Count_Num_players();
void InitChange_BANK();
void InitChange_BANK(int);
bool AskChangeWithBank(RESURS);
bool AskBuyImproveCARD();
void AskSendCardsToBank();
void Info_Change_Bank();
void AskTakeRandomCardFromPlayer(int);
void AskPlayDevelopCard(IMP_TYPE);
void Ask_Send_Resurs_To_Server();
void Ask_Server_To_Take_Resurs_From_All(RESURS);
void AskChangeWithPlayer(int);
void AskToDeleteOffer(int );
void AskToAcceptOffer(int);

void Say_Start();



#endif
