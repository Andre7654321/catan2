#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include "CatanField.h"
#include "Catan_View.h"
#include "Catan_Step.h"


int flag_font_load = 0;
sf::Font font;   //шрифт
extern int CARD_Bank[10];
extern PLAYER player[4];
extern int player_num;

extern GAME_STEP Game_Step;

int Field_CATAN_x = 230;
int Field_CATAN_y  = 120;
float field_scale = 1.3f;


//================================================================
//  
//================================================================
void Draw_Cubic(sf::RenderWindow* win)
{
    sf::Texture texture;
    texture.loadFromFile("img/Cube.png");
    sf::Sprite sprite;    sprite.setTexture(texture);

    sprite.setPosition(950, 50);

    win->draw(sprite);
    return;
}

//================================================================
//  иконка - приглашение к началу игры
//================================================================
void Draw_Start(sf::RenderWindow* win)
{
    sf::Texture texture;
    texture.loadFromFile("img/Start.png");
    sf::Sprite sprite;    sprite.setTexture(texture);

    sprite.setPosition(950,50);

    win->draw(sprite);
    return;
}

//================================================================
//  вывод информации о текущем шаге и ..
//================================================================
void Draw_Step_Info(sf::RenderWindow* win)
{
 char str[80] = "";
 char str1[30] = "";

     sf::Text text_step("", font, 20);   //для информации о текущем шаге игры
     text_step.setFillColor(sf::Color::White);

    if (Game_Step.current_step == 0)    strcpy_s(str, "Waiting for GAME Start");
    if (Game_Step.current_step == 1)    strcpy_s(str, "Roll the dice -   step 1");
    if (Game_Step.current_step == 2)    strcpy_s(str, "Set 1 Village and 1 road near -  step 2");
    if (Game_Step.current_step == 3)    strcpy_s(str, "Set 1 Village(+take RESORCE) and 1 road near - step 3");
    if (Game_Step.current_step == 4)    strcpy_s(str, "  ---------- Standart step ---------");

    if (player_num == Game_Step.current_active_player) strcat_s(str, ",  your move");
    else { 
         strcat_s(str, ",  player ");  
         _itoa_s(Game_Step.current_active_player, str1, 10);  strcat_s(str1, "  move ");
         strcat_s(str,str1);
         }
         
    text_step.setString(str);
    text_step.setPosition(450, 20);      win->draw(text_step);

    if (player_num != 0) return;

    strcpy_s(str, "First Pl = ");  _itoa_s(Game_Step.start_player, str1, 10);  strcat_s(str,str1);
    text_step.setString(str);
    text_step.setPosition(450, 50);      win->draw(text_step);
}
//------------------------------------------------------------------------------------

sf::Texture texture_town[5];
int flag_texturetown_set = 0;
sf::Sprite sprite_town;

//================================================================
//  прорисовка 1 города по заданным координатам
//================================================================
void DrawTown(sf::RenderWindow* win, int player, int x, int y, float scale)
{
    if (flag_texturetown_set == 0)
    {
        flag_texturetown_set = 1;
        texture_town[0].loadFromFile("img/Town_yellow.png");
        texture_town[1].loadFromFile("img/Town_red.png");
        texture_town[2].loadFromFile("img/Town_blue.png");
        texture_town[3].loadFromFile("img/Town_green.png");
        texture_town[4].loadFromFile("img/Town_yellow.png");
    }
    sprite_town.setTexture(texture_town[player]);

    sprite_town.setScale(scale, scale);

    auto size = sprite_town.getTexture()->getSize();
    sprite_town.setOrigin((size.x / 2) * scale, (size.y / 2) * scale);
    sprite_town.setPosition(x, y);
    win->draw(sprite_town);
    return;
}
//-----------------------------------------------------------------

sf::Texture texture_village[5];
int flag_texture_village_set = 0;
sf::Sprite sprite_village;

//================================================================
//  прорисовка 1 деревни по заданным координатам
//================================================================
void DrawVillage(sf::RenderWindow* win, int player, int x, int y, float scale)
{
    if (flag_texture_village_set == 0)
    {
        flag_texture_village_set = 1;

        texture_village[0].loadFromFile("img/Village_yellow.png");
        texture_village[1].loadFromFile("img/Village_red.png");
        texture_village[2].loadFromFile("img/Village_blue.png");
        texture_village[3].loadFromFile("img/Village_green.png");
        texture_village[4].loadFromFile("img/Village_yellow.png");
    }
    sprite_village.setTexture(texture_village[player]);

    sprite_village.setScale(scale, scale);

    auto size = sprite_village.getTexture()->getSize();
    sprite_village.setOrigin((size.x / 2) * scale, (size.y / 2) * scale);

    sprite_village.setPosition(x, y);
    win->draw(sprite_village);
    return;
}
//-------------------------------------------------------------------

sf::Texture texture_road[5];
int flag_texture_road_set = 0;
sf::Sprite sprite_road;

//================================================================
//  прорисовка 1 деревни по заданным координатам
//================================================================
void DrawRoad(sf::RenderWindow* win, int player, int x, int y, float scale,int angle)
{
    if (flag_texture_road_set == 0)
    {
        flag_texture_road_set = 1;

        texture_road[0].loadFromFile("img/Road_yellow.png");
        texture_road[1].loadFromFile("img/Road_red.png");
        texture_road[2].loadFromFile("img/Road_blue.png");
        texture_road[3].loadFromFile("img/Road_green.png");
        texture_road[4].loadFromFile("img/Road_yellow.png");
    }
    sprite_road.setTexture(texture_road[player]);

    
    auto size = sprite_road.getTexture()->getSize();
    if (angle)
        {
        sprite_road.setOrigin((size.x / 2), (size.y / 2));
        sprite_road.rotate(angle);
        }
    sprite_road.setPosition(x, y);
    sprite_road.setScale(scale, scale);
    win->draw(sprite_road);

    sprite_road.rotate(-angle);
    
    return;
}


//=====================================================================
//    ресурсы и фишки игрока 
//=====================================================================
void DrawPlayer(sf::RenderWindow* win)
{
    int f_x = 300;
    int f_y = 830;
    char str[30];

    //std::cout << " =======  DRAW PLAYER FIELD ============== " << std::endl;
    
    sf::Text text2("", font, 20); 
    text2.setFillColor(sf::Color::White);

    _itoa_s(player_num, str, 10);   strcat_s(str, " - player");
    if (player_num == 0)  strcpy_s(str, "Server Window for Control");
    text2.setString(str);
    text2.setPosition(f_x, f_y - 40);      win->draw(text2);

    //начинаем с 1, так как начальный ресурс EMPTY
    for(int i = 1; i < 6; i++)
    {
        if (player[player_num].resurs[i] >= 0)
            {
            DrawCard(win, i, f_x + 60 * (i - 1),f_y, 0.5f);
            text2.setPosition(f_x + 10 + 60*(i -1 ), f_y + 80);     _itoa_s(player[player_num].resurs[i], str, 10);    text2.setString(str); win->draw(text2);
            }
    }

    //фишки города, деревни, дороги
    DrawTown(win, player_num, f_x + 340, f_y - 40, 0.2);
    text2.setPosition(f_x + 400, f_y - 40);     _itoa_s(player[player_num].town, str, 10);    text2.setString(str); win->draw(text2);
    DrawVillage(win, player_num, f_x + 340, f_y + 30, 0.2);
    text2.setPosition(f_x + 400, f_y + 30);     _itoa_s(player[player_num].village, str, 10);  text2.setString(str);  win->draw(text2);
    DrawRoad(win, player_num, f_x + 465, f_y, 0.2,0);
    text2.setPosition(f_x + 500, f_y);      _itoa_s(player[player_num].road, str, 10);    text2.setString(str);  win->draw(text2);


    //если есть еще активные игроки - прорисовать их на экране
    auto right = false;  //не занята сторона справа
    auto left = false;
    for(int i = 1;i < 4;i++)
       {
        if (i == player_num) continue;   //свой номер
        if (player[i].active == true)
            {
            _itoa_s(i, str, 10);
            if (!right) {   right = true;    text2.setPosition(1050, 250);   DrawCard(win, 1, 1080, 300, 0.5f);   }
                else if(!left) {    left = true;    text2.setPosition(50, 250);    DrawCard(win, 1, 50, 300, 0.5f);     }
                     else             {     text2.setPosition(50, 450);  DrawCard(win, 1, 50, 500, 0.5f);              }
            strcat_s(str, " - player");
            text2.setString(str);   win->draw(text2);
            }
       }

    return;
}

//=====================================================================
//пересчет игровых координат в коор экрана
//=====================================================================
float Draw_x(int x)
{
    return (2.8f * x * field_scale + Field_CATAN_x);
}
float Draw_y(int y)
{
    return (3.03f * y * field_scale + Field_CATAN_y);
}
//=====================================================================
//пересчет координат экрана в игровые
//=====================================================================
int Game_x(int x)
{
   x -= Field_CATAN_x;    x /= (2.8f * field_scale);
   return x;
}
int Game_y(int y)
{
    y -= Field_CATAN_y;  y /= (3.03f * field_scale);
    return y;
}

//=====================================================================
void DrawGecsNum(sf::RenderWindow* window, std::vector<GECS>* Gs)
{
 int x, y;
    char str[10];
    int i = 0;
    for (auto& g : *Gs)
    {
        //получить координаты гекса
        x = g.getGecs_x();  y = g.getGecs_y();
        if (flag_font_load == 0)  {   flag_font_load = 1;   font.loadFromFile("img/Roboto.ttf");  }

        sf::Text text("", font, 33);  text.setFillColor(sf::Color::White);
        _itoa_s(i, str, 10);   //номер гекса
        text.setString(str);

        if (i < 10) text.setPosition(Draw_x(x) - 10, Draw_y(y) - 20);
            else text.setPosition(Draw_x(x) - 20, Draw_y(y) - 20);
        window->draw(text);
        i++;
    }
    return;
}


//=====================================================================
void DrawGecs(sf::RenderWindow* window,std::vector<GECS>* Gs)
{
    sf::Sprite sprite1;   //Спрайт для вывода гексов 
    //создаем текстуры для гексов
    sf::Texture gecs_textute[10];
    gecs_textute[(int)(RESURS::WOOD)].loadFromFile("img/Get_wood.png");
    gecs_textute[(int)(RESURS::OVCA)].loadFromFile("img/Get_ovca.png");
    gecs_textute[(int)(RESURS::BRICKS)].loadFromFile("img/Get_bricks.png");
    gecs_textute[(int)(RESURS::STONE)].loadFromFile("img/Get_stone.png");
    gecs_textute[(int)(RESURS::BREAD)].loadFromFile("img/Get_bread.png");
    gecs_textute[(int)(RESURS::PIRATE)].loadFromFile("img/Get_pirate.png");

    
    int x, y;
    char str[10];
    for (auto &g : *Gs)
    {
    //получить координаты гекса
    x = g.getGecs_x();  y = g.getGecs_y();

    sprite1.setPosition(Draw_x(x) - 55 * field_scale, Draw_y(y) - 59 * field_scale);
    sprite1.setTexture(gecs_textute[(int)g.type]);

    sprite1.setScale(0.56f * field_scale, 0.525f * field_scale);
    window->draw(sprite1);

    if (flag_font_load == 0)  
        {
        flag_font_load = 1;
        font.loadFromFile("img/Roboto.ttf");
        }
    
    sf::Text text("", font, 33);  text.setFillColor(sf::Color::Black);

    _itoa_s(g.gecs_game_number, str, 10);
    if (g.gecs_game_number <= 0)   str[0] = 0;
    //_itoa_s(i, str, 10);   //номер гекса
    text.setString(str);

    if (g.gecs_game_number < 10) text.setPosition(Draw_x(x)-10, Draw_y(y)-20);
    else text.setPosition(Draw_x(x) - 20, Draw_y(y) - 20);
    window->draw(text);
        
    }
 return;
}

//========================================================================
void DrawNodes(sf::RenderWindow* window, std::vector<NODE>* PtrNode)
{
 //фигура под прорисовку области узла  при отладке
 sf::CircleShape NODE_area(17.f);
 NODE_area.setOutlineThickness(1);
 NODE_area.setOutlineColor(sf::Color::Color(220, 250, 70));
 NODE_area.setFillColor(sf::Color::Transparent);

 for (auto& n : *PtrNode)
 {
     NODE_area.setPosition(Draw_x(n.getNode_x()) - 17, Draw_y(n.getNode_y()) - 16);
     //window->draw(NODE_area);

     if (n.owner != -1)
     {
         if (n.object == VILLAGE)  DrawVillage(window,n.owner, Draw_x(n.n_x) - 21, Draw_y(n.n_y) - 15, 0.2f);
         if (n.object == TOWN)    DrawTown(window, n.owner, Draw_x(n.n_x) - 21, Draw_y(n.n_y) - 15, 0.2f);
     }
 }

 return;
}

//=======================================================================================
//  прорисовка сетки дорог
//=======================================================================================
void DrawRoads(sf::RenderWindow* window, std::vector<NODE>* PtrNode, std::vector<ROAD>* Rd)
{
 int start, end;

 //задание конструктора фигуры - линии для прорисовки дороги на карте
 //тонкая линия дороги - не требует повората спрайта, рисуется из начальных координат узла до конечных
 sf::VertexArray line(sf::Lines, 2);

  for (auto &rr : *Rd)
    {
    start = rr.Node_num_start;
    line[0].position = sf::Vector2f(Draw_x(PtrNode->at(start).n_x), Draw_y(PtrNode->at(start).n_y));
    end = rr.Node_num_end;
    line[1].position = sf::Vector2f(Draw_x(PtrNode->at(end).n_x), Draw_y(PtrNode->at(end).n_y));
    line[0].color = sf::Color::Color(30, 250, 220);
    line[1].color = sf::Color::Blue;    //цвет без градиента от точки до точки
    if (rr.owner == -1) {   window->draw(line); continue;   }

    int r_x = (PtrNode->at(start).n_x + PtrNode->at(end).n_x) / 2;
    int r_y = (PtrNode->at(start).n_y + PtrNode->at(end).n_y) / 2;

    //повернуть согласно направлению дороги
    int angle = 0;
    if (PtrNode->at(start).n_x == PtrNode->at(end).n_x)   angle = 90;
    if (PtrNode->at(start).n_x < PtrNode->at(end).n_x)
        {
        if (PtrNode->at(start).n_y < PtrNode->at(end).n_y) angle = 30;
        if (PtrNode->at(start).n_y > PtrNode->at(end).n_y) angle = -30;
        }
    if (PtrNode->at(start).n_x > PtrNode->at(end).n_x)
    {
        if (PtrNode->at(start).n_y < PtrNode->at(end).n_y) angle = 150;
        if (PtrNode->at(start).n_y > PtrNode->at(end).n_y) angle = 210;
    }
    
    DrawRoad(window, rr.owner, Draw_x(r_x), Draw_y(r_y), 0.3f, angle);
    }
  return;
};
//--------------------------------------------------------------------------------


sf::Texture texture_card[10];
int flag_texturecard_set = 0;

//================================================================
//  прорисовка 1 карточки ресурса по заданным координатам
void DrawCard(sf::RenderWindow* win,int Type,int x,int y, float scale)
{
 sf::Sprite sprite_card;

    if (flag_texturecard_set == 0)
    {
        flag_texturecard_set = 1;

        texture_card[(int)RESURS::WOOD].loadFromFile("img/Card_wood.png");
        texture_card[(int)RESURS::STONE].loadFromFile("img/Card_stone.png");
        texture_card[(int)RESURS::BRICKS].loadFromFile("img/Card_bricks.png");
        texture_card[(int)RESURS::BREAD].loadFromFile("img/Card_bread.png");
        texture_card[(int)RESURS::OVCA].loadFromFile("img/Card_ovca.png");
    }

    
    sprite_card.setTexture(texture_card[Type]);

    sprite_card.setScale(scale, scale);
    sprite_card.setPosition(x, y);
    win->draw(sprite_card);
    return;
}

//================================================================
//  прорисовка банка ресурсов - карточек ресурсов
//================================================================
void DrawResursBank(sf::RenderWindow* win)
{
 //прямоугольник - область под банк ресурсов
 sf::RectangleShape Bank_area(sf::Vector2f(270.f, 100.f));   //параметры  ширина и высота
 Bank_area.setPosition(5, 5);    Bank_area.setOutlineThickness(2);
 Bank_area.setOutlineColor(sf::Color::Color(200, 200, 200));    Bank_area.setFillColor(sf::Color::Transparent);

 sf::Text text2("", font, 20);   //более мелкий шрифт для подписей
 text2.setFillColor(sf::Color::White);


 win->draw(Bank_area);
 char str[20];

 DrawCard(win, (int)RESURS::WOOD, 10, 10, 0.4);
 text2.setPosition(20, 70);     _itoa_s(CARD_Bank[(int)(RESURS::WOOD)], str, 10);    text2.setString(str); win->draw(text2);

 DrawCard(win, (int)RESURS::BRICKS, 60, 10, 0.4);
 text2.setPosition(70, 70);    _itoa_s(CARD_Bank[(int)RESURS::BRICKS], str, 10);    text2.setString(str);  win->draw(text2);

 DrawCard(win, (int)RESURS::OVCA, 110, 10, 0.4);
 text2.setPosition(120, 70);     _itoa_s(CARD_Bank[(int)RESURS::OVCA], str, 10);    text2.setString(str);  win->draw(text2);

 DrawCard(win, (int)RESURS::STONE, 160, 10, 0.4);
 text2.setPosition(170, 70);     _itoa_s(CARD_Bank[(int)RESURS::STONE], str, 10);    text2.setString(str);  win->draw(text2);

 DrawCard(win, (int)RESURS::BREAD, 210, 10, 0.4);
 text2.setPosition(220, 70);    _itoa_s(CARD_Bank[(int)RESURS::BREAD], str, 10);    text2.setString(str);  win->draw(text2);

}
