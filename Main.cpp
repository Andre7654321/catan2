#include <iostream>
#include <typeinfo>
#include <SFML/Graphics.hpp>
#include <string.h>
#include <thread>
#include <cstdlib>
#include <string.h>
#include <vector>

#include "CatanNet.h"
#include "Cat_NET.h"
#include "CatanField.h"
#include "Catan_View.h"
#include "Catan_Step.h"
#include "Catan_Count.h"

// sfml-graphics-d.lib;sfml-window-d.lib;sfml-system-d.lib;%(AdditionalDependencies)  рабочее
// smfl-graphics-d.lib; smfl-window-d.lib; smfl - system - d.lib; % (AdditionalDependencies)   smfl-graphics.lib  smfl-graphics.lib

std::vector<GECS>* gecsPtr;    //указатель на вектор гексов
std::vector<NODE>* nodePtr;    //указатель на вектор узлов поля
std::vector<ROAD>* roadPtr;    //указатель на вектор дорог

std::vector<IMP_CARD> improve_CARDS;    //банк карт развития
std::vector<IMP_CARD> develop_CARDS[7]; //карты развития игроков

bool flag_draw_gecs = true;
bool flag_draw_node = true;
bool flag_draw_road_Net = true;
bool flag_draw_gecs_num = false;
extern int Field_CATAN_x;
extern int Field_CATAN_y;

extern int develop_field_x;
extern int develop_field_y;

extern sf::Sprite sprite_road;
extern sf::Sprite sprite_town;
extern sf::Sprite sprite_village;
extern sf::Sprite cube_sprite;
extern sf::Sprite bandit_sprite;
extern sf::Sprite sprite_chBank;
extern sf::Sprite sprite_chBank_clear;
extern sf::Sprite sprite_take_card[7];
extern sf::Sprite sprite_change[7];
extern sf::Sprite Active_develop_card[7];    //спрайты карт развития
extern sf::Sprite sprite_move_over;
extern sf::Sprite sprite_start;

extern int bandit_Gecs;
extern std::string resurs_name[10];     //имена ресурсов строкой для вывода в консоль

//глобальные переменные
int max_road_owner = 0;  //владелец самого длинного тракта
int max_army = 0;        //владелец карточки самое большое войско
int player_num = 0;      //номер присваемый игроку   0 - для сервера
int CARD_Bank[10];       //структура содержащая карточки ресурсов не принадлежание игрокам
const int MAXPLAYERS = 4;
PLAYER player[7];       //резервируем массив под максимальное число игроков + server
int limit_7[7];         //для хранения числа карт, до которого надо скинуть лишнее при броске 7

//область обмена ресурсами
int ChangeBANK[7][10] = {0};
CHANGE Change[12];
extern sf::Sprite sprite_Offer[12];

int Play_two_roads = 0;     //флаг игры карты развития 2 дороги
int Play_two_resurs = 0;
int Play_ONE_resurs = 0;
int Allow_Develop_card = 0;    //разрешение сыграть карту развития

int Draw_Big_Develop_CARD = -1;
extern int Big_Message;
bool flag_get_cubic_result = false;
bool flag_cubic_was_used = false;   //сервер запоминает что текущий игрок уже бросал кубик
bool flag_develop_card_used = false;  //сервер запоминает что карта развития уже игралась в этом ходу

bool village_ismove = false;    //перетаскивание деревни
bool town_ismove = false;       //перетаскивание города
bool road_ismove = false;
bool bandit_ismove = false;
bool CARD_ismove = false;
RESURS CARD_type_move = RESURS::WOOD;
RESURS CARD_type_get = RESURS::EMPTY;
float dX, dY;              //компенсация для спрайтов при перетаскивании

//объект класса игры --------------------------------------------------------
GAME_STEP Game_Step;
int game_wait_reconnect = 0;     //счетчик сообщений о дизконнекте

int main()
{
   setlocale(LC_ALL, "rus");

   //инициализация поля игры, гексов, дорог до запуска сетевых процедур
   std::vector<GECS> Gecs;
   std::vector<NODE> Field;
   std::vector<ROAD> Roads;
   Init_CATAN_Field(&Gecs, &Field, &Roads);
   gecsPtr = &Gecs;   nodePtr = &Field;   roadPtr = &Roads;
   //--------------------------------------------------------------------

   Game_Step.current_step          = 0;           //начало игры
   Game_Step.start_player          = 1;
   Game_Step.current_active_player = 1;           //бросок кубика за игроком №1

   //выбор варианта загрузки - сервер или игрок ==================================== 
   char connectionType;
    std::cout << "Enter (s) for server, Enter (c) for client" << std::endl;
    std::cin >> connectionType;
    
    //запуск сервера приложения ========================================================================
    if (connectionType == 's')      { std::cin.clear();  Start_Server_CATAN();   player_num = 0;   }
    else
       {
        std::cin.clear();
        //при подключении сервер нам передаст расположение гексов, а дороги и узлы не должны меняться
        int ret = Init_Client_CATAN();
        if (ret == 0) { std::cout << "  No connect , exit" << std::endl;     return 0; }
       }

    //SMFL --------------------------------------------------------------------------------------
    sf::RenderWindow window(sf::VideoMode(1200, 960), "CATAN standart FIELD", sf::Style::Close);
    window.setPosition(sf::Vector2i(470.0f, 0.0f));
       
    //толстая дорога, рисуется прямоугольником, требует поворота фигуры если идет под углом
    sf::RectangleShape Road(sf::Vector2f(60.f, 7.f));   //параметры  ширина и высота
    Road.setOrigin(0, 4);  //смещаем центр разворота в середину толщины линии

    int st;  //номер текущего шага
    //main cycle ===============================================================================================
    while (window.isOpen())
    {
        //на 0 шаге не может быть ожидания переподключения
        //тех кто висели в ожидании сбрасываем в дизконнект
        if (Game_Step.current_step == 0)
            {
            game_wait_reconnect = 0;
            for (int ii = 0; ii < 7; ii++)
                {
                if (player[ii].wait == true) player[ii].active = false;
                player[ii].wait = false;
                }
            }
        //если подключились отпавшие - разблокировать игру
        bool flag = false;
        for (int ii = 0; ii < 7; ii++)  if(player[ii].wait)  flag = true;
        if(flag == false)  game_wait_reconnect = 0;

        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);   //забираем коорд курсора
        st = Game_Step.current_step;

        sf::Event event;
        while(window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)       window.close();

            //отслеживает состояние клавиш, а не событие нажатия
            //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { flag_draw_gecs ? flag_draw_gecs = false : flag_draw_gecs = true; }

            // заявки на обмен с игроком - проверка корректности по наличию ресурса  - только своих заявок
            bool del;
            for (int s = 0; s < 12; s++)
                {
                if (Change[s].status == 0)            continue;
                 //если игрок предлагает то, чего уже нет
                if (Change[s].from_pl == player_num)
                    {
                    del = false;
                    for (int i = 1; i < 6; i++)     {    if (Change[s].offer_num[i] > player[player_num].resurs[i])  del = true;  }
                    if (del == false) continue;
                    AskToDeleteOffer(s);
                    }
                //если с вас просят то, чего уже нет
                if (Change[s].to_pl == player_num)
                    {
                    del = false;
                    for (int i = 1; i < 6; i++) { if (Change[s].need_num[i] > player[player_num].resurs[i])  del = true; }
                    if (del == false) continue;
                    AskToDeleteOffer(s);
                    }
                }

            if (st == 4 && isCardsDeletedAfterSeven() == false)   continue;    //ожидание сброса карт игроками после броска 7
            
            //отпуск клавиши клавиатуры
            if (event.type == sf::Event::KeyReleased)
                {
                //переключение режимов отображения поля
                if (event.key.code == sf::Keyboard::Numpad4) { flag_draw_gecs ? flag_draw_gecs = false : flag_draw_gecs = true; }
                if (event.key.code == sf::Keyboard::Numpad6) { flag_draw_node ? flag_draw_node = false : flag_draw_node = true; }
                if (event.key.code == sf::Keyboard::Numpad8) { flag_draw_road_Net ? flag_draw_road_Net = false : flag_draw_road_Net = true; }
                if (event.key.code == sf::Keyboard::Numpad2) { flag_draw_gecs_num ? flag_draw_gecs_num = false : flag_draw_gecs_num = true; }

                 //тестовый переход по F10   на 4 шаг для отладки
                if (event.key.code == sf::Keyboard::F10 && player_num > 0 && player[2].active == true && player[1].active == true)
                {
                    std::cout << " Pressed F10  TEST "  << std::endl;
                    //добавляет ресурсов, ставит 1 и 2 игроку 2 города, позволяет нырнуть сразу на 4 шаг
                    Test_Game();
                }

                //обработка сброса карт по 7
                if (st == 4 && event.key.code == sf::Keyboard::Space && player_num > 0
                    && player_num != Game_Step.current_active_player && getPlayerNumCardResurs(player_num) >= limit_7[player_num])
                    {
                    AskSendCardsToBank();
                    continue;
                    }

                //перезапуск игры - уход на 0 уровень
                if (event.key.code == sf::Keyboard::F1)
                   {
                    Ask_Reset_Game();
                   }

                //отслеживает факт нажатия SPACE - завершение хода
                if (event.key.code == sf::Keyboard::Space && player_num == Game_Step.current_active_player)
                {
                    if (game_wait_reconnect) continue;
                    player[player_num].flag_allow_change = 0;   //запрет обменов
                    if (st == 4 && Game_Step.step[st].roll_2_dice == 0 && Game_Step.step[st].flag_bandit == 0)
                        {
                        Say_Move_Over();  Game_Step.current_active_player = 0;
                       }
                    if (st == 3 || st == 2)
                    {
                        if (Game_Step.step[st].flag_set_one_Village == 0 && Game_Step.step[st].flag_set_one_Road == 0)
                        {
                            Say_Move_Over();  Game_Step.current_active_player = 0;
                        }
                    }
                    if (st == 1) { Say_Roll_Start_Dice();   Game_Step.current_active_player = 0; }
                    if (st == 0)  Say_Start();
                }

                //переход по ESC
                if (event.key.code == sf::Keyboard::Escape && player_num == Game_Step.current_active_player
                    && Game_Step.step[st].flag_bandit == 0)
                {
                    if (st == 4)  //возврат к моменту начала строительства
                    {
                        Ask_Send_Arrays();
                        Ask_Send_Resurs();
                        Ask_Send_Objects();
                    }
                    if (st == 2)
                    {
                        Ask_Send_Arrays();    //по ECS можно переходить до момента завершения хода
                        Ask_Send_Objects();
                        Game_Step.step[st].flag_set_one_Village = 1;
                        Game_Step.step[st].flag_set_one_Road = 1;
                        //player[player_num].village = 5;
                        //player[player_num].road = 15;
                    }
                    if (st == 3)
                    {
                        Ask_Send_Arrays();     //по ECS можно переходить до момента завершения хода
                        Ask_Send_Objects();
                        //player[player_num].village = 4;
                        //player[player_num].road = 14;
                        player[player_num].last_village_node = -1;
                        Game_Step.step[st].flag_set_one_Village = 1;
                        Game_Step.step[st].flag_set_one_Road = 1;
                    }
                }
                }

            if (player_num == 0)   continue;        //блокировка для серверного приложения от событий на экране
           
            //если нажата клавиша мыши правая
            if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Right)
                {
                    //спрайт заявки на обмен с игроком - снятие заявки
                    for (int s = 0; s < 12; s++)
                        {
                        if (sprite_Offer[s].getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                            {
                            if(Change[s].status == 0)  continue;
                            if (Change[s].from_pl != player_num && Change[s].to_pl != player_num)  continue;
                            //запрос серверу отменить заявку
                            AskToDeleteOffer(s);
                            }
                        continue;
                        }

                    //карта развития - вывод в увеличенном виде
                    for (int type = 0; type < 5; type++)
                      {
                      if (Active_develop_card[type].getGlobalBounds().contains(pixelPos.x, pixelPos.y) && 
                            (getNumDevelopCARD(IMP_TYPE(type), 0) > 0  || getNumDevelopCARD(IMP_TYPE(type),-1) > 0))
                          {
                          //вывести картинку карты в увеличенном виде
                          Draw_Big_Develop_CARD = type;
                         }
                      }
                }

            //если отпустили правую клавишу мышки
            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Right)
                {
                Draw_Big_Develop_CARD = -1;
                }

            //если нажата клавиша мыши левая  
            if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left)
            {
                if (game_wait_reconnect) continue;

                //start
                if (sprite_start.getGlobalBounds().contains(pixelPos.x, pixelPos.y) && player_num == Game_Step.current_active_player && st == 0)
                    {
                    Say_Start();  Game_Step.current_active_player = 1;
                    Sleep(100);
                    }

                //окончание хода
                if (sprite_move_over.getGlobalBounds().contains(pixelPos.x, pixelPos.y) && player_num == Game_Step.current_active_player)
                    {
                    Say_Move_Over();
                    Game_Step.current_active_player = 0;
                    Sleep(30);
                    }

                //спрайт заявки на обмен с игроком - принять заявку к обмену
                for (int s = 0; s < 12; s++)
                {
                    if (sprite_Offer[s].getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                    {
                        if (Change[s].status == 0)  continue;
                        if (Change[s].to_pl != player_num)  continue;
                        //запрос серверу выполнить заявку обмена
                        AskToAcceptOffer(s);
                    }
                    continue;
                }

               //спрайт стрелочки на выставление заявки на обмен с игроком
               for (int pl = 1; pl < 7; pl++)
                   {
                   //если есть уже 6+ ваших заявок - новую не создавать
                   int num = 0;
                   for (int s = 0; s < 12; s++) { if (Change[s].from_pl == player_num && Change[s].status) num++; }
                   if (sprite_change[pl].getGlobalBounds().contains(pixelPos.x, pixelPos.y) && num < 6)
                       {
                        if (pl == player_num)  continue;
                        if (player[pl].active == false) continue;
                        AskChangeWithPlayer(pl);
                       }
                    continue;
                   }
                
                //спрайт забрать карту у игрока
                for (int pl = 1; pl < 5; pl++)
                {
                    if (sprite_take_card[pl].getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                       {
                        if (pl == player_num || player[pl].flag_allow_get_card == 0)  continue;
                        //std::cout << " ================== Запрос забрать карту у игрока  " << pl << std::endl;
                        AskTakeRandomCardFromPlayer(pl);
                        //стереть всем флаги на забор карты  !!!
                        for (int i = 0; i < 5; i++)  player[i].flag_allow_get_card = 0;
                        Beep(700, 200);
                       }
                }

                //иконка очистить банк обмена
                if (sprite_chBank_clear.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                {
                 for (int i = 0; i < 10; i++)  ChangeBANK[player_num][i] = 0;
                 Info_Change_Bank();
                }

                //карточки развития
                if (pixelPos.x > develop_field_x && pixelPos.x < develop_field_x + 300 && pixelPos.y > develop_field_y && pixelPos.y < develop_field_y + 90)
                     {
                     //std::cout << "  Develop selected  " << std::endl;
                     if(Allow_Develop_card == 0) continue;
                     if (Game_Step.current_active_player != player_num)  continue;
                     if (Game_Step.step[st].flag_bandit == 1)  continue;

                      //по реализации карт загнать все в цикл
                     for(int type = 0;type < 5;type++)
                          {
                          if (Active_develop_card[(int)type].getGlobalBounds().contains(pixelPos.x, pixelPos.y) && getNumDevelopCARD((IMP_TYPE)type, 0) > 0)
                                AskPlayDevelopCard((IMP_TYPE)type);
                          }                          
                     }
                
                //карточки  ресурсов банка общего банка - обмен ресурсов
                if (pixelPos.x > 10 && pixelPos.x < 300 && pixelPos.y > 10 && pixelPos.y < 70)
                {
                    if (st == 4 && Game_Step.step[4].roll_2_dice == 1)   continue;     //еще не кинут кубик
                    if (player_num != Game_Step.current_active_player) continue;  //только активный игрок меняется с банком
                    if (pixelPos.x > 255 && pixelPos.x < 300 && st == 4 && improve_CARDS.size() > 0)
                       {
                        AskBuyImproveCARD();
                        Sleep(20);   //дать время серверу обработать запрос и списать ресурсы
                        Beep(900, 300);
                        continue;
                       }
                    //по старинке по координатам а не по спрайту выбирается тип ресурса
                    if (pixelPos.x > 10 && pixelPos.x <= 55) { CARD_type_get = RESURS::WOOD; }
                    if (pixelPos.x > 55 && pixelPos.x <= 115) { CARD_type_get = RESURS::BRICKS; }
                    if (pixelPos.x > 110 && pixelPos.x <= 155) { CARD_type_get = RESURS::OVCA; }
                    if (pixelPos.x > 155 && pixelPos.x <= 205) { CARD_type_get = RESURS::STONE; }
                    if (pixelPos.x > 205 && pixelPos.x < 255) { CARD_type_get = RESURS::BREAD; }

                    //std::cout << " Выбран   " << resurs_name[(int)CARD_type_get] << std::endl;
                    if (Play_ONE_resurs > 0)    //стоит флаг игры карты развития
                       {
                        Ask_Server_To_Take_Resurs_From_All(CARD_type_get);
                        Play_ONE_resurs = 0;
                        Beep(900, 600);
                        continue;
                       }
                    if (Play_two_resurs > 0 && CARD_Bank[(int)CARD_type_get] > 1)  //стоит флаг игры карты развития
                        {
                        player[player_num].resurs[(int)CARD_type_get] += 1;
                        ChangeBANK[player_num][(int)CARD_type_get] += 1;
                        CARD_Bank[(int)CARD_type_get] -= 1;
                        Play_two_resurs -= 1;
                        Ask_Send_Resurs_To_Server();
                        Beep(900, 300);
                        continue;
                        }
                    if (st == 4) { AskChangeWithBank(CARD_type_get);  Sleep(20); Beep(900, 400); }
                }
                
                //карточки своих ресурсов - переноc в банк обмена
                if (pixelPos.x > 200 && pixelPos.x < 500 && pixelPos.y > 830 && pixelPos.y < 910)
                   {
                    CARD_ismove = true;
                    //std::cout << "  Cards Zone selected  " << std::endl;
                    dY = pixelPos.y - 820;  
                    if (pixelPos.x > 200 && pixelPos.x < 260) { CARD_type_move = RESURS::WOOD;      dX = pixelPos.x - 200; }
                    if (pixelPos.x > 260 && pixelPos.x < 320) { CARD_type_move = RESURS::BRICKS;    dX = pixelPos.x - 260;}
                    if (pixelPos.x > 320 && pixelPos.x < 380) { CARD_type_move = RESURS::BREAD;     dX = pixelPos.x - 320; }
                    if (pixelPos.x > 380 && pixelPos.x < 440) { CARD_type_move = RESURS::STONE;     dX = pixelPos.x - 380; }
                    if (pixelPos.x > 440 && pixelPos.x < 500) { CARD_type_move = RESURS::OVCA;      dX = pixelPos.x - 440; }
                    if (st == 4 && Game_Step.step[4].roll_2_dice == 1)    CARD_ismove = false;
                    if (st < 4 )   CARD_ismove = false;

                    if (player[player_num].resurs[(int)CARD_type_move] - ChangeBANK[player_num][(int)CARD_type_move] >= 1)
                        {
                        if ((Game_Step.step[4].roll_2_dice == 0 && player_num == Game_Step.current_active_player) || player_num != Game_Step.current_active_player)
                            {
                             ChangeBANK[player_num][(int)CARD_type_move] += 1;
                             Beep(950, 100); 
                             Info_Change_Bank();
                            }
                        }
                   }
                
                //кубик
               if (cube_sprite.getGlobalBounds().contains(pixelPos.x, pixelPos.y) && player_num == Game_Step.current_active_player)
                {
                 if (st == 4 && Game_Step.step[st].roll_2_dice == 1 && Game_Step.step[st].flag_bandit == 0)
                                   {  Say_Roll_2Dice();   Game_Step.step[st].roll_2_dice = 0;    }
                 if (st == 1) {  Say_Roll_Start_Dice();   Game_Step.current_active_player = 0; }
                }

                //цикл по узлам
               for (auto& elem : Field)
                {
                    if (elem.isNode_infocus(pixelPos.x, pixelPos.y))
                    {
                    if(elem.owner != -1)  std::cout << "Node N=  " << elem.number << " Owner =  " << elem.owner << " x = " << elem.n_x << " y = " << elem.n_y << std::endl;
                      else std::cout << " Node =  " << elem.number << " Node free  "  << "  x = " << elem.n_x << " y = " << elem.n_y << std::endl;
                    }
                }
                
               //bandit - перестановка
               if (bandit_sprite.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
               {
                   //std::cout << "  Bandit  selected  " << std::endl;
                   auto coord = bandit_sprite.getPosition();
                   dX = pixelPos.x - coord.x;    dY = pixelPos.y - coord.y;
                   if (Game_Step.step[st].flag_bandit)    bandit_ismove = true;
               }

               //village - установка
               if (sprite_village.getGlobalBounds().contains(pixelPos.x, pixelPos.y))   //расположение деревни игрока
                  {
                   if (player_num != Game_Step.current_active_player)  continue;  //только активный игрок 
                  //std::cout << "  Village  selected  " << std::endl;
                  auto coord = sprite_village.getPosition();
                  dX = pixelPos.x - coord.x;    dY = pixelPos.y - coord.y;
                  if(Game_Step.step[st].flag_set_one_Village || Game_Step.step[st].flag_setVillage)   village_ismove = true;
                  if(st == 4 && Game_Step.step[4].roll_2_dice == 1 )   village_ismove = false;
                  if (st == 4)
                     {
                      if (player[player_num].resurs[(int)RESURS::WOOD] < 1 || player[player_num].resurs[(int)RESURS::BRICKS] < 1 ||
                           player[player_num].resurs[(int)RESURS::OVCA] < 1 || player[player_num].resurs[(int)RESURS::BREAD] < 1)   village_ismove = false;
                     }
                  }

               //town - установка
               if (sprite_town.getGlobalBounds().contains(pixelPos.x, pixelPos.y))  
                  {
                   if (player_num != Game_Step.current_active_player)  continue;  //только активный игрок 
                   //std::cout << "  Town selected  "  << std::endl;
                   auto coord = sprite_town.getPosition();
                   dX = pixelPos.x - coord.x;    dY = pixelPos.y - coord.y;
                   if (Game_Step.step[st].flag_set_one_Town || Game_Step.step[st].flag_setTown)   town_ismove = true;
                   if (st == 4)
                       {
                       if(Game_Step.step[4].roll_2_dice == 1)     town_ismove = false;
                       if(player[player_num].resurs[(int)RESURS::STONE] < 3 || player[player_num].resurs[(int)RESURS::BREAD] < 2)   town_ismove = false;
                       }
                   }

               //road - установка
               if(sprite_road.getGlobalBounds().contains(pixelPos.x, pixelPos.y))   //road
                   {
                   if (player_num != Game_Step.current_active_player)  continue;  //только активный игрок 
                   //std::cout << "  Road selected  " << std::endl;
                   auto coord = sprite_road.getPosition();
                   dX = pixelPos.x - coord.x;    dY = pixelPos.y - coord.y;
                   if (Game_Step.step[st].flag_set_Road || Game_Step.step[st].flag_set_one_Road)   road_ismove = true;
                   if (st == 4)
                        {
                        if (Game_Step.step[4].roll_2_dice == 1)   road_ismove = false;
                        if (player[player_num].resurs[(int)RESURS::WOOD] < 1 || player[player_num].resurs[(int)RESURS::BRICKS] < 1)  road_ismove = false;
                        if (Play_two_roads > 0)  road_ismove = true;
                        if(player[player_num].road < 1)  road_ismove = false;  //в любом случае запрет если дороги закончились
                        }
                   }

               //при перестановке бандита другое двигать нельзя
               if (Game_Step.step[st].flag_bandit) { village_ismove = false; town_ismove = false;  road_ismove = false; }

            } //end нажатия левой кнопки мыши

            //если отпустили левую клавишу мышки
            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left)
            {
                  //если отпустили левую клавишу мышки с бандитом
                if (bandit_ismove == true)
                {
                    bandit_ismove = false;        // не можем двигать 
                    int i = 0;
                    for (auto& g : Gecs)
                    {
                        if (g.isGecs_infocus(pixelPos.x, pixelPos.y))
                        {
                            if (i == bandit_Gecs)  continue;   //на месте оставить бандита нельзя

                            bandit_Gecs = i;
                            Game_Step.step[st].flag_bandit = 0;
                            //сообщить серверу о перестановке бандита
                            Say_Move_Banditos();

                            //если у гекса есть чужие города - выставить флаг разрешения вытягивания 1 карты у владельца
                            if (isAnyAlienObjectNearGecs(i, player_num) == true)
                                {
                                //std::cout << " Есть чужие города, тяните карту " << std::endl;
                                }

                            break;
                        }
                        i++;
                    }
                }      //------------------------- end put bandit ------------------------------

                 //если отпустили левую клавишу мышки с деревней
                if (village_ismove == true)
                {
                    village_ismove = false;        // не можем двигать деревню
                    int i = 0;
                    for (auto& elem : Field)
                    {
                        if (elem.isNode_infocus(pixelPos.x, pixelPos.y))
                        {
                            //std::cout << "\n ====== Try to put village node # -  " << i <<"\n";
                            if (player[player_num].village < 1) { std::cout << " You have not any villages " << "\n";  break; }
                            if (elem.isNode_free(i) == false) { std::cout << "  NODE is busy " << "\n";  break; }
                            if (isVillageNear(i) == true && st != 4) { std::cout << " Other Village too NEAR " << "\n";  break; }
                            if (isRoadNear(i, player_num) == false && st == 4) { std::cout << " No your road  NEAR " << "\n";  break; }

                            player[player_num].flag_allow_change = 0;
                            elem.owner = player_num;
                            player[player_num].village -= 1;
                            elem.object = VILLAGE;
                            player[player_num].last_village_node = elem.number;
                            if (Game_Step.step[st].flag_set_one_Village)   Game_Step.step[st].flag_set_one_Village = 0;
                            if (st == 4)
                            {
                                player[player_num].resurs[(int)RESURS::WOOD] -= 1;   CARD_Bank[(int)RESURS::WOOD] += 1;
                                player[player_num].resurs[(int)RESURS::BRICKS] -= 1;  CARD_Bank[(int)RESURS::BRICKS] += 1;
                                player[player_num].resurs[(int)RESURS::OVCA] -= 1;  CARD_Bank[(int)RESURS::OVCA] += 1;
                                player[player_num].resurs[(int)RESURS::BREAD] -= 1;  CARD_Bank[(int)RESURS::BREAD] += 1;
                            }
                        }
                        i++;
                    }
                }      //------------------------- end put village ------------------------------

                //если отпустили левую клавишу мышки с дорогой
                if (road_ismove == true)
                {
                    road_ismove = false;        // не можем двигать дорогу
                    int i = 0;
                    for (auto& elem : Roads)
                    {
                        if (elem.isRoad_infocus(pixelPos.x, pixelPos.y, &Field))
                        {
                            //std::cout << "\n ====== Try to set road # -  " << i << "\n";
                            if (player[player_num].road < 1) { std::cout << " You have not any roads " << "\n";  break; }
                            if (elem.isYourNodeNear(player_num, &Field) == false && Game_Step.step[st].flag_set_one_Road == 1 && st != 4)
                            {
                                std::cout << " No a village near this way " << "\n";   break;
                            }
                            if (elem.isYourNodeNear(player_num, &Field) == false && elem.isYourRoadNear(player_num, i) == false && st == 4)
                            {
                                std::cout << " Can not put road this way " << "\n";   break;
                            }
                            if (elem.owner != -1) { std::cout << "  ROAD is BUSY " << "\n";   break; }
                            if (Game_Step.current_step == 3)
                            {
                                if (Game_Step.step[st].flag_set_one_Village == 1) break;
                                if (elem.isYourLastVillageNear(player[player_num].last_village_node) == false) break;
                            }

                            player[player_num].flag_allow_change = 0;
                            elem.owner = player_num;
                            player[player_num].road -= 1;
                            elem.type = 1;      //road
                            if (Game_Step.step[st].flag_set_one_Road)     Game_Step.step[st].flag_set_one_Road = 0;
                            if (st == 4 )
                            {
                                if (Play_two_roads == 0)
                                {
                                    player[player_num].resurs[(int)RESURS::WOOD] -= 1;   CARD_Bank[(int)RESURS::WOOD] += 1;
                                    player[player_num].resurs[(int)RESURS::BRICKS] -= 1;  CARD_Bank[(int)RESURS::BRICKS] += 1;
                                }
                                else
                                   {
                                    Play_two_roads--;
                                    //переслать дороги на сервер ??
                                    Send_roads();
                                   }
                            }
                            break;
                        }
                        i++;
                    }
                }      //------------------------- end put road ------------------------------

                //если отпустили левую клавишу мышки с городом
                if (town_ismove == true)
                {
                    town_ismove = false;        // не можем двигать город
                    int i = 0;
                    for (auto& elem : Field)
                    {
                        if (elem.isNode_infocus(pixelPos.x, pixelPos.y))
                        {
                            //std::cout << "\n ====== Try to put town node # -  "  << "\n";
                            if (st != 4 || elem.owner != player_num || elem.object != VILLAGE) { break; }

                            player[player_num].village += 1;  player[player_num].town -= 1;
                            player[player_num].flag_allow_change = 0;
                            elem.object = TOWN;
                            if (st == 4)
                            {
                                player[player_num].resurs[(int)RESURS::STONE] -= 3;   CARD_Bank[(int)RESURS::STONE] += 3;
                                player[player_num].resurs[(int)RESURS::BREAD] -= 2;  CARD_Bank[(int)RESURS::BREAD] += 2;
                            }
                        }
                        i++;
                    }
                }      //------------------------- end put town ------------------------------
            }

        } //end  внутренний  while

        //------------------------------ОТРИСОВКА---------------------------------------------------------------------

        //отрисовка всех спрайтов
        if (true)
        {
            window.clear();
            //std::cout << " ================ Start_Draw Field ================= " << std::endl;
            if (flag_draw_road_Net == true)  DrawRoadsNet(&window, &Field, &Roads);
            if (flag_draw_gecs == true) DrawCATAN19(&window);
            if (flag_draw_gecs_num == true) DrawGecsNum(&window, &Gecs);
            if (flag_draw_gecs == true)  DrawGecs(&window, &Gecs);
            DrawRoads(&window, &Field, &Roads);

            if (flag_draw_node == true)  DrawNodes(&window, &Field);
            else  DrawNodesInfo(&window, &Field);

            if (bandit_ismove == true)  DrawBanditOnCoord(&window, pixelPos.x - (int)dX, pixelPos.y - (int)dY, 0.2f);
            else { if (flag_draw_gecs == true) DrawBandit(&window, &Gecs); }

            DrawResursBank(&window);    //ресурсы в общем банке 
            DrawPlayer(&window);
            if (st == 4) DrawChangeBank(&window, 150, 650, player_num);

            //если можем двигать деревню
            if (village_ismove)   DrawVillage(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2f);
            //если можем двигать город
            if (town_ismove)   DrawTown(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2f);
            //если можем двигать дорогу
            if (road_ismove)   DrawRoad(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2, 0);

            Draw_Step_Info(&window);

            //спрайт окончания хода игрока
            if (player_num == Game_Step.current_active_player)
            {
                int flag = false;
                if ((st == 2 || st == 3) && Game_Step.step[st].flag_set_one_Village == 0 && Game_Step.step[st].flag_set_one_Road == 0)  flag = true;
                if (st == 4 && Game_Step.step[st].roll_2_dice == 0 && Game_Step.step[st].flag_bandit == 0 && flag_get_cubic_result == true) flag = true;
                //если разрешение на забор карты от игрока после бандита
                for (int i = 1; i < 7; i++) { if (player[i].flag_allow_get_card == 1) flag = false; }
                if (flag) Move_Over_Logo(&window);
                else sprite_move_over.setPosition(1200, 1200);
            }

            if (player_num == 1 && Count_Num_players() >= 2 && st == 0)  Draw_Start(&window);

            if (player_num == Game_Step.current_active_player && st == 1)   Draw_Cubic(&window);

            if (player_num == Game_Step.current_active_player && st == 4)
                if (Game_Step.step[st].roll_2_dice == 1)    Draw_Cubic(&window);


            //если правая клавиша мышки выбрала карту развития
            if (Draw_Big_Develop_CARD != -1)   DrawDevelopCard(&window, Draw_Big_Develop_CARD, 500, 300, 1.6f, 0);

            if (Big_Message != -1)  Big_Message_Imp_Card(&window, Big_Message);

            if (game_wait_reconnect) Game_Message(&window, "Wait for reconnect player");

            window.display();
        }
          //------------------------- конец отрисовки ------------------------
    }

    /*
    char close = '0';
    while (close != '1')
    {
        std::cout << "Enter 1  for close" << std::endl;
        std::cin >> close;
    }
    */
    return 0;
}


