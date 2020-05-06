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

// sfml-graphics-d.lib;sfml-window-d.lib;sfml-system-d.lib;%(AdditionalDependencies)  рабочее
// smfl - graphics - d.lib; smfl - window - d.lib; smfl - system - d.lib; % (AdditionalDependencies)


std::vector<GECS>* gecsPtr;    //указатель на вектор гексов
std::vector<NODE>* nodePtr;    //указатель на вектор узлов поля
std::vector<ROAD>* roadPtr;    //указатель на вектор дорог

bool flag_draw_gecs = true;
bool flag_draw_node = true;
bool flag_draw_road = true;
bool flag_draw_gecs_num = false;
extern int Field_CATAN_x;
extern int Field_CATAN_y;

extern sf::Sprite sprite_road;
extern sf::Sprite sprite_town;
extern sf::Sprite sprite_village;
extern sf::Sprite cube_sprite;
extern sf::Sprite bandit_sprite;
extern sf::Sprite sprite_chBank;
extern int bandit_Gecs;

extern std::string resurs_name[10];


//глобальные переменные
int player_num = 0;      //номер присваемый игроку   0 - для сервера
int CARD_Bank[10];       //структура содержащая карточки ресурсов не принадлежание игрокам
const int MAXPLAYERS = 4;
PLAYER player[5];       //резервируем массив под максимальное число игроков + server
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

   CARD_Bank[(int)RESURS::WOOD] = 40; CARD_Bank[(int)RESURS::STONE] = 40; CARD_Bank[(int)RESURS::OVCA] = 40;
   CARD_Bank[(int)RESURS::BREAD] = 40; CARD_Bank[(int)RESURS::BRICKS] = 40;

   //инициализация поля игры, гексов, дорог до запуска сетевых процедур
   std::vector<GECS> Gecs;
   std::vector<NODE> Field;
   std::vector<ROAD> Roads;

   gecsPtr = &Gecs;
   nodePtr = &Field;
   roadPtr = &Roads;

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

    //--------------------------------------------------------------------------------------------

    int st;
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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    { flag_draw_road ? flag_draw_road = false : flag_draw_road = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  { flag_draw_gecs_num ? flag_draw_gecs_num = false : flag_draw_gecs_num = true; }

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
                //карточки  ресурсов банка
                if (pixelPos.x > 10 && pixelPos.x < 250 && pixelPos.y > 10 && pixelPos.y < 70)
                {
                    //std::cout << "  Bank Zone selected  " << std::endl;
                    dY = pixelPos.y - 10;
                    if (pixelPos.x > 10 && pixelPos.x <=  55) { CARD_type_get = RESURS::WOOD;       dX = pixelPos.x - 10; }
                    if (pixelPos.x > 55 && pixelPos.x <=  115) { CARD_type_get = RESURS::BRICKS;    dX = pixelPos.x - 60; }
                    if (pixelPos.x > 110 && pixelPos.x <= 155) { CARD_type_get = RESURS::OVCA;       dX = pixelPos.x - 110; }
                    if (pixelPos.x > 155 && pixelPos.x <= 205) { CARD_type_get = RESURS::STONE;     dX = pixelPos.x - 160; }
                    if (pixelPos.x > 205 && pixelPos.x  < 250) { CARD_type_get = RESURS::BREAD;      dX = pixelPos.x - 210; }
                    //std::cout << " Выбран   " << resurs_name[(int)CARD_type_get] << std::endl;
                    if (st == 4)  //защита от случайного запроса на начальных шагах
                        {
                        if(AskChangeWithBank(CARD_type_get) == true)  InitChange_BANK();
                        }
                }
                
                //карточки своих ресурсов
                if (pixelPos.x > 300 && pixelPos.x < 600 && pixelPos.y > 830 && pixelPos.y < 910)
                {
                    CARD_ismove = true;
                    std::cout << "  Cards Zone selected  " << std::endl;
                    dY = pixelPos.y - 820;  
                    if (pixelPos.x > 300 && pixelPos.x < 360) { CARD_type_move = RESURS::WOOD;      dX = pixelPos.x - 300; }
                    if (pixelPos.x > 360 && pixelPos.x < 420) { CARD_type_move = RESURS::BRICKS;    dX = pixelPos.x - 360;}
                    if (pixelPos.x > 420 && pixelPos.x < 480) { CARD_type_move = RESURS::BREAD;     dX = pixelPos.x - 420; }
                    if (pixelPos.x > 480 && pixelPos.x < 540) { CARD_type_move = RESURS::STONE;     dX = pixelPos.x - 480; }
                    if (pixelPos.x > 540 && pixelPos.x < 600) { CARD_type_move = RESURS::OVCA;      dX = pixelPos.x - 540; }
                }
                
                //кубик
               if (cube_sprite.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                {
                   std::cout << "  Dice selected  " << std::endl;
                   if (st == 4 && Game_Step.step[st].roll_2_dice == 1) {  Say_Roll_2Dice();   Game_Step.step[st].roll_2_dice = 0;    }
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
                
               //bandit
               if (bandit_sprite.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
               {
                   std::cout << "  Bandit  selected  " << std::endl;
                   auto coord = bandit_sprite.getPosition();
                   dX = pixelPos.x - coord.x;    dY = pixelPos.y - coord.y;
                   if (Game_Step.step[st].flag_bandit)    bandit_ismove = true;
               }

               //village
               if (sprite_village.getGlobalBounds().contains(pixelPos.x, pixelPos.y))   //расположение деревни игрока
                  {
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

               //town
               if (sprite_town.getGlobalBounds().contains(pixelPos.x, pixelPos.y))  
                  {
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

               //road
               if(sprite_road.getGlobalBounds().contains(pixelPos.x, pixelPos.y))   //road
                   {
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

            //если отпустили левую клавишу мышки с картой ресурса
            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left && CARD_ismove == true)
            {
                CARD_ismove = false;        // не можем двигать
                if(sprite_chBank.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
                    {
                    //добавить выбранный ресурс в область обмена, если в банке есть свободный
                    if (player[player_num].resurs[(int)CARD_type_move] - ChangeBANK[player_num][(int)CARD_type_move] < 1)
                    {
                        std::cout << " Нет ресурса для добавления к обмену  " << resurs_name[(int)CARD_type_move] << std::endl;
                        break;
                    }
                    std::cout << " ADD bank =  " << player[player_num].resurs[(int)CARD_type_move] << " change= " << ChangeBANK[player_num][(int)CARD_type_move] << std::endl;
                    ChangeBANK[player_num][(int)CARD_type_move] += 1;

                    //сообщить серверу об изменении области обмена
                    //Say_Change_Resurs();
                    }
            }      //------------------------- end put resurs ------------------------------
              
              //если отпустили левую клавишу мышки с бандитом
            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left && bandit_ismove == true)
            {
                bandit_ismove = false;        // не можем двигать 
                int i = 0;
                for (auto& g : Gecs)
                {
                    if (g.isGecs_infocus(pixelPos.x, pixelPos.y))  
                           {
                           if(i == bandit_Gecs)  continue;   //на месте оставить бандита нельзя

                           bandit_Gecs = i;
                           Game_Step.step[st].flag_bandit = 0;
                           //сообщить серверу о перестановке бандита
                           Say_Move_Banditos();
                           break;  
                           }
                    i++;
                }
            }      //------------------------- end put bandit ------------------------------

             //если отпустили левую клавишу мышки с деревней-------------------------------------------------------------------
            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left && village_ismove == true)     
            {
                village_ismove = false;        // не можем двигать деревню
                int i = 0;
                for (auto& elem : Field)
                    {
                    if (elem.isNode_infocus(pixelPos.x, pixelPos.y))
                        {
                        std::cout << "\n ====== Try to put village node # -  " << i <<"\n";
                        if (player[player_num].village < 1) { std::cout << " You have not any villages " << "\n";  break; }
                        if (elem.isNode_free(i) == false)   { std::cout << "  NODE is busy " << "\n";  break; }
                                else std::cout << " OK  NODE is free " << "\n";

                        if(isVillageNear(i) == true && st != 4)  { std::cout << " Other Village too NEAR " << "\n";  break; }
                        if(isRoadNear(i,player_num) == false && st == 4) { std::cout << " No your road  NEAR " << "\n";  break; }
                          
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
            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left && road_ismove == true)
            {
                road_ismove = false;        // не можем двигать дорогу
                int i = 0;
                for (auto& elem : Roads)
                {
                    if (elem.isRoad_infocus(pixelPos.x, pixelPos.y, &Field))
                    {
                        std::cout << "\n ====== Try to set road # -  " << i << "\n";
                        if (player[player_num].road < 1) { std::cout << " You have not any roads " << "\n";  break; }
                        if (elem.isYourNodeNear(player_num, &Field) == false && Game_Step.step[st].flag_set_one_Road == 1 && st != 4) 
                                       { std::cout << " No a village near this way " << "\n";   break; }
                        if (elem.isYourNodeNear(player_num, &Field) == false && elem.isYourRoadNear(player_num,i) == false && st == 4)
                                       {   std::cout << " Can not put road this way " << "\n";   break;  }
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
                        if(st == 4)
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
            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left && town_ismove == true)
            {
                town_ismove = false;        // не можем двигать город
                int i = 0;
                for (auto& elem : Field)
                {
                    if (elem.isNode_infocus(pixelPos.x, pixelPos.y))
                    {
                     std::cout << "\n ====== Try to put town node # -  "  << "\n";
                     if (st != 4 || elem.owner != player_num || elem.object != VILLAGE)   {  break;  }

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

        } //end  внутренний  while

        
        
        //---------------------------------------------------------------------------------------------------------------------------------------
        window.clear();
        //std::cout << " ================ Start_Draw Field ================= " << std::endl;
        if (flag_draw_gecs_num == true) DrawGecsNum(&window, &Gecs);
        if(flag_draw_gecs == true)  DrawGecs(&window, &Gecs);
        if(flag_draw_road == true)  DrawRoads(&window, &Field,&Roads);
        if (flag_draw_node == true)  DrawNodes(&window, &Field);
        

        DrawResursBank(&window);    //ресурсы в банке не принадлежат игрокам
        DrawPlayer(&window);
        if(st == 4) DrawChangeBank(&window,150,650,player_num);

        if (bandit_ismove == true)  DrawBanditOnCoord(&window, pixelPos.x - dX, pixelPos.y - dY, 0.2f);
        else
            {   DrawBandit(&window, &Gecs); }

        //если передвигается карта ресурса
        if (CARD_ismove)   DrawCard(&window,(int)CARD_type_move, pixelPos.x - dX, pixelPos.y - dY, 0.5f);

        //если можем двигать деревню
        if (village_ismove)   DrawVillage(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2f);
        //если можем двигать город
        if (town_ismove)   DrawTown(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2f);
        //если можем двигать дорогу
        if (road_ismove)   DrawRoad(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2, 0);

        Draw_Step_Info(&window);

        if (player_num == 1 && Count_Num_players() >= 2 && st == 0)  Draw_Start(&window);

        if (player_num == Game_Step.current_active_player && st == 1)   Draw_Cubic(&window);

        if (player_num == Game_Step.current_active_player && st == 4)
              if(Game_Step.step[st].roll_2_dice == 1)    Draw_Cubic(&window);

        window.display();   
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


