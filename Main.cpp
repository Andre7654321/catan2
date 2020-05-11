#include <iostream>
#include <typeinfo>
#include <SFML/Graphics.hpp>
#include <string.h>
#include <thread>
#include <cstdlib>
#include <string.h>
#include <vector>

#include "CatanNet.h"
#include "CatanField.h"
#include "Catan_View.h"
#include "Catan_Step.h"
#include "Catan_Count.h"

// sfml-graphics-d.lib;sfml-window-d.lib;sfml-system-d.lib;%(AdditionalDependencies)  рабочее
// smfl - graphics - d.lib; smfl - window - d.lib; smfl - system - d.lib; % (AdditionalDependencies)


std::vector<GECS>* gecsPtr;    //указатель на вектор гексов
std::vector<NODE>* nodePtr;    //указатель на вектор узлов поля
std::vector<ROAD>* roadPtr;    //указатель на вектор дорог

std::vector<IMP_CARD> improve_CARDS;    //банк карт развития
std::vector<IMP_CARD> develop_CARDS[5]; //карты развития игроков

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
extern sf::Sprite sprite_take_card[5];
extern sf::Sprite Active_develop_card[5];    //спрайты карт развития

extern int bandit_Gecs;

extern std::string resurs_name[10];


//глобальные переменные
int max_road_owner = 0;  //владелец самого длинного тракта
int player_num = 0;      //номер присваемый игроку   0 - для сервера
int CARD_Bank[10];       //структура содержащая карточки ресурсов не принадлежание игрокам
const int MAXPLAYERS = 4;
PLAYER player[5];       //резервируем массив под максимальное число игроков + server
int limit_7[5];         //для хранения числа карт, до которого надо скинуть лишнее при броске 7
//int num_players = 0;

//область обмена ресурсами
int ChangeBANK[5][10] = {0};

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

int main()
{
   setlocale(LC_ALL, "rus");


   //std::cout << "=================== TEST  Sizeof(NODE) = " << sizeof(NODE)  << std::endl;

   CARD_Bank[(int)RESURS::WOOD] = 40; CARD_Bank[(int)RESURS::STONE] = 40; CARD_Bank[(int)RESURS::OVCA] = 40;
   CARD_Bank[(int)RESURS::BREAD] = 40; CARD_Bank[(int)RESURS::BRICKS] = 40;

   //инициализация поля игры, гексов, дорог до запуска сетевых процедур
   std::vector<GECS> Gecs;
   std::vector<NODE> Field;
   std::vector<ROAD> Roads;

   gecsPtr = &Gecs;
   nodePtr = &Field;
   roadPtr = &Roads;

   //тестовое задание карт развития
   /*
   improve_CARDS[0] = { 0, IMP_TYPE::KNIGHT };
   improve_CARDS[1] = { 0, IMP_TYPE::ROAD2 };
   improve_CARDS[2] = { 0, IMP_TYPE::RESURS_CARD2 };
   improve_CARDS[3] = { 0, IMP_TYPE::POINT1 };
   improve_CARDS[4] = { 0, IMP_TYPE::RESURS1 };
   improve_CARDS[5] = { 0, IMP_TYPE::KNIGHT };
   */

   IMP_CARD ttt = { -1, IMP_TYPE::KNIGHT };
   improve_CARDS.insert(improve_CARDS.begin(),ttt);
   improve_CARDS.insert(improve_CARDS.begin(), ttt);
   improve_CARDS.insert(improve_CARDS.begin(), ttt);
   improve_CARDS.insert(improve_CARDS.begin(), ttt);

   IMP_CARD ttt1 = { 0, IMP_TYPE::ROAD2 };
   improve_CARDS.insert(improve_CARDS.begin(), ttt1);
   improve_CARDS.insert(improve_CARDS.begin(), ttt1);
   improve_CARDS.insert(improve_CARDS.begin(), ttt1);

   IMP_CARD ttt2 = { 0, IMP_TYPE::POINT1 };
   improve_CARDS.insert(improve_CARDS.begin(), ttt2);
   improve_CARDS.insert(improve_CARDS.begin(), ttt2);
   improve_CARDS.insert(improve_CARDS.begin(), ttt2);

   improve_CARDS.insert(improve_CARDS.begin(), ttt);
  

   Init_CATAN_Field(&Gecs, &Field, &Roads);
   //--------------------------------------------------------------------

   Game_Step.current_step        = 0;           //начало игры
   Game_Step.start_player        = 1;
   Game_Step.current_active_player = 1;           //бросок кубика за игроком №0

    char connectionType;
    std::cout << "Enter (s) for server, Enter (c) for client" << std::endl;
    std::cin >> connectionType;
    
    //запуск сервера приложения
    if (connectionType == 's')      { std::cin.clear();  Start_Server_CATAN();   player_num = 0;   }
    else
       {
        std::cin.clear();
        //запускаем свою клиентскую часть если не сервер - вектора данных уже сформированы,
        //так как при подключении сервер нам передаст расположение гексов, а дороги и узлы не должны меняться
        int ret = Init_Client_CATAN();
        if (ret == 0) { std::cout << "  No connect , exit" << std::endl;     return 0; }
       }


    //SMFL --------------------------------------------------------------------------------------
    sf::RenderWindow window(sf::VideoMode(1200, 960), "CATAN standart FIELD", sf::Style::Close);
    window.setPosition(sf::Vector2i(470.0f, 0.0f));
        
    //толстая дорога, рисуется прямоугольником, требует поворота фигуры если идет под углом
    sf::RectangleShape Road(sf::Vector2f(60.f, 7.f));   //параметры  ширина и высота
    Road.setOrigin(0, 4);  //смещаем центр разворота в середину толщины линии

    //-----------------------------  бонусы узлов  -------------------------------------------
    Field.at(1).type  = 0;    Field.at(6).type = 0;  Field.at(10).type = 0;    Field.at(11).type = 0;  // 3:1
    Field.at(52).type = 0;    Field.at(53).type = 0;  Field.at(27).type = 0;    Field.at(28).type = 0; // 3:1
    Field.at(37).type = 1;    Field.at(45).type = 1;   //wood
    Field.at(22).type = 2;    Field.at(23).type = 2;   //bricks
    Field.at(47).type = 3;    Field.at(51).type = 3;   //bread
    Field.at(39).type = 4;    Field.at(40).type = 4;   //stone
    Field.at(4).type  = 5;    Field.at(17).type = 5;   //ovca

    int st;  //номер текущего шага
    //main cycle ===============================================================================================
    while (window.isOpen())
    {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);   //забираем коорд курсора
        st = Game_Step.current_step;

        sf::Event event;
        while(window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)       window.close();

            //отслеживает состояние клавиш, а не факт нажатия
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  {  flag_draw_gecs ? flag_draw_gecs = false : flag_draw_gecs = true; } 
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { flag_draw_node ? flag_draw_node = false : flag_draw_node = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    { flag_draw_road_Net ? flag_draw_road_Net = false : flag_draw_road_Net = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  { flag_draw_gecs_num ? flag_draw_gecs_num = false : flag_draw_gecs_num = true; }

            //отслеживает факт нажатия R - тест подсчета длины дороги только для сервера
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::R && player_num == 0)
                {      Count_Road_Length(1);      }

            if (isCardsDeletedAfterSeven() == false)   continue;    //ожидание сброса карт игроками после 7
            if (player_num == 0)   continue;                        //блокировка для сервера
            
            //обработка сброса карт по 7
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space 
                    && player_num != Game_Step.current_active_player && getPlayerNumCardResurs(player_num) >= limit_7[player_num])
                {
                if (st == 4)  AskSendCardsToBank();
                continue;
                }
                                                                    
            //отслеживает факт нажатия SPACE - завершение хода
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space && player_num == Game_Step.current_active_player)
                  {
                     if (st == 4 && Game_Step.step[st].roll_2_dice == 0 && Game_Step.step[st].flag_bandit == 0)  Say_Move_Over();
                     if (st == 3 || st == 2)
                     {
                         if (Game_Step.step[st].flag_set_one_Village == 0 && Game_Step.step[st].flag_set_one_Road == 0) { Say_Move_Over();  Sleep(30); }
                     }
                     if (st == 1)  Say_Roll_1Dice(); 
                     if (st == 0)  Say_Start(); 
                  }

            //переход по ESC
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape && player_num == Game_Step.current_active_player 
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
            
            //если нажата клавиша мыши левая  
            if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left)
            {
                //иконка забрать карту у игрока
                for (int pl = 1; pl < 5; pl++)
                {
                    if (sprite_take_card[pl].getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                       {
                        if (pl == player_num || player[pl].flag_allow_get_card == 0)  continue;
                        std::cout << " ================== Запрос забрать карту у игрока  " << pl << std::endl;
                        AskTakeRandomCardFromPlayer(pl);
                        //стереть всем флаги на забор карты  !!!
                        for (int i = 0; i < 5; i++)  player[i].flag_allow_get_card = 0;
                        Beep(700, 200);
                       }
                }

                //иконка стереть банк обмена
                if (sprite_chBank_clear.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                {
                 for (int i = 0; i < 10; i++)  ChangeBANK[player_num][i] = 0;
                 Info_Change_Bank();
                }

                //карточки развития
                if (pixelPos.x > develop_field_x && pixelPos.x < develop_field_x + 300 && pixelPos.y > develop_field_y && pixelPos.y < develop_field_y + 90)
                     {
                     std::cout << "  Develop selected  " << std::endl;

                      //карта 1 очко
                      if (Active_develop_card[(int)IMP_TYPE::POINT1].getGlobalBounds().contains(pixelPos.x, pixelPos.y) && getNumDevelopCARD(IMP_TYPE::POINT1, 0) > 0)
                          {
                          std::cout << "  Card 1 point selected  " << std::endl;
                          //переслать запрос серверу сыграть карту развития
                          AskPlayDevelopCard(IMP_TYPE::POINT1);
                          }

                      //карта рыцарь
                      if (Active_develop_card[(int)IMP_TYPE::KNIGHT].getGlobalBounds().contains(pixelPos.x, pixelPos.y) && getNumDevelopCARD(IMP_TYPE::KNIGHT, 0) > 0)
                          {
                          std::cout << "  Card knight selected  " << std::endl;
                          //переслать запрос серверу сыграть карту развития
                          AskPlayDevelopCard(IMP_TYPE::KNIGHT);
                          }
                          
                     }
                
                //карточки  ресурсов банка общего банка - обмен ресурсов
                if (pixelPos.x > 10 && pixelPos.x < 300 && pixelPos.y > 10 && pixelPos.y < 70)
                {
                    if (st == 4 && Game_Step.step[4].roll_2_dice == 1)   continue;     //еще не кинут кубик
                    if (player_num != Game_Step.current_active_player) continue;  //только активный игрок меняется с банком
                    if (pixelPos.x > 255 && pixelPos.x < 300 && st == 4) 
                          {
                          AskBuyImproveCARD();
                          continue;
                          }
                    if (pixelPos.x > 10 && pixelPos.x <=  55) { CARD_type_get = RESURS::WOOD;     }
                    if (pixelPos.x > 55 && pixelPos.x <=  115) { CARD_type_get = RESURS::BRICKS;  }
                    if (pixelPos.x > 110 && pixelPos.x <= 155) { CARD_type_get = RESURS::OVCA;    }
                    if (pixelPos.x > 155 && pixelPos.x <= 205) { CARD_type_get = RESURS::STONE;    }
                    if (pixelPos.x > 205 && pixelPos.x  < 255) { CARD_type_get = RESURS::BREAD;     }
                    std::cout << " Выбран   " << resurs_name[(int)CARD_type_get] << std::endl;
                    if (st == 4)  //защита от случайного запроса на начальных шагах
                        {
                        if(AskChangeWithBank(CARD_type_get) == true)  InitChange_BANK();      //Зачем INI BANK ???
                        }
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
               if (cube_sprite.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                {
                   if (player_num != Game_Step.current_active_player)  continue;  //только активный игрок 
                   //std::cout << "  Dice selected  " << std::endl;
                   if (st == 4 && Game_Step.step[st].roll_2_dice == 1) {  Say_Roll_2Dice();   Game_Step.step[st].roll_2_dice = 0;    }
                   if (st == 1 ) { Say_Roll_1Dice(); }
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
                   std::cout << "  Bandit  selected  " << std::endl;
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
                   std::cout << "  Town selected  "  << std::endl;
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
                   }
                   }

               //при перестановке бандита другое двигать нельзя
               if (Game_Step.step[st].flag_bandit) { village_ismove = false; town_ismove == false;  road_ismove == false; }

            } //end нажатия левой кнопки мыши

            //если отпустили левую клавишу мышки
            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left)
            {
                //если отпустили левую клавишу мышки с картой ресурса
                if (CARD_ismove == true)
                {
                    CARD_ismove = false;        // не можем двигать
                    if (sprite_chBank.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                    {
                        //Beep(850, 100);
                        //добавить выбранный ресурс в область обмена, если в банке есть свободный
                        /*
                        if (player[player_num].resurs[(int)CARD_type_move] - ChangeBANK[player_num][(int)CARD_type_move] < 1)
                            {
                            std::cout << " Нет ресурса для добавления к обмену  " << resurs_name[(int)CARD_type_move] << std::endl;
                            break;
                            }
                        */
                        //std::cout << " ADD bank =  " << player[player_num].resurs[(int)CARD_type_move] << " change= " << ChangeBANK[player_num][(int)CARD_type_move] << std::endl;

                        //ChangeBANK[player_num][(int)CARD_type_move] += 1;

                        //сообщить серверу об изменении области обмена
                        //Say_Change_Resurs();
                    }
                }      //------------------------- end put resurs ------------------------------

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
                                std::cout << " Есть чужие города, тяните карту " << std::endl;
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

                            elem.owner = player_num;
                            player[player_num].road -= 1;
                            elem.type = 1;      //road
                            if (Game_Step.step[st].flag_set_one_Road)     Game_Step.step[st].flag_set_one_Road = 0;
                            if (st == 4)
                            {
                                player[player_num].resurs[(int)RESURS::WOOD] -= 1;   CARD_Bank[(int)RESURS::WOOD] += 1;
                                player[player_num].resurs[(int)RESURS::BRICKS] -= 1;  CARD_Bank[(int)RESURS::BRICKS] += 1;
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
            window.clear();
            //std::cout << " ================ Start_Draw Field ================= " << std::endl;
            if (flag_draw_road_Net == true)  DrawRoadsNet(&window, &Field, &Roads);
            if (flag_draw_gecs == true) DrawCATAN19(&window);
            if (flag_draw_gecs_num == true) DrawGecsNum(&window, &Gecs);
            if (flag_draw_gecs == true)  DrawGecs(&window, &Gecs);
            DrawRoads(&window, &Field, &Roads);

            if (flag_draw_node == true)  DrawNodes(&window, &Field);
            else  DrawNodesInfo(&window, &Field);

            DrawResursBank(&window);    //ресурсы в банке не принадлежат игрокам
            DrawPlayer(&window);
            if (st == 4) DrawChangeBank(&window, 150, 650, player_num);

            if (bandit_ismove == true)  DrawBanditOnCoord(&window, pixelPos.x - dX, pixelPos.y - dY, 0.2f);
                 else   {    if (flag_draw_gecs == true) DrawBandit(&window, &Gecs);   }

            //если передвигается карта ресурса
            if (CARD_ismove)   DrawCard(&window, (int)CARD_type_move, pixelPos.x - dX, pixelPos.y - dY, 0.5f);

            //если можем двигать деревню
            if (village_ismove)   DrawVillage(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2f);
            //если можем двигать город
            if (town_ismove)   DrawTown(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2f);
            //если можем двигать дорогу
            if (road_ismove)   DrawRoad(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2, 0);

            Draw_Step_Info(&window);

            if (player_num == 1 && Count_Num_players() >= 2   &&    st == 0)  Draw_Start(&window);

            if (player_num == Game_Step.current_active_player && st == 1)   Draw_Cubic(&window);

            if (player_num == Game_Step.current_active_player && st == 4)
                 if (Game_Step.step[st].roll_2_dice == 1)    Draw_Cubic(&window);

            window.display();
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


