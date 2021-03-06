#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <algorithm>

#include "CatanNet.h"
#include "Cat_NET.h"
#include "CatanField.h"
#include "Catan_Step.h"
#include "Catan_Count.h"

#pragma warning(disable: 4996)

//int tmp_count = 0;
extern int Game_type;     //1 - Standart CATAN   2 - FISH

//��������� ���������� ����� �������� � ��� � ��������� �������
SOCKET Connection;   //����������� �������
char addr_listen[20] = "192.168.2.39";
char addr_UDP[20] = " ";
char addr_connect[20] = "192.168.2.39";

//��� �������
SOCKET Connections[10] = { 0 };    //6 + server ����������� �������
char str_Player_addr[7][20];

std::mutex mtx1;    //�������� ����� ��������� cout

std::string CATAN_command = "zCATANz";
char SERVER_command[12] = "zCATSERVERz";
//map ����� �� ���������� ������� ������� �������
std::map<NET_COMMAND, std::string> Map_Command =
{
	{NET_COMMAND::SET_N_PLAYER,		    "zCATANz SET N Player"},
	{NET_COMMAND::SET_STEP    ,		    "zCATANz SET Step"},
	{NET_COMMAND::INFO_N_ACTIVE_PLAYER,	"zCATANz INFO_N_ACTIVE_PLAYER"},
	{NET_COMMAND::SET_N_ACTIVE_PLAYER,	"zCATANz SET N ACTIVE PLAYER"},
	{NET_COMMAND::SET_FIRST_PLAYER,	    "zCATANz SET SET_FIRST_PLAYER"},
	{NET_COMMAND::ASK_SET_FIELD,	    "zCATANz ASK SET FIELD"},
	{NET_COMMAND::SET_GECS,	     	    "zCATANz SET GECS"},
	{NET_COMMAND::SAY_MOVE_OVER,	    "zCATANz SAY_MOVE_OVER"},
	{NET_COMMAND::SAY_ROLL_START_DICE,	"zCATANz SAY_ROLL_START_DICE"},
	{NET_COMMAND::ASK_ROLL_2DICE,	    "zCATANz ASK ROLL_2DICE"},
	{NET_COMMAND::SAY_GAME_START,	    "zCATANz SAY_GAME_START"},
	{NET_COMMAND::NODE_ARRAY,	        "zCATANz NODE_ARRAY"},
	{NET_COMMAND::NODE_ARRAY_ZIP,	    "zCATANz NODE_ARRAY_ZIP"},
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
	{NET_COMMAND::ASK_CHANGE_WITH_BANK,	    "zCATANz ASK_CHANGE_WITH_BANK"},
	{NET_COMMAND::PLAYER_CHANGE_AREA,	"zCATANz PLAYER_CHANGE_AREA"},
	{NET_COMMAND::MAX_WAY_ARMY_OWNER,	"zCATANz MAX_WAY_ARMY_OWNER"},
	{NET_COMMAND::SEND_CARDS_TO_BANK,	"zCATANz SEND_CARDS_TO_BANK"},
	{NET_COMMAND::INFO_CHANGE_BANK,	    "zCATANz INFO_CHANGE_BANK"},
	{NET_COMMAND::TAKE_CARD_FROM_PLAYER,"zCATANz TAKE_CARD_FROM_PLAYER"},
	{NET_COMMAND::INFO_TAKE_CARD_FROM_PLAYER,"zCATANz INFO_TAKE_CARD_FROM_PLAYER"},
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
	{NET_COMMAND::ASK_ACCEPT_OFFER,    "zCATANz ASK_ACCEPT_OFFER"},
	{NET_COMMAND::ASK_RESET_GAME,      "zCATANz ASK_RESET_GAME"},
	{NET_COMMAND::TEST_GAME,           "zCATANz TEST_GAME"},
	{NET_COMMAND::WAIT_RECONNECT,      "zCATANz WAIT_RECONNECT"},
	{NET_COMMAND::SET_CLOSE,           "zCATANz SET_CLOSE"},
	{NET_COMMAND::GAME_TYPE,           "zCATANz GAME_TYPE"},
	{NET_COMMAND::INFO_FISH_CARDS,     "zCATANz INFO_FISH_CARDS"},
	{NET_COMMAND::GAME_OVER,           "zCATANz GAME_OVER"},
	
	
};


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

extern bool flag_get_cubic_result;
extern bool flag_cubic_was_used;   //������ ���������� ��� ������� ����� ��� ������ �����
extern bool flag_develop_card_used;  //������ ���������� ��� ����� �������� ��� �������� � ���� ����

extern std::string resurs_name[12];
extern int Big_Message;              //����� ������� ��� ������ ���������
extern std::chrono::time_point<std::chrono::steady_clock>  start_Big_Message;
extern char BuffBigMessage[256];

//������� ������ ���������
extern int ChangeBANK[7][12];
extern int game_wait_reconnect;

//===============================================================================================
// �������������� ���������� � �������� ������� � ����
// 
//===============================================================================================
bool Try_Reconnect(int pl,int newConnection,const char* addr)
{
	char msg[256];

	//�������� ����� ����������� � ������������
	if (strcmp(addr, str_Player_addr[pl]) == 0)
	{
		std::cout << " ������������������ ����� -  " << pl << std::endl;

		strcpy_s(msg, "CATAN GAME in PROGRESS, TRY to RECONNECT PLAYER");
		send(newConnection, msg, sizeof(msg), NULL);

		//�������� ����������������� ��������� ����
		Connections[pl-1] = newConnection;
		player[pl].active = true;
		player[pl].wait = false;

		Set_Player_Number(pl);
		Send_Info_Game_Type(pl);
		Send_Bank_Resurs(pl);       //���� ��� �������
		Set_Game_Step(pl);
		Send_Player_CARDS(pl, pl);
		Send_Player_Objects(pl, pl);
		Send_Info_Nodes(pl);	Send_Info_Roads(pl);
		Send_Develop_CARDS(pl, pl);
		Send_To_All_Info_MaxWayArmy(pl);
		for (int ii = 1; ii < 7; ii++)  Send_Player_CARDS(pl, ii);
		//���� �������� ����� �������� � ���� � ������ ������ � ���� ����� ��������
		Set_New_Move(pl);
		return true;
	}

return false;
}

//===============================================================================================
// ��������� ������ ����� � �������
// 
//===============================================================================================
int Player_Disconnect(int index)
{
	Connections[index] = 0;

	std::cout << " ===================== DISCONNECT Player ========================" << std::endl;
	//���� �� �������� ������������
	if (Connections[0] == 0 && Connections[1] == 0 && Connections[2] == 0 && Connections[3] == 0)
	    {
		std::cout << "�� �������� ������������ �������, set 1 STEP " << std::endl;
		for (int pl = 1; pl < 7; pl++) 
		    {
			player[pl].active = 0;  player[pl].wait = false;
		    }
		Game_Step.current_step = 0;
		Game_Step.current_active_player = 1;
		Game_Step.start_player = 1;
		roadPtr->clear();	nodePtr->clear();	gecsPtr->clear();
		Init_CATAN_Field(gecsPtr, nodePtr, roadPtr);
	    }
	std::cout << " == ����� ������     : " << (index + 1) << std::endl;
	std::cout << " == Player IP adress : " << str_Player_addr[index+1] << std::endl;
	if (Game_Step.current_step > 0)
	std::cout << " == ���� ��������� � �������� ������������� �����              ==" << std::endl;
	std::cout << " ================================================================"  << std::endl;
	
	closesocket(Connections[index]);
	int pl = index + 1;
	player[pl].active = false;
	if (Game_Step.current_step > 0)   player[pl].wait = true;    //������ � ����� �������� ��������

	CATAN_SERVER_Command(NET_COMMAND::WAIT_RECONNECT,(char*)&pl,sizeof(int),TO_ALL);

	return 0;
}

//===============================================================================================
// ���������� ������� ��������� CATAN ��������� ����� ������
// �������� �� ������
// ��� ��� �� ������� ������ ���� ���������� ������, �� ����� ������ �������� �� ����
//===============================================================================================
int CATAN_CLIENT_Command(NET_COMMAND Net_Command, const char* buf, int len)
{
	int num;
	char title[16];

	if(player_num == 0)
	    {
		std::cout << " ERROR CATAN_CLIENT_Command called from server part" << Map_Command[Net_Command] << std::endl;
		return 0;
	    }

	if (Connection == 0)  return -2;

		strcpy_s(title, SERVER_command);               //������ 12 �������� (� 0 ������) ��� �������
		*(int*)&title[12] = (int)Net_Command;          //4 byte - ��� �������
		*(int*)&title[16] = len;                       //4 byte - ����� �������� ���������

		//���������
		int packet = 0;
		while (packet < 20)
		    {
			num = send(Connection, &title[packet], 20 - packet, NULL);
			packet += num;
		    }

		if (len == 0)   return 0;

		//������
		packet = 0;
		while (packet < len)
		   {
			num = send(Connection, &buf[packet], len - packet, NULL);
			if (num == INVALID_SOCKET) { return(-3); }
			packet += num;
		   }

		//if (Net_Command == NET_COMMAND::ROAD_ARRAY)		std::cout << " CLIENT_Command have send roads / len data = " << len << std::endl;

	return 0;
}

//===============================================================================================
// ���������� ������� ��������� CATAN ��������� ����� ������
// pl = 0 || TO_ALL  - �������� ����
// pl > 0            - player num 
//===============================================================================================
int CATAN_SERVER_Command(NET_COMMAND Net_Command, const char* buf, int len, int pl)
{
	int num;
	char title[16];

	if (pl < 0 || pl > 10)   return -1;  //������� �������� ������ ������ 9-10 ������������ ��� �������� ������ �����������

	if (pl > 0)
	{
		if (Connections[pl - 1] == 0)  return -2;

		strcpy_s(title, SERVER_command);               //������ 12 �������� (� 0 ������) ��� �������
		*(int*)&title[12] = (int)Net_Command;          //4 byte - ��� �������
		*(int*)&title[16] = len;                       //4 byte - ����� �������� ���������

		int packet = 0;
		while (packet < 20)
		{
			num = send(Connections[pl - 1], &title[packet], 20 - packet, NULL);
			packet += num;
		}

		packet = 0;
		while (packet < len)
		{
			num = send(Connections[pl - 1], &buf[packet], len - packet, NULL);
			if (num == INVALID_SOCKET) {     return(-3);  }
			packet += num;
		}
	}

	if (pl == 0)
	{
		//����������� ����� ��� ���� �������
		for (int i = 1; i < 7; i++)  CATAN_SERVER_Command(Net_Command, buf, len, i);
	}

	return 0;
}

//===============================================================================================
// ���������� ������� ��������� CATAN ��������� ����� ������
// pl = 0 || TO_ALL  - �������� ����
// pl > 0            - player num 
//===============================================================================================
int CATAN_Message(const char* buf, int len, int pl)
{
	int num;
	char title[16];

	if (pl < 0 || pl >= 7)  return -1;  //������� �������� ������ ������

	if (pl > 0)
	{
		if (Connections[pl - 1] == 0)  return -2;
		strcpy_s(title, SERVER_command);                //������ 12 �������� (� 0 ������) ��� �������
		*(int*)&title[12] = (int)NET_COMMAND::EMPTY;          //4 byte - ��� �������
		*(int*)&title[16] = len;                       //4 byte - ����� �������� ���������

		int packet = 0;
		while (packet < 20)
		   {	num = send(Connections[pl - 1], &title[packet], 20 - packet, NULL);		packet += num;	}

		packet = 0;
		while (packet < len)
		{
			num = send(Connections[pl - 1], &buf[packet], len - packet, NULL);
			if (num == INVALID_SOCKET) { return -3; }
			packet += num;
		}
	}

	if (pl == 0)
	{
	 //����������� ����� ��� ���� �������
		for (int i = 1; i < 7; i++)  CATAN_Message(buf, len, i);
	}
return 0;
}

//===============================================================================================
// �������� ������ �� ������������ ������� �������
//===============================================================================================
bool If_SERVER_Command(char* buf,int* Command)
{
	if (strcmp(buf, SERVER_command) == 0)
	    {
		*Command = *(int*)&buf[12];
		return true;
	    }

return false;
}

//===============================================================================================
//  ������ ������
// ������ ������ ����������� �������� ��������� CATAN ��������� ����� ������ �� �������
//===============================================================================================
int Read_SERVER_Data(int len,char* buf)
{
	int ret;
	int pack_size = 0;

	while (pack_size < len)
	   { 
		ret = recv(Connection, &buf[pack_size], len - pack_size, NULL);
		//if (ret == INVALID_SOCKET) { Server_Disconnect(); return -3; }
		pack_size += ret;
	   }

return 1;
}

//===============================================================================================
//  ������ ������
// ������ ������ ����������� �������� ��������� CATAN ��������� ����� ������ �� �������
//===============================================================================================
int Read_CLIENT_Data(int len, char* buf, int index)
{
	int ret;
	int pack_size = 0;

	while (pack_size < len)
	{
		ret = recv(Connections[index], &buf[pack_size], len - pack_size, NULL);
		    if (ret == SOCKET_ERROR) { Player_Disconnect(index);	return -1; }
		pack_size += ret;
	}

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
					
//===============================================================================================
// ������������� ������� ���� 
//===============================================================================================       START SERVER
int Start_Server_CATAN()
{
	init_WSA();

	//������� ��������� �����, ��� �������������� ����������� ������� �� TCP
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Check_Connections, NULL, NULL, NULL);

	//������� ��������� ����� ���  UDP �������, ��� ��������� ����������������� ������ �� �������
	//� �������� ��� ����� �������
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Check_UDP, NULL, NULL, NULL);

	std::cout << " ======== START SERVER OVER =========== " << std::endl;
	Sleep(50);
	return 1;
}

//=============================================================================
// ������� ������ UDP Server 
// ������� Datagram ��������� ����� � � ����� �� ��������������� ������ �������
// �������� ��������������� ������� � ����
//=============================================================================
void Check_UDP()
{
	SOCKET SendRecvSocket;  // ����� ��� ������ � ��������
	sockaddr_in ServerAddr, ClientAddr;  // ��� ����� ����� ������� � ��������
	int err, maxlen = 512, ClientAddrSize = sizeof(ClientAddr);  // ��� ������, ������ ������� � ������ ��������� ������
	char* recvbuf = new char[maxlen];  // ����� ������
	char* result_string = new char[maxlen];  // ����� ��������

	Sleep(100);
	std::cout << " ========== SERVER ADDR == " << addr_listen << std::endl;

	// Create a SOCKET for connecting to server
	SendRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendRecvSocket == INVALID_SOCKET)
	{
		std::cout << " error socket " << std::endl;
		exit(1);
	}

	// Setup the TCP listening socket
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(addr_listen);
	//����������� �������������� ���������� ������
	// inet_pton(AF_INET, �192.0.2.1�, &(ServerAddr.sin_addr));          IPv4
	// inet_pton(AF_INET6, �2001:db8:63b3 : 1::3490�, &(sa6.sin6_addr));  IPv6
	// �������� �������������� � ������ ������ -   inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
	// ��� ip4[INET_ADDRSTRLEN]; - ������ ��� ���������� ������

	ServerAddr.sin_port = htons(12345);
	err = bind(SendRecvSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if (err == SOCKET_ERROR)
	{
		printf("UDP socket bind failed: %d\n", WSAGetLastError());
		closesocket(SendRecvSocket);
		//WSACleanup();
		return;
	}
	else  printf("UDP socket bind OK\n");

	while (true)
	{
		// Accept a client socket
		err = recvfrom(SendRecvSocket, recvbuf, maxlen, 0, (sockaddr*)&ClientAddr, &ClientAddrSize);
		if (err > 0)
		{
			recvbuf[err] = 0;
			printf("Received query: %s\n", (char*)recvbuf);


			//����� ��������� ��������� --------------------------------------------------------------

			//snprintf_s(	char* buffer, size_t sizeOfBuffer, size_t count, const char* format[,argument] ...
			//  buffer	����� �������� �������� ������.
			//  sizeOfBuffer	������ ����� �������� �������� ������.������ � ������
			//  count			������������ ����� �������� ��� ��������
			//format			������ ���������� ��������.
			// ���������� ���������� ��������, ���������� � ������, ��� ����� ������������ ���� �������.

			//��������� ������ ������
			//_snprintf_s(result_string, maxlen, maxlen, "OK %d\n", result);

			//���������� ������ ����� ������� ����������� ������ � ������� � �������� �����
			strcpy(result_string, addr_listen);   //���������� � ������������ ������ ����� ������� � ��������� ����                    //

			// ���������� ���������  ������������ ����� �������
			sendto(SendRecvSocket, result_string, strlen(result_string), 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));
			printf("UDP Sent answer: %s\n", result_string);   //���������� ����� ������ �� ������ �� ����� �������
		}
		else {
			printf("UDP recv failed: %d\n", WSAGetLastError());
			closesocket(SendRecvSocket);
			//WSACleanup();
			return;
		}

	}  //while

}

//==============================================================================================        CONNECT
//     ������� ������ ����������� ��������� ����� =
//==============================================================================================
void Check_Connections(void)
{
	int num;
	int pl;
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	//addr.sin_addr.s_addr = inet_addr(addr_listen);      //����� ��������� ����
	addr.sin_port = htons(MY_PORT);
	addr.sin_family = AF_INET;      //�������� ��������
	bool disconnect_flag;


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

	//��������� ����� ������ ����� ���� 192.168.YYY.XXX  !!!  ???
	addr.sin_addr.s_addr = inet_addr(addr_listen);
	std::cout << "Listen Address =   " << addr_listen << std::endl;

	//��������� �����
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);   //create sockrt
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));         // ����������� ����� � ������
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	char msg[256] = "";     //����� ������������ �������������� �������� 256	

	std::cout << "\n ======  SERVER: ����� ����� �������� ����������� ====== \n  " << std::endl;

	//���� ����������� �������������� ������� ========================================================= CONNECT
	while (true)
	{
		std::cout << " SERVER: Wait for getting DATA \n  " << std::endl;
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);     //�� ���������� ���������� ���� �� �������� ������� �� �����������
		if (newConnection == 0) { std::cout << "ERROR newConnection" << std::endl; 	continue; }
		else
		{
			disconnect_flag = 0;
			num = 0;
			//�������� ������� �� 0 ������, ����������� �� ���-�� �������
			if (Game_Step.current_step == 0)
			{
				//�� 0 ���� �� ����� ���� �������� ���������������
				for (int ii = 0; ii < 7; ii++)  player[ii].wait = false;

				//����� ������� ���������� ����� ������
				for (int ii = 0; ii < 4; ii++)
				   {
				   if (Connections[ii] == 0) { num = ii;  break; }
				   }

				//���� � ���� ��� �������� ������� ������� ������
				if (Connections[0] != 0 && Connections[1] != 0 && Connections[2] != 0 && Connections[3] != 0)
				{
					std::cout << " SERVER: Max Players already connected \n  " << std::endl;
					std::cout << " ====  New Connection ABORT ====" << std::endl;

					strcpy_s(msg, "MAX players connected, TRY to Connect Later");
					send(newConnection, msg, sizeof(msg), NULL);
					disconnect_flag = 1;
				}
			}
			//���� ���� ���� , �� ������ ������ ���������� ������ ���� ���� ������������
			if (Game_Step.current_step > 0)
				{
				  //���� ���� ������������
				  if (player[1].wait || player[2].wait || player[3].wait || player[4].wait)
					{
						int ii;
						for (ii = 1; ii < 5; ii++)
						    {
							if (player[ii].wait == true)	if (Try_Reconnect(ii, newConnection, inet_ntoa(addr.sin_addr)) == true) { num = ii-1;  ii = 10; break; }
						    }
						if (ii < 10)      //�� ���� ����������� ������������
						    {
							std::cout << "New Connection ABORT, ��� ���������� � ������� �������  " << std::endl;

							strcpy_s(msg, "===== Wait another player to reconnect =======");
							send(newConnection, msg, sizeof(msg), NULL);
							disconnect_flag = 1;
						    }
					}
					else    //� � ���� ��� ������� ������������ - �� ������ �� ����������
					    {
						std::cout << "New Connection ABORT, CATAN GAME in PROGRESS  " << std::endl;

						strcpy_s(msg, "CATAN GAME in PROGRESS, TRY to Connect Later");
						send(newConnection, msg, sizeof(msg), NULL);
						disconnect_flag = 1;
					    }
				}
				
			if (disconnect_flag == 1)
			{
				Sleep(200);
				Connections[9] = newConnection;
				CATAN_SERVER_Command(NET_COMMAND::SET_CLOSE, nullptr, NULL, 10);
				Connections[9] = 0;

				Sleep(10);
				closesocket(newConnection);
				continue;
			}

			mtx1.lock();
			std::cout << "SERVER:  OK Player  N= " << num + 1 << "   Connected" << std::endl;
			mtx1.unlock();

			Sleep(400);

			std::cout << "About player =============================== " << std::endl;
			std::cout << "Port =  " << addr.sin_port << std::endl;
			printf("�����������  %s\n", inet_ntoa(addr.sin_addr));
			//���������� ����� ������ ��� ���������� ��������������� ��� �����������
			strcpy(str_Player_addr[num + 1], inet_ntoa(addr.sin_addr));
			std::cout << "============================================ " << std::endl << std::endl;
			//-----------------------------------------------------------------------------------
			if (Game_Step.current_step == 0)
			{
				//�������� ��������� ��������� �������������� �������, ������ ������ ������ ------------------
				strcpy_s(msg, "Hello. It's CATAN GAME ");
				send(newConnection, msg, sizeof(msg), NULL);

				Connections[num] = newConnection;
				player[num + 1].active = true;    //������������ ������
			    Set_Player_Number(num + 1);
			}

			Send_Field_GECS(num + 1);
			Send_Bandit_GECS(num + 1);
			//co������ ���� � ������������ �������
			Send_Connected_Players(TO_ALL);
			//������� ������������� ������� ������� ����� ��������� ��������� ������
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ServerClientStreamFunc, (LPVOID)(num), NULL, NULL);
		}  //else 

	} //while

	std::cout << " SERVER: ����� ����������� ��������� ������ \n  " << std::endl;
}

//================================= SERVER STREAM FUNCTION ======================================      SERVER NET CYCLE
//������� ��� ������ ��������� �� �������� � �������� ����, ����� ���� �� ���� ��� ������ �������
//===============================================================================================
void ServerClientStreamFunc(int index)
{
	char msg[256];
	char msg1[10];
	int ret;
	std::string str;
	NET_COMMAND Command = NET_COMMAND::EMPTY;
	NET_COMMAND New_Command;
	bool system = false;
	int pack_size = 0;

	while (true)
	{
		//������ ������ 20 ���� � ���� ��� ������� ������ �� ������ � ���� ���������, 
		//����� ������������� ������ � ������ �������� ������� �����
		pack_size = 0;
		while (pack_size < 20)
		    {
			ret = recv(Connections[index], &msg[pack_size], 20 - pack_size, NULL);
			if (ret == SOCKET_ERROR)    {	Player_Disconnect(index);	return;    }
			pack_size += ret;
		    }

		 //���� ����� ������ �������
		 if (If_SERVER_Command(msg, (int*)&New_Command) == true)
		    {
			int len = *((int*)&msg[16]);

			//=================== ��������� NEW �������   ======================================================= SERVER
			//�������� ������ ��� ������  � ������ � ��� ����� ������ ������
			char* buff_tmp = nullptr;
			if (len > 0)
			    {
				buff_tmp = new char[len];
				Read_CLIENT_Data(len, buff_tmp,index);
			    }

			//std::cout << " NEW TYPE NET COMMAND =  " << Map_Command[New_Command] << "   DATA len = " << len << std::endl;
			 //                                                                                                   -- SERVER
			if (New_Command == NET_COMMAND::ASK_NODE_ARRAY)
			     {
				 Send_Info_Nodes_Zip(index + 1);   //�������� ����� ������ ������������ ������(� �� 1 ������ ������ ����������)
				 //Send_Info_Nodes(TO_ALL);
			     }     
			if (New_Command == NET_COMMAND::ASK_ROAD_ARRAY)
			    {
				Send_Info_Roads(index + 1);
			    }
			if (New_Command == NET_COMMAND::ROAD_ARRAY)   //������ �������� ���� �������� � ����� �������
			    {
			    int* intPtr = (int*)buff_tmp;
				for (auto& rr : *roadPtr)
				    {
					rr.owner = *intPtr;     intPtr++;
					rr.type = *intPtr;     intPtr++;
				    }
				Send_Info_Roads(TO_ALL);
			    }
			if (New_Command == NET_COMMAND::NODE_ARRAY)
			    {
				int* intPtr = (int*)buff_tmp;
				for (auto& node : *nodePtr)    {	node.owner = *intPtr;  intPtr++;  node.object = *intPtr; intPtr++;    }
				//Send_Info_Nodes(TO_ALL);
				Send_Info_Nodes_Zip(TO_ALL);
			    }
			if (New_Command == NET_COMMAND::INFO_BANK_RESURS)  //������� �������� - �� ��� ���������
			    {
				int* intPtr = (int*)buff_tmp;
				memcpy(CARD_Bank, intPtr, 10 * sizeof(int));
				Send_Bank_Resurs(TO_ALL);
			    }
			if (New_Command == NET_COMMAND::PLAYER_OBJECTS)   
			    {
				int* intPtr = (int*)buff_tmp;
				int pl = *intPtr;  intPtr++;
				player[pl].town = *intPtr;  intPtr++;
				player[pl].village = *intPtr;  intPtr++;
				player[pl].road = *intPtr;

				Send_Player_Objects(TO_ALL, pl);
			    }
			if (New_Command == NET_COMMAND::PLAYER_RESURS)         //������ ������� ���� ������ � ��������� ����
			   {
				int* intPtr = (int*)buff_tmp;
				int pl = index + 1;
				memcpy(player[pl].resurs, intPtr, 12 * sizeof(int));   //�������� ������
				Send_Player_CARDS(TO_ALL, pl);
			   }
			if (New_Command == NET_COMMAND::SAY_ROLL_START_DICE)
			    {
				int pl = index + 1;
				int* intPtr = (int*)buff_tmp;
				int dice_num = *intPtr;       //����� ������� �� ������� �������

				player[pl].first_roll = dice_num;
				Info_Player_Last_Dice(TO_ALL, pl, player[pl].first_roll);

				//���� �� ��� ������� ����� �������� ��� ���� ������, ���� ��� �� ���� ��� ����
				if (GetNextPlayer() == Game_Step.start_player)
				{
					//mtx1.lock();  std::cout << "GetNextPlayer() == 0  ������� ��� ��� ������ " << std::endl; mtx1.unlock();
					SetFirstPlayer();    //��������� �� ����������� ������ 1 ������ ����������� ���� � ��������� step++
					std::cout << " =====================������ ����� ������ ����� -   " << Game_Step.start_player << std::endl;
					strcpy_s(msg, " ================  �������� ����� ��� 1 ���� ====================");
					CATAN_Message(msg, strlen(msg), TO_ALL);
				}
				else  Game_Step.current_active_player = GetNextPlayer();

				Set_Game_Step(TO_ALL);

				mtx1.lock();  std::cout << "SERVER  �������� ���� ������  " << Game_Step.current_active_player << std::endl;  mtx1.unlock();
				Set_New_Move(TO_ALL);
			    }
			if (New_Command == NET_COMMAND::ASK_ROLL_2DICE)
			    {
				int pl = index + 1;
				int* intPtr = (int*)buff_tmp;
				int dice2 =  *intPtr;       //����� ������� �� ������� �������

				flag_cubic_was_used = true;
				//std::cout << " ��������� ������  =  " << dice2 << std::endl;

				Info_Player_Last_Dice(TO_ALL, pl, dice2);
				if (dice2 == 7) { Info_Dise_7(TO_ALL, pl);  continue; }

				Step_Resurs(dice2);   //������������ ������� �� ����������� ������ - ���� �� ����� ������
				Send_To_All_Info_Resurs();
			    }
			if (New_Command == NET_COMMAND::SAY_MOVE_OVER)
			    {
				//������ ���� ������� �� �����
				for (int i = 0; i < 12; i++)  Change[i].status = 0;
				int pl = index + 1;
				//std::cout << " ���������� ���� �����  " << pl << std::endl;
				if (Game_Step.current_step == 4)
				   {
					Game_Step.current_active_player = GetNextPlayer();
					//������������ ������ �������� ��������, ������� ��� �� ������������
					for (auto& elem : develop_CARDS[Game_Step.current_active_player])
					    { if (elem.status == -1 )  elem.status = 0;    }
					Send_Develop_CARDS(TO_ALL, Game_Step.current_active_player);
					InitChange_BANK();    //�� ������� �������� ����� ������
					//������� ������ �������� ������    max_army
					int num_max = 0, num = 0;
					if (max_army > 0)    //���� �������� ��� � ������, ����������� ��� ����� 
					    {
						for (auto& elem : develop_CARDS[max_army])	if (elem.status == 1 && elem.type == IMP_TYPE::KNIGHT) num_max++;
					    }
					    else max_army = 0;

					//������� ������� � ������
					max_gavan = 0;
					for (int i = 1; i < 7; i++)
					{
						num = 0;
						for (auto& elem : *nodePtr) { if (elem.type != -1 && elem.owner == i) num += elem.object; }
						if (num > max_gavan && num >= 3)  max_gavan = i;
					}

					//������� ����� � ���� ������� � ������� ��� �������������
					for (int i = 1; i < 7; i++)
					    {
						num = 0;
						for (auto& elem : develop_CARDS[i]) { if (elem.status == 1 && elem.type == IMP_TYPE::KNIGHT) num++; }
						if (num > max_army && num >= 3)  max_army = i;
					    }

					//������� ������ �������� ������    max_road_owner
					int way;
					for (int i = 1; i < 7; i++)
					    {
						way = Count_Road_Length(i);
						if (way > Count_Road_Length(max_road_owner) && way >= 5)  max_road_owner = i;
					    }
					if (Count_Road_Length(max_road_owner) < 5) max_road_owner = 0;

					Send_To_All_Info_MaxWayArmy(TO_ALL);
			       }
				if (Game_Step.current_step == 3)
				    {
					Get_Resurs(player[pl].last_village_node, pl);    //������� ������� ��� �������
					if (Game_Step.current_active_player == Game_Step.start_player)
					    {
						Game_Step.current_step = 4;     //����� ����������� ����� ����
						std::cout << " =========== ������� �� �������� ����  ==========  " << pl << std::endl;
						Set_Game_Step(TO_ALL);
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
						Set_Game_Step(TO_ALL);
					    }
					 else  Game_Step.current_active_player = GetNextPlayer();
				     }

				//mtx1.lock();  std::cout << "SERVER*  �������� ���� ������  " << Game_Step.current_active_player << std::endl;  mtx1.unlock();
				flag_cubic_was_used = false;
				flag_develop_card_used = false;

				//���� ������� ����, �� ��� 10 ����� ����������� ������
				if((Game_type == 1 && CountScore(pl) >= 10) || 
					(Game_type == 2 && CountScore(pl) >= 11 && player[pl].resurs[(int)RESURS::BOOT] == 0) ||
					(Game_type == 2 && CountScore(pl) >= 12 && player[pl].resurs[(int)RESURS::BOOT] == 1))
				       {
					   Set_Game_OVER(pl);
					   Game_Step.current_step = 5;
					   Set_Game_Step(TO_ALL);
				       }
				else 	Set_New_Move(TO_ALL);
			    }
			if (New_Command == NET_COMMAND::LAST_VILLAGE)
			    {
				int pl = index + 1;
				int* intPtr = (int*)buff_tmp;
				player[pl].last_village_node = *intPtr;
			    }
			if (New_Command == NET_COMMAND::ASK_BANK_RESURS)
			      {
				  Send_Bank_Resurs(index+1);
			      }
			if (New_Command == NET_COMMAND::ASK_PLAYER_RESURS)
			     {
				 Send_Player_CARDS(index+1,index+1);
			     }
			if (New_Command == NET_COMMAND::BANDIT_GECS)
			       {
				   int* intPtr = (int*)buff_tmp;
				   bandit_Gecs = *intPtr;
				   Send_Bandit_GECS(TO_ALL);
			       }
			if (New_Command == NET_COMMAND::INFO_CHANGE_BANK)
			    {
				//��������� �������� ���� ������
				int pl = index + 1;
				int* intPtr = (int*)buff_tmp;
				memcpy(&ChangeBANK[pl][0],intPtr, sizeof(int) * 10);

				//�������� ���� ��������� �����
				Send_Info_Change_Area(TO_ALL, pl);
			    }
			if (New_Command == NET_COMMAND::ASK_PLAYER_OBJECTS)
			    { 
				int pl = index + 1;;
				Send_Player_Objects(pl, pl);
				continue;
			    }
			if (New_Command == NET_COMMAND::ASK_CHANGE_WITH_BANK)
			     {
				 int pl = index + 1;
				 int* intPtr = (int*)buff_tmp;
				 int type_put =  *intPtr;     //���������� ��� �����

				 int type_get = 0;    //��� ������� ������� ������

				 //std::cout << " ������ �� ����� � ������ " << resurs_name[type_put] << " from pl " << pl  <<  std::endl;

				 //�����  �������� ������ ����, ������� �� �� ����� ������
				 int need_card;
				 for (int i = 1; i < 6; i++)   //1--6 �������� ���� ��������
				    {
					need_card = 4;
					if (bonus31(pl)) need_card = 3;
					if (bonus21(pl, i)) need_card = 2;
					//��������� ���� �� ������ ���-�� � �������� ����� � ��� �������� � ����� ������
					if (ChangeBANK[pl][i] >= need_card && player[pl].resurs[i] >= need_card) { type_get = i; break; }
				    }

				 //��������� ����������� ������ �� ��� - ���� ��, �� ��� ��������� ���� ������ ����������
				 int num_fish = Count_Fish_In_Change(pl);
				 if (num_fish >= 4)  type_get = (int)RESURS::FISH_ALL;

				 //���� ������� ������� ������� ��� ������, � ��� ������������ � ������������� ������ ����
				 if (type_get != 0 && type_get < (int)RESURS::FISH1 && CARD_Bank[type_put] >= 1)
				       {
					   //��������� ����� � ������
					   player[pl].resurs[type_put] += 1;            ChangeBANK[pl][type_put] += 1;	        CARD_Bank[type_put] -= 1;
					   player[pl].resurs[type_get] -= need_card;    ChangeBANK[pl][type_get] -= need_card;  	CARD_Bank[type_get] += need_card;
					   Send_To_All_Info_Resurs();
				       }

				 //���� ����� �� ��� � ������������� ������ ����
				 if (type_get == (int)RESURS::FISH_ALL && CARD_Bank[type_put] >= 1)
				     {
					 //�������� ����� ������� � ������� �� �����
					 player[pl].resurs[type_put] += 1;     ChangeBANK[pl][type_put] += 1;	 CARD_Bank[type_put] -= 1;

					 //������� 4 ��� �� ����� ������
					 Get_Fish_From_Player(pl, 4);
					 Send_To_All_Info_Resurs();
				     }
			     }
			if (New_Command == NET_COMMAND::SEND_CARDS_TO_BANK)
			    {
				//����� ��������� ����� � �����
				int pl = index + 1;
				int num;
				for (int i = 1; i < 10; i++)
				   {
					num = ChangeBANK[pl][i];
					if (num != 0) { player[pl].resurs[i] -= num;     ChangeBANK[pl][i] -= num;	   CARD_Bank[i] += num; }
				   }
				//�������� ���� ��������� ������ �������� - �� ��� ����� ���� � ���� ������ ���� ��������
				Send_To_All_Info_Resurs();
			    }
			if (New_Command == NET_COMMAND::SAY_GAME_START)
			    {
				mtx1.lock();  std::cout << "  START GAME Command !!!  " << std::endl; mtx1.unlock();
				Game_Step.current_step = 1; 	Game_Step.current_active_player = 1;
				strcpy_s(msg, " ================ GAME START ===============");
				CATAN_Message(msg, strlen(msg), TO_ALL);
				Set_Game_Step(TO_ALL);  Set_New_Move(TO_ALL);
			    }
			if (New_Command == NET_COMMAND::TAKE_CARD_FROM_PLAYER)
			{
				int pl = index + 1;
				int* intPtr = (int*)buff_tmp;
				int pl_donor = *intPtr;

				//std::cout << " Take card command "  << std::endl;
				int type = TakeRandomCardFromPl(pl_donor);
				if (type) { player[pl].resurs[type] += 1;  ChangeBANK[pl][type] += 1;   player[pl_donor].resurs[type] -= 1; }
				//std::cout << " Pl " << pl << " �������� ����� �  " << pl_donor << "type " << resurs_name[type] <<  std::endl;

				//�������� ���� ��������� ������ �������� - �� ��� ����� ���� � ���� ������ ���� ��������
				Send_To_All_Info_Resurs();
			}
			if (New_Command == NET_COMMAND::INFO_TAKE_CARD_FROM_PLAYER)
			    {
				int pl = index + 1;

				//�������� ���� ��� ����� ���������� �������� ����� � ��������� �� 3 ����
				Send_To_All_Info_Take_Card(pl);
				Get_Fish_From_Player(pl,3);
				Send_To_All_Info_Resurs();
			    }
			if (New_Command == NET_COMMAND::ASK_BUY_IMPROVE_CARD && improve_CARDS.size() >= 1)
			    {
				int pl = index+1;
				//��������� ������������� ����
				if (Count_Fish_In_Change(pl) < 7  &&  (player[pl].resurs[(int)RESURS::STONE] == 0 || 
					               player[pl].resurs[(int)RESURS::OVCA] == 0 || player[pl].resurs[(int)RESURS::BREAD] == 0))
				    {
					//std::cout << "SERVER: �� ������� �������� " << std::endl;
				    }
				else
				   {
					//������� ����� � ������ � ��������� � ����
					if (Count_Fish_In_Change(pl) >= 7)  Get_Fish_From_Player(pl, 7);
					    else
					      {
						  if (ChangeBANK[pl][(int)RESURS::STONE] > 0)  ChangeBANK[pl][(int)RESURS::STONE] -= 1;
						  player[pl].resurs[(int)RESURS::STONE] -= 1;   CARD_Bank[(int)RESURS::STONE] += 1;
						  if (ChangeBANK[pl][(int)RESURS::OVCA] > 0) ChangeBANK[pl][(int)RESURS::OVCA] -= 1;
						  player[pl].resurs[(int)RESURS::OVCA] -= 1;  CARD_Bank[(int)RESURS::OVCA] += 1;
						  if (ChangeBANK[pl][(int)RESURS::BREAD] > 0) ChangeBANK[pl][(int)RESURS::BREAD] -= 1;
						  player[pl].resurs[(int)RESURS::BREAD] -= 1;  CARD_Bank[(int)RESURS::BREAD] += 1;
					      }

					//�������� ��� ����� �������� �� �������
					auto type = improve_CARDS.at(0).type;
					auto status = improve_CARDS.at(0).status;
					//�������� ������ ����� ��������, �������� ������� �� �������
					improve_CARDS.erase(improve_CARDS.begin());

					//��������� ����� � ������ ������ � �������� ������ ���� �������� ������ ����
					IMP_CARD ttt = { status, type };
					develop_CARDS[pl].push_back(ttt);
					Send_Develop_CARDS(TO_ALL, pl);

					//std::cout << " ����� �������� " << (int)type << " vector_size =  "<< develop_CARDS[pl].size()  << std::endl;
					//�������� ���� ��������� ������ �������� ��������
					Send_To_All_Info_Resurs();
				    }
			    }
			if (New_Command == NET_COMMAND::ASK_PLAY_DEVELOP_CARD)
			    {
			    int pl = index+1;
				int* intPtr = (int*)buff_tmp;
				int type  = *intPtr;

				flag_develop_card_used = true;
				//��� ����� ����� �������� �������� ������� �������  
				if (type != (int)IMP_TYPE::POINT1 && type != (int)IMP_TYPE::ROAD2)
				    {
					InitChange_BANK(pl);    //������ ���� ��������� �����
					Send_Info_Change_Area(TO_ALL,pl);
				    }

				//� ������� ���� �������� ������ ������ �������� ��������� ����� ��������� � ������ 1(�������)
				for (auto& elem : develop_CARDS[pl])
					{
					if (elem.status == 0 && type == (int)elem.type) 	{	elem.status = 1;  break;  }
					}	
				Send_Develop_CARDS(TO_ALL, pl);
				//��������� ���� ���� � ���� ������ ��������
				Info_Play_Develop_Card(TO_ALL, pl, type);
			    }
			if (New_Command == NET_COMMAND::ASK_GET_RESURS_FROM_ALL)
			    {
				int pl = index+1;
				int* intPtr = (int*)buff_tmp;
				int type = *intPtr;
				//std::cout << " ������ �� ������ " << pl << " ������� ���   " << resurs_name[(int)type] << std::endl;

				for (int i = 1; i < 7; i++)   //���� �� �������
				    {
					if (i == pl || player[i].active == 0)  continue;
					//��������� ����� � ������
					player[pl].resurs[type] += player[i].resurs[type];     ChangeBANK[pl][type] += player[i].resurs[type];
					player[i].resurs[type] = 0;                            ChangeBANK[i][type] = 0;
				    }

				 //�������� ���� ��������� ������ �������� ��������
				 Send_To_All_Info_Resurs();
			     }
			if (New_Command == NET_COMMAND::ASK_RESET_GAME)
			    {
				int pl = index + 1;
				int* intPtr = (int*)buff_tmp;
				Game_type = *intPtr;

				std::cout << "  RESET COMMAND   " << std::endl;

				Game_Step.current_step = 0;
				Game_Step.current_active_player = 1;
				Game_Step.start_player = 1;

				Init_CATAN_Field(gecsPtr, nodePtr, roadPtr);

				Send_Info_Game_Type(TO_ALL);
				Send_Info_Nodes(TO_ALL);	Send_Info_Roads(TO_ALL);
				Send_Field_GECS(TO_ALL);	Send_Bandit_GECS(TO_ALL);

				//����� �������, ��������, ����� ������
				for (int pl = 1; pl < 7; pl++)
				    {
					if (player[pl].active == false)  continue;
					develop_CARDS[pl].clear();
					Send_Develop_CARDS(TO_ALL, pl);
					Send_Player_Objects(TO_ALL, pl);
				    }

				Send_To_All_Info_Resurs();
				Send_To_All_Info_MaxWayArmy(TO_ALL);
				Set_Game_Step(TO_ALL);
				Set_New_Move(TO_ALL);
			    }
			if (New_Command == NET_COMMAND::TEST_GAME)
			    {
				Game_Step.current_step = 4;
				player[1].resurs[1] = 40;  player[1].resurs[2] = 40;  player[1].resurs[3] = 40; player[1].resurs[4] = 40;  player[1].resurs[5] = 40;
				player[2].resurs[1] = 40;  player[2].resurs[2] = 40;  player[2].resurs[3] = 40; player[2].resurs[4] = 40; player[2].resurs[5] = 40;
				nodePtr->at(18).owner = 1; nodePtr->at(18).object = VILLAGE;
				nodePtr->at(25).owner = 1; nodePtr->at(25).object = TOWN;
				nodePtr->at(9).owner = 2; nodePtr->at(9).object = TOWN;
				nodePtr->at(34).owner = 2; nodePtr->at(34).object = VILLAGE;
				Send_Info_Nodes_Zip(TO_ALL);
				Send_Info_Roads(TO_ALL);
				Send_To_All_Info_Resurs();

				Set_Game_Step(TO_ALL);
			    }
			if (New_Command == NET_COMMAND::SET_CHANGE_OFFER)
			   {
				int pl = index+1;
				int* intPtr = (int*)buff_tmp;
				int pl_change = *intPtr;

				//std::cout << " Change SET command from   "  <<  pl << "  to "  << pl_change  << std::endl;
				//��������� �� ������ ������ ������ ������� ��������� ������
				//��������� ���� �� � ������ �������
				if(ChangeBANK[pl][(int)RESURS::BOOT]) 
				     {
					  //�������� ������� ����������
					  ChangeBANK[pl][(int)RESURS::BOOT] = 0; player[pl].resurs[(int)RESURS::BOOT] = 0;
					  ChangeBANK[pl_change][(int)RESURS::BOOT] = 1; player[pl_change].resurs[(int)RESURS::BOOT] = 1;
					  //������� ��������� � �������� ������� ����

					  //��������� ���� �������� ����� ������
					 Send_Info_Change_Area(TO_ALL, pl);
					 Send_Info_Change_Area(TO_ALL, pl_change);
					 Send_To_All_Info_Resurs();

				     }
				else
				   {
					//����� �� �������� ������
					int s = 0;
					for (s = 0; s < 12; s++) { if (Change[s].status == 0) break; }
					if (s < 12)
					    {
						Change[s].from_pl = pl; 		Change[s].to_pl = pl_change;    Change[s].status = 1;
						for (int t = 0; t < 10; t++)    //���� �� ���� �������
						    {
							//��� ����������
							if (t == (int)RESURS::FISH1 || t == (int)RESURS::FISH2 || t == (int)RESURS::FISH3)  continue;
							Change[s].offer_num[t] = (int)ChangeBANK[pl][t];
							Change[s].need_num[t] = (int)ChangeBANK[pl_change][t];
						    }
						//�������� ������ ������ ��� �������
						Send_Info_Change(TO_ALL, s);
					    } //if
				    } //else
			    }
			if (New_Command == NET_COMMAND::ASK_DELETE_OFFER)
			    {
				int pl = index+1;
				int* intPtr = (int*)buff_tmp;
				int s = *intPtr;

				Change[s].status = 0;
				Send_Info_Change(TO_ALL, s);
			    }
			if (New_Command == NET_COMMAND::ASK_ACCEPT_OFFER)
			    {
				//std::cout << "  ACCEPT OFFER  " << std::endl;
				int pl = index+1;
				int* intPtr = (int*)buff_tmp;
				int s = *intPtr;

				int pl1 = Change[s].from_pl;
				int pl2 = Change[s].to_pl;

				//���������� �����
				for (int t = 0; t < 6; t++)
				    {
					player[pl1].resurs[t] += Change[s].need_num[t];   player[pl2].resurs[t] -= Change[s].need_num[t];
					player[pl2].resurs[t] += Change[s].offer_num[t];  player[pl1].resurs[t] -= Change[s].offer_num[t];
				    }

				Send_Player_CARDS(TO_ALL, pl1);
				Send_Player_CARDS(TO_ALL, pl2);

				//�������� ����� ������ ������� ������
				InitChange_BANK(pl1);	Send_Info_Change_Area(TO_ALL, pl1);
				InitChange_BANK(pl2);   Send_Info_Change_Area(TO_ALL, pl2);

				//�������� ����������� ������ ��� �������
				Change[s].status = 0;
				Send_Info_Change(TO_ALL, s);
			    }
			if (New_Command == NET_COMMAND::INFO_FISH_CARDS)
			    {
				int* intPtr = (int*)buff_tmp;
				int size = *intPtr;  intPtr++;

				FishCards.clear();
				for (int i = 0; i < size; i++)
				    {
					FishCards.push_back((RESURS)(*intPtr++));   //��������� ������ 
				    }
				Send_Fish_CARDS(TO_ALL);
			    }

			//---------------cycle new net finish -------------------------
			if(buff_tmp != nullptr) delete[] buff_tmp;
			continue;
		    }
		
		//��������� ������ ������� ������� � ������� � 256 ����
		pack_size = 20;
		while (pack_size < 256)
		    {
			ret = recv(Connections[index], &msg[pack_size], sizeof(msg) - pack_size, NULL);
			if (ret == SOCKET_ERROR)	if (ret == SOCKET_ERROR) { Player_Disconnect(index);	return; }
			pack_size += ret;
		    }

		//���� �������� ������ ����������� ��������� ��� ���
		//��������� �������� ��������� ���� ����� INDEX ���� ��� ���
		for (int i = 0; i < 5; i++)
		{
			if (i == index)    continue;     // � index  �� ����������
			if (Connections[i] == 0)    continue;
			//�������� �����������
			if (send(Connections[i], msg, sizeof(msg), NULL) == INVALID_SOCKET)
			    {
				std::cout << "**  ERROR SEND  " << std::endl;   //�������� ��������� ���� ����� ���� ������ ��� ����������� �������������� ����������� � �������
				closesocket(Connections[i]);	Connections[i] = 0;
			   }
		}   //for
	}
}

//===============================================================================================       CLIENT 
 //������� ������� ��� ��������� ��������� �� ��������� �����
 //��� ��������� ������ ������� ��������� � ��� ���� ��� ��������� ������
 //���� ��� ��������� ��������� CATAN ������������
 //==============================================================================================
void ClientHandler(int index)
{
	int ret = 1;
	char* ptr;
	char msg[256];   //--------------------------------------------------------------------------
	std::string str;
	int pack_size;
	NET_COMMAND Serv_Command;
	NET_COMMAND Command = NET_COMMAND::EMPTY;

	while (true)            //����������� ���� � ������ ��������� ������� ���������
	{
		pack_size = 0;
		Serv_Command = NET_COMMAND::EMPTY;
		//������ ������� ������ � ��������� ����� ������ -------------------------
		while (pack_size < 20)   
		     { 	 ret = recv(Connection, &msg[pack_size],20 - pack_size, NULL);	 pack_size += ret;    }

		//���� ������ ������� �������, �� ������������ � ������������ �� ������ ������� �������
		if (If_SERVER_Command(msg, (int*)&Serv_Command) == true)
			     {
				 int len = *((int*)&msg[16]);
				 //std::cout << " NEW TYPE NET COMMAND =  " << Map_Command[Serv_Command] << "DATA len = " << len << std::endl;

				 //�������� ������ ��� ������ ====================================
				 char* buff_tmp = nullptr;
				 if (len > 0)
				     {
					 buff_tmp = new char[len];
					 Read_SERVER_Data(len, buff_tmp);
				     }

				 //Perform_Server_Command            ----------------------------------                CLIENT !!!
				 if(Serv_Command == NET_COMMAND::SET_N_PLAYER)
				        {
					    player_num = *((int*)&buff_tmp[0]);
						std::cout << "��� �������� �����: " << player_num << std::endl;
						player[player_num].active = true;
				        }
				 if(Serv_Command == NET_COMMAND::BANDIT_GECS)
				        {
					    bandit_Gecs = *((int*)&buff_tmp[0]);
				        }
				 if(Serv_Command == NET_COMMAND::SET_GECS)
				        {
					    //std::cout << " sizeof(GECS) * ((*gecsPtr).size()) =  " << sizeof(GECS) * ((*gecsPtr).size())  << std::endl;
						int* IntPtr = (int*)buff_tmp;
						for(auto& elem : *gecsPtr)
						     {
							 elem.x = *IntPtr;                 IntPtr++;
							 elem.y = *IntPtr;                 IntPtr++;
							 //std::cout << "old type gecs = " << resurs_name[(int)elem.type] << std::endl;
							 elem.type = (RESURS)*IntPtr;      IntPtr++;
							 elem.gecs_game_number = *IntPtr;  IntPtr++;
						     }
						for (auto& elem : FishGecs)
						    {
							elem.x = *IntPtr;                 IntPtr++;
							elem.y = *IntPtr;                 IntPtr++;
							elem.type = (RESURS)*IntPtr;      IntPtr++;
							elem.gecs_game_number = *IntPtr;  IntPtr++;
						    }
				        }
				 if(Serv_Command == NET_COMMAND::INFO_N_ACTIVE_PLAYER)
				    {
					 int* IntPtr = (int*)buff_tmp;
					  for (int ii = 1; ii < 7; ii++)
					     {
						 if(player[ii].active == false && *IntPtr != 0)       std::cout << "����������� ����� �: " << ii << std::endl; 
						 player[ii].active = *IntPtr;
						 if(player[ii].active)   player[ii].wait = false;
						 IntPtr++;
					     }
				    }
				 if(Serv_Command == NET_COMMAND::SET_FIRST_PLAYER)
				    {
					 //������� �� �������� - ������ �� �������� ������ ���������
					 Game_Step.start_player = *((int*)&buff_tmp[0]);
				    }
				 if(Serv_Command == NET_COMMAND::SET_STEP)
				    {
					Game_Step.current_step = *((int*)&buff_tmp[0]);

					int st = Game_Step.current_step;
					//��������������� ����� 2 3 ����� - ��� �������� ������������ ����
					if (st == 2 || st == 3)
					    { Game_Step.step[st].flag_set_one_Village = 1; Game_Step.step[st].flag_set_one_Road = 1;   }
					//������� ������� ���������� ������
					if (st == 4)    {	for (int i = 1; i < 7; i++)   player[i].last_dice = 0;    }
				    }
				 if(Serv_Command == NET_COMMAND::SET_N_ACTIVE_PLAYER)
				        {
					    int* IntPtr = (int*)buff_tmp;
						Game_Step.current_active_player = *IntPtr;  IntPtr++;
						flag_cubic_was_used = *IntPtr;              IntPtr++;
						flag_develop_card_used = *IntPtr;

					    Game_Step.step[4].flag_bandit = 0;

					    //����� ������ ���� ���� ��������
					    Play_two_resurs = 0;    Play_ONE_resurs = 0;    Play_two_roads = 0;
					    if(flag_develop_card_used == false)  Allow_Develop_card = 1;
						     else Allow_Develop_card = 0;
						if (flag_cubic_was_used == false)   flag_get_cubic_result = false;
						     else   flag_get_cubic_result = true;

					    //������ ���� ������� �� �����
					    for (int i = 0; i < 12; i++)  Change[i].status = 0;

					    if (Game_Step.current_step == 4 && flag_get_cubic_result == false)  Game_Step.step[4].roll_2_dice = 1;
						        else  Game_Step.step[4].roll_2_dice = 0;
					    InitChange_BANK();    //� ������ ���� ���� ������ ����������
					    for (int ii = 1; ii < 5; ii++) { player[ii].flag_allow_get_card = 0;  limit_7[ii] = 0; }

					    //������� ������� ������ ���������� ������
					    if (player_num == Game_Step.current_active_player && (Game_Step.current_step == 4 || Game_Step.current_step < 2))
						      player[player_num].last_dice = 0;
				        }
				 if(Serv_Command == NET_COMMAND::NODE_ARRAY)
				      {
					  //info � ��������� � ���������� ����� (��� ���������� ����� � �� ���������)
					  int* intPtr = (int*)buff_tmp;
					  for (auto& node : *nodePtr)
					     {
						  node.owner = *intPtr;  intPtr++;
						  node.object = *intPtr; intPtr++;
					     }
				      }
				 if(Serv_Command == NET_COMMAND::NODE_ARRAY_ZIP)
				     {
					 //� ������ ���� ���������� ������ ���� �� ����������  - ������� �������� � ������� � ��������� ����� ????
					 int* intPtr = (int*)buff_tmp;
					 int size = *intPtr;  intPtr++;

					 //������� ���� ����� ���������� ������
					 for (auto& node : *nodePtr)     {	 node.owner = -1;	 node.object = -1;    }

					 intPtr = (int*)buff_tmp; intPtr++;
					 for(int i = 0;i < size;i++)
					   {
						int number = *intPtr; intPtr++;
						//std::cout << " Number == " << number << std::endl;
						//������� 1 - �������� ������ STL � ����������
						auto it = find_if(nodePtr->begin(), nodePtr->end(), [&number](NODE x)
							 {
						     return (x.number == number);
							 } );
						(*it).owner = *intPtr;   intPtr++;
						(*it).object = *intPtr;   intPtr++;

					   //������� 2 - ��������� ��������, �����  ���� �� ����� ������,  ���� ������������� �� ������� � �������
					   //auto it = &(*nodePtr).at(number);
					   //it->owner  = *intPtr;   intPtr++;
					   //(*it).object = *intPtr;   intPtr++;
					   }

					 //-------------------------------------------------------------------------------------------------
					 /*
					 int ii = 0;
					 std::cout << " ======== TEST node reciev from server ======== " << std::endl;
					 std::cout << " Number fill nodes on server = " << size << std::endl;
					 for (auto elem : *nodePtr)
					       { std::cout << ii++ << "\tnode owner = " << elem.owner << "\tobj =  " << elem.object << "\tnumber =  " << elem.number <<std::endl;
					         if (ii > 10)  break; 
					       }
					 std::cout << " END NODES  "  << "   size == " << nodePtr->size() << std::endl;
					 */
					 //--------------------------------------------------------------------------------------------------
				     }
				 if(Serv_Command == NET_COMMAND::ROAD_ARRAY)
				     {
					 //info � �������������� ������ � ���� ��������� (���� ������ ����� ��� ����)
					 int* intPtr = (int*)buff_tmp;
					 for (auto& rr : *roadPtr)
					     {
						 rr.owner = *intPtr;     intPtr++;
						 rr.type  = *intPtr;     intPtr++;
					     }
				     }
				 if(Serv_Command == NET_COMMAND::INFO_BANK_RESURS)
				     {
					 memcpy(CARD_Bank, buff_tmp, 10 * sizeof(int));
				     }
				 if(Serv_Command == NET_COMMAND::PLAYER_RESURS)
				     {
					 int pl = *((int*)&buff_tmp[0]);
					 int* intPtr = (int*)buff_tmp;
					 intPtr++;
					 //������ ����� ������ �� ������ �� ������ ������ int, � ������� ��� ����� ������
					 memcpy(player[pl].resurs,intPtr, 12 * sizeof(int));
				     }
				 if(Serv_Command == NET_COMMAND::PLAYER_OBJECTS)
				     {
					 int* intPtr = (int*)buff_tmp;
					 int pl = *intPtr;  intPtr++;
					 player[pl].town = *intPtr;  intPtr++;
					 player[pl].village = *intPtr;  intPtr++;
					 player[pl].road = *intPtr;
				     }
				 if(Serv_Command == NET_COMMAND::DICE_SEVEN && player[player_num].resurs[1] < 30)
				      {
					  //pl - ����� ������ ���������� 7
					  int pl = *((int*)&buff_tmp[0]);

					  if (pl == player_num)
					     {
						  std::cout << " == �� ������� 7 == "<< std::endl;
						 //���������� ���� ������  ����� ������� ����� �� ������ ��� ������ � ������ ����
						 InitChange_BANK(); 
						 Game_Step.step[4].flag_bandit = 1;
						 //���������� ������� ���� ������ �������� � ������� ����� ���������� ����
						 for (int i = 1; i < 7; i++)
						     {
							 limit_7[i] = 0;
							 if (i == player_num)   continue;
							 if (getPlayerNumCardResurs(i) >= 8)
							     {
								 limit_7[i] = getPlayerNumCardResurs(i) / 2 + getPlayerNumCardResurs(i) % 2;
								 //std::cout << "Limit card set for player = " << i << "  limit = " << limit_7[i] << std::endl;
							     }
						     }
					     }
					      else
					         {
						     //���� ������� �� ��, �� ������������ ����� �� �����
						     if(getPlayerNumCardResurs(player_num) >= 8)
						        {
							    limit_7[player_num] = getPlayerNumCardResurs(player_num)/2 + getPlayerNumCardResurs(player_num)%2;
						        }
					         }
				        }   //DICE_SEVEN
				 if(Serv_Command == NET_COMMAND::PLAY_DEVELOP_CARD)
				      {
					  int* intPtr = (int*)buff_tmp;
					  int pl = *intPtr;  intPtr++;
					  int type = *intPtr;

					  Big_Message = type;
					  start_Big_Message = std::chrono::high_resolution_clock::now();

					  if (pl == player_num)
					      {
						  if (type == (int)IMP_TYPE::KNIGHT)    	 Game_Step.step[4].flag_bandit = 1;
						  if (type == (int)IMP_TYPE::ROAD2)          Play_two_roads = 2;
						  if (type == (int)IMP_TYPE::RESURS_CARD2)   Play_two_resurs = 2;
						  if (type == (int)IMP_TYPE::MONOPOLIA)      Play_ONE_resurs = 1;
					      }
					   Allow_Develop_card = 0;
				       }
				 if(Serv_Command == NET_COMMAND::LAST_DICE)
				     {
					 int* intPtr = (int*)buff_tmp;
					 int pl = *intPtr;  intPtr++;
					 player[pl].last_dice = *intPtr;
					 player[player_num].flag_allow_change = 1;     //���������� �� �����
					 flag_get_cubic_result = true;         //���� �������� �������� ������ ��������� ����
				     }
				 if(Serv_Command == NET_COMMAND::MAX_WAY_ARMY_OWNER)
				      {
					  int* intPtr = (int*)buff_tmp;
					  max_road_owner = *intPtr;  intPtr++;
					  max_army = *intPtr;         intPtr++;
					  max_gavan = *intPtr;
				      }
				 if(Serv_Command == NET_COMMAND::PLAYER_CHANGE_AREA)
				      {
					  int* intPtr = (int*)buff_tmp;
					  int pl = *intPtr; 	intPtr++;
					  memcpy(ChangeBANK[pl],intPtr, 10 * sizeof(int));
				      }
				 if(Serv_Command == NET_COMMAND::DEVELOP_VECTOR)
				       {
					   int* intPtr = (int*)buff_tmp; 
					   int pl =   *intPtr;  intPtr++;
					   int size = *intPtr;  intPtr++;

					   develop_CARDS[pl].clear();
					   IMP_CARD tmp;
					   for (int i = 0; i < size; i++)
					      {
						  tmp.status = *intPtr;    intPtr++;
						  tmp.type = (IMP_TYPE)(*intPtr);   intPtr++;
						  develop_CARDS[pl].push_back(tmp);   //��������� ������ 
					      }
				       }
				 if(Serv_Command == NET_COMMAND::INFO_IMP_CARDS)
				     {
					 int* intPtr = (int*)buff_tmp;
					 int size    = *intPtr;  intPtr++;

					 improve_CARDS.clear();
					 IMP_CARD tmp;
					 for (int i = 0; i < size; i++)
					     {
						 tmp.status = *intPtr++;
						 tmp.type = (IMP_TYPE)(*intPtr++);
						 improve_CARDS.push_back(tmp);   //��������� ������ 
					     }
				     }
				 if(Serv_Command == NET_COMMAND::CHANGE_OFFER)
				      {
					  int* intPtr = (int*)buff_tmp;
					  int s = *intPtr;  intPtr++;   //����� ������                              
					  memcpy(&Change[s],intPtr,sizeof(CHANGE));        //����������� ��������� ������
				      }
				 if(Serv_Command == NET_COMMAND::EMPTY)
				     {
					 buff_tmp[len - 1] = 0;
					 mtx1.lock();	std::cout << "Cat_mess:   " << buff_tmp << std::endl;	mtx1.unlock();
				     }
				 if(Serv_Command == NET_COMMAND::WAIT_RECONNECT)
 				    {
					 int* intPtr = (int*)buff_tmp;
					 int player_disconnect = *intPtr;  intPtr++;
					//������� ��������� � disconnect � ��������� ���� �� ��������� ������� � �������� ������
					 std::cout << " ATTENTION  DISCONNECT Player =  " << player_disconnect << std::endl;
					 if (Game_Step.current_step > 0)
					     {
						 player[player_disconnect].wait = true;
						 game_wait_reconnect += 1;
					     }
					 else player[player_disconnect].active = false;
				    }
				 if(Serv_Command == NET_COMMAND::SET_CLOSE)
				       {
					   std::cout << " COMMAND CLOSE "  << std::endl;
					    Sleep(200);
					    exit(1);
				       }
				 if(Serv_Command == NET_COMMAND::GAME_TYPE)
				     {
					 int* intPtr = (int*)buff_tmp;
					 Game_type = *intPtr;
				     }
				 if(Serv_Command == NET_COMMAND::INFO_FISH_CARDS)
				      {
					  int* intPtr = (int*)buff_tmp;
					  int size = *intPtr;  intPtr++;

					  FishCards.clear();
					  for (int i = 0; i < size; i++)
					      {
						  FishCards.push_back((RESURS)(*intPtr++));   //��������� ������ 
					      }
				      }
				 if(Serv_Command == NET_COMMAND::INFO_TAKE_CARD_FROM_PLAYER)
				      {
					  int* intPtr = (int*)buff_tmp;
					  int pl = *intPtr;      //����� ������ ������� ����� �����

					  strcpy(BuffBigMessage, "Player Choos Getting 1 card");
					  Big_Message = 10;
					  start_Big_Message = std::chrono::high_resolution_clock::now();
					  //��������� ����� ���������� ������ �����
					  if(player_num == pl) 
					      {
						  for (int i = 0; i < 7; i++)
							  if (player[i].active && i != pl)  player[i].flag_allow_get_card = 1;
					      }
				      }
				 if(Serv_Command == NET_COMMAND::GAME_OVER)
				     {
					 int* intPtr = (int*)buff_tmp;
					 int pl = *intPtr;      //����� ������

					 strcpy(BuffBigMessage, "GAME Over, Player ");
					 _itoa_s(pl, msg, 10);
					 strcat(BuffBigMessage, msg);   strcat(BuffBigMessage, "win");
					 Big_Message = 10;
					 start_Big_Message = std::chrono::high_resolution_clock::now();
				     }

				 if(buff_tmp != nullptr)  delete[] buff_tmp;
				 //================================================================
				 continue;
			     }
		       else
		         {
				 //���������� ����� �� 256, ����� ������������� �� ����� � �������
			     while (pack_size < 256)
			          {
				      ret = recv(Connection, &msg[pack_size],sizeof(msg) - pack_size, NULL);
				      pack_size += ret;
			          }
		          }  

		//---------------------------------------------------------------------------------------------------------
		mtx1.lock();	std::cout << "CLIENT: " << msg  << std::endl; 	mtx1.unlock();

	}    //while (true)
	return;
}

//===============================================================================================
// ������������� ������� ���� 
//===============================================================================================       START CLIENT
int Init_Client_CATAN(void)
{
	init_WSA();
	std::cout << "\n\n ========== PLAYER PART WINSOCK CATAN START ============ " << std::endl;

	//============================ UDP CLIENT =====================================
	//������� ����������������� UDP ������ ��� ��������� ������ �������
	SOCKET SendRecvSocket;  // ����� ��� ������ � ��������
	sockaddr_in ServerAddr;  // ��� ����� ����� �������
	int err, maxlen = 512;  // ��� ������, ������ ������� � ������ ��������� ������
	char* recvbuf = new char[maxlen];  // ����� ������
	char* query = new char[maxlen];  // ����� ��������


	//------ �������� ���� ��������� ����� ��� ����������� VLAN --------------------------------
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
				if (nAdapter == 0)
				{
					strcpy_s(addr_listen, inet_ntoa(adr.sin_addr));    //���������� ����� ������� ��������
					//�������� ��������� ����� �� 255
					adr.sin_addr.S_un.S_un_b.s_b4 = 255;
					strcpy_s(addr_UDP, inet_ntoa(adr.sin_addr));
				}
				nAdapter++;
			}
		}
	}
	printf("Local Address : \t%s \n", addr_listen);
	printf("Broadcast Address : \t%s \n", addr_UDP);

	// Create a SOCKET for connecting to server
	//SendRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SendRecvSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (SendRecvSocket == INVALID_SOCKET)
	{
		std::cout << "Error in creating socket UDP \n";
		return 1;
	}

	char broadcast = '1';     //������� ������� ��� ����� ??
	if (setsockopt(SendRecvSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
	{
		std::cout << "Error in setting Broadcast option \n";
		closesocket(SendRecvSocket);
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(addr_UDP);   //WIDE ADDRESS FOR LOCAL DOMAIN
	ServerAddr.sin_port = htons(12345);

	//������� ��� ����� �������� ���� ��������� ������ ��������
	_snprintf_s(query, maxlen, maxlen, "WIDE UDP Ask: SERVER CATAN address\n");

	// ���������� ������ �� ������
	sendto(SendRecvSocket, query, strlen(query), 0, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
	printf("Sent: %s\n", query);

	// �������� ��������� - ����� ���� �� ������� ��� ������� �������
	err = recvfrom(SendRecvSocket, recvbuf, maxlen, 0, 0, 0);
	if (err > 0)
	{
		recvbuf[err] = 0;                         //���� ���������� � ����� ������������ �������
		//printf("Client UDP Result: %s\n", (char*)recvbuf);
	}
	else  printf("Client UDP recv failed: %d\n", WSAGetLastError());

	closesocket(SendRecvSocket);

	strcpy(addr_connect, recvbuf);     //����� ��� ����������� � ������� ����� ����
	//============================ UDP CLIENT END ==================================


	mtx1.lock();
	std::cout << "============= CLIENT PART CATAN GAME ===================  " << std::endl;
	std::cout << "Connect to server addr =  " << addr_connect << std::endl;
	mtx1.unlock();

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(addr_connect);    //------------------------------------------------------------------      ADDR
	addr.sin_port = htons(MY_PORT);
	addr.sin_family = AF_INET;      //�������� ��������

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) { std::cout << "ERROR fail connect to server" << std::endl;	  return 0; }

	mtx1.lock();  std::cout << "  OK, Connected to server " << std::endl; mtx1.unlock();

	//��������� ����� ������������� ��������� �� �������
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	//��������� ����� ����� ��������� ��� ����
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientChart, NULL, NULL, NULL);

	return 1;
}

//============================================================================
 //   ������� ���� ������� ��� ����� ��������� ������ �������������
 //============================================================================
int ClientChart()
{
	char msg1[256];

	Sleep(30);   // �������� ������ ������� ������ 
	while (true)
	{
		std::cin.getline(msg1, sizeof(msg1));
		send(Connection, msg1, sizeof(msg1), NULL);
	}

	return 0;
}