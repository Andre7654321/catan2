
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>
//#include <system>

//#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "CatanNet.h"
#include "CatanField.h"
#include "Catan_Step.h"
#include "Catan_Count.h"

#pragma warning(disable: 4996)

//������� ���������� ����������
extern int player_num;
extern int bandit_Gecs;
extern int max_road_owner;

extern int Play_two_roads;     //���� ���� ����� �������� 2 ������
extern int Play_two_resurs;    //���� ���� ����� �������� 2 �������
extern int Play_ONE_resurs;

extern std::vector<GECS>* gecsPtr;    //��������� �� ������ ������
extern std::vector<NODE>* nodePtr;    //��������� �� ������ ����� ����
extern std::vector<ROAD>* roadPtr;    //��������� �� ������ �����


//��������� ���������� ����� �������� � ��� � ��������� �������
SOCKET Connection;   //����������� �������
#define MY_PORT 2489
char addr_listen[20] = "192.168.2.39";
//char addr_listen[20] = "127.0.0.1";
char addr_connect[20] = "192.168.2.39";  //"127.0.0.1"
//char addr_connect[20] = "127.0.0.1";


//��� �������
SOCKET Connections[10] = { 0,0,0,0,0,0,0,0,0,0 };    //����������� ������� 
int Counter = 0;           //������� �����������

std::mutex mtx1;    //�������� ����� ��������� cout

extern PLAYER player[5];
extern GAME_STEP Game_Step;
extern int CARD_Bank[10];
extern CHANGE Change[12];       //������ ������ �������
extern std::vector<IMP_CARD> improve_CARDS;
extern std::vector<IMP_CARD> develop_CARDS[5];
extern int limit_7[5];

extern std::string resurs_name[10];

//������� ������ ���������
extern int ChangeBANK[5][10];

//����������� ��������� ��� ������ ����������� ������� � ��������
std::string CATAN_command = "zCATANz";
enum class NET_COMMAND
{
EMPTY,
SET_N_PLAYER,
SET_STEP,
ADD_N_ACTIVE_PLAYER,   //��������� ������
SET_N_ACTIVE_PLAYER,   //������������� ����� ������ �������� ���������� ���
SET_FIRST_PLAYER,      //������������� ����� ������ ������� ����� ������ ������
ASK_SET_FIELD,
SET_GECS,
SAY_MOVE_OVER,
ASK_ROLL_1DICE,
ASK_ROLL_2DICE,
SAY_GAME_START,
NODE_ARRAY,
ROAD_ARRAY,
ASK_ROAD_ARRAY,
ASK_NODE_ARRAY,
ASK_BANK_RESURS,
ASK_PLAYER_RESURS,
ASK_PLAYER_OBJECTS,
INFO_BANK_RESURS,
LAST_VILLAGE,
PLAYER_RESURS,
PLAYER_OBJECTS,
DICE_SEVEN,
BANDIT_GECS,
ASK_CHANGE_BANK,
PLAYER_CHANGE_AREA,
MAX_WAY_OWNER,
SEND_CARDS_TO_BANK,
INFO_CHANGE_BANK,
TAKE_CARD_FROM_PLAYER,
ASK_BUY_IMPROVE_CARD,
INFO_IMP_CARDS,        //���� ���� ��������
DEVELOP_VECTOR,
LAST_DICE,
ASK_PLAY_DEVELOP_CARD,
PLAY_DEVELOP_CARD,
ASK_GET_RESURS_FROM_ALL,
SET_CHANGE_OFFER,
CHANGE_OFFER,
ASK_DELETE_OFFER,
ASK_ACCEPT_OFFER
};

std::map<NET_COMMAND, std::string> Map_Command =
{
	{NET_COMMAND::SET_N_PLAYER,		    "zCATANz SET N Player"},
	{NET_COMMAND::SET_STEP    ,		    "zCATANz SET Step"},
	{NET_COMMAND::ADD_N_ACTIVE_PLAYER,	"zCATANz ADD N ACTIVE PLAYER"},
	{NET_COMMAND::SET_N_ACTIVE_PLAYER,	"zCATANz SET N ACTIVE PLAYER"},
	{NET_COMMAND::SET_FIRST_PLAYER,	    "zCATANz SET SET_FIRST_PLAYER"},
	{NET_COMMAND::ASK_SET_FIELD,	    "zCATANz ASK SET FIELD"},
	{NET_COMMAND::SET_GECS,	     	    "zCATANz SET GECS"},
	{NET_COMMAND::SAY_MOVE_OVER,	    "zCATANz SAY_MOVE_OVER"},
	{NET_COMMAND::ASK_ROLL_1DICE,	    "zCATANz ASK ROLL_1DICE"},
	{NET_COMMAND::ASK_ROLL_2DICE,	    "zCATANz ASK ROLL_2DICE"},
	{NET_COMMAND::SAY_GAME_START,	    "zCATANz SAY_GAME_START"},
	{NET_COMMAND::NODE_ARRAY,	        "zCATANz NODE_ARRAY"},
	{NET_COMMAND::ROAD_ARRAY,	        "zCATANz ROAD_ARRAY"},
	{NET_COMMAND::ASK_NODE_ARRAY,	    "zCATANz ASK_NODE_ARRAY"},
	{NET_COMMAND::ASK_ROAD_ARRAY,	    "zCATANz ASK_ROAD_ARRAY"},
	{NET_COMMAND::ASK_BANK_RESURS,	    "zCATANz ASK_BANK_RESURS"},
	{NET_COMMAND::ASK_PLAYER_RESURS,	"zCATANz ASK_PLAYER_RESURS"},
	{NET_COMMAND::ASK_PLAYER_OBJECTS,	"zCATANz ASK_PLAYER_OBJECTS"},
	{NET_COMMAND::PLAYER_OBJECTS,	    "zCATANz PLAYER_OBJECTS"},
	{NET_COMMAND::INFO_BANK_RESURS,	    "zCATANz INFO_BANK_RESURS"},
	{NET_COMMAND::LAST_VILLAGE,	        "zCATANz LAST_VILLAGE"},
	{NET_COMMAND::PLAYER_RESURS,	    "zCATANz PLAYER_RESURS"},
	{NET_COMMAND::DICE_SEVEN,	        "zCATANz DICE_SEVEN"},
	{NET_COMMAND::BANDIT_GECS,	        "zCATANz BANDIT_GECS"},
	{NET_COMMAND::ASK_CHANGE_BANK,	    "zCATANz ASK_CHANGE_BANK"},
	{NET_COMMAND::PLAYER_CHANGE_AREA,	"zCATANz PLAYER_CHANGE_AREA"},
	{NET_COMMAND::MAX_WAY_OWNER,	    "zCATANz MAX_WAY_OWNER"},
	{NET_COMMAND::SEND_CARDS_TO_BANK,	"zCATANz SEND_CARDS_TO_BANK"},
	{NET_COMMAND::INFO_CHANGE_BANK,	    "zCATANz INFO_CHANGE_BANK"},
	{NET_COMMAND::TAKE_CARD_FROM_PLAYER,"zCATANz TAKE_CARD_FROM_PLAYER"},
	{NET_COMMAND::ASK_BUY_IMPROVE_CARD, "zCATANz ASK_BUY_IMPROVE_CARD"},
	{NET_COMMAND::INFO_IMP_CARDS,       "zCATANz INFO_IMP_CARDS"},       //���� ���� ��������
	{NET_COMMAND::DEVELOP_VECTOR,       "zCATANz DEVELOP_VECTOR"},
	{NET_COMMAND::LAST_DICE,            "zCATANz LAST_DICE"},
	{NET_COMMAND::ASK_PLAY_DEVELOP_CARD,"zCATANz ASK_PLAY_DEVELOP_CARD"},
	{NET_COMMAND::PLAY_DEVELOP_CARD,    "zCATANz PLAY_DEVELOP_CARD"},
	{NET_COMMAND::ASK_GET_RESURS_FROM_ALL,    "zCATANz ASK_GET_RESURS_FROM_ALL"},
	{NET_COMMAND::SET_CHANGE_OFFER,    "zCATANz SET_CHANGE_OFFER"},
	{NET_COMMAND::CHANGE_OFFER,        "zCATANz CHANGE_OFFER"},
	{NET_COMMAND::ASK_DELETE_OFFER,    "zCATANz ASK_DELETE_OFFER"},
	{NET_COMMAND::ASK_ACCEPT_OFFER,    "zCATANz ASK_ACCEPT_OFFER"}
};

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
// ������ ��������� ��������� �������� �� ������
//=======================================================
void Info_Player_Resurs()
{
	char msg[256];

	//�������  ������
	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
	_itoa(player_num, &msg[50], 10);     //����� ������
	memcpy(&msg[54], player[player_num].resurs, 10 * sizeof(int));   //��������
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//=======================================================
// ������ ��������� ��������� �������� �� ������
//=======================================================
void Info_Main_Bank()
{
	char msg[256];

	//����� ���� ��������
	strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
	memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//=======================================================
// ������ ������� ����� ��������
//=======================================================
void AskPlayDevelopCard(IMP_TYPE type)
{
	char msg[256];
	std::cout << "  Ask to play card  " << (int)type  << std::endl;

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
strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
send(Connection, msg, sizeof(msg), NULL);
Sleep(10);

//�������  ������ - �������� ��� ��� ����� ���������� �� ������ ��� ���������
strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
_itoa(player_num, &msg[50], 10);     //����� ������
memcpy(&msg[54], player[player_num].resurs, 10 * sizeof(int));   //��������
send(Connection, msg, sizeof(msg), NULL);
Sleep(10);

//��������� �� ������ ���� �������� ���� � ��������� �� ����� ������
strcpy_s(msg, Map_Command[NET_COMMAND::ASK_BUY_IMPROVE_CARD].c_str());
itoa(player_num, &msg[50], 10);
memcpy(&msg[60], &ChangeBANK[player_num][0], sizeof(int) * 10);
send(Connection, msg, sizeof(msg), NULL);
Sleep(10);

//� ����� ������ ������ �������� ���� �������� � ������� ������

return true;
}

//=======================================================
//  ���� ������ � ������ ���� ������ ������������������
//=======================================================
void InitChange_BANK()
{
int i, j;

    for (i = 0; i < 5; i++)  InitChange_BANK(i);

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
// ������ �������� ���� �� �������
//========================================================
void Send_roads()
{
	char msg[256];
	int* intPtr;
	int i = 0;

	//���� �� �������
	strcpy_s(msg, Map_Command[NET_COMMAND::ROAD_ARRAY].c_str());   //�������� �������, ��� ��� ����� ����� ��������� ������ � 50 ������
	intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //� 1 ������ ����� ���������� ����
	intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� 2 ������� �������   0, 1, 2
	for (size_t p = 0; p < roadPtr->size(); p++)
	{
		*intPtr++ = roadPtr->at(p).owner;
		*intPtr++ = roadPtr->at(p).type;
		i++;
		if (i == 20 || p == roadPtr->size() - 1)                //��� ����� ��������� 20 ����� - ���������� 
		{
			intPtr = (int*)&msg[50];	*intPtr = i;            //� 50 ������ ���������� �����           
			send(Connection, msg, sizeof(msg), NULL);
			i = 0;
			intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = p + 1;   //� 1 ������ ����� ���������� ����
			intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� 2 ������� �������   0, 1, 2
		}
	}

return;
}

//========================================================
// �������� ���� �� ����� - � ���������� �� ������
//========================================================
void Send_nodes()
{
char msg[256] = { '\0' };
int* intPtr;
int i = 0;

	//���� �� ����� ������
    strcpy_s(msg, Map_Command[NET_COMMAND::NODE_ARRAY].c_str());   //�������� �������, ��� ��� ����� ����� ��������� ������ � 50 ������
    intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //� 1 ������(54 ������) ����� ���������� ���� � ������������ ������
	intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� ���� ������� ������� (58 ������)
    for (size_t p = 0; p < nodePtr->size(); p++)
	   {
	   *intPtr++ = nodePtr->at(p).owner;   
	   *intPtr++ = nodePtr->at(p).object;
	   i++;
	   if (i == 20 || p == nodePtr->size()-1)                        //��� ����� ��������� 20 ����� - ���������� 
	        {
		    intPtr = (int*)&msg[50];	*intPtr = i;                 //� 50 ������ ���������� �����  � ������         
			send(Connection, msg, sizeof(msg), NULL);
			i = 0;
			intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = p+1;   //� 1 ������ ����� ���������� ����
			intPtr = (int*)&msg[50];    intPtr += 2;                 //������ �� ���� ������� ������� 
	        }
	   }

	return;
}

//========================================================================
// ���������� ��� ������ ����� ���� ��� �������������� ���������
// ������ �� ��������� �� ������� ����� �������, ��������, �����, 
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

//========================================================================
// ���������� ��� ������ ����� ���� ��� �������������� ���������
// ������ �� ��������� �� ������� ����� �������� � �������� �������
//========================================================================
void Ask_Send_Resurs()
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_BANK_RESURS].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_PLAYER_RESURS].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);
	Sleep(10);
}

//========================================================
// ������ �� ��������� �� ������� ����� � �����
//========================================================
void Ask_Send_Arrays()
{
char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_NODE_ARRAY].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL); Sleep(10);

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ROAD_ARRAY].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL); Sleep(10);
	Sleep(10);
}

//========================================================
// �������� ��������� ��� ��� ��������
//========================================================
void Say_Move_Over()
{
	char msg[256] = { '\0' };

	//std::cout << " ��� �������� " << std::endl;

	//������ ���� ������� �� �����
	for (int i = 0; i < 12;i++)  Change[i].status = 0;

	//����� ������������� �������� ���� �� ������� � �����
	Send_nodes();	
	Send_roads();

	//����� ������������� �������� ���� �� ����� �������� � ����� �������� �� ������
	//����� ���� ��������
	//Info_Main_Bank();  Sleep(5);

	//�������  ������
	Info_Player_Resurs();

	//�������, ������ .. ������
	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_OBJECTS].c_str());
	_itoa(player_num,                 &msg[50], 10);                    //����� ������
	_itoa(player[player_num].town,    &msg[60], 10);                    //������
	_itoa(player[player_num].village, &msg[70], 10);                    //�������
	_itoa(player[player_num].road,    &msg[80], 10);                    //������
	send(Connection, msg, sizeof(msg), NULL); 


	//�� 3 ���� ��������� ����� ���� ������������ ��������� �������
	if (Game_Step.current_step == 3)  
	   {
		std::cout << " Send Last village =  " << player[player_num].last_village_node  << std::endl;

		strcpy_s(msg, Map_Command[NET_COMMAND::LAST_VILLAGE].c_str());
		itoa(player[player_num].last_village_node, &msg[50], 10);
		send(Connection, msg, sizeof(msg), NULL); Sleep(5);
	   }

	//���� ��������� � ���������� ����
	strcpy_s(msg, Map_Command[NET_COMMAND::SAY_MOVE_OVER].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//========================================================
// �������� ��������� ��� �����  1 �����
//========================================================
void Say_Roll_2Dice()
{
	char msg[256] = { '\0' };

	//std::cout << " �� �������  ������ " << std::endl;

	// ��������� � ������ ������
	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ROLL_2DICE].c_str());
	itoa(player_num, &msg[50], 8);
	send(Connection, msg, sizeof(msg), NULL);	Sleep(5);

	return;
}

//========================================================
// �������� ��������� ��� �����  1 �����
//========================================================
void Say_Roll_1Dice()
{
char msg[256];

	std::cout << " �� �������  ����� " << std::endl;

	// ��������� � ������ ������
	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ROLL_1DICE].c_str());
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
	std::cout << " func say start " << std::endl;
	//if (Count_Num_players() < 2)  return;

	// ��������� ������� � ������ ����
	strcpy_s(msg, Map_Command[NET_COMMAND::SAY_GAME_START].c_str());
	send(Connection, msg, sizeof(msg), NULL);	Sleep(10);

return;
}

//========================================================
// �������� ��������� ������� � ����� �����������
//========================================================
void Say_Move_Banditos()
{
 char msg[256] = { '\0' };

	strcpy_s(msg, Map_Command[NET_COMMAND::BANDIT_GECS].c_str());
	_itoa(bandit_Gecs, &msg[50], 10);
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

//===============================================================================================       CLIENT NET !!!!
 //������� ������� ��� ��������� ��������� �� ��������� �����
 //��� ��������� ������ ������� ��������� � ��� ���� ��� ��������� ������
 //���� ��� ��������� ��������� CATAN ������������
 //==============================================================================================
void ClientHandler(int index)
{
	int ret = 1;
	int i;
	char* ptr;
	char msg[256];   //--------------------------------------------------------------------------
	std::string str;
	int pack_size;
	NET_COMMAND Command = NET_COMMAND::EMPTY;

	while (true)            //����������� ���� � ������ ��������� ������� ���������
	{
		pack_size = 0;
		while (pack_size < 256)
		{
			ret = recv(Connection, &msg[pack_size], sizeof(msg) - pack_size, NULL); 
			if (ret == SOCKET_ERROR)
			    {
				std::cout << " SOCKET_ERROR " << std::endl;		closesocket(Connection);	Connection = 0; 	return;
			    }
			pack_size += ret;
		}

			str.assign(msg, 0, 7);    //����� ������ 7 �������� ���������
			 //���� ��������� ��������� $CATAN&
			if (str.compare(CATAN_command) == 0)   //��������� �� ������� $CATAN&
			    {
				str.assign(msg, 0, strlen(msg));
				//std::cout << " Get CATAN COMMAND  length =  " << pack_size << std::endl;

				//���������� ��� ������� � ������� ��������� (� str � ��� ������ ������ ������ �� ������)
				for (const auto &key : Map_Command)   {	if (key.second == str)  Command = key.first;   }

				//std::cout << " ����� �������: " << Map_Command[Command]  << std::endl;
				
				if (Command == NET_COMMAND::SET_N_PLAYER)
				    {
					player_num = atoi(&msg[50]);	player[player_num].active = true;    //� ����� ��������� ���������� ������
					std::cout << "��� �������� �����: " << player_num << std::endl;
					continue;
				    }

				if (Command == NET_COMMAND::ADD_N_ACTIVE_PLAYER)  //���� � ����������� ������ ������
				    {
					mtx1.lock();  std::cout << "����������� ����� �: " << atoi(&msg[50]) << std::endl; mtx1.unlock();
					player[atoi(&msg[50])].active = true;    //� ����� ��������� ���������� ������
					continue;
				    }

				if (Command == NET_COMMAND::SET_FIRST_PLAYER)  //
				    {
					mtx1.lock();  std::cout << "������ ������� �������� ����� �: " << atoi(&msg[50]) << std::endl;  mtx1.unlock();
					Game_Step.start_player = atoi(&msg[50]);
					continue;
				    }

				if (Command == NET_COMMAND::SET_STEP)  
				    {
					//std::cout << "������� ����� ���� ���� " << atoi(&msg[50]) << std::endl;
					Game_Step.current_step = atoi(&msg[50]);
					//������� ������� ������ ���������� ������
					if (Game_Step.current_step == 4)
					    {
						for (int i = 1; i < 5; i++)   player[i].last_dice = 0;
					    }
					continue;
				    }

				if (Command == NET_COMMAND::SET_N_ACTIVE_PLAYER)
				    {
					//������ ���� ������� �� �����
					for (int i = 0; i < 12; i++)  Change[i].status = 0;

					if(Game_Step.current_step == 4)  Game_Step.step[4].roll_2_dice = 1;
					Game_Step.current_active_player = atoi(&msg[50]);
					InitChange_BANK();    //� ������ ���� ���� ������ ����������
					for (int ii = 1; ii < 5; ii++)   { player[ii].flag_allow_get_card = 0;  limit_7[ii] = 0; }

					//������� ������� ������ ���������� ������
					if(player_num == Game_Step.current_active_player && Game_Step.current_step == 4)
						          player[player_num].last_dice = 0;
					continue;
				    }

				if (Command == NET_COMMAND::SET_GECS)
					{
					int gecs_num = atoi(&msg[50]);
					//std::cout << "������� ���� �����:  " << gecs_num  << std::endl;
					gecsPtr->at(gecs_num).type = (RESURS)atoi(&msg[60]);
					gecsPtr->at(gecs_num).gecs_game_number = atoi(&msg[70]);
					continue;
					}

				if (Command == NET_COMMAND::NODE_ARRAY)
				    {
					int* intPtr = (int*)&msg[50];
					int node_num = *intPtr;  intPtr++;
					int first_node = *intPtr;  intPtr++;
					//std::cout << " ARRAY    num  " << node_num << "   Start N" << first_node  << std::endl;
					for (int p = 0; p < node_num; p++) { nodePtr->at(first_node + p).owner = *intPtr++;	nodePtr->at(first_node + p).object = *intPtr++; }
				
					continue;
				    }

				if (Command == NET_COMMAND::ROAD_ARRAY)
				    {
					int* intPtr = (int*)&msg[50];
					int road_num = *intPtr;  intPtr++;
					int first_road = *intPtr;  intPtr++;
					//std::cout << " ARRAY    num  " << road_num << "   Start N" << first_road << std::endl;
					for (int p = 0; p < road_num; p++) { roadPtr->at(first_road + p).owner = *intPtr++; 	roadPtr->at(first_road + p).type = *intPtr++; }					
				    }

				if (Command == NET_COMMAND::INFO_BANK_RESURS)
				    {
					int* intPtr = (int*)&msg[50];
					_itoa(improve_CARDS.size(), &msg[100], 10);
					memcpy(CARD_Bank, &msg[50], 10 * sizeof(int));
					continue;
				    }

				if (Command == NET_COMMAND::PLAYER_RESURS)
				{
					int pl = atoi(&msg[50]);
					memcpy(player[pl].resurs, &msg[54], 10 * sizeof(int));   //��������
					continue;
				}

				if (Command == NET_COMMAND::PLAYER_OBJECTS)    //� ������� �������� ����� �������� � ���� ������ ��� 
				{
					//����� �������, ��������, ����� ������
					player[player_num].town = atoi(&msg[60]);
					player[player_num].village = atoi(&msg[70]);
					player[player_num].road = atoi(&msg[80]);
					
					continue;
				}

				if (Command == NET_COMMAND::DICE_SEVEN)
				    {
					int pl = atoi(&msg[50]);
					if(pl == player_num)
					       {
						   //std::cout << " �� ������� 7 �� ������� !!!! " << std::endl;
						   Game_Step.step[4].flag_bandit = 1;
						   //���������� ������� ���� ������ �������� � ������� ����� ���������� ����
						   for (int i = 1; i < 5; i++)
						        {
							    limit_7[i] = 0;
							    if (i == player_num)   continue;
							    if (getPlayerNumCardResurs(i) > 7)   limit_7[i] = getPlayerNumCardResurs(i) / 2 + getPlayerNumCardResurs(i) % 2;
						        }
					       }
					     else
					       {
						   if (getPlayerNumCardResurs(player_num) >= 8)
						          {
							      limit_7[player_num] = getPlayerNumCardResurs(player_num) / 2 + getPlayerNumCardResurs(player_num) % 2;
							      //std::cout << " ������ 7, ���� �������� "<< limit_7[player_num] << "���� �������" << std::endl;
						          }
					       }
					continue;
				    }

				if (Command == NET_COMMAND::PLAY_DEVELOP_CARD)
				{
					int pl = atoi(&msg[50]);
					int type = atoi(&msg[54]);
					if (pl == player_num && type == (int)IMP_TYPE::KNIGHT)
					    {
					    //std::cout << " �� ������� ����� ������ " << std::endl;
						Game_Step.step[4].flag_bandit = 1;
					    }

					if (pl == player_num && type == (int)IMP_TYPE::ROAD2)
					    {
						//std::cout << " �� ������� ����� 2 ������ " << std::endl;
						Play_two_roads = 2;
					    }
					if (pl == player_num && type == (int)IMP_TYPE::RESURS_CARD2)
					    {
						std::cout << " �� ������� ����� 2 ������� " << std::endl;
						Play_two_resurs = 2;
					    }
					if (pl == player_num && type == (int)IMP_TYPE::RESURS1)
					    {
						std::cout << " �� ������� ����� 1 ������ �� ���� " << std::endl;
						Play_ONE_resurs = 1;
					    }

					continue;
				}

				if (Command == NET_COMMAND::LAST_DICE)
				    {
					int pl = atoi(&msg[50]);
					player[pl].last_dice = atoi(&msg[54]);
					player[player_num].flag_allow_change = 1;     //���������� �� �����
					continue;
				   }

				if (Command == NET_COMMAND::BANDIT_GECS)
				{
					bandit_Gecs = atoi(&msg[50]);
					continue;
				}

				if (Command == NET_COMMAND::MAX_WAY_OWNER)
				    {
					max_road_owner = atoi(&msg[50]);
					continue;
				    }

				if (Command == NET_COMMAND::PLAYER_CHANGE_AREA)
				   {
					int pl = atoi(&msg[50]);                                 //����� ������
					memcpy(ChangeBANK[pl] ,&msg[54], 10 * sizeof(int));      //��������
					continue;
				   }

				if (Command == NET_COMMAND::INFO_IMP_CARDS)    //����� ����
				    {
					int size = atoi(&msg[50]);
					int* intPtr = (int*)&msg[54];
					improve_CARDS.clear();
					IMP_CARD tmp;
					for (int i = 0; i < size; i++)
					    {
						tmp.status = *intPtr++;
						tmp.type = (IMP_TYPE)(*intPtr++);
						improve_CARDS.push_back(tmp);   //��������� ������ 
					    }
					continue;
				    }

				if (Command == NET_COMMAND::DEVELOP_VECTOR)  //������ ���� �������� ������
				    {
					int pl = atoi(&msg[50]);
					develop_CARDS[pl].clear();

					int size = atoi(&msg[54]);
					if (size == 0)   continue;
					
					int* intPtr = (int*)&msg[58];
					IMP_CARD tmp;
					for (int i = 0;i < size; i++)
					    {
						tmp.status = *intPtr++;
						tmp.type = (IMP_TYPE)(*intPtr++);
						develop_CARDS[pl].push_back(tmp);   //��������� ������ 
					    }

					//std::cout << "Get develop, size =  " << develop_CARDS[pl].size() << std::endl;
					continue;
				    }

				if (Command == NET_COMMAND::CHANGE_OFFER)
				    {
					int s = atoi(&msg[50]);                              //����� ������
					memcpy(&Change[s], &msg[54], sizeof(CHANGE));        //����������� ��������� ������

				    //�������� ����� ������
					/*
					std::cout << " ������ N " << s << " from   " << Change[s].from_pl << "  to " << Change[s].to_pl << std::endl;
					std::cout << " ����������� -----------------------" << std::endl;
					for (int t = 0; t < 6; t++)
					    {
						if (Change[s].offer_num[t]) std::cout << " CARD " << resurs_name[t] << " num  " << Change[s].offer_num[t] << std::endl;
					    }
					std::cout << " ��������� -----------------------" << std::endl;
					for (int t = 0; t < 6; t++)
					    {
						if (Change[s].need_num[t]) std::cout << " CARD " << resurs_name[t] << " num  " << Change[s].need_num[t] << std::endl;
					    }
					std::cout << " -------------------------------" << std::endl;
					*/

					continue;
				    }
						
			     continue;
			    }

			mtx1.lock();
			std::cout << "CLIENT: " << msg  << std::endl;
			mtx1.unlock();
	
		}
    return;
	}

//===============================================================================================
// ������������� ������� ���� 
//===============================================================================================
int Start_Server_CATAN()
{
	init_WSA();

	//������� ��������� �����, ��� �������������� ����������� �������
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Check_Connections, NULL, NULL, NULL);

	//std::thread t_server(Check_Connections);
	//t_server.join();

	Sleep(50);
	return 1;
}

//===============================================================================================
// ������������� ������� ���� 
//===============================================================================================
int Init_Client_CATAN(void)
{
	init_WSA();
	std::cout << "\n\n ========== PLAYER PART WINSOCK CATAN START ============ " << std::endl;

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_addr.s_addr = inet_addr(addr_connect);    //------------------------------------------------------------------      ADDR
	addr.sin_port = htons(MY_PORT);
	addr.sin_family = AF_INET;      //�������� ��������

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)	{ std::cout << "ERROR fail connect to server" << std::endl;	  return 0; 	}

	mtx1.lock();  std::cout << "  OK, Connected to server " << std::endl; mtx1.unlock();

	//��������� ����� ������������� ��������� �� �������
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	//��������� ����� ����� ��������� ��� ����
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientChart, NULL, NULL, NULL);

	return 1;
}

//===============================================================================================
// ������������� WSA Data
//===============================================================================================
int init_WSA(void)
{
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) { std::cout << " ERROR WSA " << std::endl;  exit(1); }

	return 0;
}

//==============================================================================================  CONNECT
//     ������� ������ ����������� ��������� ����� =
//==============================================================================================
void Check_Connections(void)
{
	int i;
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	//addr.sin_addr.s_addr = inet_addr(addr_listen);      //����� ��������� ����
	addr.sin_port = htons(MY_PORT);
	addr.sin_family = AF_INET;      //�������� ��������


	//-----------���� �� ������ ����� - �������� ���� ��������� ����� --------------------------------
	char chInfo[64];
	if (!gethostname(chInfo, sizeof(chInfo)))
	{
		std::cout << "Host Name =   " << chInfo << std::endl;
		struct hostent* sh;
		sh = gethostbyname((char*)&chInfo);
		if (sh != NULL)
		{
			int nAdapter = 0;
			while (sh->h_addr_list[nAdapter])
			{
				struct sockaddr_in adr;
				memcpy(&adr.sin_addr, sh->h_addr_list[nAdapter], sh->h_length);
				printf("%s\n", inet_ntoa(adr.sin_addr));
				if (nAdapter == 0)  strcpy_s(addr_listen, inet_ntoa(adr.sin_addr));    //���������� ����� ������� ��������
				nAdapter++;
			}
		}
	}

	//��������� ����� ������ ����� ���� 192.168.YYY.XXX  !!!
	addr.sin_addr.s_addr = inet_addr(addr_listen);
	std::cout << "Listen Address =   " << addr_listen << std::endl;

	//����������� ��������� ���� �� ������������ ������ -----------------------------------------
	//using System;
	//using System.Net;
	/*
	if (!System.Net.NetworkInformation.NetworkInterface.GetIsNetworkAvailable())
	    {
		System.
		std::cout << "Net Error   "  << std::endl;
	    }
		*/
	//-------------------------------------------------------------------------------------------

	//��������� �����
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);   //create sockrt
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));         // ����������� ����� � ������
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	char msg[256] = "";     //����� ������������ �������������� �������� 256	
	
	std::cout << "\n ======  SERVER: ����� ����� �������� ����������� ====== \n  " << std::endl;

	for (i = 0; i < 4; i++)
	{
		std::cout << " SERVER: Wait for next player \n  " << std::endl;
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);     //�� ���������� ���������� ���� �� �������� ������� �� �����������
		if (newConnection == 0)  {	std::cout << "ERROR newConnection" << std::endl;  }
		else
		{
			mtx1.lock();
			std::cout << "SERVER:  OK Player  N= " << (Counter+1) << "   Connected" << std::endl;
			mtx1.unlock();

			std::cout << "About player =============================== " << std::endl;
			std::cout << "Port =  " << addr.sin_port << std::endl;
			//std::cout << "Addr =  " << addr.sin_addr.S_un.S_un_b.s_b1 << std::endl;
			//std::cout << "Addr =  " << addr.sin_addr.S_un.S_un_b.s_b2 << std::endl;
			//std::cout << "Addr =  " << addr.sin_addr.S_un.S_un_b.s_b3 << std::endl;
			//std::cout << "Addr =  " << addr.sin_addr.S_un.S_un_b.s_b4 << std::endl;

			printf("�����������  %s\n", inet_ntoa(addr.sin_addr));
			std::cout << "============================================ " << std::endl << std::endl;

			//-----------------------------------------------------------------------------------

			//�������� ��������� ��������� �������������� �������, ������ ������ ������
			strcpy_s(msg, "Hello. It's CATAN GAME ");
			send(newConnection, msg, sizeof(msg), NULL); 

			// ������ ������ ������
			strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_PLAYER].c_str());
			_itoa(Counter + 1, &msg[50], 10);
			send(newConnection, msg, sizeof(msg), NULL); 
			//_itoa(i, msg, 10);  strcat_s(msg,"  -   ����� ���������� ");
			//send(newConnection, msg, sizeof(msg), NULL);

			//�������� ��������������� ���� - ����� � ������ - ��������� ������ ����������
			int gecs_num = 0;
			for (auto& elem : *gecsPtr)
			   {
			   strcpy_s(msg, Map_Command[NET_COMMAND::SET_GECS].c_str());
			   //���� �������� � ������ ������ �����
			   _itoa(gecs_num++, &msg[50], 10);       //�� 50 ������� ����� �����
			   _itoa((int)elem.type, &msg[60], 10);   //�� 60 ������� ��� �����
			   _itoa((int)elem.gecs_game_number, &msg[70], 10);   //�� 70 ������� ����������� ������� �����
			   send(newConnection, msg, sizeof(msg), NULL); Sleep(1);
			   }
			

			//�������� � ����� ������������ �����������
			strcpy_s(msg, Map_Command[NET_COMMAND::BANDIT_GECS].c_str());
			_itoa(bandit_Gecs, &msg[50], 10);       
			send(newConnection, msg, sizeof(msg), NULL); 

			//���� ����� ����������� �� ������, �� ��� ���� �������� � ����� ��������������
			if(Counter > 0)
			   {
			   for(int ii = 0;ii < Counter;ii++)
			      {
				   strcpy_s(msg, Map_Command[NET_COMMAND::ADD_N_ACTIVE_PLAYER].c_str());
				   _itoa(ii+1, &msg[50], 10);
				   send(newConnection, msg, sizeof(msg), NULL); 
			      }
			   }

			//��������� �������� ������� � ����������� ������
			strcpy_s(msg, Map_Command[NET_COMMAND::ADD_N_ACTIVE_PLAYER].c_str());
			_itoa(Counter + 1, &msg[50], 10);
			Send_To_All(msg, sizeof(msg));

			player[Counter+1].active = true;  //������������ ������ ������
			Connections[i] = newConnection;
			Counter++;

			//������� ������������� ������� ������� ����� ��������� ��������� ������
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ServerClientStreamFunc, (LPVOID)(i), NULL, NULL);
		}   //else
	}

	std::cout << " SERVER: ������������ ���-�� ������� 4 ������������ \n  " << std::endl;
	std::cout << " SERVER: ����� ����������� ��������� ������ \n  " << std::endl;
}

//================================= SERVER STREAM FUNCTION ======================================  SERVER NET
//������� ��� ������ ��������� �� �������� � �������� ����, ����� ���� �� ���� ��� ������ �������
//===============================================================================================
void ServerClientStreamFunc(int index)
{
	char msg[256];
	char msg1[10];
	int ret;
	std::string str;
	NET_COMMAND Command = NET_COMMAND::EMPTY;
	bool system = false;
	int pack_size = 0;

	//����������� ����� � ������ 20��� ���������� �������� ��������� ���������� �����
	//��� ����� �������� �� ������������������ ���� �� �����

	while (true)
	{
		pack_size = 0;
		while (pack_size < 256)
		    {
			ret = recv(Connections[index], &msg[pack_size], sizeof(msg) - pack_size, NULL);
			if (ret == SOCKET_ERROR)
			   {	std::cout << " SOCKET_ERROR " << std::endl;	 closesocket(Connections[index]);  Connections[index] = 0; 	return;   }
			pack_size += ret;
		    }

		//������� �� ��������� ��������� CATAN ==================================================================
		str.assign(msg, 0, 7);    //����� ������ 7 �������� ���������

	    //���� ��������� ��������� zCATANz
		if (str.compare(CATAN_command) == 0)  
		{
			str.assign(msg, 0, strlen(msg));  //���������� 0 ������ ������ ��� ��������� ������ � ������ ������ str
			//std::cout << " Get CATAN COMMAND  " << std::endl;

			//���������� ��� ������� ��������� ��������� ������� �� �������  
			//(� str � ��� ������ ������ ������ �� ������)
			for (const auto& key : Map_Command)
			    {
				if (key.second == str)  Command = key.first;
			    }

			//std::cout << " ����� �������: " << Map_Command[Command] << std::endl;

			if (Command == NET_COMMAND::SAY_GAME_START)
			{	
				mtx1.lock();  std::cout << "  START GAME Command !!!  " << std::endl; mtx1.unlock();

				Game_Step.current_step = 1;
				Game_Step.current_active_player = 1;

				strcpy_s(msg, " ================ ���� ���������� ===============");
				Send_To_All(msg, sizeof(msg));
				
				//���������� ���������� ����� �������

				//�������� ���� ����� ����
				strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	_itoa(Game_Step.current_step, &msg[50], 10);
				Send_To_All(msg, sizeof(msg)); 

				//�������� ���� ��� ��� ������� ������ �1 ��� ������ ������
				strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	_itoa(1, &msg[50], 10);
				Send_To_All(msg, sizeof(msg));
				continue;
			}

			if (Command == NET_COMMAND::ROAD_ARRAY)
			    {
				int* intPtr = (int*)&msg[50];
				int road_num = *intPtr;  intPtr++;
				int first_road = *intPtr;  intPtr++;
				//std::cout << " ARRAY    num  " << road_num << "   Start N" << first_road << std::endl;
				for (int p = 0; p < road_num; p++)   {    roadPtr->at(first_road + p).owner = *intPtr++; 	roadPtr->at(first_road + p).type = *intPtr++;    }
				//��������� ������ ��������
				Send_To_All(msg, sizeof(msg));
				continue;
			    }

			if (Command == NET_COMMAND::NODE_ARRAY)
			{
				int* intPtr = (int*)&msg[50];
				int node_num   = *intPtr;  intPtr++;
				int first_node = *intPtr;  intPtr++;
				//std::cout << " ARRAY    num  " << node_num << "   Start N" << first_node  << std::endl;
				for(int p = 0; p < node_num; p++)  { nodePtr->at(first_node + p).owner = *intPtr++;	nodePtr->at(first_node + p).object = *intPtr++;   }
				//��������� ���� ��������
				Send_To_All(msg, sizeof(msg));
				continue;
			}

			if (Command == NET_COMMAND::ASK_NODE_ARRAY)
			   {
			   Send_To_All_Info_Nodes();
			   continue;
			   }

			if (Command == NET_COMMAND::ASK_ROAD_ARRAY)
			    {
				int* intPtr;
				int i = 0;
				//������� �������� ��������� �����
				strcpy_s(msg, Map_Command[NET_COMMAND::ROAD_ARRAY].c_str());   //�������� �������, ��� ��� ����� ����� ��������� ������ � 50 ������
				intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //� 1 ������ ����� ���������� ����
				intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� 2 ������� �������   0, 1, 2
				for (size_t p = 0; p < roadPtr->size(); p++)
				   {
					*intPtr++ = roadPtr->at(p).owner;
					*intPtr++ = roadPtr->at(p).type;
					i++;
					if (i == 20 || p == roadPtr->size() - 1)                //��� ����� ��������� 20 ����� - ���������� 
					    {
						intPtr = (int*)&msg[50];	*intPtr = i;            //� 50 ������ ���������� �����           
						Send_To_All(msg, sizeof(msg));
						i = 0;
						intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = p + 1;   //� 1 ������ ����� ���������� ����
						intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� 2 ������� �������   0, 1, 2
					    }
				    }
				continue;
			    }

			if (Command == NET_COMMAND::LAST_VILLAGE)
			    {
			    player[Game_Step.current_active_player].last_village_node = atoi(&msg[50]); 
			    continue;
			    }
				
			if (Command == NET_COMMAND::SAY_MOVE_OVER)
			{
				//������ ���� ������� �� �����
				for (int i = 0; i < 12; i++)  Change[i].status = 0;

				int pl = atoi(&msg[50]);
				//std::cout << " ���������� ���� �����  " << pl << std::endl;
				if ( Game_Step.current_step == 4)
				    {
					Game_Step.current_active_player = GetNextPlayer();

					//������������ ������ �������� �������, ���� ��� �� ������������
					for (auto& elem : develop_CARDS[Game_Step.current_active_player])
					    {
						if (elem.status == -1 && elem.type == IMP_TYPE::KNIGHT)  elem.status = 0;
					    }
					Send_To_All_Develop_CARDS(Game_Step.current_active_player);  Sleep(1);

					std::cout << "SERVER  �������� ���� ������  " << Game_Step.current_active_player << std::endl;
					strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	
					_itoa(Game_Step.current_active_player, &msg[50], 10);
					Send_To_All(msg, sizeof(msg)); Sleep(1);

					InitChange_BANK();
					//������� ������ �������� ������    max_road_owner
					int way;
					for(int i = 1;i < 5;i++)    
					   {
						way = Count_Road_Length(i);
						if (way > Count_Road_Length(max_road_owner) && way >= 5)  max_road_owner = i;
					   }
					if(Count_Road_Length(max_road_owner) < 5) max_road_owner = 0;
					Send_To_All_Info_Max_Way();
					continue;
				    }
				if (Game_Step.current_step == 3)
				      {
					  Get_Resurs(player[pl].last_village_node, pl);    //������� ������� ��� �������
					  //std::cout << "Step3  M over, active = " << Game_Step.current_active_player << " start= " << Game_Step.start_player << std::endl;

					  if (Game_Step.current_active_player == Game_Step.start_player)
					       {
						   Game_Step.current_step = 4;     //����� ����������� ����� ����
						   std::cout << " =========== ������� �� �������� ����  ==========  " << pl << std::endl;
						   strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());
						   _itoa(Game_Step.current_step, &msg[50], 9);
						   Send_To_All(msg, sizeof(msg)); Sleep(1);
					       }
					     else Game_Step.current_active_player = GetPrevPlayer();
					  Send_To_All_Info_Resurs();
				      }
				if (Game_Step.current_step == 2)
				    {
					if (GetNextPlayer() == Game_Step.start_player)
					    { 
						std::cout << " ============ ������� �� 3 ��� ===========  " << pl << std::endl;
						if (Game_Step.current_step == 2)   Game_Step.current_step = 3;
						strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());
						_itoa(Game_Step.current_step, &msg[50], 9);
						Send_To_All(msg, sizeof(msg));		Sleep(2);
					    }
					else  Game_Step.current_active_player = GetNextPlayer();
				    }

				mtx1.lock();  std::cout << "SERVER*  �������� ���� ������  " << Game_Step.current_active_player << std::endl;  mtx1.unlock(); 
				strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	
				_itoa(Game_Step.current_active_player, &msg[50], 10);
				Send_To_All(msg, sizeof(msg));	Sleep(1);   

			 continue;
			}

			if (Command == NET_COMMAND::ASK_ROLL_1DICE)
			{
			int pl = atoi(&msg[50]);

			srand(time(0));
			player[pl].first_roll = rand() % 6 + 1;

			//��������� ��������� --------------------------------------
			strcpy_s(msg, Map_Command[NET_COMMAND::LAST_DICE].c_str());
			_itoa(pl, &msg[50], 10);
			_itoa(player[pl].first_roll, &msg[54], 10);
			Send_To_All(msg, sizeof(msg)); Sleep(2);

            //���� �� ��� ������� ����� �������� ��� ���� ������, ���� ��� �� ���� ��� ����
			if (GetNextPlayer() == Game_Step.start_player)
			    {
				//mtx1.lock();  std::cout << "GetNextPlayer() == 0  ������� ��� ��� ������ " << std::endl; mtx1.unlock();
				SetFirstPlayer();    //��������� �� ����������� ������ 1 ������ ����������� ���� � ��������� step++
			    std::cout << "������ ����� ������ ����� -   " << Game_Step.start_player << std::endl;
			    }
			    else  Game_Step.current_active_player = GetNextPlayer();

			strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	
			_itoa(Game_Step.current_step, &msg[50], 9);
			Send_To_All(msg, sizeof(msg)); Sleep(2);
		
			mtx1.lock();  std::cout << "SERVER  �������� ���� ������  " << Game_Step.current_active_player << std::endl;  mtx1.unlock();
			strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());
			_itoa(Game_Step.current_active_player, &msg[50], 9);
			Send_To_All(msg, sizeof(msg));
			continue;
			}

			if (Command == NET_COMMAND::ASK_ROLL_2DICE)
			{
				int pl = atoi(&msg[50]);

				//srand(time(0));
				int dice2 = rand() % 6 + 1  + rand() % 6 + 1;   //2 ������

				//�������� ���� ��������� ������ ������ - �� ��������� ������� ���������� 
				std::cout << " ��������� ������  =  " <<  dice2  << std::endl;

				strcpy_s(msg, Map_Command[NET_COMMAND::LAST_DICE].c_str());	
				_itoa(pl, &msg[50], 10);
				_itoa(dice2, &msg[54], 10);
				Send_To_All(msg, sizeof(msg));

				if(dice2 == 7)
				    {
					Send_To_All_Info_Resurs();
					strcpy_s(msg, Map_Command[NET_COMMAND::DICE_SEVEN].c_str());	_itoa(pl, &msg[50], 10);
					Send_To_All(msg, sizeof(msg));
					continue;
				    }
				   else
				       {
					   //������������ ������� �� ����������� ������ - ���� �� ����� ������
					   Step_Resurs(dice2);
					   Send_To_All_Info_Resurs();
				       }
				continue;
			}

			if (Command == NET_COMMAND::INFO_BANK_RESURS)  //������� �������� - �� ��� ���������
			{
				int* intPtr = (int*)&msg[50];
				memcpy(CARD_Bank, &msg[50], 10 * sizeof(int));
				Send_To_All(msg, sizeof(msg)); Sleep(2);
				continue;
			}

			if (Command == NET_COMMAND::ASK_BANK_RESURS)    //� ������� �������� ������ � ������ ��� ����
			{
				int pl = atoi(&msg[50]);
				//����� ���� ��������
				strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
				memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
				send(Connections[pl-1], msg, sizeof(msg), NULL);
				continue;
			}

			if (Command == NET_COMMAND::ASK_PLAYER_RESURS)    //� ������� �������� ����� �������� � ������ ��� ����
			{
				int pl = atoi(&msg[50]);
				//���� �������� ������
				strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
				_itoa(pl, &msg[50], 10);                                 //����� ������
				memcpy(&msg[54], player[pl].resurs, 10 * sizeof(int));   //��������
				send(Connections[pl - 1], msg, sizeof(msg), NULL);
				continue;
			}

			if (Command == NET_COMMAND::ASK_PLAYER_OBJECTS)    //� ������� �������� ����� �������� � ���� ������ ��� 
			{
				int pl = atoi(&msg[50]);
				//����� �������, ��������, ����� ������
				strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_OBJECTS].c_str());
				_itoa(pl, &msg[50], 10);                                 //����� ������
				_itoa(player[pl].town, &msg[60], 10);                    //������
				_itoa(player[pl].village, &msg[70], 10);                 //�������
				_itoa(player[pl].road, &msg[80], 10);                    //������
				send(Connections[pl - 1], msg, sizeof(msg), NULL); Sleep(1);
				continue;
			}

			if (Command == NET_COMMAND::PLAYER_OBJECTS)    //� ������� �������� ����� �������� � ���� ������ ��� 
			{
				int pl = atoi(&msg[50]);
				//����� �������, ��������, ����� ������
				player[pl].town = atoi(&msg[60]);
				player[pl].village = atoi(&msg[70]);
				player[pl].road = atoi(&msg[80]);

				continue;
			}

			if (Command == NET_COMMAND::PLAYER_RESURS)         //������ ������� ���� ������ � ��������� ����
			{
				int pl = atoi(&msg[50]);
				memcpy(player[pl].resurs, &msg[54], 10 * sizeof(int));   //��������
				Send_To_All(msg, sizeof(msg));  Sleep(1);
				continue;
			}

			if (Command == NET_COMMAND::BANDIT_GECS)
			{
				bandit_Gecs = atoi(&msg[50]);
				Send_To_All(msg, sizeof(msg));
				continue;
			}

			if (Command == NET_COMMAND::ASK_CHANGE_BANK)    //������ �� ����� ��������
			{
				int pl = atoi(&msg[50]);
				int type_put = atoi(&msg[54]);
				memcpy(&ChangeBANK[pl][0],&msg[60], sizeof(int) * 10);

				//std::cout << " ������ �� ����� � ������ " << resurs_name[type_put] << " from pl " << pl  <<  std::endl;

				//����� 4 �������� ������ ����, ������� �� �� ����� ������
				int type_get = 0;
				int need_card;
				for(int i = 1;i < 10;i++)
				   {
					need_card = 4;
					if (bonus31(pl)) need_card = 3;
					if(bonus21(pl,i)) need_card = 2;
					//��������� ���� �� ������ ���-�� � �������� ����� � ��� �������� � ����� ������
					if (ChangeBANK[pl][i] >= need_card && player[pl].resurs[i] >= need_card) { type_get = i; break; }        
				   }

				if (type_get == 0)	    {   std::cout << " �� ������� ������ ���-�� ���� "  << std::endl;	continue;      }
				//��������� ���� ������������� ������ � ����� �����
				if (CARD_Bank[type_put] < 1)  { std::cout << " � ����� ��� ������� " << std::endl;	continue; }

				//��������� ����� � ������
				player[pl].resurs[type_put] += 1;            ChangeBANK[pl][type_put] += 1;	           CARD_Bank[type_put] -= 1;
				player[pl].resurs[type_get] -= need_card;    ChangeBANK[pl][type_get] -= need_card;  	CARD_Bank[type_get] += need_card;

				//�������� ���� ��������� ������ ��������
				Send_To_All_Info_Resurs();
				continue;
			}

			if (Command == NET_COMMAND::SEND_CARDS_TO_BANK)
			   {
				//��������� �������� ����
				int pl = atoi(&msg[50]);
				int num;
				for (int i = 1; i < 10; i++)
				    {
					num = ChangeBANK[pl][i];
				    if (num != 0)    {   player[pl].resurs[i] -= num;     ChangeBANK[pl][i] -= num;	   CARD_Bank[i] += num;   }
				    }

				//�������� ���� ��������� ������ �������� - �� ��� ����� ���� � ���� ������ ���� ��������
				Send_To_All_Info_Resurs();
				continue;
			   }

			if (Command == NET_COMMAND::INFO_CHANGE_BANK)
			{
			    //��������� �������� ���� ������
				int pl = atoi(&msg[50]);
				int type_put = atoi(&msg[54]);
				memcpy(&ChangeBANK[pl][0], &msg[60], sizeof(int) * 10);

				//�������� ���� ��������� �����
				Send_To_All_Info_Change_Area(pl);
			}

			if (Command == NET_COMMAND::TAKE_CARD_FROM_PLAYER)
			   {
				int pl = atoi(&msg[50]);
				int pl_donor = atoi(&msg[54]);

				//std::cout << " Take card command "  << std::endl;
				int type = TakeRandomCardFromPl(pl_donor);
				if (type) { player[pl].resurs[type] += 1;  ChangeBANK[pl][type] += 1;   player[pl_donor].resurs[type] -= 1;  }
				//std::cout << " Pl " << pl << " �������� ����� �  " << pl_donor << "type " << resurs_name[type] <<  std::endl;

				//�������� ���� ��������� ������ �������� - �� ��� ����� ���� � ���� ������ ���� ��������
				Send_To_All_Info_Resurs();
				continue;
			   }

			if (Command == NET_COMMAND::ASK_BUY_IMPROVE_CARD)
			{
				if (improve_CARDS.size() < 1)    continue;    //����������� ����� ��������
				int pl = atoi(&msg[50]);
				memcpy(&ChangeBANK[pl][0], &msg[60], sizeof(int) * 10);

				//��������� ������������� ����
				if (ChangeBANK[pl][(int)RESURS::STONE] == 0 || ChangeBANK[pl][(int)RESURS::OVCA] == 0 || 
					    ChangeBANK[pl][(int)RESURS::BREAD] == 0	) 
				                   { 
					               std::cout << "SERVER: �� ������� ������ ���-�� ���� " << std::endl;	
								   continue; 
				                   }

				//������� ����� � ������ � ��������� � ����
				ChangeBANK[pl][(int)RESURS::STONE] -= 1;  player[pl].resurs[(int)RESURS::STONE] -= 1;  CARD_Bank[(int)RESURS::STONE] += 1;
				ChangeBANK[pl][(int)RESURS::OVCA]  -= 1;  player[pl].resurs[(int)RESURS::OVCA] -= 1;   CARD_Bank[(int)RESURS::OVCA] += 1;
				ChangeBANK[pl][(int)RESURS::BREAD] -= 1;  player[pl].resurs[(int)RESURS::BREAD] -= 1;  CARD_Bank[(int)RESURS::BREAD] += 1;

				//�������� ��� ����� �������� �� �������
				auto type = improve_CARDS.at(0).type;
				auto status = improve_CARDS.at(0).status;
				//�������� ������ ����� ��������, �������� ������� �� �������
				improve_CARDS.erase(improve_CARDS.begin());

				//��������� ����� � ������ ������ � �������� ������ ���� �������� ������ ����
				IMP_CARD ttt = {status, type};
				develop_CARDS[pl].push_back(ttt);
				Send_To_All_Develop_CARDS(pl); Sleep(10);

				//std::cout << " ����� �������� " << (int)type << " size befor "<< improve_CARDS.size()  << std::endl;
				//�������� ���� ��������� ������ �������� ��������
				Send_To_All_Info_Resurs();

				continue;
			}

			if (Command == NET_COMMAND::ASK_PLAY_DEVELOP_CARD)
			{
				int pl = atoi(&msg[50]);
				int type = atoi(&msg[54]);

				//� ������� ���� �������� ������ ������ �������� ��������� ����� ��������� � ������ 1(�������)
				for(auto& elem : develop_CARDS[pl])
				    { 
					if (elem.status == 0 && type == (int)elem.type) 
					    {	
						elem.status = 1;  
						break;  
					    }    
				    }

				//��������� ������ ���� �������� ������ ���������� �����
				Send_To_All_Develop_CARDS(pl);

				//��������� ���� ���� � ���� ������ ��������
				strcpy_s(msg, Map_Command[NET_COMMAND::PLAY_DEVELOP_CARD].c_str());
				_itoa(pl, &msg[50], 10);
				_itoa(type, &msg[54], 10);
				Send_To_All(msg, sizeof(msg));   //��� �������� ���� �� ���� ����� ��������

				continue;
			}

			if (Command == NET_COMMAND::ASK_GET_RESURS_FROM_ALL)
			    {
				int pl = atoi(&msg[50]);
				int type = atoi(&msg[54]);
				//std::cout << " ������ �� ������ " << pl << " ������� ���   " << resurs_name[(int)type] << std::endl;

				for (int i = 1; i < 5; i++)
				    {
					if (i == pl || player[i].active == 0)  continue;
					//��������� ����� � ������
					player[pl].resurs[type] += player[i].resurs[type];     ChangeBANK[pl][type] += player[i].resurs[type];
					player[i].resurs[type] = 0;                            ChangeBANK[i][type] = 0;
				    }

				//�������� ���� ��������� ������ �������� ��������
				Send_To_All_Info_Resurs();
			    continue;
			    }

			if (Command == NET_COMMAND::SET_CHANGE_OFFER)
			{
				int pl = atoi(&msg[50]);
				int pl_change = atoi(&msg[54]);

				//std::cout << " Change SET command from   "  <<  pl << "  to "  << pl_change  << std::endl;
				//��������� �� ������ ������ ������ ������� ��������� ������
				//����� �� �������� ������
				int s = 0;
				for (s = 0; s < 12; s++)	   {  if (Change[s].status == 0) break;   }
				if (s == 12)  continue;

				Change[s].from_pl = pl; 		Change[s].to_pl = pl_change;    Change[s].status = 1;
				for (int t = 0; t < 6; t++)
				    {
					Change[s].offer_num[t] = (int)ChangeBANK[pl][t];
					Change[s].need_num[t] = (int)ChangeBANK[pl_change][t];
				    }
				//�������� ������ ������ ��� �������
				Send_To_All_Info_Change(s);
				continue;
			}

			if (Command == NET_COMMAND::ASK_DELETE_OFFER)
			    {
				//std::cout << "  DELETE OFFER  " << std::endl;
				int pl = atoi(&msg[50]);
				int s = atoi(&msg[54]);  //����� ������
				Change[s].status = 0;
				//�������� ������ ������ ��� �������
				Send_To_All_Info_Change(s);
				continue;
			    }

			if (Command == NET_COMMAND::ASK_ACCEPT_OFFER)
			   {
				std::cout << "  ACCEPT OFFER  " << std::endl;

				int pl = atoi(&msg[50]);
				int s = atoi(&msg[54]);  //����� ������

				int pl1 = Change[s].from_pl;
				int pl2 = Change[s].to_pl;

				if (pl2 != pl)   continue;   //���� �� ������ ��������� ������ �� ���������� ������
				//���������� �����
				for(int t = 0; t < 6; t++)
				    {
					player[pl1].resurs[t] += Change[s].need_num[t];   player[pl2].resurs[t] -= Change[s].need_num[t];
					player[pl2].resurs[t] += Change[s].offer_num[t];  player[pl1].resurs[t] -= Change[s].offer_num[t];
				    }

				Send_To_All_Player_CARDS(pl1);
				Send_To_All_Player_CARDS(pl2);
				
				//�������� ����� ������ ������� ������
				InitChange_BANK(pl1);	Send_To_All_Info_Change_Area(pl1);
				InitChange_BANK(pl2);   Send_To_All_Info_Change_Area(pl2);

				//�������� ��� ������ �������, ��� ������������ ���� ��� ������
				//!!!!!!!!!!!!!!!!!!!!!!!!!!

				//�������� ����������� ������ ��� �������
				Change[s].status = 0;
				Send_To_All_Info_Change(s);
				continue;
			}
			
		continue;
		}  //��������� ��������� , ���� � ������� ��� CONTINUE, �� ��� ����� ����������� ����


		//���� �������� ������ ����������� ���������
		//��������� �������� ��������� ���� ����� INDEX ���� ��� ���
		for (int i = 0; i < 4; i++)
		{
			if (i == index )    continue;     // � index  �� ����������
			if (Connections[i] == 0)    continue;
			//�������� �����������
			if (send(Connections[i], msg, sizeof(msg), NULL) == INVALID_SOCKET)
			{
				std::cout << "  ERROR SEND  " << std::endl;   //�������� ��������� ���� ����� ���� ������ ��� ����������� �������������� ����������� � �������
				closesocket(Connections[i]);	Connections[i] = 0;
			}

		}   //for
	}
}

//=======================================================================
//�������� ��������� ���� �������
//=======================================================================
void Send_To_All(char* msg,int size)
{
	//std::cout << "  To ALL - " <<  msg  << std::endl;
  for (int i = 0; i < 4; i++)
	{
	if (Connections[i] == 0)    continue;
	if (player[i + 1].active == 0) continue;     //���������� �� �������� �������
		
	if (send(Connections[i], msg, size, NULL) == INVALID_SOCKET)
		{
		std::cout << "  ERROR SEND  " << std::endl;   //�������� ��������� ���� ����� ���� ������ ��� ����������� �������������� ����������� � �������
		closesocket(Connections[i]);	Connections[i] = 0;
		}
	} 

return;
}

//=======================================================================
//�������� ��������� � �������� �� ������� ���� �������
//=======================================================================
void Send_To_All_Info_Resurs()
{
 char msg[256];

 //����� ���� ��������
 strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
 memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
 Send_To_All(msg,sizeof(msg));

 //����� ���� ���� ��������
 Send_To_All_Improve_CARDS();

 //������� ������� ������
 for (int i = 1; i < 5; i++)  Send_To_All_Player_CARDS(i);

 //����� ������ �������
 for (int i = 1; i < 5; i++)   Send_To_All_Info_Change_Area(i);

 return;
}

//=======================================================================
// ���� ������� ������ ������
//=======================================================================
void Send_To_All_Player_CARDS(int pl)
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
	_itoa(pl, &msg[50], 10);     //����� ������
	memcpy(&msg[54], player[pl].resurs, 10 * sizeof(int));   //��������
	Send_To_All(msg, sizeof(msg));
	return;
}

//=======================================================================
//�������� ��������� � ��������� ����� ������ ������
//=======================================================================
void Send_To_All_Info_Change_Area(int pl)
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_CHANGE_AREA].c_str());
	_itoa(pl, &msg[50], 10);                                 //����� ������
	memcpy(&msg[54], ChangeBANK[pl], 10 * sizeof(int));   //��������
	Send_To_All(msg, sizeof(msg));
}

//=======================================================================
//�������� ��������� � �������� ������ �������� ����
//=======================================================================
void Send_To_All_Info_Max_Way()
{
char msg[256];
	strcpy_s(msg, Map_Command[NET_COMMAND::MAX_WAY_OWNER].c_str());
	_itoa(max_road_owner, &msg[50], 10);
	Send_To_All(msg, sizeof(msg));
}

//=======================================================================
//�������� ��������� � �������� �� ������� ���� �������
// ��������� �� ���������� ������� ��������� 
//=======================================================================
void Send_To_All_Info_Nodes()
{
	char msg[256];

	int* intPtr;
	int i = 0;
	//������� �������� ������ ����� c ���������� �� ������
	strcpy_s(msg, Map_Command[NET_COMMAND::NODE_ARRAY].c_str());   //�������� �������, ��� ��� ����� ����� ��������� ������ � 50 ������
	intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //� 1 ������ ����� ���������� ����
	intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� 2 ������� �������   0, 1, 2
	for (size_t p = 0; p < nodePtr->size(); p++)
	{
		*intPtr++ = nodePtr->at(p).owner;
		*intPtr++ = nodePtr->at(p).object;
		i++;
		if (i == 20 || p == nodePtr->size() - 1)                //��� ����� ��������� 20 ����� - ���������� 
		{
			intPtr = (int*)&msg[50];	*intPtr = i;                                 //� 50 ������ ���������� �����           
			Send_To_All(msg, sizeof(msg));
			i = 0;
			intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = p + 1;   //� 1 ������ ����� ���������� ����
			intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� 2 ������� �������   0, 1, 2
		}
	}
	return;
}

//=======================================================================
//�������� ����� ���� �������� �� ������� ��������
//=======================================================================
void Send_To_All_Improve_CARDS()
{
	char msg[256];
	int* intPtr;

	strcpy_s(msg, Map_Command[NET_COMMAND::INFO_IMP_CARDS].c_str());
	intPtr = (int*)&msg[54];
	for (auto& elem : improve_CARDS)
	    {
		*intPtr++ = elem.status;
		*intPtr++ = (int)elem.type;
	    }
	_itoa(improve_CARDS.size(), &msg[50], 10);     //������ �������

	Send_To_All(msg, sizeof(msg));
	return;
}

//=======================================================================
//�������� ������� ���� �������� 1 ������ �� ������� ��������
//=======================================================================
void Send_To_All_Develop_CARDS(int pl)
{
	char msg[256];
	int* intPtr;
	
	strcpy_s(msg, Map_Command[NET_COMMAND::DEVELOP_VECTOR].c_str());
	intPtr = (int*)&msg[58];
	for(auto& elem : develop_CARDS[pl])
	    {
		*intPtr++ = elem.status;
		*intPtr++ = (int)elem.type;
	    }
	_itoa(develop_CARDS[pl].size(), &msg[54], 10);     //������ �������
	_itoa(pl, &msg[50], 10);     //����� ������

	Send_To_All(msg, sizeof(msg));
	return;
}

//=======================================================================
//�������� ������ �� ����� �� ������ ������ �������
//=======================================================================
void Send_To_All_Info_Change(int s)
{
	char msg[256];

	if (s < 0 || s >= 12)   {  Beep(900, 1000);  return; }
	
	strcpy_s(msg, Map_Command[NET_COMMAND::CHANGE_OFFER].c_str());
	_itoa(s, &msg[50], 10);                              //����� ������
	memcpy(&msg[54], &Change[s], sizeof(CHANGE));        //����������� ��������� ������
	Send_To_All(msg, sizeof(msg));
	return;
}

//============================================================================
 //   ������� ���� ������� ��� ����� ��������� ������ �������������
 //============================================================================
int ClientChart()
{
	char msg1[256];

	Sleep(30);   //������� �������� ������ ������� ������ 
	while (true)
	{
		std::cin.getline(msg1, sizeof(msg1));
		send(Connection, msg1, sizeof(msg1), NULL);
	}

	return 0;
}



