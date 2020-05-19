#pragma once
#ifndef NET_CAT_H
#define NET_CAT_H

#include <winsock2.h>
#include <mutex>
#include <iostream>
#include <thread>
#include <map>
#include <stdlib.h>

//стандартные сообщения для обмена управлением клиента с сервером
enum class NET_COMMAND
{
	EMPTY,
	SET_N_PLAYER,
	SET_STEP,
	INFO_N_ACTIVE_PLAYER,   //добавляет игрока
	SET_N_ACTIVE_PLAYER,   //устанавливает номер игрока которому передается ход
	SET_FIRST_PLAYER,      //устанавливает номер игрока который будет ходить первым
	ASK_SET_FIELD,
	SET_GECS,
	SAY_MOVE_OVER,
	SAY_ROLL_START_DICE,
	ASK_ROLL_2DICE,
	SAY_GAME_START,
	NODE_ARRAY,
	NODE_ARRAY_ZIP,
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
	ASK_CHANGE_WITH_BANK,
	PLAYER_CHANGE_AREA,
	MAX_WAY_ARMY_OWNER,
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
	ASK_ACCEPT_OFFER,
	ASK_RESET_GAME,
	TEST_GAME,
	WAIT_RECONNECT,
	SET_CLOSE
};
#define MY_PORT 2489
#define TO_ALL  0


int CATAN_CLIENT_Command(NET_COMMAND Net_Command, const char* buf, int len);
int CATAN_SERVER_Command(NET_COMMAND Net_Command, const char* buf, int len,int pl);

int Read_CLIENT_Data(int len, char* buf, int index);

int CATAN_Message(const char* buf, int len, int pl);

int Start_Server_CATAN(void);
void Check_UDP();
void ServerClientStreamFunc(int);
void Check_Connections(void);
int  init_WSA(void);
int Init_Client_CATAN(void);
void ClientHandler(int);
int  ClientChart();


#endif