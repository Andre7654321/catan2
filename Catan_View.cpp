#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include "CatanField.h"
#include "Catan_View.h"
#include "Catan_Step.h"
#include "Catan_Count.h"

int flag_font_load = 0;
sf::Font font;   //шрифт
extern int CARD_Bank[12];
extern std::vector<IMP_CARD> improve_CARDS;
extern std::vector<IMP_CARD> develop_CARDS[5]; //карты развития игроков
extern PLAYER player[7];
extern int limit_7[7];
extern int player_num;
extern int max_road_owner;
extern int max_army;        //владелец карточки самое большое войско
extern int max_gavan;

extern int Play_two_roads;     //флаг игры карты развития 2 дороги
extern int Play_two_resurs;
extern int Play_ONE_resurs;

extern std::vector<GECS>  FishGecs;   //вектор рыбных отмелей
extern std::vector<RESURS>  FishCards;  //вектор карточек с рыбами
extern int Game_type;     //1 - Standart CATAN   2 - FISH

//область обмена ресурсами
extern int ChangeBANK[7][12];
extern CHANGE Change[12];

extern GAME_STEP Game_Step;

int Field_CATAN_x = 330;
int Field_CATAN_y  = 160;
float field_scale = 1.1f;


sf::Sprite sprite_move_over;
sf::Texture texture_moveover;
int flag_move_over = 0;

//================================================================
//  прорисовка спрайта окончания хода
//================================================================
void Move_Over_Logo(sf::RenderWindow* win)
{
    if(flag_move_over == 0)
        {
        flag_move_over = 1;
        texture_moveover.loadFromFile("img/Move_over.png");
        }

    sprite_move_over.setTexture(texture_moveover);
    sprite_move_over.setPosition(1000, 50);
    sprite_move_over.setScale(0.5, 0.5);

    win->draw(sprite_move_over);
    return;
}
//-------------------------------------------------------------------

int Big_Message = -1;
std::chrono::time_point<std::chrono::steady_clock>  start_Big_Message;
//================================================================
//  сообщение на весь экран об игре карты развития
//================================================================
void Big_Message_Imp_Card(sf::RenderWindow* win,int type)
    {
    //задать время иницициализации сообщения
    std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start_Big_Message;
    //отводим 2 сек на отображение
    if((float)(duration.count()) > 2.0)   Big_Message = -1;

    float scale = 2.5;
    if ((float)duration.count() < 1)  scale = 2.5 * duration.count();
    if(type >= 0 && type < 5) DrawDevelopCard(win, type, 450,100,scale, 0);
    //окно с выводом текста из назначенного буфера
    if (type == 10)  DrawBigMessageBuff(win);
    return;
    }
//----------------------------------------------------------------

char BuffBigMessage[256];
//================================================================
//  сообщение на весь экран c текстом из буфера сообщений
//================================================================
void DrawBigMessageBuff(sf::RenderWindow* win)
{
    //вывести спрайт сообщения

    //вывести текст сообщения
    sf::Text text_step("", font, 70);
    text_step.setFillColor(sf::Color::Red);

    text_step.setString(BuffBigMessage);
    text_step.setPosition(300, 300);      win->draw(text_step);

}

//================================================================
//  сообщение на весь экран об игре карты развития
//================================================================
void Game_Message(sf::RenderWindow* win, const  char* mess)
{
    sf::Text text_step("", font, 70);  
    text_step.setFillColor(sf::Color::Red);

    text_step.setCharacterSize(70);
    text_step.setString(mess);
    text_step.setPosition(300,300);      win->draw(text_step);

    return;
}
//----------------------------------------------------------------

sf::Sprite bandit_sprite;
int bandit_Gecs = -1;
//================================================================
//  если Гекс == -1 - бандит в привязке к гексу
//================================================================
void DrawBandit(sf::RenderWindow* win, std::vector<GECS>* PtrGecs)
{
    int x, y;
    float scale = 0.2;

    if (bandit_Gecs == -1)    {   scale = 0.2f;   x = Draw_x(60);  y = Draw_y(-1);    }
      else
        {
        scale = 0.25f;
        //по координатам гекса поставить бандита
        x = Draw_x(PtrGecs->at(bandit_Gecs).x);
        y = Draw_y(PtrGecs->at(bandit_Gecs).y);
        }
    DrawBanditOnCoord(win,x,y, scale);
    return;
}

//================================================================
//  прорисовка спрайта бандита по координатам
//================================================================
void DrawBanditOnCoord(sf::RenderWindow* win,int x, int y, float scale)
{

 sf::Texture texture;
 texture.loadFromFile("img/Bandit.png");
 bandit_sprite.setTexture(texture);

 bandit_sprite.setScale(scale, scale);
 auto size = bandit_sprite.getTexture()->getSize();
 bandit_sprite.setOrigin(size.x / 2, size.y / 1.8);

 bandit_sprite.setPosition(x,y);
 win->draw(bandit_sprite);

 return;
}
//------------------------------------------------------------------------

sf::Sprite cube_sprite;
//================================================================
//  
//================================================================
void Draw_Cubic(sf::RenderWindow* win)
{
    sf::Texture texture;
    texture.loadFromFile("img/Cube.png");
    cube_sprite.setTexture(texture);

    cube_sprite.setPosition(850, 50);
    cube_sprite.setScale(1.5,1.5);

    win->draw(cube_sprite);
    return;
}

sf::Sprite sprite_start;
//================================================================
//  иконка - приглашение к началу игры
//================================================================
void Draw_Start(sf::RenderWindow* win)
{
    sf::Texture texture;
    texture.loadFromFile("img/Start.png");
    sprite_start.setTexture(texture);

    sprite_start.setPosition(950,50);

    win->draw(sprite_start);
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

     if (Game_Step.current_step < 4)
         {
         if (Game_Step.current_step == 0)    strcpy_s(str, "Waiting for GAME Start");
         if (Game_Step.current_step == 1)    strcpy_s(str, "Roll the dice -   step 1");
         if (Game_Step.current_step == 2)    strcpy_s(str, "Set 1 Village and 1 road near -  step 2");
         if (Game_Step.current_step == 3)    strcpy_s(str, "Set 1 Village(+take RESORCE) and 1 road near - step 3");

         strcat_s(str, ",  player ");
         _itoa_s(Game_Step.current_active_player, str1, 10);  strcat_s(str1, "  move ");   strcat_s(str, str1);
         text_step.setString(str);
         text_step.setPosition(450, 20);
         win->draw(text_step);
         }

    if (player_num == Game_Step.current_active_player && Game_Step.current_step >= 1)
        {
        strcpy_s(str, "Your move"); 
        text_step.setFillColor(sf::Color::Color(80, 255, 120));
        if (Game_Step.step[4].flag_bandit == 1) { strcpy_s(str, "   Move Banditos  "); text_step.setFillColor(sf::Color::Color(250, 125, 120));  }
        if (isCardsDeletedAfterSeven() == false) { strcpy_s(str, "Wait for cut cards "); text_step.setFillColor(sf::Color::Color(250, 125, 120)); }
        text_step.setCharacterSize(50);  
        text_step.setString(str);
        text_step.setPosition(540, 41);      win->draw(text_step);
        }

    if (player_num != Game_Step.current_active_player && limit_7[player_num] < getPlayerNumCardResurs(player_num) &&  limit_7[player_num] > 0)
    {
        strcpy_s(str, "Send cards to BANK");      text_step.setFillColor(sf::Color::Color(250, 125, 120));
        text_step.setCharacterSize(50);
        text_step.setString(str);
        text_step.setPosition(500, 41);      win->draw(text_step);
    }

    if(Play_two_roads > 0)
        {
        strcpy_s(str, "You may set 2 roads");      text_step.setFillColor(sf::Color::Color(120, 255, 120));
        text_step.setCharacterSize(50);
        text_step.setString(str);
        text_step.setPosition(480, 2);      win->draw(text_step);
        }

    if (Play_two_resurs > 0)
        {
        strcpy_s(str, "You may get 2 cards");      text_step.setFillColor(sf::Color::Color(120, 255, 120));
        text_step.setCharacterSize(50);
        text_step.setString(str);
        text_step.setPosition(480, 2);      win->draw(text_step);
        }

    if (Play_ONE_resurs > 0)
    {
        strcpy_s(str, "Choos RESURS Type");      text_step.setFillColor(sf::Color::Color(120, 255, 120));
        text_step.setCharacterSize(50);
        text_step.setString(str);
        text_step.setPosition(490, 2);      win->draw(text_step);
    }
         
    if (player_num != 0) return;

    strcpy_s(str, "First Player = ");  _itoa_s(Game_Step.start_player, str1, 10);  strcat_s(str,str1);
    text_step.setString(str);
    text_step.setPosition(550, 50);      win->draw(text_step);

    return;
}
//------------------------------------------------------------------------------------

sf::Texture texture_town[7];
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

sf::Texture texture_village[7];
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

sf::Texture texture_road[7];
int flag_texture_road_set = 0;
sf::Sprite sprite_road;

//================================================================
//  прорисовка 1 деревни по заданным координатам
//================================================================
void DrawRoad(sf::RenderWindow* win, int player, int x, int y, float scale, int angle)
{
    if (player < 0) return;

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
//-----------------------------------------------------------------------

sf::Sprite sprite_chBank;
sf::Sprite sprite_chBank_clear;
sf::Sprite sprite_take_card[7];
sf::Sprite sprite_change[7];
sf::Texture texture_change;
sf::Texture texture_chBank_set;
sf::Texture texture_chBank_clear;
sf::Texture texture_take_card;
int flag_texture_chBank_set = 0;

//=====================================================================
//    ресурсы выставленные для обмена
//=====================================================================
void DrawChangeBank(sf::RenderWindow* win,int x,int y,int pl)
{
    char str[20];

 //область под банк ресурсов
 if (flag_texture_chBank_set == 0)
    {
    flag_texture_chBank_set = 1;
    texture_chBank_set.loadFromFile("img/ch_bank_grey.png");
    texture_chBank_clear.loadFromFile("img/Crest_red.png");
    texture_take_card.loadFromFile("img/Take_card.png");
    texture_change.loadFromFile("img/change.png");
    }
 sprite_chBank.setTexture(texture_chBank_set);  
 sprite_chBank.setScale(0.75,0.70);
 sprite_chBank.setPosition(x, y); 
 win->draw(sprite_chBank); 
 
 //кнопка очистки обмена
 if (pl == player_num)
     {
     sprite_chBank_clear.setTexture(texture_chBank_clear);
     sprite_chBank_clear.setScale(0.1, 0.1);
     sprite_chBank_clear.setPosition(x + 200, y + 7);
     win->draw(sprite_chBank_clear);
     }

 //спрайт разрешения обмена -------------------------------------
 //выводится после того как брошен кубик до строительства ходящего или конца хода
 //можно меняться с пустым банком если вы ложите в обмен ботинок, тогда заявка отправляется с пустым полем от получателя
 //сервер получая такую заявку сразу ее отрабатывает
 if ((player_num == Game_Step.current_active_player || pl == Game_Step.current_active_player)  && player[player_num].flag_allow_change == 1 && 
         (numCardChange(pl) > 0 || ChangeBANK[player_num][(int)RESURS::BOOT] != 0) && numCardChange(player_num) > 0 && pl != player_num)
       {
         sprite_change[pl].setTexture(texture_change);
         sprite_change[pl].setScale(0.3, 0.3);
         sprite_change[pl].setPosition(x + 195, y + 55);
         win->draw(sprite_change[pl]);  
       }
      else sprite_change[pl].setPosition(1000,1000);

 if(pl != player_num) Draw_Change_Offers(win, pl, y);
       else Draw_Change_Offers_ForActive(win);

 //кнопка разрешения забрать карту
 if (pl != player_num && player[pl].flag_allow_get_card == 1)
 {
     sprite_take_card[pl].setTexture(texture_take_card);
     sprite_take_card[pl].setScale(0.15, 0.15);
     sprite_take_card[pl].setPosition(x + 195, y + 82);
     win->draw(sprite_take_card[pl]);
 }

 sf::Text text2("", font, 16);
 text2.setFillColor(sf::Color::Black);
 
 //бонус  3:1
 if (bonus31(pl) == true)
 {
     strcpy_s(str, " 3 : 1");  text2.setString(str);
     text2.setPosition(x + 20, y + 12);
     win->draw(text2);
 }
 //бонусы 2:1
 int place = 0;
 for(int i = 1;i < 6;i++)
    {
     if (bonus21(pl, i) == true)
          {
          DrawCard_NoSprate(win, i, x + 60 + 25*place, y + 1, 0.15);
          place++;
          }
    }

 //карты войска, длинной дороги, гавань, башмак
 place = 0;
 if (pl != player_num)
     {
     if (max_road_owner == pl)  Draw_MaxWay(win, x + 80 + 25 * place++, y + 95, 0.2);
     if (max_army == pl)  Draw_MaxArmy(win, x + 80 + 25 * place++, y + 95, 0.09);
     if (max_gavan == pl)  Draw_MaxGavan(win, x + 80 + 25 * place, y + 95, 0.09);
     if(player[pl].resurs[(int)RESURS::BOOT])  DrawCard_NoSprate(win, (int)RESURS::BOOT, x + 80 + 25 * place++, y + 95, 0.16);
     }

 //сами ресурсы рубашкой в зоне обмена
 int num = getPlayerNumCardResurs(pl);
 if (num && numCardChange(pl) == 0 && player_num != pl)
     {
      if(num > 5)  num = 5;
      while(num-- > 0)  DrawCard_NoSprate(win, 0, x + 25 + num * 5, y + 27 + num * 2, 0.35);
     }

  //показываем явно в зоне обмена только существующие в этой реализации ресурсы
 int pos = 0;
  for (int i = 0; i < 11; i++)
    {
    if(ChangeBANK[pl][i] == 0) continue;  //если ресурса нет, то не прорисовываем
    if (i == (int)RESURS::EMPTY) continue;
    if (i == (int)RESURS::PIRATE) continue;

    DrawCard_NoSprate(win, i, x + pos * 35 + 20, y + 33, 0.3);
    text2.setPosition(x + i * 35 + 10, y + 82);
    
    //количество
    _itoa_s(ChangeBANK[pl][i], str, 10);
    text2.setPosition(x + pos * 35 + 30, y + 82);
    text2.setString(str);
    win->draw(text2);
    pos++;
    }

 return;
}

sf::Sprite sprite_Offer[12];
//=====================================================================
//   предложения по обмену для иных игроков в левой части экрана
//=====================================================================
void Draw_Change_Offers(sf::RenderWindow* win,int pl,int pl_y)
{
    int num = 0;
    char str[10];

    sf::Text text2("", font, 14);
    text2.setFillColor(sf::Color::White);

    //более 2 предложений у не активного игрока не может быть
    for (int s = 0; s < 12; s++)
        {
        if (Change[s].status == 0) continue;
        if (Change[s].to_pl != pl) continue;

        //область аналогичная банку обмена - заменить потом на свою
        sprite_Offer[s].setTexture(texture_chBank_set);
        sprite_Offer[s].setScale(0.6, 0.35);
        sprite_Offer[s].setPosition(310, pl_y + num*60);   //у активного игрока другие координаты
        win->draw(sprite_Offer[s]);
        int pos = 0,pos1 = 0;
        //основные ресурсы, рыбы не меняются, а заявка с сапогом обрабатывается сразу
        //но для теста будем рисовать сапог
        for (int t = 1; t < 10; t++)
        {
            if (Change[s].need_num[t] > 0)
                 {
                 DrawCard_NoSprate(win, t, 330 + (pos) * 20, pl_y + 13 + num * 60, 0.19);
                 _itoa_s(Change[s].need_num[t], str, 10);
                 text2.setPosition(330 + (pos++) * 20, pl_y + 5 + num * 60);
                 text2.setString(str);
                 win->draw(text2);
                 }
            if (Change[s].offer_num[t] > 0)
                {
                DrawCard_NoSprate(win, t, 450 - (pos1) * 20, pl_y + 13 + num * 60, 0.19);
                _itoa_s(Change[s].offer_num[t], str, 10);
                text2.setPosition(450 - (pos1++) * 20, pl_y + 5 + num * 60);
                text2.setString(str);
                win->draw(text2);
                }
            }
        num++;  //номер для вывода очередной заявки
       }

    return;
}

//=====================================================================
//   предложения по обмену игроку делающему ход
//=====================================================================
void Draw_Change_Offers_ForActive(sf::RenderWindow* win)
{
    int num = 0;
    int s_x = 380;
    int s_y = 700;
    char str[10];

    sf::Text text2("", font, 14);
    text2.setFillColor(sf::Color::White);

    //более 2 предложений у не активного игрока не может быть
    for (int s = 0; s < 12; s++)
    {
        if (Change[s].status == 0) continue;
        if (Change[s].to_pl != player_num) continue;

        //область аналогичная банку обмена - заменить потом на свою
        sprite_Offer[s].setTexture(texture_chBank_set);
        sprite_Offer[s].setScale(0.8, 0.35);
        sprite_Offer[s].setPosition(s_x, s_y - num * 60);
        win->draw(sprite_Offer[s]);
        int pos = 0, pos1 = 0;
        for (int t = 1; t < 10; t++)
            {
            if (Change[s].need_num[t] > 0)
                {
                DrawCard_NoSprate(win, t, s_x + 30 + (pos) * 20, s_y + 13 - num * 60, 0.19);
                _itoa_s(Change[s].need_num[t], str, 10);
                text2.setPosition(s_x + 30 + (pos++) * 20, s_y + 5 - num * 60);
                text2.setString(str);  
                win->draw(text2);
                }

            if (Change[s].offer_num[t] > 0)
                {
                DrawCard_NoSprate(win, t, s_x + 150 - (pos1) * 20, s_y + 13 - num * 60, 0.19);
                _itoa_s(Change[s].offer_num[t], str, 10);
                text2.setPosition(s_x + 150 - (pos1++) * 20, s_y + 5 - num * 60);
                text2.setString(str);
                win->draw(text2);
                }
            }
        // лого игрока - понимание от кого заявка на обмен
        DrawTown(win, Change[s].from_pl, s_x + 184, s_y + 7 - num * 60,0.2);
        num++;
    }

    return;
}

sf::Sprite Player_resurs_card[12];    //спрайты под карточки соего банка игрока
//=====================================================================
//    прорисовка основного игрока приложения и остальных слева экрана
//=====================================================================
void DrawPlayer(sf::RenderWindow* win)
{
    int f_x = 200;
    int f_y = 830;
    char str[90],str1[20];
    sf::Sprite sprite_red_box;

    //std::cout << " =======  DRAW PLAYER FIELD ============== " << std::endl;
    
    sf::Text text2("", font, 15); 
    text2.setFillColor(sf::Color::White);

    //если есть еще активные игроки - прорисовать их в левой части поля ----------------------
    int place = 0, num;
    for (int i = 1; i < 7; i++)
    {
        if (i == player_num) continue;   //свой номер
        if (player[i].active == true || player[i].wait == true)
        {
           if(player[i].active == true && player[i].wait == false)    DrawTown(win, i, 30, 190 + place * 160, 0.20);
           if(player[i].wait == true)      DrawRoad(win, i, 30, 190 + place * 160, 0.20,0);
            
            //количество карт игрока
            num = getPlayerNumCardResurs(i);  
            strcpy_s(str, "Cards: ");   _itoa_s(num, str1, 10);   strcat_s(str, str1);
            text2.setPosition(15, 135 + place * 160);    text2.setString(str);   win->draw(text2);

            strcpy_s(str, "Score: ");  _itoa_s(CountScore(i), str1, 10);   strcat_s(str, str1);
            text2.setPosition(15, 155 + place * 160);  text2.setString(str);  win->draw(text2);

            _itoa_s(i, str, 10);   strcat_s(str, " - player");
            if (i == Game_Step.current_active_player)  text2.setFillColor(sf::Color::Green);
            text2.setPosition(10, 240 + place * 160);
            text2.setString(str);   win->draw(text2);  text2.setFillColor(sf::Color::White);

            //последний бросок кубика
            if (player[i].last_dice != 0)
                {
                text2.setFillColor(sf::Color::Red);
                strcpy_s(str, "Last dice: ");  _itoa_s(player[i].last_dice, str1, 10);   strcat_s(str, str1);
                text2.setPosition(180, 240 + place * 160);  
                text2.setString(str);  win->draw(text2);
                text2.setFillColor(sf::Color::White);
                }

            if (Game_Step.current_step == 4)  DrawChangeBank(win, 90, 110 + 160 * place, i);
            place++;
        }
    }    // end another players
    //----------------------------------------------------------------------------------

    // поле основного игрока -----------------------------------------------------------
    _itoa_s(player_num, str, 10);   strcat_s(str, " - player");
    if (player_num == 0)  strcpy_s(str, "Server Window for Control");
    text2.setString(str);
    text2.setPosition(f_x + 40, f_y - 40);      win->draw(text2);

    //начинаем с 1, так как начальный ресурс EMPTY
    //надо чтобы спрайт ресурса был доступен для перетаскивания в зону обмена
    int num_resurs = 6;
    if (Game_type == 2) num_resurs = 10;
    for(int i = 1; i < num_resurs; i++)
    {
     if (player[player_num].resurs[i] == 0 && i >= 6)  continue;
     if (player[player_num].resurs[i] >= 0)
            {
            int y = f_y;
            if (i >= 6)  y += 20;
            DrawCard(win, i, f_x + 60 * (i - 1),y, 0.5f, &Player_resurs_card[i]);
            text2.setPosition(f_x + 10 + 60*(i -1 ), f_y + 80);     _itoa_s(player[player_num].resurs[i], str, 10);    text2.setString(str); win->draw(text2);
            }
    }

    if (player_num > 0)   //для сервера не рисуем фишки городов и деревень
    {
        //количество карт игрока
        num = getPlayerNumCardResurs(player_num);
        strcpy_s(str, "Resurs: ");   _itoa_s(num, str1, 10);  strcat_s(str, str1);
        text2.setPosition(f_x - 100, f_y + 20);    text2.setString(str);   win->draw(text2);

        //последний бросок кубика
        if (player[player_num].last_dice != 0)
        {
            text2.setFillColor(sf::Color::Red);      text2.setCharacterSize(55);
            _itoa_s(player[player_num].last_dice, str, 10);
            text2.setPosition(f_x - 90, f_y - 60);  text2.setString(str);  win->draw(text2);
            text2.setFillColor(sf::Color::White);  text2.setCharacterSize(15);
        }

        //фишки города, деревни, дороги основного игрока
        DrawTown(win, player_num, f_x + 360, f_y - 50, 0.2);
        text2.setPosition(f_x + 410, f_y - 40);     _itoa_s(player[player_num].town, str, 10);    text2.setString(str); win->draw(text2);
        DrawVillage(win, player_num, f_x + 440, f_y -45, 0.2);
        text2.setPosition(f_x + 490, f_y - 45);     _itoa_s(player[player_num].village, str, 10);  text2.setString(str);  win->draw(text2);
        DrawRoad(win, player_num, f_x + 530, f_y - 30, 0.2, 0);
        text2.setPosition(f_x + 560, f_y - 40);      _itoa_s(player[player_num].road, str, 10);    text2.setString(str);  win->draw(text2);

        //счет и дорога
        if (player_num > 0)
        {
            strcpy_s(str, "Score : ");
            _itoa_s(CountScore(player_num), str1, 10); strcat_s(str, str1);
            text2.setPosition(f_x + 130, f_y - 40);  text2.setString(str);  win->draw(text2);

            strcpy_s(str, "Max Road : ");
            _itoa_s(Count_Road_Length(player_num), str1, 10);  strcat_s(str, str1);
            text2.setPosition(f_x + 225, f_y - 40);  text2.setString(str);  win->draw(text2);

            if (max_road_owner == player_num)  Draw_MaxWay(win, f_x + 850, f_y + 30,0.4);
            if (max_army == player_num)        Draw_MaxArmy(win, f_x + 910, f_y + 30,0.2);

            //карты развития
            Draw_Develop_Cards_Field(win);
        }
    }
       else
            {
            //общий банк карт развития - выводится на сервере
            for (int i = 0; i < 5 && i < improve_CARDS.size(); i++)
                 {
                 DrawDevelopCard(win, (int)improve_CARDS.at(i).type, f_x + 550 + i * 50, f_y + 10, 0.22, 0);
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

//----------------------------------------------------------------------
sf::Texture textute_fon;
sf::Sprite sprite_fon;
int flag_load_fon = 0;
//=====================================================================
//  фон основного поля
//=====================================================================
void DrawCATAN19(sf::RenderWindow* win)
{
    if (flag_load_fon == 0)
        {
        textute_fon.loadFromFile("img/CATAN19.png");
        sprite_fon.setTexture(textute_fon);
        }

    sprite_fon.setPosition(Field_CATAN_x - 27 * field_scale, Field_CATAN_y - 57 * field_scale);
    sprite_fon.setScale(0.98f * field_scale, 0.92f * field_scale);
    win->draw(sprite_fon);

    return;
}

//=====================================================================
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
//=====================================================================
void DrawGecs(sf::RenderWindow* window,std::vector<GECS>* Gs)
{
    sf::Sprite sprite1;   //Спрайт для вывода гексов 
    //создаем текстуры для гексов
    sf::Texture gecs_textute[12];
    gecs_textute[(int)(RESURS::WOOD)].loadFromFile("img/Get_wood.png");
    gecs_textute[(int)(RESURS::OVCA)].loadFromFile("img/Get_ovca.png");
    gecs_textute[(int)(RESURS::BRICKS)].loadFromFile("img/Get_bricks.png");
    gecs_textute[(int)(RESURS::STONE)].loadFromFile("img/Get_stone.png");
    gecs_textute[(int)(RESURS::BREAD)].loadFromFile("img/Get_bread.png");
    gecs_textute[(int)(RESURS::PIRATE)].loadFromFile("img/Get_pirate.png");
    gecs_textute[(int)(RESURS::FISH1)].loadFromFile("img/Get_fish.png");

    int x, y, i = 0;
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

    if(i == bandit_Gecs)  text.setPosition(Draw_x(x) - 2, Draw_y(y) + 15);   //!!!!!!!!!!
    window->draw(text);
    i++;
    }

    sf::Texture fish_textute[11];
    fish_textute[4].loadFromFile("img/Fish4.png");
    fish_textute[5].loadFromFile("img/Fish5.png");
    fish_textute[6].loadFromFile("img/Fish6.png");
    fish_textute[8].loadFromFile("img/Fish8.png");
    fish_textute[9].loadFromFile("img/Fish9.png");
    fish_textute[10].loadFromFile("img/Fish10.png");
    
    //рыбные
    if (Game_type == 1)  return;

    int k,t_num;
    k = 0;
    for (auto& g : FishGecs)
    {
        //получить координаты гекса
        x = g.getGecs_x();  y = g.getGecs_y();

        sprite1.setPosition(Draw_x(x) - 0 * field_scale, Draw_y(y) - 0 * field_scale);
        if(k >= 3)  sprite1.setPosition(Draw_x(x) + 2 * field_scale, Draw_y(y) + 4 * field_scale);

        t_num = FishGecs[k].gecs_game_number;
        sprite1.setTexture(fish_textute[t_num]);
        sprite1.setScale(0.56f * field_scale, 0.525f * field_scale);

        auto size = sprite1.getTexture()->getSize();
        sprite1.setOrigin((size.x / 2), (size.y / 2));

        int rotate;
        if (k == 0) rotate = 0;
        if (k == 1) rotate = 60;
        if (k == 2) rotate = -60;
        if (k == 3) rotate = -120;
        if (k == 4) rotate = 120;
        if (k == 5) rotate = 180;
        sprite1.rotate(rotate);
        k++;

        window->draw(sprite1);
        sprite1.rotate(-rotate);
    }


 return;
}

//========================================================================
//   города в узлах, деревни
//========================================================================
void DrawNodes(sf::RenderWindow* window, std::vector<NODE>* PtrNode)
{
    for (auto& n : *PtrNode)
    {
    if (n.object == VILLAGE)  DrawVillage(window, n.owner, Draw_x(n.n_x) - 21, Draw_y(n.n_y) - 15, 0.2f);
    if (n.object == TOWN)    DrawTown(window, n.owner, Draw_x(n.n_x) - 21, Draw_y(n.n_y) - 15, 0.2f);
    }

 return;
}

//========================================================================
//  прорисовка системной информации по свойствам узлов
//========================================================================
void DrawNodesInfo(sf::RenderWindow* window, std::vector<NODE>* PtrNode)
{
 char str[50];

 //фигура под прорисовку области узла  при отладке
 sf::CircleShape NODE_area(17.f);
 NODE_area.setOutlineThickness(1);
 NODE_area.setOutlineColor(sf::Color::Color(220, 250, 70));
 NODE_area.setFillColor(sf::Color::Transparent);

 sf::Text text2("", font, 12);   //более мелкий шрифт для подписей

 for (auto& n : *PtrNode)
 {
     NODE_area.setPosition(Draw_x(n.getNode_x()) - 17, Draw_y(n.getNode_y()) - 16);
     window->draw(NODE_area);

     //номер узла
     text2.setFillColor(sf::Color::White);
     text2.setPosition(Draw_x(n.getNode_x()) - 3, Draw_y(n.getNode_y()) - 10);
     _itoa_s(n.number, str, 10);
     text2.setString(str); window->draw(text2);

     //тип узла - бонус для обмена
     text2.setPosition(Draw_x(n.getNode_x()) + 16, Draw_y(n.getNode_y()) - 10);
     //_itoa_s(n.type, str, 10);
     if (n.type == -1)  strcpy_s(str, " -- ");
     if (n.type == 0)   { strcpy_s(str, "   3:1"); text2.setFillColor(sf::Color::Yellow);  }
     if (n.type == (int)RESURS::WOOD) { strcpy_s(str, " WOOD 2:1");     text2.setFillColor(sf::Color::Color(100, 255, 100)); }
     if (n.type == (int)RESURS::OVCA) { strcpy_s(str, " OVCA 2:1");    text2.setFillColor(sf::Color::Color(150, 255, 150)); }
     if (n.type == (int)RESURS::STONE) { strcpy_s(str, " STONE 2:1");  text2.setFillColor(sf::Color::Color(150,140,255)); }
     if (n.type == (int)RESURS::BRICKS) { strcpy_s(str, " BRICKS 2:1");  text2.setFillColor(sf::Color::Red); }
     if (n.type == (int)RESURS::BREAD)    {  strcpy_s(str, " BREAD 2:1");  text2.setFillColor(sf::Color::Yellow); }
     text2.setString(str); window->draw(text2);
 }

 return;
}

//=======================================================================================
//  прорисовка сетки дорог
//=======================================================================================
void DrawRoadsNet(sf::RenderWindow* window, std::vector<NODE>* PtrNode, std::vector<ROAD>* Rd)
{
    int start, end;
    //задание конструктора фигуры - линии 
    //тонкая линия дороги - не требует повората спрайта, рисуется из начальных координат узла до конечных
    sf::VertexArray line(sf::Lines, 2);

    for (auto& rr : *Rd)
    {
        start = rr.Node_num_start;
        line[0].position = sf::Vector2f(Draw_x(PtrNode->at(start).n_x), Draw_y(PtrNode->at(start).n_y));
        end = rr.Node_num_end;
        line[1].position = sf::Vector2f(Draw_x(PtrNode->at(end).n_x), Draw_y(PtrNode->at(end).n_y));
        line[0].color = sf::Color::Color(30, 250, 220);
        line[1].color = sf::Color::Blue;    //цвет без градиента от точки до точки
        window->draw(line);
    }
    return;
};
//--------------------------------------------------------------------------------

//=======================================================================================
//  прорисовка  дорог в игре
//=======================================================================================
void DrawRoads(sf::RenderWindow* window, std::vector<NODE>* PtrNode, std::vector<ROAD>* Rd)
{
 int start, end;

  for (auto &rr : *Rd)
    {
    if (rr.owner < 0) continue;

    start = rr.Node_num_start;
    end = rr.Node_num_end;
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

sf::Texture texture_card[12];
int flag_texturecard_set = 0;
//================================================================
//  прорисовка 1 карточки ресурса по заданным координатам
//=================================================================
void DrawCard(sf::RenderWindow* win,int Type,int x,int y, float scale,sf::Sprite* spritePtr)
{
   if (flag_texturecard_set == 0)
    {
        flag_texturecard_set = 1;

        texture_card[(int)RESURS::EMPTY].loadFromFile("img/Card_empty.png");
        texture_card[(int)RESURS::WOOD].loadFromFile("img/Card_wood.png");
        texture_card[(int)RESURS::STONE].loadFromFile("img/Card_stone.png");
        texture_card[(int)RESURS::BRICKS].loadFromFile("img/Card_bricks.png");
        texture_card[(int)RESURS::BREAD].loadFromFile("img/Card_bread.png");
        texture_card[(int)RESURS::OVCA].loadFromFile("img/Card_ovca.png");

        texture_card[(int)RESURS::FISH1].loadFromFile("img/Card_fish1.png");
        texture_card[(int)RESURS::FISH2].loadFromFile("img/Card_fish2.png");
        texture_card[(int)RESURS::FISH3].loadFromFile("img/Card_fish3.png");
        texture_card[(int)RESURS::BOOT].loadFromFile("img/Card_boot.png");

        texture_card[(int)RESURS::FISH_ALL].loadFromFile("img/Card_fish.png");
    }

    
   spritePtr->setTexture(texture_card[Type]);

   spritePtr->setScale(scale, scale);
   spritePtr->setPosition(x, y);
    win->draw(*spritePtr);
    return;
}

//================================================================
//  прорисовка 1 карточки ресурса по заданным координатам без публичного спрайта
//=================================================================
void DrawCard_NoSprate(sf::RenderWindow* win, int Type, int x, int y, float scale)
{
 sf::Sprite Sprite;
 DrawCard(win, Type,x,y, scale,&Sprite);
}

//------------------------------------------------------------------------
sf::Texture texture_d_card[5];
sf::Sprite Active_develop_card[5];
int flag_textureDcard_set = 0;
//================================================================
//  прорисовка 1 карточки развития
//  status == 1 - привязывать к спрайту для главного окна
//=================================================================
void DrawDevelopCard(sf::RenderWindow* win, int Type, int x, int y, float scale,int status)
{
    sf::Sprite sprite_card;

    if (flag_textureDcard_set == 0)
    {
        flag_textureDcard_set = 1;

        texture_d_card[(int)IMP_TYPE::KNIGHT].loadFromFile("img/Card_knight.png");
        texture_d_card[(int)IMP_TYPE::ROAD2].loadFromFile("img/Card_road2.png");
        texture_d_card[(int)IMP_TYPE::MONOPOLIA].loadFromFile("img/Card_resurs1.png");
        texture_d_card[(int)IMP_TYPE::RESURS_CARD2].loadFromFile("img/Card_resurs_card2.png");
        texture_d_card[(int)IMP_TYPE::POINT1].loadFromFile("img/Card_point1.png");
    }

    if (status == 1)
       {
        Active_develop_card[Type].setTexture(texture_d_card[Type]);
        Active_develop_card[Type].setScale(scale, scale);
        Active_develop_card[Type].setPosition(x, y);
        win->draw(Active_develop_card[Type]);
       }
    else
        {
        sprite_card.setTexture(texture_d_card[Type]);
        sprite_card.setScale(scale, scale);
        sprite_card.setPosition(x, y);
        win->draw(sprite_card);
        }
    return;
}

//=========================================================================================
//получение количества запрашиваемых карт развития в векторе игрока с определенным статусом
//=========================================================================================
int getNumDevelopCARD(IMP_TYPE type, int  status)
{
    int num = 0;

    for (auto& elem : develop_CARDS[player_num])
    {
        if (elem.status == status && elem.type == type) num++;
    }

    return num;
}
//-----------------------------------------------------------------

int develop_field_x = 730;
int develop_field_y = 780;
//=================================================================
//  область карточек развития
//=================================================================
void Draw_Develop_Cards_Field(sf::RenderWindow* win)
{
int num,num_inactive;
char str[10],str1[10];

sf::Text text2("", font, 16);
text2.setFillColor(sf::Color::White);

// верхний ряд - активные карточки
// нижний ряд  - сыгранные карточки
    
    //рыцари
    num = getNumDevelopCARD(IMP_TYPE::KNIGHT,0);
    num_inactive = getNumDevelopCARD(IMP_TYPE::KNIGHT, -1);
    if (num || num_inactive) {
        DrawDevelopCard(win, (int)IMP_TYPE::KNIGHT, develop_field_x + 50, develop_field_y + 10, 0.24,1);
        text2.setPosition(develop_field_x + 55, develop_field_y - 8);
        _itoa_s(num, str, 10);     _itoa_s(num_inactive, str1, 10);
        if (num_inactive > 0) { strcat_s(str, " + ");    strcat_s(str, str1); }
        text2.setString(str); win->draw(text2);
       }

    // 2 дороги
    num = getNumDevelopCARD(IMP_TYPE::ROAD2,0);
    num_inactive = getNumDevelopCARD(IMP_TYPE::ROAD2, -1);
    if (num || num_inactive) {
        DrawDevelopCard(win, (int)IMP_TYPE::ROAD2, develop_field_x + 100, develop_field_y + 10, 0.24,1);
        text2.setPosition(develop_field_x + 105, develop_field_y - 8);
        _itoa_s(num, str, 10);     _itoa_s(num_inactive, str1, 10);
        if (num_inactive > 0) { strcat_s(str, " + ");    strcat_s(str, str1); }
        text2.setString(str); win->draw(text2);
        }

    //
    num = getNumDevelopCARD(IMP_TYPE::MONOPOLIA,0);
    num_inactive = getNumDevelopCARD(IMP_TYPE::MONOPOLIA, -1);
    if (num || num_inactive) {
        DrawDevelopCard(win, (int)IMP_TYPE::MONOPOLIA, develop_field_x + 150, develop_field_y + 10, 0.24,1);
        text2.setPosition(develop_field_x + 155, develop_field_y - 8);
        _itoa_s(num, str, 10);     _itoa_s(num_inactive, str1, 10);
        if (num_inactive > 0) { strcat_s(str, " + ");    strcat_s(str, str1); }
        text2.setString(str); win->draw(text2);
        }

    //
    num = getNumDevelopCARD(IMP_TYPE::RESURS_CARD2,0);
    num_inactive = getNumDevelopCARD(IMP_TYPE::RESURS_CARD2, -1);
    if (num || num_inactive) {
        DrawDevelopCard(win, (int)IMP_TYPE::RESURS_CARD2, develop_field_x + 200, develop_field_y + 10, 0.24,1);
        text2.setPosition(develop_field_x + 205, develop_field_y - 8);
        _itoa_s(num, str, 10);     _itoa_s(num_inactive, str1, 10);
        if (num_inactive > 0) { strcat_s(str, " + ");    strcat_s(str, str1); }
        text2.setString(str); win->draw(text2);
        }

    //
    num = getNumDevelopCARD(IMP_TYPE::POINT1,0);
    num_inactive = getNumDevelopCARD(IMP_TYPE::POINT1, -1);
    if (num || num_inactive) {
        DrawDevelopCard(win, (int)IMP_TYPE::POINT1, develop_field_x + 250, develop_field_y + 10, 0.24,1);
        text2.setPosition(develop_field_x + 255, develop_field_y - 8);
        _itoa_s(num, str, 10);     _itoa_s(num_inactive, str1, 10);
        if (num_inactive > 0) { strcat_s(str, " + ");    strcat_s(str, str1); }
        text2.setString(str); win->draw(text2);
        }


    //------------- нижний ряд - сыгранные карты не выбираются мышкой --------------------------------------
    num = getNumDevelopCARD(IMP_TYPE::KNIGHT, 1);
    if (num) {
        DrawDevelopCard(win, (int)IMP_TYPE::KNIGHT, develop_field_x + 50, develop_field_y + 95, 0.17, 0);
        text2.setPosition(develop_field_x + 64, develop_field_y + 150);
        _itoa_s(num, str, 10);    text2.setString(str); win->draw(text2);
    }

    // 2 дороги - сыгранные
    num = getNumDevelopCARD(IMP_TYPE::ROAD2, 1);
    if (num) {
        DrawDevelopCard(win, (int)IMP_TYPE::ROAD2, develop_field_x + 100, develop_field_y + 95, 0.17, 0);
        text2.setPosition(develop_field_x + 114, develop_field_y + 150);
        _itoa_s(num, str, 10);    text2.setString(str); win->draw(text2);
    }

    //
    num = getNumDevelopCARD(IMP_TYPE::MONOPOLIA, 1);
    if (num) {
        DrawDevelopCard(win, (int)IMP_TYPE::MONOPOLIA, develop_field_x + 150, develop_field_y + 95, 0.17, 0);
        text2.setPosition(develop_field_x + 164, develop_field_y + 150);
        _itoa_s(num, str, 10);    text2.setString(str); win->draw(text2);
    }

    //
    num = getNumDevelopCARD(IMP_TYPE::RESURS_CARD2, 1);
    if (num) {
        DrawDevelopCard(win, (int)IMP_TYPE::RESURS_CARD2, develop_field_x + 200, develop_field_y + 95, 0.17, 0);
        text2.setPosition(develop_field_x + 214, develop_field_y +150);
        _itoa_s(num, str, 10);    text2.setString(str); win->draw(text2);
    }
    
    num = getNumDevelopCARD(IMP_TYPE::POINT1, 1);
    if (num) {
        DrawDevelopCard(win, (int)IMP_TYPE::POINT1, develop_field_x + 250, develop_field_y + 95, 0.17, 0);
        text2.setPosition(develop_field_x + 264, develop_field_y + 150);
        _itoa_s(num, str, 10);    text2.setString(str); win->draw(text2);
    }

    return;
}

//================================================================
//  прорисовка 
//================================================================
void Draw_MaxWay(sf::RenderWindow* win,int x,int y,float scale)
{
    sf::Sprite sprite;
    sf::Texture texture;

    texture.loadFromFile("img/Max_way.png");
    sprite.setTexture(texture);

    sprite.setScale(scale, scale);
    sprite.setPosition(x, y);
    win->draw(sprite);
}

//================================================================
//  прорисовка карточки армии
//================================================================
void Draw_MaxArmy(sf::RenderWindow* win, int x, int y, float scale)
{
    sf::Sprite sprite;
    sf::Texture texture;

    texture.loadFromFile("img/Max_army.png");
    sprite.setTexture(texture);

    sprite.setScale(scale, scale);
    sprite.setPosition(x, y);
    win->draw(sprite);
}

//================================================================
//  прорисовка карточки гавани
//================================================================
void Draw_MaxGavan(sf::RenderWindow* win, int x, int y, float scale)
{
    sf::Sprite sprite;
    sf::Texture texture;

    texture.loadFromFile("img/Max_gavan.png");
    sprite.setTexture(texture);

    sprite.setScale(scale, scale);
    sprite.setPosition(x, y);
    win->draw(sprite);
}

//================================================================
//  прорисовка банка ресурсов - карточек ресурсов
//================================================================
void DrawResursBank(sf::RenderWindow* win)
{
 sf::Sprite Bank_resurs_card;

 //прямоугольник - область под банк ресурсов
sf::RectangleShape Bank_area;
if(Game_type == 1)  Bank_area.setSize(sf::Vector2f(300.f, 95.f));
if(Game_type == 2)  Bank_area.setSize(sf::Vector2f(350.f, 95.f));  

 Bank_area.setPosition(5, 5);    Bank_area.setOutlineThickness(2);
 Bank_area.setOutlineColor(sf::Color::Color(200, 200, 200));    Bank_area.setFillColor(sf::Color::Transparent);

 sf::Text text2("", font, 20);   //более мелкий шрифт для подписей
 text2.setFillColor(sf::Color::White);


 win->draw(Bank_area);
 char str[20];

 DrawCard(win, (int)RESURS::WOOD, 10, 10, 0.4,&Bank_resurs_card);
 text2.setPosition(20, 70);     _itoa_s(CARD_Bank[(int)(RESURS::WOOD)], str, 10);    text2.setString(str); win->draw(text2);

 DrawCard(win, (int)RESURS::BRICKS, 60, 10, 0.4, &Bank_resurs_card);
 text2.setPosition(70, 70);    _itoa_s(CARD_Bank[(int)RESURS::BRICKS], str, 10);    text2.setString(str);  win->draw(text2);

 DrawCard(win, (int)RESURS::OVCA, 110, 10, 0.4, &Bank_resurs_card);
 text2.setPosition(120, 70);     _itoa_s(CARD_Bank[(int)RESURS::OVCA], str, 10);    text2.setString(str);  win->draw(text2);

 DrawCard(win, (int)RESURS::STONE, 160, 10, 0.4, &Bank_resurs_card);
 text2.setPosition(170, 70);     _itoa_s(CARD_Bank[(int)RESURS::STONE], str, 10);    text2.setString(str);  win->draw(text2);

 DrawCard(win, (int)RESURS::BREAD, 210, 10, 0.4, &Bank_resurs_card);
 text2.setPosition(220, 70);    _itoa_s(CARD_Bank[(int)RESURS::BREAD], str, 10);    text2.setString(str);  win->draw(text2);

 DrawCard(win, (int)RESURS::EMPTY, 260, 10, 0.4, &Bank_resurs_card);
 text2.setPosition(270, 70);    _itoa_s(improve_CARDS.size(), str, 10);    text2.setString(str);  win->draw(text2);

 if (Game_type == 2)
     {
     DrawCard(win, (int)RESURS::FISH_ALL, 310, 10, 0.4, &Bank_resurs_card);
     text2.setPosition(320, 70);    _itoa_s(FishCards.size(), str, 10);    text2.setString(str);  win->draw(text2);
     }

}
