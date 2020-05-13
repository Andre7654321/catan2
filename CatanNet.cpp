
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

//внешние глобальные переменные
extern int player_num;
extern int bandit_Gecs;
extern int max_road_owner;

extern int Play_two_roads;     //флаг игры карты развития 2 дороги
extern int Play_two_resurs;    //флаг игры карты развития 2 ресурса
extern int Play_ONE_resurs;

extern std::vector<GECS>* gecsPtr;    //указатель на вектор гексов
extern std::vector<NODE>* nodePtr;    //указатель на вектор узлов поля
extern std::vector<ROAD>* roadPtr;    //указатель на вектор дорог


//объявляем глобальной чтобы работать с ней в отдельной функции
SOCKET Connection;   //подключение клиента
#define MY_PORT 2489
char addr_listen[20] = "192.168.2.39";
//char addr_listen[20] = "127.0.0.1";
char addr_connect[20] = "192.168.2.39";  //"127.0.0.1"
//char addr_connect[20] = "127.0.0.1";


//для сервера
SOCKET Connections[10] = { 0,0,0,0,0,0,0,0,0,0 };    //подключения игроков 
int Counter = 0;           //счетчик подключений

std::mutex mtx1;    //защищает поток сообщений cout

extern PLAYER player[5];
extern GAME_STEP Game_Step;
extern int CARD_Bank[10];
extern CHANGE Change[12];       //сделки обмена игроков
extern std::vector<IMP_CARD> improve_CARDS;
extern std::vector<IMP_CARD> develop_CARDS[5];
extern int limit_7[5];

extern std::string resurs_name[10];

//область обмена ресурсами
extern int ChangeBANK[5][10];

//стандартные сообщения для обмена управлением клиента с сервером
std::string CATAN_command = "zCATANz";
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
PLAYER_CHANGE_AREA,
MAX_WAY_OWNER,
SEND_CARDS_TO_BANK,
INFO_CHANGE_BANK,
TAKE_CARD_FROM_PLAYER,
ASK_BUY_IMPROVE_CARD,
INFO_IMP_CARDS,        //банк карт развития
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
	{NET_COMMAND::INFO_IMP_CARDS,       "zCATANz INFO_IMP_CARDS"},       //банк карт развития
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
// Запрос на выполнение обмена по заявке
//=======================================================
void AskToAcceptOffer(int s)
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ACCEPT_OFFER].c_str());
	itoa(player_num, &msg[50], 10);    //свой номер
	itoa(s, &msg[54], 10);            //номер заявки
	send(Connection, msg, sizeof(msg), NULL);
	return;
}

//=======================================================
// Запрос на отмену заявки на обмен с игроком
//=======================================================
void AskToDeleteOffer(int s)
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_DELETE_OFFER].c_str());
	itoa(player_num, &msg[50], 10);    //свой номер
	itoa(s, &msg[54], 10);            //номер заявки
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//=======================================================
// Запрос на обмен картами ресурсов с игроком
//=======================================================
void AskChangeWithPlayer(int pl)
{
	char msg[256];
   //std::cout << " ====== Func AskChangeWithPlayer - " << pl << std::endl;

	//на сервер надо передать структуру сделки обмена - 
	//кто предлагает и кому
	//а карточки находятся в банках обмена - передавать не надо

	strcpy_s(msg, Map_Command[NET_COMMAND::SET_CHANGE_OFFER].c_str());
	itoa(player_num, &msg[50], 10);    //свой номер
	itoa(pl, &msg[54], 10);            //кому предложение
	send(Connection, msg, sizeof(msg), NULL);

return;
}

//=======================================================
// Запрос забрать у игроков ресурс
//=======================================================
void Ask_Server_To_Take_Resurs_From_All(RESURS type)
{
	char msg[256];
 
	//std::cout << " запрос серверу забрать все   " << resurs_name[(int)type] << std::endl;

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_GET_RESURS_FROM_ALL].c_str());
	itoa(player_num, &msg[50], 10);
	itoa((int)type, &msg[54], 10);
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

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
// Запрос переслать состояние ресурсов на сервер
//=======================================================
void Info_Player_Resurs()
{
	char msg[256];

	//ресурсы  игрока
	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
	_itoa(player_num, &msg[50], 10);     //номер игрока
	memcpy(&msg[54], player[player_num].resurs, 10 * sizeof(int));   //карточки
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//=======================================================
// Запрос переслать состояние ресурсов на сервер
//=======================================================
void Info_Main_Bank()
{
	char msg[256];

	//общий банк карточек
	strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
	memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//=======================================================
// Запрос сыграть карту развития
//=======================================================
void AskPlayDevelopCard(IMP_TYPE type)
{
	char msg[256];
	std::cout << "  Ask to play card  " << (int)type  << std::endl;

	//переслать на сервер 
	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_PLAY_DEVELOP_CARD].c_str());
	itoa(player_num, &msg[50], 10);
	itoa((int)type, &msg[54], 10);
	send(Connection, msg, sizeof(msg), NULL);
	return;
}

//=======================================================
// Запрос забрать карту у игрока PL
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
//  сообщение серверу об изменении в банке обмена
//=======================================================
void Info_Change_Bank()
{
	char msg[256];

	//переслать на сервер свой обменный банк
	strcpy_s(msg, Map_Command[NET_COMMAND::INFO_CHANGE_BANK].c_str());
	itoa(player_num, &msg[50], 10);
	memcpy(&msg[60], &ChangeBANK[player_num][0], sizeof(int) * 10);
	send(Connection, msg, sizeof(msg), NULL);
	return;
}

//=======================================================
//  запрос на сброс карт в банк (обработка 7)
//=======================================================
void AskSendCardsToBank()
{
	char msg[256];

	//проверить достаточно ли карт скидывается 

	
	//переслать на сервер коменду сбросить банк обмена в общий банк
	strcpy_s(msg, Map_Command[NET_COMMAND::SEND_CARDS_TO_BANK].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//=======================================================
//  запрос обмена ресурсов с банком
//=======================================================
bool AskChangeWithBank(RESURS CARD_type)
{
char msg[256];
int i;
int need_card;


//проверить есть ли в банке обмена нужное количество одинаковых карточек
for(i = 1;i < 10;i++)   
       {
	   need_card = 4;
	   if(bonus31(player_num))    need_card = 3;
	   if(bonus21(player_num, i))   need_card = 2;
	   if(ChangeBANK[player_num][i] >= need_card)  i = 20;
       }
if (i < 20){	std::cout << " В банке нехватает ресурсов для обмена " << std::endl;  return false;  }

//информируем банк о завершенных постройках в узлах
//так как бонус по картам мог появиться в процессе хода
Send_nodes();
Send_roads();

//общий банк карточек - посылаем, так как мог измениться до обмена при постройке
Info_Main_Bank();

//ресурсы  игрока - посылаем так как могли измениться до обмена при постройке
Info_Player_Resurs();

//переслать на сервер свой обменный банк и требуемый от банка ресурс
strcpy_s(msg, Map_Command[NET_COMMAND::ASK_CHANGE_BANK].c_str());
itoa(player_num, &msg[50], 10);
itoa((int)CARD_type, &msg[54], 10);
memcpy(&msg[60], &ChangeBANK[player_num][0], sizeof(int) * 10);
send(Connection, msg, sizeof(msg), NULL);

return true;
}

//=======================================================
//  запрос покупки карты развития
//=======================================================
bool AskBuyImproveCARD()
{
char msg[256];

//проверитть наличие карточек для покупки
//if no   return false;  

//информируем банк о завершенных постройках в узлах
//так как бонус по картам мог появиться в процессе хода
Send_nodes();
Send_roads();

//общий банк карточек - посылаем, так как мог измениться до обмена при постройке
strcpy_s(msg, Map_Command[NET_COMMAND::INFO_BANK_RESURS].c_str());
memcpy(&msg[50], &CARD_Bank[0], 10 * sizeof(int));
send(Connection, msg, sizeof(msg), NULL);
Sleep(10);

//ресурсы  игрока - посылаем так как могли измениться до обмена при постройке
strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
_itoa(player_num, &msg[50], 10);     //номер игрока
memcpy(&msg[54], player[player_num].resurs, 10 * sizeof(int));   //карточки
send(Connection, msg, sizeof(msg), NULL);
Sleep(10);

//переслать на сервер свой обменный банк и требуемый от банка ресурс
strcpy_s(msg, Map_Command[NET_COMMAND::ASK_BUY_IMPROVE_CARD].c_str());
itoa(player_num, &msg[50], 10);
memcpy(&msg[60], &ChangeBANK[player_num][0], sizeof(int) * 10);
send(Connection, msg, sizeof(msg), NULL);
Sleep(10);

//в ответ сервер должен обновить банк ресурсов и ресурсы игрока

return true;
}

//=======================================================
//  банк обмена в начале хода должен инициализироваться
//=======================================================
void InitChange_BANK()
{
int i, j;

    for (i = 0; i < 5; i++)  InitChange_BANK(i);

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
// клиент посылает инфо по дорогам
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
	for (size_t p = 0; p < roadPtr->size(); p++)
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
// посылает инфо по узлам - о постройках на сервер
//========================================================
void Send_nodes()
{
char msg[256] = { '\0' };
int* intPtr;
int i = 0;

	//цикл по узлам катана
    strcpy_s(msg, Map_Command[NET_COMMAND::NODE_ARRAY].c_str());   //копируем сначала, так как после может испортить данные в 50 ячейке
    intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //в 1 ячейке(54 символ) номер стартового узла в передаваемом пакете
	intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на след элемент массива (58 ячейка)
    for (size_t p = 0; p < nodePtr->size(); p++)
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
			intPtr = (int*)&msg[50];    intPtr += 2;                 //встаем на след элемент массива 
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
// вызывается при отмене части хода для восстановления состояния
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
	send(Connection, msg, sizeof(msg), NULL); Sleep(10);

	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ROAD_ARRAY].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL); Sleep(10);
	Sleep(10);
}

//========================================================
// посылает сообщение что ход завершен
//========================================================
void Say_Move_Over()
{
	char msg[256] = { '\0' };

	//std::cout << " Ход завершен " << std::endl;

	//отмена всех предлож на обмен
	for (int i = 0; i < 12;i++)  Change[i].status = 0;

	//после строительства передать инфо по дорогам и узлам
	Send_nodes();	
	Send_roads();

	//после строительства передать инфо по своим ресурсам и банку ресурсов на сервер
	//общий банк карточек
	//Info_Main_Bank();  Sleep(5);

	//ресурсы  игрока
	Info_Player_Resurs();

	//деревни, города .. игрока
	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_OBJECTS].c_str());
	_itoa(player_num,                 &msg[50], 10);                    //номер игрока
	_itoa(player[player_num].town,    &msg[60], 10);                    //города
	_itoa(player[player_num].village, &msg[70], 10);                    //деревни
	_itoa(player[player_num].road,    &msg[80], 10);                    //дороги
	send(Connection, msg, sizeof(msg), NULL); 


	//на 3 шаге переслать номер узла поставленной последней деревни
	if (Game_Step.current_step == 3)  
	   {
		std::cout << " Send Last village =  " << player[player_num].last_village_node  << std::endl;

		strcpy_s(msg, Map_Command[NET_COMMAND::LAST_VILLAGE].c_str());
		itoa(player[player_num].last_village_node, &msg[50], 10);
		send(Connection, msg, sizeof(msg), NULL); Sleep(5);
	   }

	//само сообщение о завершении хода
	strcpy_s(msg, Map_Command[NET_COMMAND::SAY_MOVE_OVER].c_str());
	itoa(player_num, &msg[50], 10);
	send(Connection, msg, sizeof(msg), NULL);

	return;
}

//========================================================
// посылает сообщение что кинут  1 кубик
//========================================================
void Say_Roll_2Dice()
{
	char msg[256] = { '\0' };

	//std::cout << " Вы бросили  кубики " << std::endl;

	// сообщение о броске кубика
	strcpy_s(msg, Map_Command[NET_COMMAND::ASK_ROLL_2DICE].c_str());
	itoa(player_num, &msg[50], 8);
	send(Connection, msg, sizeof(msg), NULL);	Sleep(5);

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
	send(Connection, msg, sizeof(msg), NULL);	Sleep(10);

return;
}

//========================================================
// посылает сообщение серверу о месте разбойников
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

//===============================================================================================       CLIENT NET !!!!
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
	int pack_size;
	NET_COMMAND Command = NET_COMMAND::EMPTY;

	while (true)            //бесконечный цикл в потоке обработки входных сообщений
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

			str.assign(msg, 0, 7);    //берем первые 7 символов сообщения
			 //если системное сообщение $CATAN&
			if (str.compare(CATAN_command) == 0)   //сравнение со строкой $CATAN&
			    {
				str.assign(msg, 0, strlen(msg));
				//std::cout << " Get CATAN COMMAND  length =  " << pack_size << std::endl;

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
					//стираем надпись своего последнего броска
					if (Game_Step.current_step == 4)
					    {
						for (int i = 1; i < 5; i++)   player[i].last_dice = 0;
					    }
					continue;
				    }

				if (Command == NET_COMMAND::SET_N_ACTIVE_PLAYER)
				    {
					//отмена всех предлож на обмен
					for (int i = 0; i < 12; i++)  Change[i].status = 0;

					if(Game_Step.current_step == 4)  Game_Step.step[4].roll_2_dice = 1;
					Game_Step.current_active_player = atoi(&msg[50]);
					InitChange_BANK();    //в начале хода банк обмена обнуляется
					for (int ii = 1; ii < 5; ii++)   { player[ii].flag_allow_get_card = 0;  limit_7[ii] = 0; }

					//стираем надпись своего последнего броска
					if(player_num == Game_Step.current_active_player && Game_Step.current_step == 4)
						          player[player_num].last_dice = 0;
					continue;
				    }

				if (Command == NET_COMMAND::SET_GECS)
					{
					int gecs_num = atoi(&msg[50]);
					//std::cout << "Получен гекс номер:  " << gecs_num  << std::endl;
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
						   //std::cout << " Вы бросили 7 на кубиках !!!! " << std::endl;
						   Game_Step.step[4].flag_bandit = 1;
						   //подсчитать сколько карт должно остаться у игроков чтобы продолжить игру
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
							      //std::cout << " Выпало 7, надо оставить "<< limit_7[player_num] << "карт ресурса" << std::endl;
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
					    //std::cout << " Вы играете карту Рыцаря " << std::endl;
						Game_Step.step[4].flag_bandit = 1;
					    }

					if (pl == player_num && type == (int)IMP_TYPE::ROAD2)
					    {
						//std::cout << " Вы играете карту 2 дороги " << std::endl;
						Play_two_roads = 2;
					    }
					if (pl == player_num && type == (int)IMP_TYPE::RESURS_CARD2)
					    {
						std::cout << " Вы играете карту 2 ресурса " << std::endl;
						Play_two_resurs = 2;
					    }
					if (pl == player_num && type == (int)IMP_TYPE::RESURS1)
					    {
						std::cout << " Вы играете карту 1 ресурс со всех " << std::endl;
						Play_ONE_resurs = 1;
					    }

					continue;
				}

				if (Command == NET_COMMAND::LAST_DICE)
				    {
					int pl = atoi(&msg[50]);
					player[pl].last_dice = atoi(&msg[54]);
					player[player_num].flag_allow_change = 1;     //разрешение на обмен
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
					int pl = atoi(&msg[50]);                                 //номер игрока
					memcpy(ChangeBANK[pl] ,&msg[54], 10 * sizeof(int));      //карточки
					continue;
				   }

				if (Command == NET_COMMAND::INFO_IMP_CARDS)    //общий банк
				    {
					int size = atoi(&msg[50]);
					int* intPtr = (int*)&msg[54];
					improve_CARDS.clear();
					IMP_CARD tmp;
					for (int i = 0; i < size; i++)
					    {
						tmp.status = *intPtr++;
						tmp.type = (IMP_TYPE)(*intPtr++);
						improve_CARDS.push_back(tmp);   //заполняем вектор 
					    }
					continue;
				    }

				if (Command == NET_COMMAND::DEVELOP_VECTOR)  //вектор карт развития игрока
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
						develop_CARDS[pl].push_back(tmp);   //заполняем вектор 
					    }

					//std::cout << "Get develop, size =  " << develop_CARDS[pl].size() << std::endl;
					continue;
				    }

				if (Command == NET_COMMAND::CHANGE_OFFER)
				    {
					int s = atoi(&msg[50]);                              //номер сделки
					memcpy(&Change[s], &msg[54], sizeof(CHANGE));        //заполненный экземпляр сделки

				    //тестовый вывод сделки
					/*
					std::cout << " Сделка N " << s << " from   " << Change[s].from_pl << "  to " << Change[s].to_pl << std::endl;
					std::cout << " Предложение -----------------------" << std::endl;
					for (int t = 0; t < 6; t++)
					    {
						if (Change[s].offer_num[t]) std::cout << " CARD " << resurs_name[t] << " num  " << Change[s].offer_num[t] << std::endl;
					    }
					std::cout << " Требуется -----------------------" << std::endl;
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
	addr.sin_addr.s_addr = inet_addr(addr_connect);    //------------------------------------------------------------------      ADDR
	addr.sin_port = htons(MY_PORT);
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
	//addr.sin_addr.s_addr = inet_addr(addr_listen);      //адрес определим ниже
	addr.sin_port = htons(MY_PORT);
	addr.sin_family = AF_INET;      //интернет протокол


	//-----------инфо по нашему компу - получаем свой локальный адрес --------------------------------
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
				if (nAdapter == 0)  strcpy_s(addr_listen, inet_ntoa(adr.sin_addr));    //запоминаем адрес первого адаптера
				nAdapter++;
			}
		}
	}

	//проверить чтобы первые цифры были 192.168.YYY.XXX  !!!
	addr.sin_addr.s_addr = inet_addr(addr_listen);
	std::cout << "Listen Address =   " << addr_listen << std::endl;

	//сканировать локальную сеть на подключенные адреса -----------------------------------------
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

	//слушающий сокет
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);   //create sockrt
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));         // привязываем адрес к сокету
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	char msg[256] = "";     //будем обмениваться фиксированными пакетами 256	
	
	std::cout << "\n ======  SERVER: Старт цикла ожидания подключений ====== \n  " << std::endl;

	for (i = 0; i < 4; i++)
	{
		std::cout << " SERVER: Wait for next player \n  " << std::endl;
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);     //не возвращает управление пока не дождется запроса на подключение
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

			printf("Подключился  %s\n", inet_ntoa(addr.sin_addr));
			std::cout << "============================================ " << std::endl << std::endl;

			//-----------------------------------------------------------------------------------

			//отправка стартовых сообщений подключившейся станции, выдача номера игроку
			strcpy_s(msg, "Hello. It's CATAN GAME ");
			send(newConnection, msg, sizeof(msg), NULL); 

			// выдача номера игроку
			strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_PLAYER].c_str());
			_itoa(Counter + 1, &msg[50], 10);
			send(newConnection, msg, sizeof(msg), NULL); 
			//_itoa(i, msg, 10);  strcat_s(msg,"  -   номер соединения ");
			//send(newConnection, msg, sizeof(msg), NULL);

			//передача сформированного поля - гексы и номера - остальное клиент сформирует
			int gecs_num = 0;
			for (auto& elem : *gecsPtr)
			   {
			   strcpy_s(msg, Map_Command[NET_COMMAND::SET_GECS].c_str());
			   //надо записать в строку данные гекса
			   _itoa(gecs_num++, &msg[50], 10);       //на 50 позиции номер гекса
			   _itoa((int)elem.type, &msg[60], 10);   //на 60 позиции тип гекса
			   _itoa((int)elem.gecs_game_number, &msg[70], 10);   //на 70 позиции присвоенная игровая цифра
			   send(newConnection, msg, sizeof(msg), NULL); Sleep(1);
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
				   strcpy_s(msg, Map_Command[NET_COMMAND::ADD_N_ACTIVE_PLAYER].c_str());
				   _itoa(ii+1, &msg[50], 10);
				   send(newConnection, msg, sizeof(msg), NULL); 
			      }
			   }

			//уведомить активных игроков о подключении нового
			strcpy_s(msg, Map_Command[NET_COMMAND::ADD_N_ACTIVE_PLAYER].c_str());
			_itoa(Counter + 1, &msg[50], 10);
			Send_To_All(msg, sizeof(msg));

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
	char msg[256];
	char msg1[10];
	int ret;
	std::string str;
	NET_COMMAND Command = NET_COMMAND::EMPTY;
	bool system = false;
	int pack_size = 0;

	//запрашивать время и каждые 20сек отправлять тестовое сообщение клиентской части
	//так можно смотреть за работоспособностью чата на прием

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

		//выбрать из сообщений системные CATAN ==================================================================
		str.assign(msg, 0, 7);    //берем первые 7 символов сообщения

	    //если системное сообщение zCATANz
		if (str.compare(CATAN_command) == 0)  
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

				strcpy_s(msg, " ================ Игра стартовала ===============");
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
			   Send_To_All_Info_Nodes();
			   continue;
			   }

			if (Command == NET_COMMAND::ASK_ROAD_ARRAY)
			    {
				int* intPtr;
				int i = 0;
				//послать станциям состояние дорог
				strcpy_s(msg, Map_Command[NET_COMMAND::ROAD_ARRAY].c_str());   //копируем сначала, так как после может испортить данные в 50 ячейке
				intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //в 1 ячейке номер стартового узла
				intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на 2 элемент массива   0, 1, 2
				for (size_t p = 0; p < roadPtr->size(); p++)
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
				//отмена всех предлож на обмен
				for (int i = 0; i < 12; i++)  Change[i].status = 0;

				int pl = atoi(&msg[50]);
				//std::cout << " Завершение хода игрок  " << pl << std::endl;
				if ( Game_Step.current_step == 4)
				    {
					Game_Step.current_active_player = GetNextPlayer();

					//активировать игроку карточки рыцарей, если еще не активированы
					for (auto& elem : develop_CARDS[Game_Step.current_active_player])
					    {
						if (elem.status == -1 && elem.type == IMP_TYPE::KNIGHT)  elem.status = 0;
					    }
					Send_To_All_Develop_CARDS(Game_Step.current_active_player);  Sleep(1);

					std::cout << "SERVER  Передача хода игроку  " << Game_Step.current_active_player << std::endl;
					strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());	
					_itoa(Game_Step.current_active_player, &msg[50], 10);
					Send_To_All(msg, sizeof(msg)); Sleep(1);

					InitChange_BANK();
					//переход самого длинного тракта    max_road_owner
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
					  Get_Resurs(player[pl].last_village_node, pl);    //распред ресурсы для деревни
					  //std::cout << "Step3  M over, active = " << Game_Step.current_active_player << " start= " << Game_Step.start_player << std::endl;

					  if (Game_Step.current_active_player == Game_Step.start_player)
					       {
						   Game_Step.current_step = 4;     //старт стандартных ходов игры
						   std::cout << " =========== Переход на основную игру  ==========  " << pl << std::endl;
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
						std::cout << " ============ Переход на 3 шаг ===========  " << pl << std::endl;
						if (Game_Step.current_step == 2)   Game_Step.current_step = 3;
						strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());
						_itoa(Game_Step.current_step, &msg[50], 9);
						Send_To_All(msg, sizeof(msg));		Sleep(2);
					    }
					else  Game_Step.current_active_player = GetNextPlayer();
				    }

				mtx1.lock();  std::cout << "SERVER*  Передача хода игроку  " << Game_Step.current_active_player << std::endl;  mtx1.unlock(); 
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

			//системное сообщение --------------------------------------
			strcpy_s(msg, Map_Command[NET_COMMAND::LAST_DICE].c_str());
			_itoa(pl, &msg[50], 10);
			_itoa(player[pl].first_roll, &msg[54], 10);
			Send_To_All(msg, sizeof(msg)); Sleep(2);

            //если не все бросили кубик передать ход след игроку, если все то след шаг игры
			if (GetNextPlayer() == Game_Step.start_player)
			    {
				//mtx1.lock();  std::cout << "GetNextPlayer() == 0  говорит что все кинули " << std::endl; mtx1.unlock();
				SetFirstPlayer();    //назначает по результатам броска 1 кубика очередность хода и переводит step++
			    std::cout << "Первым будет ходить игрок -   " << Game_Step.start_player << std::endl;
			    }
			    else  Game_Step.current_active_player = GetNextPlayer();

			strcpy_s(msg, Map_Command[NET_COMMAND::SET_STEP].c_str());	
			_itoa(Game_Step.current_step, &msg[50], 9);
			Send_To_All(msg, sizeof(msg)); Sleep(2);
		
			mtx1.lock();  std::cout << "SERVER  Передача хода игроку  " << Game_Step.current_active_player << std::endl;  mtx1.unlock();
			strcpy_s(msg, Map_Command[NET_COMMAND::SET_N_ACTIVE_PLAYER].c_str());
			_itoa(Game_Step.current_active_player, &msg[50], 9);
			Send_To_All(msg, sizeof(msg));
			continue;
			}

			if (Command == NET_COMMAND::ASK_ROLL_2DICE)
			{
				int pl = atoi(&msg[50]);

				//srand(time(0));
				int dice2 = rand() % 6 + 1  + rand() % 6 + 1;   //2 кубика

				//сообщить всем результат броска кубика - не системное простое оповещение 
				std::cout << " Результат броска  =  " <<  dice2  << std::endl;

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
					   //распределить ресурсы по результатам броска - цикл по узлам катана
					   Step_Resurs(dice2);
					   Send_To_All_Info_Resurs();
				       }
				continue;
			}

			if (Command == NET_COMMAND::INFO_BANK_RESURS)  //серверу прислали - он все дублирует
			{
				int* intPtr = (int*)&msg[50];
				memcpy(CARD_Bank, &msg[50], 10 * sizeof(int));
				Send_To_All(msg, sizeof(msg)); Sleep(2);
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
				send(Connections[pl - 1], msg, sizeof(msg), NULL); Sleep(1);
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
				Send_To_All(msg, sizeof(msg));  Sleep(1);
				continue;
			}

			if (Command == NET_COMMAND::BANDIT_GECS)
			{
				bandit_Gecs = atoi(&msg[50]);
				Send_To_All(msg, sizeof(msg));
				continue;
			}

			if (Command == NET_COMMAND::ASK_CHANGE_BANK)    //запрос на обмен ресурсов
			{
				int pl = atoi(&msg[50]);
				int type_put = atoi(&msg[54]);
				memcpy(&ChangeBANK[pl][0],&msg[60], sizeof(int) * 10);

				//std::cout << " Запрос на обмен с банком " << resurs_name[type_put] << " from pl " << pl  <<  std::endl;

				//найти 4 карточки одного типа, вычесть их из банка игрока
				int type_get = 0;
				int need_card;
				for(int i = 1;i < 10;i++)
				   {
					need_card = 4;
					if (bonus31(pl)) need_card = 3;
					if(bonus21(pl,i)) need_card = 2;
					//проверить есть ли нужное кол-во в обменном банке и для контроля в банке игрока
					if (ChangeBANK[pl][i] >= need_card && player[pl].resurs[i] >= need_card) { type_get = i; break; }        
				   }

				if (type_get == 0)	    {   std::cout << " не нашлось нужное кол-во карт "  << std::endl;	continue;      }
				//проверить если запрашиваемый ресурс в общем банке
				if (CARD_Bank[type_put] < 1)  { std::cout << " В банке нет ресурса " << std::endl;	continue; }

				//перенести карты в банках
				player[pl].resurs[type_put] += 1;            ChangeBANK[pl][type_put] += 1;	           CARD_Bank[type_put] -= 1;
				player[pl].resurs[type_get] -= need_card;    ChangeBANK[pl][type_get] -= need_card;  	CARD_Bank[type_get] += need_card;

				//сообщить всем состояние банков карточек
				Send_To_All_Info_Resurs();
				continue;
			}

			if (Command == NET_COMMAND::SEND_CARDS_TO_BANK)
			   {
				//принимаем обменный банк
				int pl = atoi(&msg[50]);
				int num;
				for (int i = 1; i < 10; i++)
				    {
					num = ChangeBANK[pl][i];
				    if (num != 0)    {   player[pl].resurs[i] -= num;     ChangeBANK[pl][i] -= num;	   CARD_Bank[i] += num;   }
				    }

				//сообщить всем состояние банков карточек - тк как общий банк и банк игрока тоже меняется
				Send_To_All_Info_Resurs();
				continue;
			   }

			if (Command == NET_COMMAND::INFO_CHANGE_BANK)
			{
			    //принимаем обменный банк игрока
				int pl = atoi(&msg[50]);
				int type_put = atoi(&msg[54]);
				memcpy(&ChangeBANK[pl][0], &msg[60], sizeof(int) * 10);

				//сообщить всем состояние банка
				Send_To_All_Info_Change_Area(pl);
			}

			if (Command == NET_COMMAND::TAKE_CARD_FROM_PLAYER)
			   {
				int pl = atoi(&msg[50]);
				int pl_donor = atoi(&msg[54]);

				//std::cout << " Take card command "  << std::endl;
				int type = TakeRandomCardFromPl(pl_donor);
				if (type) { player[pl].resurs[type] += 1;  ChangeBANK[pl][type] += 1;   player[pl_donor].resurs[type] -= 1;  }
				//std::cout << " Pl " << pl << " забирает карту у  " << pl_donor << "type " << resurs_name[type] <<  std::endl;

				//сообщить всем состояние банков карточек - тк как общий банк и банк игрока тоже меняется
				Send_To_All_Info_Resurs();
				continue;
			   }

			if (Command == NET_COMMAND::ASK_BUY_IMPROVE_CARD)
			{
				if (improve_CARDS.size() < 1)    continue;    //закончились карты развития
				int pl = atoi(&msg[50]);
				memcpy(&ChangeBANK[pl][0], &msg[60], sizeof(int) * 10);

				//проверить достаточность карт
				if (ChangeBANK[pl][(int)RESURS::STONE] == 0 || ChangeBANK[pl][(int)RESURS::OVCA] == 0 || 
					    ChangeBANK[pl][(int)RESURS::BREAD] == 0	) 
				                   { 
					               std::cout << "SERVER: не нашлось нужное кол-во карт " << std::endl;	
								   continue; 
				                   }

				//забрать карты у игрока и перенести в банк
				ChangeBANK[pl][(int)RESURS::STONE] -= 1;  player[pl].resurs[(int)RESURS::STONE] -= 1;  CARD_Bank[(int)RESURS::STONE] += 1;
				ChangeBANK[pl][(int)RESURS::OVCA]  -= 1;  player[pl].resurs[(int)RESURS::OVCA] -= 1;   CARD_Bank[(int)RESURS::OVCA] += 1;
				ChangeBANK[pl][(int)RESURS::BREAD] -= 1;  player[pl].resurs[(int)RESURS::BREAD] -= 1;  CARD_Bank[(int)RESURS::BREAD] += 1;

				//получить тип карты развития из вектора
				auto type = improve_CARDS.at(0).type;
				auto status = improve_CARDS.at(0).status;
				//вытащить первую карту развития, отданную клиенту из вектора
				improve_CARDS.erase(improve_CARDS.begin());

				//поместить карту в вектор игрока и обновить данные карт развития игрока всем
				IMP_CARD ttt = {status, type};
				develop_CARDS[pl].push_back(ttt);
				Send_To_All_Develop_CARDS(pl); Sleep(10);

				//std::cout << " Карта развития " << (int)type << " size befor "<< improve_CARDS.size()  << std::endl;
				//сообщить всем состояние банков карточек ресурсов
				Send_To_All_Info_Resurs();

				continue;
			}

			if (Command == NET_COMMAND::ASK_PLAY_DEVELOP_CARD)
			{
				int pl = atoi(&msg[50]);
				int type = atoi(&msg[54]);

				//в векторе карт развития игрока первую активную найденную карту перевести в статус 1(сыграна)
				for(auto& elem : develop_CARDS[pl])
				    { 
					if (elem.status == 0 && type == (int)elem.type) 
					    {	
						elem.status = 1;  
						break;  
					    }    
				    }

				//разослать вектор карт развития игрока сыгравшего карту
				Send_To_All_Develop_CARDS(pl);

				//переслать всем инфо о игре картой развития
				strcpy_s(msg, Map_Command[NET_COMMAND::PLAY_DEVELOP_CARD].c_str());
				_itoa(pl, &msg[50], 10);
				_itoa(type, &msg[54], 10);
				Send_To_All(msg, sizeof(msg));   //все получают инфо об игре карты развития

				continue;
			}

			if (Command == NET_COMMAND::ASK_GET_RESURS_FROM_ALL)
			    {
				int pl = atoi(&msg[50]);
				int type = atoi(&msg[54]);
				//std::cout << " запрос от игрока " << pl << " забрать все   " << resurs_name[(int)type] << std::endl;

				for (int i = 1; i < 5; i++)
				    {
					if (i == pl || player[i].active == 0)  continue;
					//перенести карты в банках
					player[pl].resurs[type] += player[i].resurs[type];     ChangeBANK[pl][type] += player[i].resurs[type];
					player[i].resurs[type] = 0;                            ChangeBANK[i][type] = 0;
				    }

				//сообщить всем состояние банков карточек ресурсов
				Send_To_All_Info_Resurs();
			    continue;
			    }

			if (Command == NET_COMMAND::SET_CHANGE_OFFER)
			{
				int pl = atoi(&msg[50]);
				int pl_change = atoi(&msg[54]);

				//std::cout << " Change SET command from   "  <<  pl << "  to "  << pl_change  << std::endl;
				//заполнить на основе банков обмена игроков структуру сделки
				//найти не активную сделку
				int s = 0;
				for (s = 0; s < 12; s++)	   {  if (Change[s].status == 0) break;   }
				if (s == 12)  continue;

				Change[s].from_pl = pl; 		Change[s].to_pl = pl_change;    Change[s].status = 1;
				for (int t = 0; t < 6; t++)
				    {
					Change[s].offer_num[t] = (int)ChangeBANK[pl][t];
					Change[s].need_num[t] = (int)ChangeBANK[pl_change][t];
				    }
				//передать говоую сделку все игрокам
				Send_To_All_Info_Change(s);
				continue;
			}

			if (Command == NET_COMMAND::ASK_DELETE_OFFER)
			    {
				//std::cout << "  DELETE OFFER  " << std::endl;
				int pl = atoi(&msg[50]);
				int s = atoi(&msg[54]);  //номер заявки
				Change[s].status = 0;
				//передать говоую сделку все игрокам
				Send_To_All_Info_Change(s);
				continue;
			    }

			if (Command == NET_COMMAND::ASK_ACCEPT_OFFER)
			   {
				std::cout << "  ACCEPT OFFER  " << std::endl;

				int pl = atoi(&msg[50]);
				int s = atoi(&msg[54]);  //номер заявки

				int pl1 = Change[s].from_pl;
				int pl2 = Change[s].to_pl;

				if (pl2 != pl)   continue;   //если по ошибке инициатор обмена не получатель заявки
				//произвести обмен
				for(int t = 0; t < 6; t++)
				    {
					player[pl1].resurs[t] += Change[s].need_num[t];   player[pl2].resurs[t] -= Change[s].need_num[t];
					player[pl2].resurs[t] += Change[s].offer_num[t];  player[pl1].resurs[t] -= Change[s].offer_num[t];
				    }

				Send_To_All_Player_CARDS(pl1);
				Send_To_All_Player_CARDS(pl2);
				
				//обнулить банки обмена игроков сделки
				InitChange_BANK(pl1);	Send_To_All_Info_Change_Area(pl1);
				InitChange_BANK(pl2);   Send_To_All_Info_Change_Area(pl2);

				//отменить все заявки игроков, где недостаточно карт для обмена
				//!!!!!!!!!!!!!!!!!!!!!!!!!!

				//передать завершенную сделку все игрокам
				Change[s].status = 0;
				Send_To_All_Info_Change(s);
				continue;
			}
			
		continue;
		}  //закончена обработка , если в команде нет CONTINUE, то она будет дублирована всем


		//сюда попадают только несистемные сообщения
		//повторить отправку сообщения всем кроме INDEX если это чат
		for (int i = 0; i < 4; i++)
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
  for (int i = 0; i < 4; i++)
	{
	if (Connections[i] == 0)    continue;
	if (player[i + 1].active == 0) continue;     //пропускаем не активных игроков
		
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

 //общий банк карт развития
 Send_To_All_Improve_CARDS();

 //ресурсы каждого игрока
 for (int i = 1; i < 5; i++)  Send_To_All_Player_CARDS(i);

 //банки обмена игроков
 for (int i = 1; i < 5; i++)   Send_To_All_Info_Change_Area(i);

 return;
}

//=======================================================================
// всем ресурсы одного игрока
//=======================================================================
void Send_To_All_Player_CARDS(int pl)
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_RESURS].c_str());
	_itoa(pl, &msg[50], 10);     //номер игрока
	memcpy(&msg[54], player[pl].resurs, 10 * sizeof(int));   //карточки
	Send_To_All(msg, sizeof(msg));
	return;
}

//=======================================================================
//отправка сообщения о состоянии банка обмена игрока
//=======================================================================
void Send_To_All_Info_Change_Area(int pl)
{
	char msg[256];

	strcpy_s(msg, Map_Command[NET_COMMAND::PLAYER_CHANGE_AREA].c_str());
	_itoa(pl, &msg[50], 10);                                 //номер игрока
	memcpy(&msg[54], ChangeBANK[pl], 10 * sizeof(int));   //карточки
	Send_To_All(msg, sizeof(msg));
}

//=======================================================================
//отправка сообщения о карточке самого длинного пути
//=======================================================================
void Send_To_All_Info_Max_Way()
{
char msg[256];
	strcpy_s(msg, Map_Command[NET_COMMAND::MAX_WAY_OWNER].c_str());
	_itoa(max_road_owner, &msg[50], 10);
	Send_To_All(msg, sizeof(msg));
}

//=======================================================================
//отправка сообщения о ресурсах от сервера всем игрокам
// проверить на превышение размера сообщения 
//=======================================================================
void Send_To_All_Info_Nodes()
{
	char msg[256];

	int* intPtr;
	int i = 0;
	//послать станциям массив узлов c разбиением на пакеты
	strcpy_s(msg, Map_Command[NET_COMMAND::NODE_ARRAY].c_str());   //копируем сначала, так как после может испортить данные в 50 ячейке
	intPtr = (int*)&msg[50];  	intPtr++;	*intPtr = 0;  //в 1 ячейке номер стартового узла
	intPtr = (int*)&msg[50];    intPtr += 2;              //встаем на 2 элемент массива   0, 1, 2
	for (size_t p = 0; p < nodePtr->size(); p++)
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
	return;
}

//=======================================================================
//отправка банка карт развития от сервера клиентам
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
	_itoa(improve_CARDS.size(), &msg[50], 10);     //размер вектора

	Send_To_All(msg, sizeof(msg));
	return;
}

//=======================================================================
//отправка вектора карт развития 1 игрока от сервера клиентам
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
	_itoa(develop_CARDS[pl].size(), &msg[54], 10);     //размер вектора
	_itoa(pl, &msg[50], 10);     //номер игрока

	Send_To_All(msg, sizeof(msg));
	return;
}

//=======================================================================
//отправка сделки на обмен от одного игрока другому
//=======================================================================
void Send_To_All_Info_Change(int s)
{
	char msg[256];

	if (s < 0 || s >= 12)   {  Beep(900, 1000);  return; }
	
	strcpy_s(msg, Map_Command[NET_COMMAND::CHANGE_OFFER].c_str());
	_itoa(s, &msg[50], 10);                              //номер сделки
	memcpy(&msg[54], &Change[s], sizeof(CHANGE));        //заполненный экземпляр сделки
	Send_To_All(msg, sizeof(msg));
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
	}

	return 0;
}



