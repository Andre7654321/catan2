
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>

//#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "CatanNet.h"
#include "CatanField.h"
#include "Catan_Step.h"

#pragma warning(disable: 4996)

//������� ���������� ����������
extern int player_num;
extern int bandit_Gecs;

extern std::vector<GECS>* gecsPtr;    //��������� �� ������ ������
extern std::vector<NODE>* nodePtr;    //��������� �� ������ ����� ����
extern std::vector<ROAD>* roadPtr;    //��������� �� ������ �����

//��������� ���������� ����� �������� � ��� � ��������� �������
SOCKET Connection;   //����������� �������

//��� �������
SOCKET Connections[10] = { 0,0,0,0,0,0,0,0,0,0 };    //����������� ������� 
int Counter = 0;           //������� �����������

std::mutex mtx1;    //�������� ����� ��������� cout

extern PLAYER player[5];
extern GAME_STEP Game_Step;
extern int CARD_Bank[10];

extern std::string resurs_name[10];

//������� ������ ���������
extern int ChangeBANK[5][10];

//����������� ��������� ��� ������ ����������� ������� � ��������
std::string CATAN_command = "$CATAN$";
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
};

std::map<NET_COMMAND, std::string> Map_Command =
{
	{NET_COMMAND::SET_N_PLAYER,		    "$CATAN$ SET N Player"},
	{NET_COMMAND::SET_STEP    ,		    "$CATAN$ SET Step"},
	{NET_COMMAND::ADD_N_ACTIVE_PLAYER,	"$CATAN$ ADD N ACTIVE PLAYER"},
	{NET_COMMAND::SET_N_ACTIVE_PLAYER,	"$CATAN$ SET N ACTIVE PLAYER"},
	{NET_COMMAND::SET_FIRST_PLAYER,	    "$CATAN$ SET SET_FIRST_PLAYER"},
	{NET_COMMAND::ASK_SET_FIELD,	    "$CATAN$ ASK SET FIELD"},
	{NET_COMMAND::SET_GECS,	     	    "$CATAN$ SET GECS"},
	{NET_COMMAND::SAY_MOVE_OVER,	    "$CATAN$ SAY_MOVE_OVER"},
	{NET_COMMAND::ASK_ROLL_1DICE,	    "$CATAN$ ASK ROLL_1DICE"},
	{NET_COMMAND::ASK_ROLL_2DICE,	    "$CATAN$ ASK ROLL_2DICE"},
	{NET_COMMAND::SAY_GAME_START,	    "$CATAN$ SAY_GAME_START"},
	{NET_COMMAND::NODE_ARRAY,	        "$CATAN$ NODE_ARRAY"},
	{NET_COMMAND::ROAD_ARRAY,	        "$CATAN$ ROAD_ARRAY"},
	{NET_COMMAND::ASK_NODE_ARRAY,	    "$CATAN$ ASK_NODE_ARRAY"},
	{NET_COMMAND::ASK_ROAD_ARRAY,	    "$CATAN$ ASK_ROAD_ARRAY"},
	{NET_COMMAND::ASK_BANK_RESURS,	    "$CATAN$ ASK_BANK_RESURS"},
	{NET_COMMAND::ASK_PLAYER_RESURS,	"$CATAN$ ASK_PLAYER_RESURS"},
	{NET_COMMAND::ASK_PLAYER_OBJECTS,	"$CATAN$ ASK_PLAYER_OBJECTS"},
	{NET_COMMAND::PLAYER_OBJECTS,	    "$CATAN$ PLAYER_OBJECTS"},
	{NET_COMMAND::INFO_BANK_RESURS,	    "$CATAN$ INFO_BANK_RESURS"},
	{NET_COMMAND::LAST_VILLAGE,	        "$CATAN$ LAST_VILLAGE"},
	{NET_COMMAND::PLAYER_RESURS,	    "$CATAN$ PLAYER_RESURS"},
	{NET_COMMAND::DICE_SEVEN,	        "$CATAN$ DICE_SEVEN"},
	{NET_COMMAND::BANDIT_GECS,	        "$CATAN$ BANDIT_GECS"},
	{NET_COMMAND::ASK_CHANGE_BANK,	    "$CATAN$ ASK_CHANGE_BANK"},
};


//=======================================================
//  ������ ������ �������� � ������
//=======================================================
bool AskChangeWithBank(RESURS CARD_type)
{
char msg[256];
int i;

//��������� ���� �� � ����� ������ 4 ���������� ��������
for(i = 1;i < 10;i++)   { if (ChangeBANK[player_num][i] >= 4)  i = 20;  }
if (i < 20){	std::cout << " � ����� ��������� �������� ��� ������ " << std::endl;  return false;  }

//����� ���� �������� - ��������, ��� ��� ��� ���������� �� ������
strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
send(Connection, msg, sizeof(msg), NULL);

//�������  ������ - �������� ��� ��� ����� ���������� �� ������
strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
_itoa(player_num, &msg[50], 10);     //����� ������
memcpy(&msg[54], player[player_num].resurs, 10 * sizeof(int));   //��������
send(Connection, msg, sizeof(msg), NULL);

//��������� �� ������ ���� �������� ���� � ��������� �� ����� ������
strcpy_s(msg, Map_Command[NET_COMMAND::ASK_CHANGE_BANK].c_str());
itoa(player_num, &msg[50], 10);
itoa((int)CARD_type, &msg[54], 10);
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
int i, j;

    for (i = 0; i < 5; i++)
      {
	  for (j = 0; j < 10; j++)  ChangeBANK[i][j] = 0;
      }

	return;
}

//========================================================
// �������� ���� �� �������
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
	for (int p = 0; p < roadPtr->size(); p++)
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
// �������� ���� �� �����
//========================================================
void Send_nodes()
{
char msg[256];
int* intPtr;
int i = 0;

	//���� �� ����� ������
    strcpy_s(msg, Map_Command[NET_COMMAND::NODE_ARRAY].c_str());   //�������� �������, ��� ��� ����� ����� ��������� ������ � 50 ������
    intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //� 1 ������ ����� ���������� ���� � ������������ ������
	intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� 2 ������� �������   0, 1, 2
    for (int p = 0; p < nodePtr->size(); p++)
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
			intPtr = (int*)&msg[50];    intPtr += 2;                 //������ �� 2 ������� �������   0, 1, 2
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
	send(Connection, msg, sizeof(msg), NULL);

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ROAD_ARRAY].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);
	Sleep(10);
}

//========================================================
// �������� ��������� ��� ��� ��������
//========================================================
void Say_Move_Over()
{
	char msg[256];

	std::cout << " ��� �������� " << std::endl;

	//����� ������������� �������� ���� �� ������� � �����
	Send_nodes();	
	Send_roads();

	//����� ������������� �������� ���� �� ����� �������� � ����� �������� �� ������
	//����� ���� ��������
	strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
	memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
	send(Connection, msg, sizeof(msg), NULL);

	//�������  ������
    strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
	_itoa(player_num, &msg[50], 10);     //����� ������
	memcpy(&msg[54], player[player_num].resurs, 10 * sizeof(int));   //��������
	send(Connection, msg, sizeof(msg), NULL);

	//�������, ������ .. ������
	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_OBJECTS].c_str());
	_itoa(player_num, &msg[50], 10);                                 //����� ������
	_itoa(player[player_num].town, &msg[60], 10);                    //������
	_itoa(player[player_num].village, &msg[70], 10);                 //�������
	_itoa(player[player_num].road, &msg[80], 10);                    //������
	send(Connection, msg, sizeof(msg), NULL);


	//�� 3 ���� ��������� ����� ���� ������������ ��������� �������
	if (Game_Step.current_step == 3)  
	   {
		std::cout << " Send Last village =  " << player[player_num].last_village_node  << std::endl;

		strcpy_s(msg, Map_Command[NET_COMMAND::LAST_VILLAGE].c_str());
		itoa(player[player_num].last_village_node, &msg[50], 10);
		send(Connection, msg, sizeof(msg), NULL);
	   }

	strcpy_s(msg, Map_Command[NET_COMMAND::SAY_MOVE_OVER].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);
	Sleep(10);

	return;
}

//========================================================
// �������� ��������� ��� �����  1 �����
//========================================================
void Say_Roll_2Dice()
{
	char msg[256];

	std::cout << " �� �������  ������ " << std::endl;

	// ��������� � ������ ������
	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ROLL_2DICE].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);
	Sleep(10);

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
	Sleep(10);

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
	send(Connection, msg, sizeof(msg), NULL);
	Sleep(10);

return;
}

//========================================================
// �������� ��������� ������� � ����� �����������
//========================================================
void Say_Move_Banditos()
{
 char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::BANDIT_GECS].c_str());
	_itoa(bandit_Gecs, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);
	Sleep(10);

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

//===============================================================================================  CLIENT NET
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
	NET_COMMAND Command = NET_COMMAND::EMPTY;


	while (true)            //����������� ���� � ������ ��������� ������� ���������
	{
		ret = recv(Connection, msg, sizeof(msg), NULL);
		if (ret == SOCKET_ERROR)
		    {
			//�� ������� ���� � ������� � ����������� ����� ��������� ���������
			std::cout << " SOCKET_ERROR , Close SOCKET " << std::endl;
			closesocket(Connection);
			Connection = 0;
			return;
		    }
		if (ret == 0) { ret = 1; std::cout << " 0 byte recieved " << std::endl; }



		if (Connection != 0)
		    {
			str.assign(msg, 0, 7);    //����� ������ 7 �������� ���������

			 //���� ��������� ��������� $CATAN&
			if (str.compare(CATAN_command) == 0)   //��������� �� ������� $CATAN&
			    {
				str.assign(msg, 0, strlen(msg));
				//std::cout << " Get CATAN COMMAND  " << std::endl;

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
					continue;
				    }

				if (Command == NET_COMMAND::SET_N_ACTIVE_PLAYER)
				{
					if(Game_Step.current_step == 4)  Game_Step.step[4].roll_2_dice = 1;
					Game_Step.current_active_player = atoi(&msg[50]);
					InitChange_BANK();    //� ������ ���� ���� ������ ����������
					continue;
				}

				if (Command == NET_COMMAND::SET_GECS)
					{
					int gecs_num = atoi(&msg[50]);
					//std::cout << "������� ���� �����:  " << gecs_num  << std::endl;

					auto it = gecsPtr->begin();
					//�������� ���������� ������ �� ������ �����
					for (i = 0, ptr = (char*)(&*(it+ gecs_num)); i < sizeof(GECS); i++)   *ptr++ = msg[60+i];

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
						   std::cout << " ������ 7 , ����������� ����������� " << std::endl;
						   Game_Step.step[4].flag_bandit = 1;
					       }
					else
					   {
						if ((CARD_Bank[(int)RESURS::WOOD] + CARD_Bank[(int)RESURS::BREAD] + CARD_Bank[(int)RESURS::STONE] + 
							      CARD_Bank[(int)RESURS::BRICKS] + CARD_Bank[(int)RESURS::OVCA]) >= 8)
						    {
							std::cout << " ������ 7 , ���� �������� �������� �������� " << std::endl;
						    }
					   }
					continue;
				    }

				if (Command == NET_COMMAND::BANDIT_GECS)
				{
					bandit_Gecs = atoi(&msg[50]);
					continue;
				}
				
			     continue;
			    }

			mtx1.lock();
			//std::cout << " CLIENT: " << msg << "  Connection = " << Connection << std::endl;    //���� ����� �� ������
			std::cout << " CLIENT: " << msg  << std::endl;
			mtx1.unlock();
			}  //if (Connection != 0)
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
	addr.sin_addr.s_addr = inet_addr("192.168.1.118");    //------------------------------------------------------------------
	addr.sin_port = htons(1111);
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
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_addr.s_addr = inet_addr("192.168.1.118");    //--------------------------------------------------------
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;      //�������� ��������

	//��������� �����
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	char msg[256] = "";     //����� ������������ �������������� �������� 256
	char str[40];

	std::cout << " ======  SERVER: ����� ����� �������� ����������� ====== \n  " << std::endl;

	for (i = 0; i < 4; i++)
	{
		std::cout << " SERVER: Wait for next player \n  " << std::endl;
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
		if (newConnection == 0)  {	std::cout << "ERROR newConnection" << std::endl;  }
		else
		{
			mtx1.lock();
			std::cout << "SERVER:  OK Player  N= " << (Counter+1) << "   Connected" << std::endl;
			mtx1.unlock();

			//�������� ��������� ��������� �������������� �������, ������ ������ ������
			strcpy_s(msg, "Hello. It's CATAN GAME ");
			send(newConnection, msg, sizeof(msg), NULL);
			Sleep(10);

			// ������ ������ ������
			strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_PLAYER].c_str());
			_itoa(Counter + 1, &msg[50], 10);
			send(newConnection, msg, sizeof(msg), NULL);
			_itoa(i, msg, 10);  strcat_s(msg,"  -   ����� ���������� ");
			send(newConnection, msg, sizeof(msg), NULL);

			//�������� ��������������� ���� - ����� � ������ - ��������� ������ ����������
			int gecs_num = 0;
			for (auto& elem : *gecsPtr)
			   {
			   strcpy_s(msg, Map_Command[NET_COMMAND::SET_GECS].c_str());
			   //���� �������� � ������ ������ �����
			   _itoa(gecs_num++, &msg[50], 10);       //�� 50 ������� ����� �����
			   memcpy(&msg[60], &elem, sizeof(GECS));
			   send(newConnection, msg, sizeof(msg), NULL);
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
				   strcpy_s(msg, Map_Command[NET_COMMAND::ADD_N_ACTIVE_PLAYER].c_str());   _itoa(ii+1, &msg[50], 10);
				   send(newConnection, msg, sizeof(msg), NULL);
			      }
			   }

			//��������� �������� ������� � ����������� ������
			for(int ii = 0;ii < 10;ii++)
			  {
				if (Connections[ii])
				{
					strcpy_s(msg, Map_Command[NET_COMMAND::ADD_N_ACTIVE_PLAYER].c_str());	_itoa(Counter + 1, &msg[50], 10);
					send(Connections[ii], msg, sizeof(msg), NULL);
				}
			  }

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
	char msg[256];   //-------------------------------------
	char msg1[10];
	int ret;
	std::string str;
	NET_COMMAND Command;
	bool system = false;

	//����������� ����� � ������ 20��� ���������� �������� ��������� ���������� �����
	//��� ����� �������� �� ������������������ ���� �� �����

	while (true)
	{
		//������ �� ������  ��� ��� ������� ������������ 1 �����, �� ����� � �������� ����������� ������ � ������
		//������� ������ ��� ��� ������ �� RECV ������������� �� ��� ���������.
		//��� ���� ������ ������� ���������������� ������������ �������� � �� ������ ���������������
		ret = recv(Connections[index], msg, sizeof(msg), NULL);
		if (ret == INVALID_SOCKET)
		    {
			std::cout << "   ERROR READING SOCKET   " << index << std::endl;
			Connections[index] = 0; 	closesocket(Connections[index]);	return;
		    }

		//������� �� ��������� ��������� CATAN ==================================================================
		str.assign(msg, 0, 7);    //����� ������ 7 �������� ���������

	    //���� ��������� ��������� $CATAN&
		if (str.compare(CATAN_command) == 0)   //��������� �� ������� $CATAN&
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

				strcpy_s(msg, " ================================ ���� ���������� ===================================");
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
			int* intPtr;
			int i=0,p;
				//������� �������� ������ �����
				//���� �� ����� ������
				strcpy_s(msg, Map_Command[NET_COMMAND::NODE_ARRAY].c_str());   //�������� �������, ��� ��� ����� ����� ��������� ������ � 50 ������
				intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //� 1 ������ ����� ���������� ����
				intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� 2 ������� �������   0, 1, 2
				for (int p = 0; p < nodePtr->size(); p++)
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
			continue;
			}

			if (Command == NET_COMMAND::ASK_ROAD_ARRAY)
			{
				int* intPtr;
				int i = 0, p;
				//������� �������� ��������� �����
				strcpy_s(msg, Map_Command[NET_COMMAND::ROAD_ARRAY].c_str());   //�������� �������, ��� ��� ����� ����� ��������� ������ � 50 ������
				intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //� 1 ������ ����� ���������� ����
				intPtr = (int*)&msg[50];    intPtr += 2;              //������ �� 2 ������� �������   0, 1, 2
				for (int p = 0; p < roadPtr->size(); p++)
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
				int pl = atoi(&msg[50]);
				//std::cout << " ���������� ���� �����  " << pl << std::endl;
				if ( Game_Step.current_step == 4)
				    {
					Game_Step.current_active_player = GetNextPlayer();
					std::cout << "SERVER  �������� ���� ������  " << Game_Step.current_active_player << std::endl;
					strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	_itoa(Game_Step.current_active_player, &msg[50], 10);
					Send_To_All(msg, sizeof(msg));
					InitChange_BANK();
					continue;
				    }
				if (Game_Step.current_step == 3)   Get_Resurs(player[pl].last_village_node,pl);    //������� ������� ��� �������
				Send_To_All_Info_Resurs();
				if (Game_Step.current_active_player == Game_Step.start_player && Game_Step.current_step == 3)
				      {
					  Game_Step.current_step = 4;     //����� ����������� ����� ����
					  std::cout << " ================= ������� �� �������� ����  =====================  " << pl << std::endl;
					  //�������� ���� ����� ����
					  strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	_itoa(Game_Step.current_step, &msg[50], 10);
					  Send_To_All(msg, sizeof(msg));
					  continue;
				      }
				if (GetNextPlayer() == Game_Step.start_player && Game_Step.current_step == 2)
				{
					std::cout << " ================= ������� �� 3 ��� =====================  " << player << std::endl;
					//�� ���� ���
					if (Game_Step.current_step == 2)   Game_Step.current_step = 3;

					//�������� ���� ����� ����
					strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	_itoa(Game_Step.current_step, &msg[50], 10);
					Send_To_All(msg, sizeof(msg));
				}
				  else
				    { 
					if(Game_Step.current_step == 2)  Game_Step.current_active_player = GetNextPlayer();
					if(Game_Step.current_step == 3)  Game_Step.current_active_player = GetPrevPlayer();
					mtx1.lock();  std::cout << "SERVER  �������� ���� ������  " << Game_Step.current_active_player << std::endl;  mtx1.unlock();
					//�������� ���� ��� ��� ������� ������ � 
					strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	_itoa(Game_Step.current_active_player, &msg[50], 10);
					Send_To_All(msg, sizeof(msg));
				    }

			 continue;
			}

			if (Command == NET_COMMAND::ASK_ROLL_1DICE)
			{
			int pl = atoi(&msg[50]);

			srand(time(0));
			player[pl].first_roll = rand() % 6 + 1;

			//�������� ���� ��������� ������ ������ - �� ��������� ������� ����������
			strcpy(msg, "Player "); _itoa(pl, msg1, 10); strcat(msg, msg1); 
			strcat(msg, " roll  -   ");  _itoa(player[pl].first_roll, msg1, 10);   strcat(msg,msg1);
			Send_To_All(msg, sizeof(msg));

            //���� �� ��� ������� ����� �������� ��� ���� ������, ���� ��� �� ���� ��� ����
			if (GetNextPlayer() == Game_Step.start_player)
			{
				//mtx1.lock();  std::cout << "GetNextPlayer() == 0  ������� ��� ��� ������ " << std::endl; mtx1.unlock();
				SetFirstPlayer();    //��������� �� ����������� ������ 1 ������ ����������� ����

				//�������� �������� � ���������� ������� ������
				strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	_itoa(Game_Step.current_active_player, &msg[50], 10);
				Send_To_All(msg, sizeof(msg));
			}
			else  Game_Step.current_active_player = GetNextPlayer();

			strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	_itoa(Game_Step.current_step, &msg[50], 10);
			Send_To_All(msg, sizeof(msg));
		
			mtx1.lock();  std::cout << "SERVER  ������� ��� ������  " << Game_Step.current_active_player << std::endl;  mtx1.unlock();
			//�������� ���� ��� ��� ������� ������ � ��� ������ ������
			strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	_itoa(Game_Step.current_active_player, &msg[50], 10);
			Send_To_All(msg, sizeof(msg));    
			continue;
			}

			if (Command == NET_COMMAND::ASK_ROLL_2DICE)
			{
				int pl = atoi(&msg[50]);

				//srand(time(0));
				int dice2 = rand() % 6 + 1  + rand() % 6 + 1;   //2 ������

				//�������� ���� ��������� ������ ������ - �� ��������� ������� ���������� 
				std::cout << " ������ ������� =  " <<  dice2  << std::endl;
				strcpy(msg, "Player "); _itoa(pl, msg1, 10); strcat(msg, msg1);
				strcat(msg, " roll  -   ");  _itoa(dice2, msg1, 10);   strcat(msg, msg1);
				Send_To_All(msg, sizeof(msg));

				if(dice2 == 7)
				    {
					strcpy_s(msg, Map_Command[NET_COMMAND::DICE_SEVEN].c_str());	_itoa(pl, &msg[50], 10);
					Send_To_All(msg, sizeof(msg));
					continue;
				    }

				//������������ ������� �� ����������� ������ - ���� �� ����� ������
				Step_Resurs(dice2);
				Send_To_All_Info_Resurs();
				continue;
			}

			if (Command == NET_COMMAND::INFO_BANK_RESURS)  //������� �������� - �� ��� ���������
			{
				int* intPtr = (int*)&msg[50];
				memcpy(CARD_Bank, &msg[50], 10 * sizeof(int));
				Send_To_All(msg, sizeof(msg));
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
				send(Connections[pl - 1], msg, sizeof(msg), NULL);
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
				Send_To_All(msg, sizeof(msg));
				continue;
			}

			if (Command == NET_COMMAND::BANDIT_GECS)
			{
				bandit_Gecs = atoi(&msg[50]);
				Send_To_All(msg, sizeof(msg));
				continue;
			}

			if (Command == NET_COMMAND::ASK_CHANGE_BANK)
			{
				InitChange_BANK();
				int pl = atoi(&msg[50]);
				int type_put = atoi(&msg[54]);
				memcpy(&ChangeBANK[pl][0],&msg[60], sizeof(int) * 10);

				std::cout << " ������ �� get " << resurs_name[type_put] << " from pl " << pl  <<  std::endl;

				//����� 4 �������� ������ ����, ������� �� �� ����� ������
				int type_get = 0;
				for(int i = 1;i < 10;i++)
				   {
					if (ChangeBANK[pl][i] >= 4) { type_get = i; break; }
				   }

				if (type_get == 0)
				    {
					std::cout << " �� ������� 4 ���� "  << std::endl;
					continue;     //���� ��� ������ �� �������
				    }
				//��������� ���� ������������� ������ � ����� �����
				if (CARD_Bank[type_put] < 1)    continue;

				//��������� ����� � ������
				player[pl].resurs[type_put] += 1;   CARD_Bank[type_put] -= 1;
				player[pl].resurs[type_get] -= 4;   CARD_Bank[type_get] += 4;

				Send_To_All_Info_Resurs();

				continue;
			}

		continue;
		}  //��������� ��������� , ���� � ������� ��� CONTINUE, �� ��� ����� ����������� ����


		//��������� �������� ��������� ���� ����� INDEX ���� ��� ���
		for (int i = 0; i < 5; i++)
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
	for (int i = 0; i < 5; i++)
	{
	if (Connections[i] == 0)    continue;
		
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

 //������� ������� ������
 for (int i = 1; i < 5; i++)
    {
	 strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
	 _itoa(i, &msg[50], 10);     //����� ������
	 memcpy(&msg[54],player[i].resurs, 10 * sizeof(int));   //��������
	 Send_To_All(msg, sizeof(msg));
    }

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
		Sleep(10);
	}

	return 0;
}



