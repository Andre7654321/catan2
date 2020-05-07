
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

//внешние глобальные переменные
extern int player_num;
extern int bandit_Gecs;

extern std::vector<GECS>* gecsPtr;    //указатель на вектор гексов
extern std::vector<NODE>* nodePtr;    //указатель на вектор узлов поля
extern std::vector<ROAD>* roadPtr;    //указатель на вектор дорог

//объявляем глобальной чтобы работать с ней в отдельной функции
SOCKET Connection;   //подключение клиента

//для сервера
SOCKET Connections[10] = { 0,0,0,0,0,0,0,0,0,0 };    //подключения игроков 
int Counter = 0;           //счетчик подключений

std::mutex mtx1;    //защищает поток сообщений cout

extern PLAYER player[5];
extern GAME_STEP Game_Step;
extern int CARD_Bank[10];

extern std::string resurs_name[10];

//область обмена ресурсами
extern int ChangeBANK[5][10];

//стандартные сообщения для обмена управлением клиента с сервером
std::string CATAN_command = "$CATAN$";
enum class NET_COMMAND
{
EMPTY,
SET_N_PLAYER,
SET_STEP,
ADD_N_ACTIVE_PLAYER,   //добавляет игрока
SET_N_ACTIVE_PLAYER,   //устанавливает номер игрока которому передается ход
SET_FIRST_PLAYER,      //устанавливает номер игрока который будет ходить первым
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
//  запрос обмена ресурсов с банком
//=======================================================
bool AskChangeWithBank(RESURS CARD_type)
{
char msg[256];
int i;

//проверить есть ли в банке обмена 4 одинаковые карточки
for(i = 1;i < 10;i++)   { if (ChangeBANK[player_num][i] >= 4)  i = 20;  }
if (i < 20){	std::cout << " В банке нехватает ресурсов для обмена " << std::endl;  return false;  }

//общий банк карточек - посылаем, так как мог измениться до обмена
strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
send(Connection, msg, sizeof(msg), NULL);

//ресурсы  игрока - посылаем так как могли измениться до обмена
strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
_itoa(player_num, &msg[50], 10);     //номер игрока
memcpy(&msg[54], player[player_num].resurs, 10 * sizeof(int));   //карточки
send(Connection, msg, sizeof(msg), NULL);

//переслать на сервер свой обменный банк и требуемый от банка ресурс
strcpy_s(msg, Map_Command[NET_COMMAND::ASK_CHANGE_BANK].c_str());
itoa(player_num, &msg[50], 10);
itoa((int)CARD_type, &msg[54], 10);
memcpy(&msg[60], &ChangeBANK[player_num][0], sizeof(int) * 10);
send(Connection, msg, sizeof(msg), NULL);

//в ответ сервер должен обновить банк ресурсов и ресурсы игрока

return true;
}

//=======================================================
//  банк обмена в начале хода должен инициализироваться
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
// посылает инфо по дорогам
//========================================================
void Send_roads()
{
	char msg[256];
	int* intPtr;
	int i = 0;

	//цикл по дорогам
	strcpy_s(msg, Map_Command[NET_COMMAND::ROAD_ARRAY].c_str());   //копируем сначала, так как после может испортить данные в 50 ячейке
	intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //в 1 ячейке номер стартового узла
	intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на 2 элемент массива   0, 1, 2
	for (int p = 0; p < roadPtr->size(); p++)
	{
		*intPtr++ = roadPtr->at(p).owner;
		*intPtr++ = roadPtr->at(p).type;
		i++;
		if (i == 20 || p == roadPtr->size() - 1)                //как пакет достигает 20 узлов - отправляем 
		{
			intPtr = (int*)&msg[50];	*intPtr = i;            //в 50 ячейке количество дорог           
			send(Connection, msg, sizeof(msg), NULL);
			i = 0;
			intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = p + 1;   //в 1 ячейке номер стартового узла
			intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на 2 элемент массива   0, 1, 2
		}
	}

	return;
}

//========================================================
// посылает инфо по узлам
//========================================================
void Send_nodes()
{
char msg[256];
int* intPtr;
int i = 0;

	//цикл по узлам катана
    strcpy_s(msg, Map_Command[NET_COMMAND::NODE_ARRAY].c_str());   //копируем сначала, так как после может испортить данные в 50 ячейке
    intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //в 1 ячейке номер стартового узла в передаваемом пакете
	intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на 2 элемент массива   0, 1, 2
    for (int p = 0; p < nodePtr->size(); p++)
	   {
	   *intPtr++ = nodePtr->at(p).owner;   
	   *intPtr++ = nodePtr->at(p).object;
	   i++;
	   if (i == 20 || p == nodePtr->size()-1)                        //как пакет достигает 20 узлов - отправляем 
	        {
		    intPtr = (int*)&msg[50];	*intPtr = i;                 //в 50 ячейке количество узлов  в пакете         
			send(Connection, msg, sizeof(msg), NULL);
			i = 0;
			intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = p+1;   //в 1 ячейке номер стартового узла
			intPtr = (int*)&msg[50];    intPtr += 2;                 //встаем на 2 элемент массива   0, 1, 2
	        }
	   }

	return;
}

//========================================================================
// вызывается при отмене части ходя для восстановления состояния
// запрос на пересылку от сервера числа городов, деревень, дорог, 
// ??  карт развития ??
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
// вызывается при отмене части ходя для восстановления состояния
// запрос на пересылку от сервера банка ресурсов и ресурсов игроков
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
// запрос на пересылку от сервера дорог и узлов
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
// посылает сообщение что ход завершен
//========================================================
void Say_Move_Over()
{
	char msg[256];

	std::cout << " Ход завершен " << std::endl;

	//после строительства передать инфо по дорогам и узлам
	Send_nodes();	
	Send_roads();

	//после строительства передать инфо по своим ресурсам и банку ресурсов на сервер
	//общий банк карточек
	strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
	memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
	send(Connection, msg, sizeof(msg), NULL);

	//ресурсы  игрока
    strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
	_itoa(player_num, &msg[50], 10);     //номер игрока
	memcpy(&msg[54], player[player_num].resurs, 10 * sizeof(int));   //карточки
	send(Connection, msg, sizeof(msg), NULL);

	//деревни, города .. игрока
	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_OBJECTS].c_str());
	_itoa(player_num, &msg[50], 10);                                 //номер игрока
	_itoa(player[player_num].town, &msg[60], 10);                    //города
	_itoa(player[player_num].village, &msg[70], 10);                 //деревни
	_itoa(player[player_num].road, &msg[80], 10);                    //дороги
	send(Connection, msg, sizeof(msg), NULL);


	//на 3 шаге переслать номер узла поставленной последней деревни
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
// посылает сообщение что кинут  1 кубик
//========================================================
void Say_Roll_2Dice()
{
	char msg[256];

	std::cout << " Вы бросили  кубики " << std::endl;

	// сообщение о броске кубика
	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ROLL_2DICE].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);
	Sleep(10);

	return;
}

//========================================================
// посылает сообщение что кинут  1 кубик
//========================================================
void Say_Roll_1Dice()
{
char msg[256];

	std::cout << " Вы бросили  кубик " << std::endl;

	// сообщение о броске кубика
	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ROLL_1DICE].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);
	Sleep(10);

return;
}

//========================================================
// посылает сообщение серверу о старте игры
//========================================================
void Say_Start()
{
char msg[256];

	if (player_num != 1) return;
	std::cout << " func say start " << std::endl;
	//if (Count_Num_players() < 2)  return;

	// сообщение серверу о старте игры
	strcpy_s(msg, Map_Command[NET_COMMAND::SAY_GAME_START].c_str());
	send(Connection, msg, sizeof(msg), NULL);
	Sleep(10);

return;
}

//========================================================
// посылает сообщение серверу о месте разбойников
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
// подсчет числа подключившихся игроков
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
 //функция клиента для обработки сообщений от серверной части
 //при получении пакета выводит сообщение в чат если это сообщение игрока
 //если это системное сообщение CATAN обрабатывает
 //==============================================================================================
void ClientHandler(int index)
{
	int ret = 1;
	int i;
	char* ptr;
	char msg[256];   //--------------------------------------------------------------------------
	std::string str;
	NET_COMMAND Command = NET_COMMAND::EMPTY;


	while (true)            //бесконечный цикл в потоке обработки входных сообщений
	{
		ret = recv(Connection, msg, sizeof(msg), NULL);
		if (ret == SOCKET_ERROR)
		    {
			//не заходим сюда и выводим в бесконечном цикле последнее сообщение
			std::cout << " SOCKET_ERROR , Close SOCKET " << std::endl;
			closesocket(Connection);
			Connection = 0;
			return;
		    }
		if (ret == 0) { ret = 1; std::cout << " 0 byte recieved " << std::endl; }



		if (Connection != 0)
		    {
			str.assign(msg, 0, 7);    //берем первые 7 символов сообщения

			 //если системное сообщение $CATAN&
			if (str.compare(CATAN_command) == 0)   //сравнение со строкой $CATAN&
			    {
				str.assign(msg, 0, strlen(msg));
				//std::cout << " Get CATAN COMMAND  " << std::endl;

				//определить тип команды и вывести сообщение (В str у нас первая строка взятая из пакета)
				for (const auto &key : Map_Command)   {	if (key.second == str)  Command = key.first;   }

				//std::cout << " Текст команды: " << Map_Command[Command]  << std::endl;
				
				if (Command == NET_COMMAND::SET_N_PLAYER)
				    {
					player_num = atoi(&msg[50]);	player[player_num].active = true;    //в своей структуре активируем игрока
					std::cout << "Вам присвоен номер: " << player_num << std::endl;
					continue;
				    }

				if (Command == NET_COMMAND::ADD_N_ACTIVE_PLAYER)  //инфо о подключении нового игрока
				   {
					mtx1.lock();  std::cout << "Подключился игрок №: " << atoi(&msg[50]) << std::endl; mtx1.unlock();
					player[atoi(&msg[50])].active = true;    //в своей структуре активируем игрока
					continue;
				    }

				if (Command == NET_COMMAND::SET_FIRST_PLAYER)  //
				{
					mtx1.lock();  std::cout << "Первым ходящим назначен игрок №: " << atoi(&msg[50]) << std::endl;  mtx1.unlock();
					Game_Step.start_player = atoi(&msg[50]);
					continue;
				}

				if (Command == NET_COMMAND::SET_STEP)  
				    {
					//std::cout << "Получен номер шага игры " << atoi(&msg[50]) << std::endl;
					Game_Step.current_step = atoi(&msg[50]);
					continue;
				    }

				if (Command == NET_COMMAND::SET_N_ACTIVE_PLAYER)
				{
					if(Game_Step.current_step == 4)  Game_Step.step[4].roll_2_dice = 1;
					Game_Step.current_active_player = atoi(&msg[50]);
					InitChange_BANK();    //в начале хода банк обмена обнуляется
					continue;
				}

				if (Command == NET_COMMAND::SET_GECS)
					{
					int gecs_num = atoi(&msg[50]);
					//std::cout << "Получен гекс номер:  " << gecs_num  << std::endl;

					auto it = gecsPtr->begin();
					//копируем полученный массив по адресу гекса
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
					memcpy(player[pl].resurs, &msg[54], 10 * sizeof(int));   //карточки
					continue;
				}

				if (Command == NET_COMMAND::PLAYER_OBJECTS)    //у сервера попросил игрок обновить и шлем только ему 
				{
					//число городов, деревень, дорог игрока
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
						   std::cout << " Выпало 7 , переместите разбойников " << std::endl;
						   Game_Step.step[4].flag_bandit = 1;
					       }
					else
					   {
						if ((CARD_Bank[(int)RESURS::WOOD] + CARD_Bank[(int)RESURS::BREAD] + CARD_Bank[(int)RESURS::STONE] + 
							      CARD_Bank[(int)RESURS::BRICKS] + CARD_Bank[(int)RESURS::OVCA]) >= 8)
						    {
							std::cout << " Выпало 7 , надо сбросить половину ресурсов " << std::endl;
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
			//std::cout << " CLIENT: " << msg << "  Connection = " << Connection << std::endl;    //если сокет не закрыт
			std::cout << " CLIENT: " << msg  << std::endl;
			mtx1.unlock();
			}  //if (Connection != 0)
		}
    return;
	}

//===============================================================================================
// инициализация сервера игры 
//===============================================================================================
int Start_Server_CATAN()
{
	init_WSA();

	//Создаем отдельный поток, где обрабатываются подключения игроков
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Check_Connections, NULL, NULL, NULL);

	//std::thread t_server(Check_Connections);
	//t_server.join();

	Sleep(50);
	return 1;
}

//===============================================================================================
// инициализация клиента игры 
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
	addr.sin_family = AF_INET;      //интернет протокол

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)	{ std::cout << "ERROR fail connect to server" << std::endl;	  return 0; 	}

	mtx1.lock();  std::cout << "  OK, Connected to server " << std::endl; mtx1.unlock();

	//Запускаем поток прослушивания сообщений от сервера
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	//запустить поток ввода сообщений для чата
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientChart, NULL, NULL, NULL);

	return 1;
}

//===============================================================================================
// инициализация WSA Data
//===============================================================================================
int init_WSA(void)
{
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) { std::cout << " ERROR WSA " << std::endl;  exit(1); }

	return 0;
}

//==============================================================================================  CONNECT
//     Функция потока подключений серверная часть =
//==============================================================================================
void Check_Connections(void)
{
	int i;
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_addr.s_addr = inet_addr("192.168.1.118");    //--------------------------------------------------------
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;      //интернет протокол

	//слушающий сокет
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	char msg[256] = "";     //будем обмениваться фиксированными пакетами 256
	char str[40];

	std::cout << " ======  SERVER: Старт цикла ожидания подключений ====== \n  " << std::endl;

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

			//отправка стартовых сообщений подключившейся станции, выдача номера игроку
			strcpy_s(msg, "Hello. It's CATAN GAME ");
			send(newConnection, msg, sizeof(msg), NULL);
			Sleep(10);

			// выдача номера игроку
			strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_PLAYER].c_str());
			_itoa(Counter + 1, &msg[50], 10);
			send(newConnection, msg, sizeof(msg), NULL);
			_itoa(i, msg, 10);  strcat_s(msg,"  -   номер соединения ");
			send(newConnection, msg, sizeof(msg), NULL);

			//передача сформированного поля - гексы и номера - остальное клиент сформирует
			int gecs_num = 0;
			for (auto& elem : *gecsPtr)
			   {
			   strcpy_s(msg, Map_Command[NET_COMMAND::SET_GECS].c_str());
			   //надо записать в строку данные гекса
			   _itoa(gecs_num++, &msg[50], 10);       //на 50 позиции номер гекса
			   memcpy(&msg[60], &elem, sizeof(GECS));
			   send(newConnection, msg, sizeof(msg), NULL);
			   }

			//сообщить о месте расположения разбойников
			strcpy_s(msg, Map_Command[NET_COMMAND::BANDIT_GECS].c_str());
			_itoa(bandit_Gecs, &msg[50], 10);       
			send(newConnection, msg, sizeof(msg), NULL);

			//если игрок подключился не первый, то ему надо сообщить о ранее подключившихся
			if(Counter > 0)
			   {
			   for(int ii = 0;ii < Counter;ii++)
			      {
				   strcpy_s(msg, Map_Command[NET_COMMAND::ADD_N_ACTIVE_PLAYER].c_str());   _itoa(ii+1, &msg[50], 10);
				   send(newConnection, msg, sizeof(msg), NULL);
			      }
			   }

			//уведомить активных игроков о подключении нового
			for(int ii = 0;ii < 10;ii++)
			  {
				if (Connections[ii])
				{
					strcpy_s(msg, Map_Command[NET_COMMAND::ADD_N_ACTIVE_PLAYER].c_str());	_itoa(Counter + 1, &msg[50], 10);
					send(Connections[ii], msg, sizeof(msg), NULL);
				}
			  }

			player[Counter+1].active = true;  //регистрируем нового игрока
			Connections[i] = newConnection;
			Counter++;

			//каждому подключенному клиенту создаем поток обработки сообщений сокета
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ServerClientStreamFunc, (LPVOID)(i), NULL, NULL);
		}   //else
	}

	std::cout << " SERVER: Максимальное кол-во игроков 4 подключилось \n  " << std::endl;
	std::cout << " SERVER: Поток подключений завершает работу \n  " << std::endl;
}

//================================= SERVER STREAM FUNCTION ======================================  SERVER NET
//функция для приема сообщений от клиентов и отправке всем, кроме того от кого оно пришло прислал
//===============================================================================================
void ServerClientStreamFunc(int index)
{
	char msg[256];   //-------------------------------------
	char msg1[10];
	int ret;
	std::string str;
	NET_COMMAND Command;
	bool system = false;

	//запрашивать время и каждые 20сек отправлять тестовое сообщение клиентской части
	//так можно смотреть за работоспособностью чата на прием

	while (true)
	{
		//чтение из сокета  так как функция обрабатывает 1 сокет, то висит в ожидании поступления данных и только
		//получив данные или код ошибки от RECV проваливается на его обработку.
		//при этом виящая функция администрируется операционной системой и не мешает многопоточности
		ret = recv(Connections[index], msg, sizeof(msg), NULL);
		if (ret == INVALID_SOCKET)
		    {
			std::cout << "   ERROR READING SOCKET   " << index << std::endl;
			Connections[index] = 0; 	closesocket(Connections[index]);	return;
		    }

		//выбрать из сообщений системные CATAN ==================================================================
		str.assign(msg, 0, 7);    //берем первые 7 символов сообщения

	    //если системное сообщение $CATAN&
		if (str.compare(CATAN_command) == 0)   //сравнение со строкой $CATAN&
		{
			str.assign(msg, 0, strlen(msg));  //копируется 0 термин строка без бинарного хвоста в начало строки str
			//std::cout << " Get CATAN COMMAND  " << std::endl;

			//определить тип команды перебором сравнения массива со строкой  
			//(В str у нас первая строка взятая из пакета)
			for (const auto& key : Map_Command)
			    {
				if (key.second == str)  Command = key.first;
			    }

			//std::cout << " Текст команды: " << Map_Command[Command] << std::endl;

			if (Command == NET_COMMAND::SAY_GAME_START)
			{
				
				mtx1.lock();  std::cout << "  START GAME Command !!!  " << std::endl; mtx1.unlock();

				Game_Step.current_step = 1;
				Game_Step.current_active_player = 1;

				strcpy_s(msg, " ================================ Игра стартовала ===================================");
				Send_To_All(msg, sizeof(msg));
				
				//прекратить подключать новых игроков

				//сообщить всем номер хода
				strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	_itoa(Game_Step.current_step, &msg[50], 10);
				Send_To_All(msg, sizeof(msg));

				//передать всем что ход передан игроку №1 для броска кубика
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
				//переслать дороги клиентам
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
				//переслать узлы клиентам
				Send_To_All(msg, sizeof(msg));
				continue;
			}

			if (Command == NET_COMMAND::ASK_NODE_ARRAY)
			{
			int* intPtr;
			int i=0,p;
				//послать станциям массив узлов
				//цикл по узлам катана
				strcpy_s(msg, Map_Command[NET_COMMAND::NODE_ARRAY].c_str());   //копируем сначала, так как после может испортить данные в 50 ячейке
				intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //в 1 ячейке номер стартового узла
				intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на 2 элемент массива   0, 1, 2
				for (int p = 0; p < nodePtr->size(); p++)
				{
					*intPtr++ = nodePtr->at(p).owner;
					*intPtr++ = nodePtr->at(p).object;
					i++;
					if (i == 20 || p == nodePtr->size() - 1)                //как пакет достигает 20 узлов - отправляем 
					{
						intPtr = (int*)&msg[50];	*intPtr = i;                                 //в 50 ячейке количество узлов           
						Send_To_All(msg, sizeof(msg));
						i = 0;
						intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = p + 1;   //в 1 ячейке номер стартового узла
						intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на 2 элемент массива   0, 1, 2
					}
				}
			continue;
			}

			if (Command == NET_COMMAND::ASK_ROAD_ARRAY)
			{
				int* intPtr;
				int i = 0, p;
				//послать станциям состояние дорог
				strcpy_s(msg, Map_Command[NET_COMMAND::ROAD_ARRAY].c_str());   //копируем сначала, так как после может испортить данные в 50 ячейке
				intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //в 1 ячейке номер стартового узла
				intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на 2 элемент массива   0, 1, 2
				for (int p = 0; p < roadPtr->size(); p++)
				{
					*intPtr++ = roadPtr->at(p).owner;
					*intPtr++ = roadPtr->at(p).type;
					i++;
					if (i == 20 || p == roadPtr->size() - 1)                //как пакет достигает 20 узлов - отправляем 
					{
						intPtr = (int*)&msg[50];	*intPtr = i;            //в 50 ячейке количество дорог           
						Send_To_All(msg, sizeof(msg));
						i = 0;
						intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = p + 1;   //в 1 ячейке номер стартового узла
						intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на 2 элемент массива   0, 1, 2
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
				//std::cout << " Завершение хода игрок  " << pl << std::endl;
				if ( Game_Step.current_step == 4)
				    {
					Game_Step.current_active_player = GetNextPlayer();
					std::cout << "SERVER  Передача хода игроку  " << Game_Step.current_active_player << std::endl;
					strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	_itoa(Game_Step.current_active_player, &msg[50], 10);
					Send_To_All(msg, sizeof(msg));
					InitChange_BANK();
					continue;
				    }
				if (Game_Step.current_step == 3)   Get_Resurs(player[pl].last_village_node,pl);    //распред ресурсы для деревни
				Send_To_All_Info_Resurs();
				if (Game_Step.current_active_player == Game_Step.start_player && Game_Step.current_step == 3)
				      {
					  Game_Step.current_step = 4;     //старт стандартных ходов игры
					  std::cout << " ================= Переход на основную игру  =====================  " << pl << std::endl;
					  //сообщить всем номер хода
					  strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	_itoa(Game_Step.current_step, &msg[50], 10);
					  Send_To_All(msg, sizeof(msg));
					  continue;
				      }
				if (GetNextPlayer() == Game_Step.start_player && Game_Step.current_step == 2)
				{
					std::cout << " ================= Переход на 3 шаг =====================  " << player << std::endl;
					//на след шаг
					if (Game_Step.current_step == 2)   Game_Step.current_step = 3;

					//сообщить всем номер хода
					strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	_itoa(Game_Step.current_step, &msg[50], 10);
					Send_To_All(msg, sizeof(msg));
				}
				  else
				    { 
					if(Game_Step.current_step == 2)  Game_Step.current_active_player = GetNextPlayer();
					if(Game_Step.current_step == 3)  Game_Step.current_active_player = GetPrevPlayer();
					mtx1.lock();  std::cout << "SERVER  Передача хода игроку  " << Game_Step.current_active_player << std::endl;  mtx1.unlock();
					//передать всем что ход передан игроку № 
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

			//сообщить всем результат броска кубика - не системное простое оповещение
			strcpy(msg, "Player "); _itoa(pl, msg1, 10); strcat(msg, msg1); 
			strcat(msg, " roll  -   ");  _itoa(player[pl].first_roll, msg1, 10);   strcat(msg,msg1);
			Send_To_All(msg, sizeof(msg));

            //если не все бросили кубик передать ход след игроку, если все то след шаг игры
			if (GetNextPlayer() == Game_Step.start_player)
			{
				//mtx1.lock();  std::cout << "GetNextPlayer() == 0  говорит что все кинули " << std::endl; mtx1.unlock();
				SetFirstPlayer();    //назначает по результатам броска 1 кубика очередность хода

				//сообщать клментам о назначении первого игрока
				strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	_itoa(Game_Step.current_active_player, &msg[50], 10);
				Send_To_All(msg, sizeof(msg));
			}
			else  Game_Step.current_active_player = GetNextPlayer();

			strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	_itoa(Game_Step.current_step, &msg[50], 10);
			Send_To_All(msg, sizeof(msg));
		
			mtx1.lock();  std::cout << "SERVER  Передаю ход игроку  " << Game_Step.current_active_player << std::endl;  mtx1.unlock();
			//передать всем что ход передан игроку № для броска кубика
			strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	_itoa(Game_Step.current_active_player, &msg[50], 10);
			Send_To_All(msg, sizeof(msg));    
			continue;
			}

			if (Command == NET_COMMAND::ASK_ROLL_2DICE)
			{
				int pl = atoi(&msg[50]);

				//srand(time(0));
				int dice2 = rand() % 6 + 1  + rand() % 6 + 1;   //2 кубика

				//сообщить всем результат броска кубика - не системное простое оповещение 
				std::cout << " Бросок кубиков =  " <<  dice2  << std::endl;
				strcpy(msg, "Player "); _itoa(pl, msg1, 10); strcat(msg, msg1);
				strcat(msg, " roll  -   ");  _itoa(dice2, msg1, 10);   strcat(msg, msg1);
				Send_To_All(msg, sizeof(msg));

				if(dice2 == 7)
				    {
					strcpy_s(msg, Map_Command[NET_COMMAND::DICE_SEVEN].c_str());	_itoa(pl, &msg[50], 10);
					Send_To_All(msg, sizeof(msg));
					continue;
				    }

				//распределить ресурсы по результатам броска - цикл по узлам катана
				Step_Resurs(dice2);
				Send_To_All_Info_Resurs();
				continue;
			}

			if (Command == NET_COMMAND::INFO_BANK_RESURS)  //серверу прислали - он все дублирует
			{
				int* intPtr = (int*)&msg[50];
				memcpy(CARD_Bank, &msg[50], 10 * sizeof(int));
				Send_To_All(msg, sizeof(msg));
				continue;
			}

			if (Command == NET_COMMAND::ASK_BANK_RESURS)    //у сервера попросил клиент и только ему шлем
			{
				int pl = atoi(&msg[50]);
				//общий банк карточек
				strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
				memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
				send(Connections[pl-1], msg, sizeof(msg), NULL);
				continue;
			}

			if (Command == NET_COMMAND::ASK_PLAYER_RESURS)    //у сервера попросил игрок обновить и только ему шлем
			{
				int pl = atoi(&msg[50]);
				//банк карточек игрока
				strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
				_itoa(pl, &msg[50], 10);                                 //номер игрока
				memcpy(&msg[54], player[pl].resurs, 10 * sizeof(int));   //карточки
				send(Connections[pl - 1], msg, sizeof(msg), NULL);
				continue;
			}

			if (Command == NET_COMMAND::ASK_PLAYER_OBJECTS)    //у сервера попросил игрок обновить и шлем только ему 
			{
				int pl = atoi(&msg[50]);
				//число городов, деревень, дорог игрока
				strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_OBJECTS].c_str());
				_itoa(pl, &msg[50], 10);                                 //номер игрока
				_itoa(player[pl].town, &msg[60], 10);                    //города
				_itoa(player[pl].village, &msg[70], 10);                 //деревни
				_itoa(player[pl].road, &msg[80], 10);                    //дороги
				send(Connections[pl - 1], msg, sizeof(msg), NULL);
				continue;
			}

			if (Command == NET_COMMAND::PLAYER_OBJECTS)    //у сервера попросил игрок обновить и шлем только ему 
			{
				int pl = atoi(&msg[50]);
				//число городов, деревень, дорог игрока
				player[pl].town = atoi(&msg[60]);
				player[pl].village = atoi(&msg[70]);
				player[pl].road = atoi(&msg[80]);

				continue;
			}

			if (Command == NET_COMMAND::PLAYER_RESURS)         //сервер получил банк игрока и дублирует всем
			{
				int pl = atoi(&msg[50]);
				memcpy(player[pl].resurs, &msg[54], 10 * sizeof(int));   //карточки
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

				std::cout << " Запрос на get " << resurs_name[type_put] << " from pl " << pl  <<  std::endl;

				//найти 4 карточки одного типа, вычесть их из банка игрока
				int type_get = 0;
				for(int i = 1;i < 10;i++)
				   {
					if (ChangeBANK[pl][i] >= 4) { type_get = i; break; }
				   }

				if (type_get == 0)
				    {
					std::cout << " не нашлось 4 карт "  << std::endl;
					continue;     //карт для обмена не нашлось
				    }
				//проверить если запрашиваемый ресурс в общем банке
				if (CARD_Bank[type_put] < 1)    continue;

				//перенести карты в банках
				player[pl].resurs[type_put] += 1;   CARD_Bank[type_put] -= 1;
				player[pl].resurs[type_get] -= 4;   CARD_Bank[type_get] += 4;

				Send_To_All_Info_Resurs();

				continue;
			}

		continue;
		}  //закончена обработка , если в команде нет CONTINUE, то она будет дублирована всем


		//повторить отправку сообщения всем кроме INDEX если это чат
		for (int i = 0; i < 5; i++)
		{
			if (i == index )    continue;     // в index  не отправляем
			if (Connections[i] == 0)    continue;
			//рассылка получателям
			if (send(Connections[i], msg, sizeof(msg), NULL) == INVALID_SOCKET)
			{
				std::cout << "  ERROR SEND  " << std::endl;   //вероятно процедуру сбоя связи надо менять для возможности восстановления подключения к серверу
				closesocket(Connections[i]);	Connections[i] = 0;
			}

		}   //for
	}
}

//=======================================================================
//отправка сообщения всем игрокам
//=======================================================================
void Send_To_All(char* msg,int size)
{
	//std::cout << "  To ALL - " <<  msg  << std::endl;
	for (int i = 0; i < 5; i++)
	{
	if (Connections[i] == 0)    continue;
		
		if (send(Connections[i], msg, size, NULL) == INVALID_SOCKET)
		{
		std::cout << "  ERROR SEND  " << std::endl;   //вероятно процедуру сбоя связи надо менять для возможности восстановления подключения к серверу
		closesocket(Connections[i]);	Connections[i] = 0;
		}
	} 

return;
}

//=======================================================================
//отправка сообщения о ресурсах от сервера всем игрокам
//=======================================================================
void Send_To_All_Info_Resurs()
{
 char msg[256];

 //общий банк карточек
 strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
 memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
 Send_To_All(msg,sizeof(msg));

 //ресурсы каждого игрока
 for (int i = 1; i < 5; i++)
    {
	 strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
	 _itoa(i, &msg[50], 10);     //номер игрока
	 memcpy(&msg[54],player[i].resurs, 10 * sizeof(int));   //карточки
	 Send_To_All(msg, sizeof(msg));
    }

 return;
}

//============================================================================
 //   функция чата клиента для ввода сообщений другим пользователям
 //============================================================================
int ClientChart()
{
	char msg1[256];

	Sleep(30);   //попытка избежать ошибки запуска потока 
	while (true)
	{
		std::cin.getline(msg1, sizeof(msg1));
		send(Connection, msg1, sizeof(msg1), NULL);
		Sleep(10);
	}

	return 0;
}



