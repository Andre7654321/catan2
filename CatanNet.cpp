
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

//extern char addr_listen[20] = "192.168.2.39";
//extern char addr_UDP[20] = " ";
//extern char addr_connect[20] = "192.168.2.39";

//��� �������
extern SOCKET Connections[7];    //6 + server ����������� �������
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

extern int Play_two_roads;     //���� ���� ����� �������� 2 ������
extern int Play_two_resurs;    //���� ���� ����� �������� 2 �������
extern int Play_ONE_resurs;
extern int Allow_Develop_card;

extern std::vector<GECS>* gecsPtr;    //��������� �� ������ ������
extern std::vector<NODE>* nodePtr;    //��������� �� ������ ����� ����
extern std::vector<ROAD>* roadPtr;    //��������� �� ������ �����

extern PLAYER player[7];
extern GAME_STEP Game_Step;
extern int CARD_Bank[10];
extern CHANGE Change[12];       //������ ������ �������
extern std::vector<IMP_CARD> improve_CARDS;
extern std::vector<IMP_CARD> develop_CARDS[7];
extern int limit_7[7];

extern std::string resurs_name[10];
extern int Big_Message;              //����� ������� ��� ������ ���������
extern std::chrono::time_point<std::chrono::steady_clock>  start_Big_Message;

//������� ������ ���������
extern int ChangeBANK[7][10];

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
	char msg[256];

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

	std::cout << " �� �������  ����� =  " << rnum << std::endl;

	CATAN_CLIENT_Command(NET_COMMAND::ASK_ROLL_2DICE, (char*)&rnum, sizeof(int));
	return;
}

//=======================================================
// ��������� ��������� �������� ������ �� ������
//=======================================================
void Info_Player_Resurs()
{
CATAN_CLIENT_Command(NET_COMMAND::PLAYER_RESURS, (char*)&player[player_num].resurs, 10 * sizeof(int));
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

//=======================================================================
//�������� ������ �� ����� �� ������ ������ �������
//=======================================================================
void Send_Info_Change(int pl,int s)
{
char buff[sizeof(CHANGE) + sizeof(int) + 10];
int* IntPtr;
IntPtr = (int*)buff;

    if (s < 0 || s >= 12) { Beep(900, 1000);  return; }
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
	char buff[2 * sizeof(int)];
	int* IntPtr;

	IntPtr = (int*)buff;
	*IntPtr = max_road_owner;	IntPtr++;
	*IntPtr = max_army;

	CATAN_SERVER_Command(NET_COMMAND::MAX_WAY_ARMY_OWNER, buff, 2 * sizeof(int), pl);
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
	char buff[11* sizeof(int)];
	int* IntPtr;
	IntPtr = (int*)buff;

	*IntPtr = nplayer;	IntPtr++;
	memcpy(IntPtr, player[nplayer].resurs, 10 * sizeof(int)); 

	CATAN_SERVER_Command(NET_COMMAND::PLAYER_RESURS, buff, 11 * sizeof(int), pl);
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
  CATAN_SERVER_Command(NET_COMMAND::SET_N_ACTIVE_PLAYER, (char*)&Game_Step.current_active_player, sizeof(int), pl);
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
 int size = (*gecsPtr).size() * sizeof(int) * 4;
 char* buff = new char[size];

 int* IntPtr;
 IntPtr = (int*)buff;
 
 for(auto elem : *gecsPtr)
     {
	  //std::cout << "SERVER type gecs = " << resurs_name[(int)elem.type] << std::endl;
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
// ������ ������� ������������� ����
//=======================================================
void Ask_Reset_Game()
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_RESET_GAME].c_str());
	send(Connection, msg, sizeof(msg), NULL);
	return;
}

//=======================================================
// ������ �� ���������� ������ �� ������
//=======================================================
void AskToAcceptOffer(int s)
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ACCEPT_OFFER].c_str());
	itoa(player_num, &msg[50], 10);    //���� �����
	itoa(s, &msg[54], 10);            //����� ������
	send(Connection, msg, sizeof(msg), NULL);
	return;
}

//=======================================================
// ������ �� ������ ������ �� ����� � �������
//=======================================================
void AskToDeleteOffer(int s)
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_DELETE_OFFER].c_str());
	itoa(player_num, &msg[50], 10);    //���� �����
	itoa(s, &msg[54], 10);            //����� ������
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//=======================================================
// ������ �� ����� ������� �������� � �������
//=======================================================
void AskChangeWithPlayer(int pl)
{
	char msg[256];
   //std::cout << " ====== Func AskChangeWithPlayer - " << pl << std::endl;

	Info_Player_Resurs();

   //�� ������ ���� �������� ��������� ������ ������ - 
	//��� ���������� � ����
	//� �������� ��������� � ������ ������ - ���������� �� ����

	strcpy_s(msg, Map_Command[NET_COMMAND::SET_CHANGE_OFFER].c_str());
	itoa(player_num, &msg[50], 10);    //���� �����
	itoa(pl, &msg[54], 10);            //���� �����������
	send(Connection, msg, sizeof(msg), NULL);

return;
}

//=======================================================
// ������ ������� � ������� ������
//=======================================================
void Ask_Server_To_Take_Resurs_From_All(RESURS type)
{
	char msg[256];
 
	//��������� ������� ���� �������
	Info_Player_Resurs();

	//std::cout << " ������ ������� ������� ���   " << resurs_name[(int)type] << std::endl;

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_GET_RESURS_FROM_ALL].c_str());
	itoa(player_num, &msg[50], 10);
	itoa((int)type, &msg[54], 10);
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

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
// ������ ������� ����� ��������
//=======================================================
void AskPlayDevelopCard(IMP_TYPE type)
{
	char msg[256];
	//std::cout << "  Ask to play card  " << (int)type  << std::endl;

	//��������� �� ������ 
	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_PLAY_DEVELOP_CARD].c_str());
	itoa(player_num, &msg[50], 10);
	itoa((int)type, &msg[54], 10);
	send(Connection, msg, sizeof(msg), NULL);
	return;
}

//=======================================================
// ������ ������� ����� � ������ PL
//=======================================================
void AskTakeRandomCardFromPlayer(int pl)
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::TAKE_CARD_FROM_PLAYER].c_str());
	itoa(player_num, &msg[50], 10);
	itoa(pl, &msg[54], 10);
	send(Connection, msg, sizeof(msg), NULL);
}

//=======================================================
//  ��������� ������� �� ��������� � ����� ������
//=======================================================
void Info_Change_Bank()
{
	char msg[256];

	//��������� �� ������ ���� �������� ����
	strcpy_s(msg, Map_Command[NET_COMMAND::INFO_CHANGE_BANK].c_str());
	itoa(player_num, &msg[50], 10);
	memcpy(&msg[60], &ChangeBANK[player_num][0], sizeof(int) * 10);
	send(Connection, msg, sizeof(msg), NULL);
	return;
}

//=======================================================
//  ������ �� ����� ���� � ���� (��������� 7)
//=======================================================
void AskSendCardsToBank()
{
	char msg[256];

	//��������� ���������� �� ���� ����������� 

	
	//��������� �� ������ ������� �������� ���� ������ � ����� ����
	strcpy_s(msg, Map_Command[NET_COMMAND::SEND_CARDS_TO_BANK].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//=======================================================
//  ������ ������ �������� � ������
//=======================================================
bool AskChangeWithBank(RESURS CARD_type)
{
char msg[256];
int i;
int need_card;


//��������� ���� �� � ����� ������ ������ ���������� ���������� ��������
for(i = 1;i < 10;i++)   
       {
	   need_card = 4;
	   if(bonus31(player_num))    need_card = 3;
	   if(bonus21(player_num, i))   need_card = 2;
	   if(ChangeBANK[player_num][i] >= need_card)  i = 20;
       }
if (i < 20){	std::cout << " � ����� ��������� �������� ��� ������ " << std::endl;  return false;  }

//����������� ���� � ����������� ���������� � �����
//��� ��� ����� �� ������ ��� ��������� � �������� ����
Send_nodes();
Send_roads();

//����� ���� �������� - ��������, ��� ��� ��� ���������� �� ������ ��� ���������
Info_Main_Bank();

//�������  ������ - �������� ��� ��� ����� ���������� �� ������ ��� ���������
Info_Player_Resurs();

//��������� �� ������ ���� �������� ���� � ��������� �� ����� ������
strcpy_s(msg, Map_Command[NET_COMMAND::ASK_CHANGE_BANK].c_str());
itoa(player_num, &msg[50], 10);
itoa((int)CARD_type, &msg[54], 10);
memcpy(&msg[60], &ChangeBANK[player_num][0], sizeof(int) * 10);
send(Connection, msg, sizeof(msg), NULL);

return true;
}

//=======================================================
//  ������ ������� ����� ��������
//=======================================================
bool AskBuyImproveCARD()
{
char msg[256];

//���������� ������� �������� ��� �������
//if no   return false;  

//����������� ���� � ����������� ���������� � �����
//��� ��� ����� �� ������ ��� ��������� � �������� ����
Send_nodes();
Send_roads();

//����� ���� �������� - ��������, ��� ��� ��� ���������� �� ������ ��� ���������
Info_Main_Bank();

//�������  ������ - �������� ��� ��� ����� ���������� �� ������ ��� ���������
Info_Player_Resurs();

//��������� �� ������ ���� �������� ���� � ��������� �� ����� ������
strcpy_s(msg, Map_Command[NET_COMMAND::ASK_BUY_IMPROVE_CARD].c_str());
itoa(player_num, &msg[50], 10);
memcpy(&msg[60], &ChangeBANK[player_num][0], sizeof(int) * 10);
send(Connection, msg, sizeof(msg), NULL);

//� ����� ������ ������ �������� ���� �������� � ������� ������

return true;
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

//========================================================================
// ���������� ��� ������ ����� ���� ��� �������������� ���������
// ������ ������� �� ��������� ����� �������, ��������, �����, 
// ??  ���� �������� ??
//========================================================================
void Ask_Send_Objects()
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_PLAYER_OBJECTS].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//========================================================
// �������� ��������� ������� � ������ ����
//========================================================
void Say_Start()
{
char msg[256];

	if (player_num != 1) return;

	// ��������� ������� � ������ ����
	strcpy_s(msg, Map_Command[NET_COMMAND::SAY_GAME_START].c_str());
	send(Connection, msg, sizeof(msg), NULL);	Sleep(10);

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

//=======================================================
// ������ ������� ������������� ����
//=======================================================
void Test_Game()
{
	char msg[256];

	std::cout << " ASK FOR TEST MODE STEP 4 " << std::endl;

	strcpy_s(msg, Map_Command[NET_COMMAND::TEST_GAME].c_str());
	send(Connection, msg, sizeof(msg), NULL);
	return;
}








