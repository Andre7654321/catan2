
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

extern SOCKET Connection;   //подключение клиента

//extern char addr_listen[20] = "192.168.2.39";
//extern char addr_UDP[20] = " ";
//extern char addr_connect[20] = "192.168.2.39";

//для сервера
extern SOCKET Connections[10];    //6 + server подключения игроков
extern char str_Player_addr[7][20];
//extern int Counter = 0;           //счетчик подключений

extern std::mutex mtx1;    //защищает поток сообщений cout

extern std::string CATAN_command;      // = "zCATANz";
//map класс со строковыми именами сетевых комманд
extern std::map<NET_COMMAND, std::string> Map_Command;

//внешние глобальные переменные
extern int player_num;
extern int bandit_Gecs;
extern int max_road_owner;
extern int max_army;        //владелец карточки самое большое войско

extern int Play_two_roads;     //флаг игры карты развития 2 дороги
extern int Play_two_resurs;    //флаг игры карты развития 2 ресурса
extern int Play_ONE_resurs;
extern int Allow_Develop_card;

extern bool flag_cubic_was_used;   //сервер запоминает что текущий игрок уже бросал кубик
extern bool flag_develop_card_used;  //сервер запоминает что карта развития уже игралась в этом ходу

extern std::vector<GECS>* gecsPtr;    //указатель на вектор гексов
extern std::vector<NODE>* nodePtr;    //указатель на вектор узлов поля
extern std::vector<ROAD>* roadPtr;    //указатель на вектор дорог

extern PLAYER player[7];
extern GAME_STEP Game_Step;
extern int CARD_Bank[10];
extern CHANGE Change[12];       //сделки обмена игроков
extern std::vector<IMP_CARD> improve_CARDS;
extern std::vector<IMP_CARD> develop_CARDS[7];
extern int limit_7[7];

extern std::string resurs_name[10];
extern int Big_Message;              //номер ресурса для вывода сообщения
extern std::chrono::time_point<std::chrono::steady_clock>  start_Big_Message;

//область обмена ресурсами
extern int ChangeBANK[7][10];


//=======================================================
// Запрос на выполнение обмена по заявке
//=======================================================
void AskToAcceptOffer(int s)
{
CATAN_CLIENT_Command(NET_COMMAND::ASK_ACCEPT_OFFER, (char*)&s, sizeof(int));
return;
}

//=======================================================
// Запрос на отмену заявки на обмен с игроком
//=======================================================
void AskToDeleteOffer(int s)
{
CATAN_CLIENT_Command(NET_COMMAND::ASK_DELETE_OFFER, (char*)&s, sizeof(int));
return;
}

//=======================================================
// Запрос на обмен картами ресурсов с игроком
//=======================================================
void AskChangeWithPlayer(int pl)
{
    //все обмены с игроками - до строительства, потому инфо по объектам не отправляем
	//Info_Player_Resurs();
	//на сервер надо передать номер игрока  с кем обмен 
	//а карточки находятся в банках обмена - передавать не надо

	CATAN_CLIENT_Command(NET_COMMAND::SET_CHANGE_OFFER, (char*)&pl, sizeof(int));

	return;
}

//=======================================================
// запрос серверу перезапустить игру
//=======================================================
void Test_Game()
{
std::cout << " ASK FOR TEST MODE STEP 4 " << std::endl;
CATAN_CLIENT_Command(NET_COMMAND::TEST_GAME, nullptr, NULL);
return;
}

//=======================================================
// запрос серверу перезапустить игру
//=======================================================
void Ask_Reset_Game()
{
CATAN_CLIENT_Command(NET_COMMAND::ASK_RESET_GAME,nullptr,NULL);
return;
}

//=======================================================
// Запрос забрать у игроков ресурс
//=======================================================
void Ask_Server_To_Take_Resurs_From_All(RESURS type)
{
//обновляем серверу свои ресурсы
Info_Player_Resurs();

//std::cout << " запрос серверу забрать все   " << resurs_name[(int)type] << std::endl;
CATAN_CLIENT_Command(NET_COMMAND::ASK_GET_RESURS_FROM_ALL, (char*)&type, sizeof(int));
return;
}

//=======================================================
// Запрос сыграть карту развития
//=======================================================
void AskPlayDevelopCard(IMP_TYPE type)
{
	Send_nodes();	Send_roads();
	Info_Player_Resurs();   //поменялось при строительстве
	Info_Main_Bank();       //поменялось при строительстве
	//деревни, города .. игрока
	Player_Objects_To_Server();

	CATAN_CLIENT_Command(NET_COMMAND::ASK_PLAY_DEVELOP_CARD, (char*)&type, sizeof(int));
	return;
}

//=======================================================
//  запрос покупки карты развития
//=======================================================
bool AskBuyImproveCARD()
{ 
	Send_nodes();	Send_roads();
	Info_Player_Resurs();   //поменялось при строительстве
	Info_Main_Bank();       //поменялось при строительстве
	//деревни, города .. игрока
	Player_Objects_To_Server();

	CATAN_CLIENT_Command(NET_COMMAND::ASK_BUY_IMPROVE_CARD,nullptr,NULL);
	//в ответ сервер должен обновить банк ресурсов и ресурсы игрока
	return true;
}

//=======================================================
// Запрос забрать карту у игрока PL
//=======================================================
void AskTakeRandomCardFromPlayer(int pl)
{
CATAN_CLIENT_Command(NET_COMMAND::TAKE_CARD_FROM_PLAYER, (char*)&pl, sizeof(int));
return;
}

//========================================================
// посылает сообщение серверу о старте игры
//========================================================
void Say_Start()
{
	if (player_num != 1) return;
	CATAN_CLIENT_Command(NET_COMMAND::SAY_GAME_START, nullptr, NULL);
	return;
}

//=======================================================
//  запрос на сброс карт в банк (обработка 7)
//=======================================================
void AskSendCardsToBank()
{
//переслать на сервер коменду сбросить банк обмена в общий банк
CATAN_CLIENT_Command(NET_COMMAND::SEND_CARDS_TO_BANK, nullptr, NULL);
return;
}

//=======================================================
//  запрос обмена ресурсов с банком
//=======================================================
bool AskChangeWithBank(RESURS CARD_type)
{
	int i;
	int need_card;

	//проверить есть ли в банке обмена нужное количество одинаковых карточек
	bool flag = false;
	for (i = 1; i < 10; i++)
	{
		need_card = 4;
		if (bonus31(player_num))      need_card = 3;
		if (bonus21(player_num, i))   need_card = 2;
		if (ChangeBANK[player_num][i] >= need_card)  flag = true;
	}
	if (flag == false) { std::cout << " В банке нехватает ресурсов для обмена " << std::endl;  return false; }

	//информируем банк о завершенных постройках в узлах
	//так как бонус по картам мог появиться в процессе хода
	Send_nodes();	Send_roads();
	Info_Player_Resurs();   //поменялось при строительстве
	Info_Main_Bank();       //поменялось при строительстве
	//деревни, города .. игрока
	Player_Objects_To_Server();

	//переслать на сервер требуемый от банка ресурс
	CATAN_CLIENT_Command(NET_COMMAND::ASK_CHANGE_WITH_BANK, (char*)&CARD_type,sizeof(int));
	return true;
}

//========================================================================
// вызывается при отмене части хода для восстановления состояния
// запрос серверу на пересылку числа городов, деревень, дорог, 
//========================================================================
void Ask_Send_Objects()
{
CATAN_CLIENT_Command(NET_COMMAND::ASK_PLAYER_OBJECTS,nullptr,NULL);
return;
}

//=======================================================
//  сообщение серверу об изменении в банке обмена
//=======================================================
void Info_Change_Bank()
{
//переслать на сервер свой обменный банк
CATAN_CLIENT_Command(NET_COMMAND::INFO_CHANGE_BANK, (char*)&ChangeBANK[player_num][0], 10* sizeof(int));
return;
}

//========================================================
// посылает сообщение серверу о новом месте разбойников
//========================================================
void Say_Move_Banditos()
{
CATAN_CLIENT_Command(NET_COMMAND::BANDIT_GECS, (char*)&bandit_Gecs, sizeof(int));
return;
}

//========================================================================
// вызывается при отмене части хода для восстановления состояния
// запрос на пересылку от сервера банка ресурсов и ресурсов игроков
//========================================================================
void Ask_Send_Resurs()
{
	CATAN_CLIENT_Command(NET_COMMAND::ASK_BANK_RESURS,nullptr,NULL);
	CATAN_CLIENT_Command(NET_COMMAND::ASK_PLAYER_RESURS, nullptr, NULL);
}

//========================================================
// посылает сообщение что ход завершен
//========================================================
void Say_Move_Over()
{
	if (player_num != Game_Step.current_active_player)  return;
	//отмена всех предлож на обмен
	for (int i = 0; i < 12; i++)  Change[i].status = 0;

	//после строительства передать инфо по дорогам и узлам
	Send_nodes();	Send_roads();
	Info_Player_Resurs();   //поменялось при строительстве
	Info_Main_Bank();       //поменялось при строительстве
	//деревни, города .. игрока
	Player_Objects_To_Server();

	//на 3 шаге переслать номер узла поставленной последней деревни
	if (Game_Step.current_step == 3)
	    {
	    CATAN_CLIENT_Command(NET_COMMAND::LAST_VILLAGE, (char*)&player[player_num].last_village_node,sizeof(int));
	    }

	//само сообщение о завершении хода
	CATAN_CLIENT_Command(NET_COMMAND::SAY_MOVE_OVER,nullptr,NULL);
	return;
}

//========================================================
// посылает сообщение что кинут  1 кубик
//========================================================
void Say_Roll_Start_Dice()
{
	int rnum = 0;

	rnum = Random_Number(1, 6);
	std::cout << " Вы бросили  кубик =  " <<  rnum  << std::endl;

	CATAN_CLIENT_Command(NET_COMMAND::SAY_ROLL_START_DICE, (char*)&rnum, sizeof(int));

	return;
}

//========================================================
// посылает сообщение что кинуто  2 кубика
//========================================================
void Say_Roll_2Dice()
{
    int rnum = 0;

	rnum = Random_Number(1, 6);
	Random_Number(2, 16);
	Random_Number(0, 22);
	rnum += Random_Number(1, 6);

	//std::cout << " Вы бросили  кубик =  " << rnum << std::endl;

	CATAN_CLIENT_Command(NET_COMMAND::ASK_ROLL_2DICE, (char*)&rnum, sizeof(int));
	return;
}

//=======================================================
// Переслать состояние ресурсов игрока на сервер
//=======================================================
void Info_Player_Resurs()
{
CATAN_CLIENT_Command(NET_COMMAND::PLAYER_RESURS, (char*)&player[player_num].resurs, 10 * sizeof(int));
return;
}

//=======================================================
//  фишки игрока на сервер
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
//  переслать состояние ресурсов на сервер
//=======================================================
void Info_Main_Bank()
{
CATAN_CLIENT_Command(NET_COMMAND::INFO_BANK_RESURS, (char*)&CARD_Bank[0], 10 * sizeof(int));
return;
}

//========================================================
// клиент посылает инфо по дорогам на сервер
//========================================================
void Send_roads()
{
	//по 2 int значения на road
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
// посылает инфо по узлам - о постройках на сервер
//========================================================
void Send_nodes()
{
	//по 2 int значения на node
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
// запрос на пересылку от сервера дорог и узлов
//========================================================
void Ask_Send_Arrays()
{
	CATAN_CLIENT_Command(NET_COMMAND::ASK_NODE_ARRAY, nullptr, NULL);
	CATAN_CLIENT_Command(NET_COMMAND::ASK_ROAD_ARRAY, nullptr, NULL);
	return;
}

//=======================================================================
//отправка сделки на обмен от одного игрока другому
//=======================================================================
void Send_Info_Change(int pl,int s)
{
char buff[sizeof(CHANGE) + sizeof(int) + 10];
int* IntPtr;
IntPtr = (int*)buff;

    if (s < 0 || s >= 12)   { Beep(900, 1000);  return; }
    *IntPtr = s;	 IntPtr++;   //номер сделки                             
	memcpy(IntPtr, &Change[s], sizeof(CHANGE)); 

	CATAN_SERVER_Command(NET_COMMAND::CHANGE_OFFER, buff, sizeof(CHANGE) +  sizeof(int), pl);
	return;
}

//=======================================================================
//отправка сообщения о ресурсах от сервера всем игрокам
//=======================================================================
void Send_To_All_Info_Resurs()
{
	//общий банк карточек
	Send_Bank_Resurs(TO_ALL);

	//общий банк карт развития
	Send_Improve_CARDS(TO_ALL);

	//ресурсы каждого игрока
	for (int i = 1; i < 7; i++)  Send_Player_CARDS(TO_ALL, i);
	//банки обмена игроков
	for (int i = 1; i < 7; i++)  Send_Info_Change_Area(TO_ALL, i);

	return;
}

//=======================================================================
//отправка банка карт развития от сервера клиентам
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
//отправка вектора карт развития 1 игрока от сервера клиентам
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
//отправка сообщения о состоянии банка обмена игрока
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
//отправка клиентам сообщения о карточке самого длинного пути и войска
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
//  уведомление от сервера о игроке и результате броска кубика
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
//  уведомление от сервера о игре карты развития
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
//  уведомление от сервера о броске 7
//=======================================================
void Info_Dise_7(int pl,int npl)
{
  CATAN_SERVER_Command(NET_COMMAND::DICE_SEVEN,(char* )&npl, sizeof(int), pl);
}

//=======================================================================
// от SERVER всем фишки одного игрока
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
// от SERVER всем ресурсы одного игрока
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
//  банк карточек ресурсов
//=======================================================
void Send_Bank_Resurs(int pl)
{
 CATAN_SERVER_Command(NET_COMMAND::INFO_BANK_RESURS, (char*)&CARD_Bank[0], 10*sizeof(int), pl);
}

//=======================================================
//  инфо о передаче хода игроку
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
//  местоположение разбойника
//=======================================================
void Set_Game_Step(int pl)
{
	CATAN_SERVER_Command(NET_COMMAND::SET_STEP, (char*)&Game_Step.current_step, sizeof(int), pl);
}

//=======================================================
//  инфо о подключенных игроках
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
//  местоположение разбойника шлем игрокам
//=======================================================
void Send_Bandit_GECS(int pl)
{
 CATAN_SERVER_Command(NET_COMMAND::BANDIT_GECS, (char*)&bandit_Gecs, sizeof(int),pl);
}

//=======================================================
// сервер посылает раскладку поля гексов
//=======================================================
void Send_Field_GECS(int pl)
{
 //по 4 int значения на гекс
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
// отправка сообщения городах и их владельцах от сервера игрокам
//=======================================================================
void Send_Info_Nodes(int pl)
{
	//по 2 int значения на node
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
// отправка сообщения городах и их владельцах от сервера игрокам
// в сокращенном виде - только заполненые узлы- 2-3 кратная экономия трафика
//=======================================================================
void Send_Info_Nodes_Zip(int pl)
{
	//по 2 int значения на node
	int max_size = (*nodePtr).size() * sizeof(int) * 3;     //max_size
	int size = 0;
	char* buff = new char[max_size];

	int* IntPtr;
	IntPtr = (int*)buff; IntPtr++;

	for (auto node : *nodePtr) 
	     {
		if (node.owner > 0)
		    {
			*IntPtr++ = node.number;
			*IntPtr++ = node.owner;
			*IntPtr++ = node.object;
			size += 3;
		    }
	     }
	IntPtr = (int*)buff;
	*IntPtr = size;    //в первом слове массива количество узлов
	size = (size + 1) * sizeof(int);
	CATAN_SERVER_Command(NET_COMMAND::NODE_ARRAY_ZIP, buff, size, pl);

	delete[] buff;
	return;
}

//========================================================
// сервер посылает инфо по дорогам клиентам
//========================================================
void Send_Info_Roads(int pl)
{
	//по 2 int значения на road
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
// сервер выдает номер игроку
//=======================================================
void Set_Player_Number(int num)
{
 //std::cout << " ======== SEND NEW NUM FOR PLAYER == " <<  num << std::endl;
 CATAN_SERVER_Command(NET_COMMAND::SET_N_PLAYER,(char*)&num,sizeof(int),num);
}


//*******************************************************************************************
//=============== old net ******************************************************************* OLD

//=======================================================
// Запрос переслать состояние ресурсов на сервер
//=======================================================
void Ask_Send_Resurs_To_Server()
{
    Info_Change_Bank();
	Info_Main_Bank();       //общий банк карточек ресурсов
	Info_Player_Resurs();   //ресурсы CARD игрока

   return;
}


//=======================================================
//  банк обмена в начале хода должен инициализироваться
//=======================================================
void InitChange_BANK()
{
int i;

    for (i = 0; i < 7; i++)  InitChange_BANK(i);

	return;
}

//=======================================================
//  банк обмена одного игрока
//=======================================================
void InitChange_BANK(int i)
{
 for (int j = 0; j < 10; j++)  ChangeBANK[i][j] = 0;

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









