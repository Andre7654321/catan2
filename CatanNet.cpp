
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <stdlib.h>
#include <time.h>

#include "CatanNet.h"
#include "Cat_NET.h"
#include "CatanField.h"
#include "Catan_Step.h"
#include "Catan_Count.h"

#pragma warning(disable: 4996)

extern SOCKET Connection;   //����������� �������
extern int Game_type;     //1 - Standart CATAN   2 - FISH

//extern char addr_listen[20] = "192.168.2.39";
//extern char addr_UDP[20] = " ";
//extern char addr_connect[20] = "192.168.2.39";

//��� �������
extern SOCKET Connections[10];    //6 + server ����������� �������
extern char str_Player_addr[7][20];
//extern int Counter = 0;           //������� �����������

extern std::mutex mtx1;    //�������� ����� ��������� cout

extern std::string CATAN_command;      // = "zCATANz";
//map ����� �� ���������� ������� ������� �������
extern std::map<NET_COMMAND, std::string> Map_Command;

//������� ���������� ����������
extern int player_num;
extern int bandit_Gecs;
extern int max_road_owner;
extern int max_army;        //�������� �������� ����� ������� ������
extern int max_gavan;

extern int Play_two_roads;     //���� ���� ����� �������� 2 ������
extern int Play_two_resurs;    //���� ���� ����� �������� 2 �������
extern int Play_ONE_resurs;
extern int Allow_Develop_card;

extern bool flag_cubic_was_used;   //������ ���������� ��� ������� ����� ��� ������ �����
extern bool flag_develop_card_used;  //������ ���������� ��� ����� �������� ��� �������� � ���� ����

extern std::vector<GECS>* gecsPtr;    //��������� �� ������ ������
extern std::vector<NODE>* nodePtr;    //��������� �� ������ ����� ����
extern std::vector<ROAD>* roadPtr;    //��������� �� ������ �����
extern std::vector<GECS>  FishGecs;   //������ ������ �������
extern std::vector<RESURS>  FishCards;  //������ �������� � ������

extern PLAYER player[7];
extern GAME_STEP Game_Step;
extern int CARD_Bank[12];
extern CHANGE Change[12];       //������ ������ �������
extern std::vector<IMP_CARD> improve_CARDS;
extern std::vector<IMP_CARD> develop_CARDS[5];
extern int limit_7[7];

extern std::string resurs_name[12];
extern int Big_Message;              //����� ������� ��� ������ ���������
extern std::chrono::time_point<std::chrono::steady_clock>  start_Big_Message;

//������� ������ ���������
extern int ChangeBANK[7][12];


//=======================================================
// ������ �� ���������� ������ �� ������
//=======================================================
void Sent_Chart_Mess(std::string text)
{
 char msg[256];

 strcpy(msg, text.c_str());
 send(Connection, msg, sizeof(msg), NULL);
 return;
}

//=======================================================
// ������ �� ���������� ������ �� ������
//=======================================================
void AskToAcceptOffer(int s)
{
CATAN_CLIENT_Command(NET_COMMAND::ASK_ACCEPT_OFFER, (char*)&s, sizeof(int));
return;
}

//=======================================================
// ������ �� ������ ������ �� ����� � �������
//=======================================================
void AskToDeleteOffer(int s)
{
CATAN_CLIENT_Command(NET_COMMAND::ASK_DELETE_OFFER, (char*)&s, sizeof(int));
return;
}

//=======================================================
// ������ �� ����� ������� �������� � �������
//=======================================================
void AskChangeWithPlayer(int pl)
{
    //��� ������ � �������� - �� �������������, ������ ���� �� �������� �� ����������
	//Info_Player_Resurs();
	//�� ������ ���� �������� ����� ������  � ��� ����� 
	//� �������� ��������� � ������ ������ - ���������� �� ����

	CATAN_CLIENT_Command(NET_COMMAND::SET_CHANGE_OFFER, (char*)&pl, sizeof(int));

	return;
}

//=======================================================
// ������ ������� ������������� ����
//=======================================================
void Test_Game()
{
std::cout << " ASK FOR TEST MODE STEP 4 " << std::endl;
CATAN_CLIENT_Command(NET_COMMAND::TEST_GAME, nullptr, NULL);
return;
}

//=======================================================
// ������ ������� ������������� ����
//=======================================================
void Ask_Reset_Game(int game_type)
{
CATAN_CLIENT_Command(NET_COMMAND::ASK_RESET_GAME,(char *)&game_type,sizeof(int));
return;
}

//=======================================================
// ������ ������� � ������� ������
//=======================================================
void Ask_Server_To_Take_Resurs_From_All(RESURS type)
{
//��������� ������� ���� �������
Info_Player_Resurs();

//std::cout << " ������ ������� ������� ���   " << resurs_name[(int)type] << std::endl;
CATAN_CLIENT_Command(NET_COMMAND::ASK_GET_RESURS_FROM_ALL, (char*)&type, sizeof(int));
return;
}

//=======================================================
// ������ ������� ����� ��������
//=======================================================
void AskPlayDevelopCard(IMP_TYPE type)
{
	Send_nodes();	Send_roads();
	Info_Player_Resurs();   //���������� ��� �������������
	Info_Main_Bank();       //���������� ��� �������������
	//�������, ������ .. ������
	Player_Objects_To_Server();

	CATAN_CLIENT_Command(NET_COMMAND::ASK_PLAY_DEVELOP_CARD, (char*)&type, sizeof(int));
	return;
}

//=======================================================
//  ������ ������� ����� ��������
//=======================================================
bool AskBuyImproveCARD()
{ 
	Send_nodes();	Send_roads();
	Info_Player_Resurs();   //���������� ��� �������������
	Info_Main_Bank();       //���������� ��� �������������
	//�������, ������ .. ������
	Player_Objects_To_Server();

	CATAN_CLIENT_Command(NET_COMMAND::ASK_BUY_IMPROVE_CARD,nullptr,NULL);
	//� ����� ������ ������ �������� ���� �������� � ������� ������
	return true;
}

//=======================================================
// ������ �� ������ ��������� ������� ����� � ���������
//=======================================================
void InfoTakeRandomCardFromPlayer()
{
	CATAN_CLIENT_Command(NET_COMMAND::INFO_TAKE_CARD_FROM_PLAYER,nullptr,NULL);
	return;
}

//=======================================================
// ������ �������� ����� ����� � ������ PL
//=======================================================
void AskTakeRandomCardFromPlayer(int pl)
{
CATAN_CLIENT_Command(NET_COMMAND::TAKE_CARD_FROM_PLAYER, (char*)&pl, sizeof(int));
return;
}

//========================================================
// �������� ��������� ������� � ������ ����
//========================================================
void Say_Start()
{
	if (player_num != 1) return;
	CATAN_CLIENT_Command(NET_COMMAND::SAY_GAME_START, nullptr, NULL);
	return;
}

//=======================================================
//  ������ �� ����� ���� � ���� (��������� 7)
//=======================================================
void AskSendCardsToBank()
{
//��������� �� ������ ������� �������� ���� ������ � ����� ����
CATAN_CLIENT_Command(NET_COMMAND::SEND_CARDS_TO_BANK, nullptr, NULL);
return;
}

//=======================================================
//  ������ ������ �������� � ������
//=======================================================
bool AskChangeWithBank(RESURS CARD_type)
{
	int i;
	int need_card;

	//���������� �������� ������������� �������� �������, ��� �� ������

	//����������� ���� � ����������� ���������� � �����
	//��� ��� ����� �� ������ ��� ��������� � �������� ����
	Send_nodes();	Send_roads();
	Info_Player_Resurs();   //���������� ��� �������������
	Info_Main_Bank();       //���������� ��� �������������
	//�������, ������ .. ������
	Player_Objects_To_Server();

	//��������� �� ������ ��������� �� ����� ������
	CATAN_CLIENT_Command(NET_COMMAND::ASK_CHANGE_WITH_BANK, (char*)&CARD_type,sizeof(int));
	return true;
}

//========================================================================
// ���������� ��� ������ ����� ���� ��� �������������� ���������
// ������ ������� �� ��������� ����� �������, ��������, �����, 
//========================================================================
void Ask_Send_Objects()
{
CATAN_CLIENT_Command(NET_COMMAND::ASK_PLAYER_OBJECTS,nullptr,NULL);
return;
}

//=======================================================
//  ��������� ������� �� ��������� � ����� ������
//=======================================================
void Info_Change_Bank()
{
//��������� �� ������ ���� �������� ����
CATAN_CLIENT_Command(NET_COMMAND::INFO_CHANGE_BANK, (char*)&ChangeBANK[player_num][0], 10* sizeof(int));
return;
}

//========================================================
// �������� ��������� ������� � ����� ����� �����������
//========================================================
void Say_Move_Banditos()
{
CATAN_CLIENT_Command(NET_COMMAND::BANDIT_GECS, (char*)&bandit_Gecs, sizeof(int));
return;
}

//========================================================================
// ���������� ��� ������ ����� ���� ��� �������������� ���������
// ������ �� ��������� �� ������� ����� �������� � �������� �������
//========================================================================
void Ask_Send_Resurs()
{
	CATAN_CLIENT_Command(NET_COMMAND::ASK_BANK_RESURS,nullptr,NULL);
	CATAN_CLIENT_Command(NET_COMMAND::ASK_PLAYER_RESURS, nullptr, NULL);
}

//========================================================
// �������� ��������� ��� ��� ��������
//========================================================
void Say_Move_Over()
{
	if (player_num != Game_Step.current_active_player)  return;
	//������ ���� ������� �� �����
	for (int i = 0; i < 12; i++)  Change[i].status = 0;

	//����� ������������� �������� ���� �� ������� � �����
	Send_nodes();	Send_roads();
	Info_Player_Resurs();   //���������� ��� �������������
	Info_Main_Bank();       //���������� ��� �������������
	//�������, ������ .. ������
	Player_Objects_To_Server();

	//�� 3 ���� ��������� ����� ���� ������������ ��������� �������
	if (Game_Step.current_step == 3)
	    {
	    CATAN_CLIENT_Command(NET_COMMAND::LAST_VILLAGE, (char*)&player[player_num].last_village_node,sizeof(int));
	    }

	//���� ��������� � ���������� ����
	CATAN_CLIENT_Command(NET_COMMAND::SAY_MOVE_OVER,nullptr,NULL);
	return;
}

//========================================================
// �������� ��������� ��� �����  1 �����
//========================================================
void Say_Roll_Start_Dice()
{
	int rnum = 0;

	rnum = Random_Number(1, 6);
	std::cout << " �� �������  ����� =  " <<  rnum  << std::endl;

	CATAN_CLIENT_Command(NET_COMMAND::SAY_ROLL_START_DICE, (char*)&rnum, sizeof(int));

	return;
}

//========================================================
// �������� ��������� ��� ������  2 ������
//========================================================
void Say_Roll_2Dice()
{
    int rnum = 0;

	rnum = Random_Number(1, 6);
	Random_Number(2, 16);
	Random_Number(0, 22);
	rnum += Random_Number(1, 6);

	//std::cout << " �� �������  ����� =  " << rnum << std::endl;

	CATAN_CLIENT_Command(NET_COMMAND::ASK_ROLL_2DICE, (char*)&rnum, sizeof(int));
	return;
}

//=======================================================
// ��������� ��������� �������� ������ �� ������
//=======================================================
void Info_Player_Resurs()
{
CATAN_CLIENT_Command(NET_COMMAND::PLAYER_RESURS, (char*)&player[player_num].resurs, 12 * sizeof(int));
return;
}

//=======================================================
//  ����� ������ �� ������
//=======================================================
void Player_Objects_To_Server()
{
	char buff[4 * sizeof(int)];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = player_num;	IntPtr++;
	*IntPtr = player[player_num].town;	IntPtr++;
	*IntPtr = player[player_num].village;	IntPtr++;
	*IntPtr = player[player_num].road;

	CATAN_CLIENT_Command(NET_COMMAND::PLAYER_OBJECTS, buff, 4 * sizeof(int));
	return;
}

//=======================================================
//  ��������� ��������� �������� �� ������
//=======================================================
void Info_Main_Bank()
{
CATAN_CLIENT_Command(NET_COMMAND::INFO_BANK_RESURS, (char*)&CARD_Bank[0], 10 * sizeof(int));
return;
}

//========================================================
// 
//========================================================
void Send_Fish_Vector_To_Server()
{
	int size = (2 + FishCards.size()) * sizeof(int);
	char* buff = new char[size];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = FishCards.size();	IntPtr++;
	for (auto& elem : FishCards)
	    {
		*IntPtr++ = (int)elem;
	    }

	CATAN_CLIENT_Command(NET_COMMAND::INFO_FISH_CARDS, buff, size);
	delete[] buff;
	return;
}

//========================================================
// ������ �������� ���� �� ������� �� ������
//========================================================
void Send_roads()
{
	//�� 2 int �������� �� road
	int size = (*roadPtr).size() * sizeof(int) * 2;
	char* buff = new char[size];

	int* IntPtr;
	IntPtr = (int*)buff;

	for (auto rr : *roadPtr) { *IntPtr++ = rr.owner;     *IntPtr++ = rr.type; }
	CATAN_CLIENT_Command(NET_COMMAND::ROAD_ARRAY, buff, size);

	delete[] buff;
	return;
}

//========================================================
// �������� ���� �� ����� - � ���������� �� ������
//========================================================
void Send_nodes()
{
	//�� 2 int �������� �� node
	int size = (*nodePtr).size() * sizeof(int) * 2;
	char* buff = new char[size];

	int* IntPtr;
	IntPtr = (int*)buff;

	for (auto node : *nodePtr) { *IntPtr++ = node.owner;   	*IntPtr++ = node.object; }
	CATAN_CLIENT_Command(NET_COMMAND::NODE_ARRAY, buff, size);
	delete[] buff;
	return;
}

//========================================================
// ������ �� ��������� �� ������� ����� � �����
//========================================================
void Ask_Send_Arrays()
{
	CATAN_CLIENT_Command(NET_COMMAND::ASK_NODE_ARRAY, nullptr, NULL);
	CATAN_CLIENT_Command(NET_COMMAND::ASK_ROAD_ARRAY, nullptr, NULL);
	return;
}

//-----------------------------------------------------------------------
//=======================================================================
// ���������� ������ ������
//=======================================================================
void Set_Game_OVER(int pl)
{
 CATAN_SERVER_Command(NET_COMMAND::GAME_OVER, (char*)&pl, sizeof(int), pl);
}

//=======================================================================
// �������������� ��������� ���� ��� ����� ���������� ������ ����� � ���������
//=======================================================================
void Send_To_All_Info_Take_Card(int pl)
{
 CATAN_SERVER_Command(NET_COMMAND::INFO_TAKE_CARD_FROM_PLAYER, (char*)&pl, sizeof(int), pl);
}

//=======================================================================
// ��������� � ���� ����
//=======================================================================
void Send_Info_Game_Type(int pl)
{
	CATAN_SERVER_Command(NET_COMMAND::GAME_TYPE,(char*)&Game_type,sizeof(int), pl);
}

//=======================================================================
//�������� ������ �� ����� �� ������ ������ �������
//=======================================================================
void Send_Info_Change(int pl,int s)
{
char buff[sizeof(CHANGE) + sizeof(int) + 10];
int* IntPtr;
IntPtr = (int*)buff;

    if (s < 0 || s >= 12)   { Beep(900, 1000);  return; }
    *IntPtr = s;	 IntPtr++;   //����� ������                             
	memcpy(IntPtr, &Change[s], sizeof(CHANGE)); 

	CATAN_SERVER_Command(NET_COMMAND::CHANGE_OFFER, buff, sizeof(CHANGE) +  sizeof(int), pl);
	return;
}

//=======================================================================
//�������� ��������� � �������� �� ������� ���� �������
//=======================================================================
void Send_To_All_Info_Resurs()
{
	//����� ���� ��������
	Send_Bank_Resurs(TO_ALL);

	//����� ���� ���� ��������
	Send_Improve_CARDS(TO_ALL);
	Send_Fish_CARDS(TO_ALL);

	//������� ������� ������
	for (int i = 1; i < 7; i++)  Send_Player_CARDS(TO_ALL, i);
	//����� ������ �������
	for (int i = 1; i < 7; i++)  Send_Info_Change_Area(TO_ALL, i);

	return;
}

//=======================================================================
//�������� ����� ���� �������� �� ������� ��������
//=======================================================================
void Send_Improve_CARDS(int pl)
{
	int size = (2 + 2 * improve_CARDS.size()) * sizeof(int);
	char* buff = new char[size];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = improve_CARDS.size();	IntPtr++;
	for (auto& elem : improve_CARDS)
	    {
		*IntPtr++ = elem.status;
		*IntPtr++ = (int)elem.type;
	    }

	CATAN_SERVER_Command(NET_COMMAND::INFO_IMP_CARDS, buff, size, pl);
	delete[] buff;
	return;
}

//=======================================================================
//�������� ������� ���� ��� �� ������� 
//=======================================================================
void Send_Fish_CARDS(int pl)
{
	int size = (2 + FishCards.size()) * sizeof(int);
	char* buff = new char[size];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = FishCards.size();	IntPtr++;
	for (auto& elem : FishCards)
	    {
		*IntPtr++ = (int)elem;
	   }

	CATAN_SERVER_Command(NET_COMMAND::INFO_FISH_CARDS, buff, size, pl);
	delete[] buff;
	return;
}

//=======================================================================
//�������� ������� ���� �������� 1 ������ �� ������� ��������
//=======================================================================
void Send_Develop_CARDS(int pl,int npl)
{
	int size = (3 + 2 * develop_CARDS[npl].size()) * sizeof(int);
	char* buff = new char[size];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = npl;	                        IntPtr++;
	*IntPtr = develop_CARDS[npl].size();	IntPtr++;

	for (IMP_CARD& elem : develop_CARDS[npl])
	    {
		*IntPtr = elem.status;      IntPtr++;
		*IntPtr = (int)elem.type;   IntPtr++;
	    }

	CATAN_SERVER_Command(NET_COMMAND::DEVELOP_VECTOR, buff , size, pl);
	delete[] buff;
	return;
}

//=======================================================================
//�������� ��������� � ��������� ����� ������ ������
//=======================================================================
void Send_Info_Change_Area(int pl,int npl)
{
	char buff[11 * sizeof(int)];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = npl;	IntPtr++;
	memcpy(IntPtr, ChangeBANK[npl], 10 * sizeof(int));

	CATAN_SERVER_Command(NET_COMMAND::PLAYER_CHANGE_AREA, buff, 11 * sizeof(int), pl);
}

//=======================================================================
//�������� �������� ��������� � �������� ������ �������� ���� � ������
//=======================================================================
void Send_To_All_Info_MaxWayArmy(int pl)
{
	char buff[3 * sizeof(int)];
	int* IntPtr;

	IntPtr = (int*)buff;
	*IntPtr = max_road_owner;	IntPtr++;
	*IntPtr = max_army;         IntPtr++;
	*IntPtr = max_gavan;

	CATAN_SERVER_Command(NET_COMMAND::MAX_WAY_ARMY_OWNER, buff, 3 * sizeof(int), pl);
}

//=======================================================
//  ����������� �� ������� � ������ � ���������� ������ ������
//=======================================================
void Info_Player_Last_Dice(int pl, int npl, int dice)
{
	char buff[2 * sizeof(int)];
	int* IntPtr;

	IntPtr = (int*)buff;
	*IntPtr = npl;	IntPtr++;
	*IntPtr = dice;

	CATAN_SERVER_Command(NET_COMMAND::LAST_DICE, buff, 2 * sizeof(int), pl);
}

//=======================================================
//  ����������� �� ������� � ���� ����� ��������
//=======================================================
void Info_Play_Develop_Card(int pl,int npl, int type)
{
	char buff[2 * sizeof(int)];
	int* IntPtr;
	
	IntPtr = (int*)buff;
	*IntPtr = npl;	IntPtr++;
	*IntPtr = type;	
	CATAN_SERVER_Command(NET_COMMAND::PLAY_DEVELOP_CARD, buff, 2*sizeof(int), pl);
}

//=======================================================
//  ����������� �� ������� � ������ 7
//=======================================================
void Info_Dise_7(int pl,int npl)
{
  CATAN_SERVER_Command(NET_COMMAND::DICE_SEVEN,(char* )&npl, sizeof(int), pl);
}

//=======================================================================
// �� SERVER ���� ����� ������ ������
//=======================================================================
void Send_Player_Objects(int pl, int nplayer)
{
	char buff[4 * sizeof(int)];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = nplayer;	IntPtr++;
	*IntPtr = player[nplayer].town;	IntPtr++;
	*IntPtr = player[nplayer].village;	IntPtr++;
	*IntPtr = player[nplayer].road;	

	CATAN_SERVER_Command(NET_COMMAND::PLAYER_OBJECTS, buff, 4 * sizeof(int), pl);
	return;
}

//=======================================================================
// �� SERVER ���� ������� ������ ������
//=======================================================================
void Send_Player_CARDS(int pl,int nplayer)
{
	char buff[13* sizeof(int)];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = nplayer;	IntPtr++;
	memcpy(IntPtr, player[nplayer].resurs, 12 * sizeof(int)); 

	CATAN_SERVER_Command(NET_COMMAND::PLAYER_RESURS, buff, 13 * sizeof(int), pl);
	return;
}

//=======================================================
//  ���� �������� ��������
//=======================================================
void Send_Bank_Resurs(int pl)
{
 CATAN_SERVER_Command(NET_COMMAND::INFO_BANK_RESURS, (char*)&CARD_Bank[0], 10*sizeof(int), pl);
}

//=======================================================
//  ���� � �������� ���� ������
//=======================================================
void Set_New_Move(int pl)
{
	char buff[3 * sizeof(int)];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = Game_Step.current_active_player;	IntPtr++;
	*IntPtr = flag_cubic_was_used;	IntPtr++;
	*IntPtr = flag_develop_card_used;

  CATAN_SERVER_Command(NET_COMMAND::SET_N_ACTIVE_PLAYER, (char*)&buff, 3 * sizeof(int), pl);
  return;
}

//=======================================================
//  �������������� ����������
//=======================================================
void Set_Game_Step(int pl)
{
	CATAN_SERVER_Command(NET_COMMAND::SET_STEP, (char*)&Game_Step.current_step, sizeof(int), pl);
}

//=======================================================
//  ���� � ������������ �������
//=======================================================
void Send_Connected_Players(int pl)
{
	char buff[50];
	int* IntPtr;
	IntPtr = (int*)buff;

	for (int i = 1; i < 7; i++)
	   {
	   *IntPtr = 0;
	   if (player[i].active == true)  *IntPtr = 1;
	   IntPtr++;
	   }

CATAN_SERVER_Command(NET_COMMAND::INFO_N_ACTIVE_PLAYER, buff,sizeof(int) * 6, pl);
}

//=======================================================
//  �������������� ���������� ���� �������
//=======================================================
void Send_Bandit_GECS(int pl)
{
 CATAN_SERVER_Command(NET_COMMAND::BANDIT_GECS, (char*)&bandit_Gecs, sizeof(int),pl);
}

//=======================================================
// ������ �������� ��������� ���� ������
//=======================================================
void Send_Field_GECS(int pl)
{
 //�� 4 int �������� �� ����
 int size = (*gecsPtr).size() * sizeof(int) * 4 + FishGecs.size() * sizeof(int) * 4;
 char* buff = new char[size];

 int* IntPtr;
 IntPtr = (int*)buff;
 
 for(auto& elem : *gecsPtr)
     {
	  //std::cout << "SERVER type gecs = " << resurs_name[(int)elem.type] << std::endl;
	  *IntPtr++ = elem.x;              *IntPtr++ = elem.y;
	  *IntPtr++ = (int)elem.type;      *IntPtr++ = elem.gecs_game_number;
     }

 for (auto& elem : FishGecs)
     {
	 *IntPtr++ = elem.x;              *IntPtr++ = elem.y;
	 *IntPtr++ = (int)elem.type;      *IntPtr++ = elem.gecs_game_number;
     }
	
  CATAN_SERVER_Command(NET_COMMAND::SET_GECS,buff,size,pl);

 delete[] buff;
 return;
}

//=======================================================================
// �������� ��������� ������� � �� ���������� �� ������� �������
//=======================================================================
void Send_Info_Nodes(int pl)
{
	//�� 2 int �������� �� node
	int size = (*nodePtr).size() * sizeof(int) * 2;
	char* buff = new char[size];

	int* IntPtr;
	IntPtr = (int*)buff;

	for (auto node : *nodePtr)     {	*IntPtr++ = node.owner;   	*IntPtr++ = node.object;    }
	CATAN_SERVER_Command(NET_COMMAND::NODE_ARRAY, buff, size, pl);

	delete[] buff;
	return;
}

//=======================================================================
// �������� ��������� ������� � �� ���������� �� ������� �������
// � ����������� ���� - ������ ���������� ����- 2-3 ������� �������� �������
//=======================================================================
void Send_Info_Nodes_Zip(int pl)
{
	//�� 2 int �������� �� node
	int max_size = nodePtr->size() * sizeof(int) * 3;     //max_size
	int size = 0;
	char* buff = new char[max_size];

	int* IntPtr;
	IntPtr = (int*)buff;     IntPtr++;   //?????????????????????????   ++

	for (auto& node : *nodePtr) 
	     {
		if (node.owner > 0)
		    {
			*IntPtr = node.number;  IntPtr++;
			*IntPtr = node.owner;   IntPtr++;
			*IntPtr = node.object;  IntPtr++;
			//std::cout << "\t SERVER ZIP--- " << node.number << "\tnode owner = " << node.owner << "\tobj =  " << node.object << std::endl;
			size += 1;
		    }
	     }

	IntPtr = (int*)buff;
	*IntPtr = size;   //� ������ ����� ������� ���������� �����
	size = (size * 3 + 1) * sizeof(int);
	CATAN_SERVER_Command(NET_COMMAND::NODE_ARRAY_ZIP, buff, size, pl);

	delete[] buff;
	return;
}

//========================================================
// ������ �������� ���� �� ������� ��������
//========================================================
void Send_Info_Roads(int pl)
{
	//�� 2 int �������� �� road
	int size = (*roadPtr).size() * sizeof(int) * 2;
	char* buff = new char[size];

	int* IntPtr;
	IntPtr = (int*)buff;

	for (auto rr : *roadPtr)      { *IntPtr++ = rr.owner;     *IntPtr++ = rr.type;  }
	CATAN_SERVER_Command(NET_COMMAND::ROAD_ARRAY, buff, size, pl);

	delete[] buff;
	return;
}

//=======================================================
// ������ ������ ����� ������
//=======================================================
void Set_Player_Number(int num)
{
 //std::cout << " ======== SEND NEW NUM FOR PLAYER == " <<  num << std::endl;
 CATAN_SERVER_Command(NET_COMMAND::SET_N_PLAYER,(char*)&num,sizeof(int),num);
}


//*******************************************************************************************
//=============== old net ******************************************************************* OLD

//=======================================================
// ������ ��������� ��������� �������� �� ������
//=======================================================
void Ask_Send_Resurs_To_Server()
{
    Info_Change_Bank();
	Info_Main_Bank();       //����� ���� �������� ��������
	Info_Player_Resurs();   //������� CARD ������

   return;
}


//=======================================================
//  ���� ������ � ������ ���� ������ ������������������
//=======================================================
void InitChange_BANK()
{
int i;

    for (i = 0; i < 7; i++)  InitChange_BANK(i);

	return;
}

//=======================================================
//  ���� ������ ������ ������
//=======================================================
void InitChange_BANK(int i)
{
 for (int j = 0; j < 10; j++)  ChangeBANK[i][j] = 0;

	return;
}
							   
//========================================================
// ������� ����� �������������� �������
//========================================================
int Count_Num_players()
{
	int num = 0;
	for (int i = 1; i < 5; i++)
	    {
		if (player[i].active == true) num++;
	    }
return num;
}









