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

// sfml-graphics-d.lib;sfml-window-d.lib;sfml-system-d.lib;%(AdditionalDependencies)  �������
// smfl - graphics - d.lib; smfl - window - d.lib; smfl - system - d.lib; % (AdditionalDependencies)


std::vector<GECS>* gecsPtr;    //��������� �� ������ ������
std::vector<NODE>* nodePtr;    //��������� �� ������ ����� ����
std::vector<ROAD>* roadPtr;    //��������� �� ������ �����

bool flag_draw_gecs = true;
bool flag_draw_node = true;
bool flag_draw_road = true;
bool flag_draw_gecs_num = false;
extern int Field_CATAN_x;
extern int Field_CATAN_y;

extern sf::Sprite sprite_road;
extern sf::Sprite sprite_town;
extern sf::Sprite sprite_village;


//���������� ����������
int player_num = 0;      //����� ���������� ������   0 - ��� �������
int CARD_Bank[10];       //��������� ���������� �������� �������� �� ������������� �������
const int MAXPLAYERS = 4;
PLAYER player[5];       //����������� ������ ��� ������������ ����� ������� + server
int num_players = 0;

bool village_ismove = false;    //�������������� �������
bool town_ismove = false;       //�������������� ������
bool road_ismove = false;
float dX, dY;              //����������� ��� �������� ��� ��������������

//������ ������ ���� --------------------------------------------------------
GAME_STEP Game_Step;

int main()
{
   setlocale(LC_ALL, "rus");

   CARD_Bank[(int)RESURS::WOOD] = 40; CARD_Bank[(int)RESURS::STONE] = 40; CARD_Bank[(int)RESURS::OVCA] = 40;
   CARD_Bank[(int)RESURS::BREAD] = 40; CARD_Bank[(int)RESURS::BRICKS] = 40;

   //������������� ���� ����, ������, ����� �� ������� ������� ��������
   std::vector<GECS> Gecs;
   std::vector<NODE> Field;
   std::vector<ROAD> Roads;

   gecsPtr = &Gecs;
   nodePtr = &Field;
   roadPtr = &Roads;

   std::cout << "  Call init main field " << std::endl;
   Init_CATAN_Field(&Gecs, &Field, &Roads);
   //--------------------------------------------------------------------

   Game_Step.current_step        = 0;           //������ ����
   Game_Step.start_player        = 1;
   Game_Step.current_active_player = 1;           //������ ������ �� ������� �0


    char connectionType;
    std::cout << "Enter (s) for server, Enter (c) for client" << std::endl;
    std::cin >> connectionType;
    
    //������ ������� ����������
    if (connectionType == 's')      { std::cin.clear();  Start_Server_CATAN();   player_num = 0;   }
    else
       {
        std::cin.clear();
        //��������� ���� ���������� ����� ���� �� ������ - ������� ������ ��� ������������,
        //��� ��� ��� ����������� ������ ��� �������� ������������ ������, � ������ � ���� �� ������ ��������
        int ret = Init_Client_CATAN();
        if (ret == 0) { std::cout << "  No connect , exit" << std::endl;     return 0; }
       }


    //SMFL --------------------------------------------------------------------------------------
    sf::RenderWindow window(sf::VideoMode(1200, 960), "CATAN standart FIELD", sf::Style::Close);
    window.setPosition(sf::Vector2i(470.0f, 0.0f));
        
    //������� ������, �������� ���������������, ������� �������� ������ ���� ���� ��� �����
    sf::RectangleShape Road(sf::Vector2f(60.f, 7.f));   //���������  ������ � ������
    Road.setOrigin(0, 4);  //������� ����� ��������� � �������� ������� �����

    //--------------------------------------------------------------------------------------------



    int st;
    //main cycle ===============================================================================================
    while (window.isOpen())
    {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);   //�������� ����� �������
        st = Game_Step.current_step;

        sf::Event event;
        while(window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)       window.close();

            //����������� ��������� ������, � �� ���� �������
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  {  flag_draw_gecs ? flag_draw_gecs = false : flag_draw_gecs = true; } 
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { flag_draw_node ? flag_draw_node = false : flag_draw_node = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    { flag_draw_road ? flag_draw_road = false : flag_draw_road = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { flag_draw_gecs_num ? flag_draw_gecs_num = false : flag_draw_gecs_num = true; }

            //����������� ���� ������� �������
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space && player_num == Game_Step.current_active_player)
                  {
                     if (Game_Step.current_step == 3 && Game_Step.step[st].flag_set_one_Village == 0 && Game_Step.step[st].flag_set_one_Road == 0)
                                 {    Say_Move_Over();     }
                     if (Game_Step.current_step == 2 && Game_Step.step[st].flag_set_one_Village == 0 && Game_Step.step[st].flag_set_one_Road == 0)
                                 {        Say_Move_Over();      }
                     if (Game_Step.current_step == 1) { Say_Roll_1Dice(); }
                     if (Game_Step.current_step == 0) { Say_Start(); }
                  }

            //���������� ��������� �������� ���� ���������
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape && player_num == Game_Step.current_active_player)
            {
                if (Game_Step.current_step == 2)
                    {
                    Ask_Send_Arrays();    //�� ECS ����� ���������� �� ������� ���������� ����
                    Game_Step.step[st].flag_set_one_Village = 1;
                    Game_Step.step[st].flag_set_one_Road = 1;
                    player[player_num].village = 5;
                    player[player_num].road = 15;
                    }
                if (Game_Step.current_step == 3)
                {
                    Ask_Send_Arrays();     //�� ECS ����� ���������� �� ������� ���������� ����
                    Game_Step.step[st].flag_set_one_Village = 1;
                    Game_Step.step[st].flag_set_one_Road = 1;
                    player[player_num].village = 4;
                    player[player_num].road = 14;
                    //�������� �������
                }
            }
            
            //���� ������ ������� ���� �����  
            if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left)
            {
                for (auto& elem : Field)
                {
                    if (elem.isNode_infocus(pixelPos.x, pixelPos.y))
                    {
                    std::cout << " Node selected =  " << elem.number << "  Pl owner =  " << elem.owner << std::endl;
                    }
                }
                
               if (sprite_village.getGlobalBounds().contains(pixelPos.x, pixelPos.y))   //������������ ������� ������
                {
                std::cout << "  Village  selected  " << std::endl;
                auto coord = sprite_village.getPosition();
                dX = pixelPos.x - coord.x;    dY = pixelPos.y - coord.y;
                if (Game_Step.step[st].flag_set_one_Village || Game_Step.step[st].flag_setVillage)   village_ismove = true; 						
                }

               if (sprite_town.getGlobalBounds().contains(pixelPos.x, pixelPos.y))  //������������ ������ ������
                  {
                   std::cout << "  Town selected  "  << std::endl;
                   auto coord = sprite_town.getPosition();
                   dX = pixelPos.x - coord.x;    dY = pixelPos.y - coord.y;
                   if (Game_Step.step[st].flag_set_one_Town || Game_Step.step[st].flag_setTown)   town_ismove = true;
                   }

               if(sprite_road.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
               {
                   std::cout << "  Road selected  " << std::endl;
                   auto coord = sprite_road.getPosition();
                   dX = pixelPos.x - coord.x;    dY = pixelPos.y - coord.y;
                   if (Game_Step.step[st].flag_set_Road || Game_Step.step[st].flag_set_one_Road)   road_ismove = true;
               }


            } //end ������� ����� ������ ����


            //���� ��������� ����� �������  -----------------------------------------------------------------------------
            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left && village_ismove == true)     
            {
                village_ismove = false;        // �� ����� ������� �������
                int i = 0;
                for (auto& elem : Field)
                    {
                    if (elem.isNode_infocus(pixelPos.x, pixelPos.y))
                        {
                        std::cout << "\n ====== Try to put village node # -  " << i <<"\n";
                        if (player[player_num].village < 1) { std::cout << " You have not any villages " << "\n";  break; }
                        if (elem.isNode_free(i) == false)   { std::cout << "  NODE is busy " << "\n";  break; }
                                else std::cout << " OK  NODE is free " << "\n";

                        if (isVillageNear(i) == true) { std::cout << " Other Village too NEAR " << "\n";  break; }
                          
                        //if (Roads.isRoadNearNode(i, player_num) == false && Game_Step.step[st].flag_setVillage == 1 && player[player_num].village < 5)
                        //    {
                        //    std::cout << " No a road near site " << "\n";   break;
                        //    }
                        elem.owner = player_num;
                        player[player_num].village -= 1;
                        elem.object = VILLAGE;
                        if (Game_Step.step[st].flag_set_one_Village)   Game_Step.step[st].flag_set_one_Village = 0;
                        }
                    i++;
                    }
            }      //------------------------- end put village ------------------------------

            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left && road_ismove == true)
            {
                road_ismove = false;        // �� ����� ������� ������
                int i = 0;
                for (auto& elem : Roads)
                {
                    if (elem.isRoad_infocus(pixelPos.x, pixelPos.y, &Field))
                    {
                        std::cout << "\n ====== Try to set road # -  " << i << "\n";
                        if (player[player_num].road < 1) { std::cout << " You have not any roads " << "\n";  break; }
                        if (elem.isYourNodeNear(player_num, &Field) == false && Game_Step.step[st].flag_set_one_Road == 1) 
                                                                          { std::cout << " No a village near this way " << "\n";   break; }
                        if (elem.owner != -1) { std::cout << "  ROAD is BUSY " << "\n";   break; }
                        elem.owner = player_num;
                        player[player_num].road -= 1;
                        elem.type = 1;      //road
                        if (Game_Step.step[st].flag_set_one_Road)     Game_Step.step[st].flag_set_one_Road = 0;
                        break;
                    }
                    i++;
                }
            }      //------------------------- end put road ------------------------------

            if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left && town_ismove == true)
            {
                town_ismove = false;        // �� ����� ������� �����
                for (auto& elem : Field)
                {
                    if (elem.isNode_infocus(pixelPos.x, pixelPos.y))
                    {
                        std::cout << "\n ====== Try to put town node # -  "  << "\n";
                    }
                }
            }      //------------------------- end put town ------------------------------

        } //end  ����������  while

        
        
        //---------------------------------------------------------------------------------------------------------------------------------------
        window.clear();
        //std::cout << " ================ Start_Draw Field ================= " << std::endl;
        if (flag_draw_gecs_num == true) DrawGecsNum(&window, &Gecs);
        if(flag_draw_gecs == true)  DrawGecs(&window, &Gecs);
        if(flag_draw_road == true)  DrawRoads(&window, &Field,&Roads);
        if (flag_draw_node == true)  DrawNodes(&window, &Field);
        

        DrawResursBank(&window);
        DrawPlayer(&window);

        //���� ����� ������� �������
        if (village_ismove)   DrawVillage(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2);
        //���� ����� ������� �����
        if (town_ismove)   DrawTown(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2);
        //���� ����� ������� ������
        if (road_ismove)   DrawRoad(&window, player_num, pixelPos.x - dX, pixelPos.y - dY, 0.2, 0);

        Draw_Step_Info(&window);

        if (player_num == 1 && Count_Num_players() >= 2 && Game_Step.current_step == 0)  Draw_Start(&window);

        if (player_num == Game_Step.current_active_player && Game_Step.current_step == 1)   Draw_Cubic(&window);


        window.display();   
    }


    char close = '0';
    while (close != '1')
    {
        std::cout << "Enter 1  for close" << std::endl;
        std::cin >> close;
    }
    return 0;
}


