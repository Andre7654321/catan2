#pragma once

#include "CatanField.h"

#ifndef NET_PART_H
#define NET_PART_H

#include <winsock2.h>

//server  send  ====================================================
void Set_Player_Number(int num);     //
void Send_Field_GECS(int pl);        //
void Send_Bandit_GECS(int pl);       //
void Send_Connected_Players(int pl); //
void Set_Game_Step(int pl);          //
void Set_New_Move(int pl);           //
void Send_Info_Nodes(int pl);        //
void Send_Info_Nodes_Zip(int pl);    //

void Send_Info_Roads(int pl);        //
void Send_Bank_Resurs(int pl);       //
void Send_Player_CARDS(int pl, int nplayer);   //
void Send_Player_Objects(int pl, int nplayer); //
void Info_Dise_7(int pl, int npl);            
void Info_Play_Develop_Card(int pl, int npl, int type);   
void Info_Player_Last_Dice(int pl, int npl, int dice);   
void Send_To_All_Info_MaxWayArmy(int pl);      
void Send_Info_Change_Area(int pl, int npl);  
void Send_Develop_CARDS(int pl, int npl);  
void Send_Fish_CARDS(int pl);
void Send_Improve_CARDS(int pl);             
void Send_To_All_Info_Resurs();         
void Send_Info_Change(int pl, int s);  
void Send_Info_Game_Type(int pl);
void Send_To_All_Info_Take_Card(int pl);

//====================================================================

void Ask_Send_Arrays();     
void Send_roads();                 
void Send_nodes();                 
void Info_Main_Bank();             
void Player_Objects_To_Server();    
void Info_Player_Resurs();       
void Say_Roll_Start_Dice();      
void Say_Roll_2Dice();           
void Say_Move_Over();           
void Ask_Send_Resurs();       
void Say_Move_Banditos();        
void Info_Change_Bank();          
void Ask_Send_Objects();          
bool AskChangeWithBank(RESURS);   
void AskSendCardsToBank();         
void Say_Start();                 
void AskTakeRandomCardFromPlayer(int); 
bool AskBuyImproveCARD();         
void AskPlayDevelopCard(IMP_TYPE);  
void Ask_Reset_Game(int game_type);
void Test_Game();                 
void AskChangeWithPlayer(int);      
void AskToDeleteOffer(int);
void AskToAcceptOffer(int);
void InfoTakeRandomCardFromPlayer();
void Send_Fish_Vector_To_Server();

//--------------------------------------------------------------------
//  use  old  packets

int  Count_Num_players();
void InitChange_BANK();
void InitChange_BANK(int);


void Ask_Send_Resurs_To_Server();
void Ask_Server_To_Take_Resurs_From_All(RESURS);








#endif
