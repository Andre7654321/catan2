#include <iostream>
#include <cstdlib>
#include <vector>
#include <typeinfo>
#include <time.h>
#include <string.h>
#include "CatanField.h"
#include "Catan_View.h"
#include "Catan_Count.h"
#include <algorithm>

extern int Game_type;     //1 - Standart CATAN   2 - FISH

extern std::vector<GECS>* gecsPtr;    //указатель на вектор гексов
extern std::vector<NODE>* nodePtr;    //указатель на вектор узлов поля
extern std::vector<ROAD>* roadPtr;    //указатель на вектор дорог

extern std::vector<GECS>  FishGecs;   //вектор рыбных отмелей
extern std::vector<RESURS>  FishCards;  //вектор карточек с рыбами

extern PLAYER player[7];
extern int limit_7[5];
extern int player_num;
extern int CARD_Bank[12];
extern int ChangeBANK[5][12];
extern std::vector<IMP_CARD> develop_CARDS[5]; //карты развития игроков
extern std::vector<IMP_CARD> improve_CARDS;    //банк карт развития

extern int bandit_Gecs;
extern int max_road_owner;
extern int max_army;
extern int max_gavan;

//размер 15 на 20 - универсальный для большого поля
int CATAN19[15][20] =
{
    {           },  //  
    {   2,3,4   },   //   0, 1, 2
    {  2,3,4,5  },   //  3, 4, 5, 6
    { 1,2,3,4,5 },   // 7, 8, 9,10,11
    {  2,3,4,5  },   //  12,13,14,15
    {   2,3,4   },    //   16,17,18
    {           }    //   
};
//расположение гексов рыболовных отмелей
int CATAN19_Fish[15][20] =
{
    {     3 },   //  
    {   5   },   //   0, 1, 2
    { 1     },   //  3, 4, 5, 6
    {       },   // 7, 8, 9,10,11
    { 1     },   //  12,13,14,15
    {  5    },   //   16,17,18
    {    4  }    // 
};

std::string resurs_name[12] =
{
    "empty",
    "WOOD",
    "BRICKS",
    "BREAD",
    "STONE",
    "OVCA",
    "FISH1",
    "FISH2",
    "FISH3",
    "BOOT",
    "PIRATE",
    "FISH_ALL"
};


//для 19 гексов определяем узлы привязки к полю
int GECS_POSITION[6][19] =
{
{ 0,1,2,6,11,15,18,17,16,12,7,3,4,5,10,14,13,8,9 },
{ 2,6,11,15,18,17,16,12,7,3,0,1,5,10,14,13,8,4,9 },
{ 11,15,18,17,16,12,7,3,0,1,2,6,10,14,13,8,4,5,9 },
{ 18,17,16,12,7,3,0,1,2,6,11,15,14,13,8,4,5,10,9 },
{ 16,12,7,3,0,1,2,6,11,15,18,17,13,8,4,5,10,14,9 },
{ 7,3,0,1,2,6,11,15,18,17,16,12,8,4,5,10,14,13,9 }
};

//в игре - камни 3.кирпичи 3, колос - 4. баран 4 дерево 4 разбойник 1
std::vector<RESURS> Standart_Game = { RESURS::PIRATE,
                           RESURS::STONE,RESURS::STONE,RESURS::STONE,
                           RESURS::BRICKS,RESURS::BRICKS,RESURS::BRICKS,
                           RESURS::BREAD,RESURS::BREAD,RESURS::BREAD,RESURS::BREAD,
                           RESURS::OVCA,RESURS::OVCA,RESURS::OVCA,RESURS::OVCA,
                           RESURS::WOOD,RESURS::WOOD,RESURS::WOOD,RESURS::WOOD };

std::vector<RESURS> Fish_Game = { RESURS::FISH1,
                           RESURS::STONE,RESURS::STONE,RESURS::STONE,
                           RESURS::BRICKS,RESURS::BRICKS,RESURS::BRICKS,
                           RESURS::BREAD,RESURS::BREAD,RESURS::BREAD,RESURS::BREAD,
                           RESURS::OVCA,RESURS::OVCA,RESURS::OVCA,RESURS::OVCA,
                           RESURS::WOOD,RESURS::WOOD,RESURS::WOOD,RESURS::WOOD };

//============================= класс PLAYER =====================================
//class PLAYER

    //int village = 5;
    //int town = 4;
    //int road = 15;
    //int resurs[12];
    //bool active = false;
PLAYER::PLAYER()
{
    resurs[(int)RESURS::EMPTY] = 0;
    resurs[(int)RESURS::WOOD]  = 0;
    resurs[(int)RESURS::STONE] = 0;
    resurs[(int)RESURS::BRICKS] = 0;
    resurs[(int)RESURS::OVCA]  = 0;
    resurs[(int)RESURS::BREAD] = 0;
    resurs[(int)RESURS::FISH1] = 0;
    resurs[(int)RESURS::FISH2] = 0;
    resurs[(int)RESURS::FISH3] = 0;
};
//============================= PLAYER   end =============================================

//===========================================================================
// возвращает количество карт в банке обмена
//===========================================================================	
int numCardChange(int pl)
{
  int num = 0;
    
  num += ChangeBANK[pl][(int)RESURS::WOOD];
  num += ChangeBANK[pl][(int)RESURS::STONE];
  num += ChangeBANK[pl][(int)RESURS::OVCA];
  num += ChangeBANK[pl][(int)RESURS::BRICKS];
  num += ChangeBANK[pl][(int)RESURS::BREAD];
  num += ChangeBANK[pl][(int)RESURS::FISH1];
  num += ChangeBANK[pl][(int)RESURS::FISH2];
  num += ChangeBANK[pl][(int)RESURS::FISH3];
  num += ChangeBANK[pl][(int)RESURS::BOOT];

   return num;
}

//===========================================================================
// возвращает тип забираемого ресурса
//===========================================================================	
int TakeRandomCardFromPl(int pl_donor)
{
    int type = 0, i = 0;

   int num = getPlayerNumCardResurs(pl_donor);
   if (num == 0)  return 0;

    while (type == 0)
    {
     i = Random_Number(1, 5);
     if(player[pl_donor].resurs[i] > 0)  type = i;
    }

return type;
}

//===========================================================================
// возвращает true если вокруг указанного гекса есть чужие города,
// игрокам около гекса бандита выставляется флаг разрешающий забрать у них карту
//===========================================================================	
bool isAnyAlienObjectNearGecs(int gecs,int pl)
{
 bool flag = false;

    for (auto node : *nodePtr)
        {
        if(abs(node.n_x - gecsPtr->at(gecs).x) > 24  || abs(node.n_y - gecsPtr->at(gecs).y) > 24) continue;

        if (node.owner != -1 && node.owner != pl)
            {
            flag = true;
            player[node.owner].flag_allow_get_card = 1;    //разрешение забрать N карт активному игроку хода
            }
        }

    return flag;
}

//===========================================================================
// возвращает true если игроки скинули карты после броска 7
//===========================================================================									  
bool isCardsDeletedAfterSeven()
{
    for (int i = 1; i < 7; i++)
        {
        if (player[i].active == false) continue;
        if (i == player_num) continue;
        if (limit_7[i] == 0)  continue;
        if (getPlayerNumCardResurs(i) > limit_7[i])  return false;
        }

 return true;
}

//===========================================================================
// возвращает количество карт ресурса у игрока
//===========================================================================									  
int getPlayerNumCardResurs(int pl)
{
    int sum = 0;
    if (player[pl].active == false)  return 0;

    sum += player[pl].resurs[(int)RESURS::WOOD];
    sum += player[pl].resurs[(int)RESURS::STONE];
    sum += player[pl].resurs[(int)RESURS::BRICKS];
    sum += player[pl].resurs[(int)RESURS::OVCA];
    sum += player[pl].resurs[(int)RESURS::BREAD];

 return sum;
}

//=========================================================================
// Подсчет очков пока без дорог
//=========================================================================
int CountScore(int pl)
{
 int score = 0;

 //города и деревни  
 for (auto& n : *nodePtr)
    {
     if (n.object == VILLAGE && n.owner == pl) score += 1;
     if (n.object == TOWN  && n.owner == pl) score += 2;
    }

 //дорога
 if (max_road_owner == pl) score += 2;
 if (max_army == pl) score += 2;
 if (max_gavan == pl) score += 2;

 //карточки развития - очки
 for (auto& elem : develop_CARDS[pl])
     {
     if (elem.status == 1 && elem.type == IMP_TYPE::POINT1)  score += 1;
     }

 //гавани

 return score;
}

//========================================================================================
// получить ресурсы с окружающих переданную точку гексов
//========================================================================================
void Step_Resurs(int dice2)
{
 int num = 0;
 int i;

    for (auto& nd : *nodePtr)
    {
        if (nd.owner == -1)  continue;    //узел не застроен

        i = 0;
        //основные гексы
        for (auto& g : *gecsPtr)
        {
         if (CARD_Bank[(int)g.type] == 0 && g.type != RESURS::FISH1)
                    { i++; continue; }         //если ресурса нет, даже не рассматриваем

         if ((dice2 == g.gecs_game_number || (g.type == RESURS::FISH1 && (dice2 == 2 || dice2 == 3 || dice2 == 11 || dice2 == 12)))
                && abs(nd.n_x - g.x) < 25 && abs(nd.n_y - g.y) < 25 && g.type != RESURS::PIRATE)   
            {
             if (i == bandit_Gecs)    //пираты  стоят на гексе
                {
                //std::cout << " Gesc " << i << " resurs -  " << resurs_name[(int)g.type] << " под Бандитами " << std::endl;
                i++;   
                continue;
                }
             num = 0;
             if (nd.object == VILLAGE) num = 1;
             if (nd.object == TOWN) num = 2;

             if (g.type != RESURS::FISH1)
                 {
                 if (CARD_Bank[(int)g.type] >= num)  CARD_Bank[(int)g.type] -= num;
                 else
                     {
                     num -= 1;
                     if (CARD_Bank[(int)g.type] >= num)  CARD_Bank[(int)g.type] -= num;
                     else continue;
                     }
                  player[nd.owner].resurs[(int)g.type] += num;  //в банк игрока
                  ChangeBANK[nd.owner][(int)g.type] += num;     //в обменный банк игрока для инфо
                  }
             else
                 {
                 //забрать карточки рыб из общего вектора
                 // добавить карточки рыб игроку
                 while (num > 0) 
                      {
                      //более 8 карт рыб на руках не может быть, и то надо 1 скидывать
                      if(player[nd.owner].resurs[(int)RESURS::FISH1] + player[nd.owner].resurs[(int)RESURS::FISH2] + player[nd.owner].resurs[(int)RESURS::FISH3] >= 8)  break;
                      player[nd.owner].resurs[(int)FishCards.back()] += 1;
                      ChangeBANK[nd.owner][(int)FishCards.back()] += 1;
                      FishCards.pop_back();       //удаляем из основного банка
                      num--;
                      }
                 }
            } //if(dice == .......
        i++;
        }  //for (auto& g : *gecsPtr)
       
        i = 0;
        //гексы рыбных отмелей
        for (auto& f : FishGecs)
          {
            if (dice2 == f.gecs_game_number  && abs(nd.n_x - f.x) < 25 && abs(nd.n_y - f.y) < 25)
            {
            //std::cout << " Рыб отмель Gesc " << i << " FISH NUM " << f.gecs_game_number << std::endl;
            //std::cout << " Выдаст  FISH resurs " << (int)FishCards.back() << " |  " << resurs_name[(int)FishCards.back()] << std::endl;

            num = 0;
            if (nd.object == VILLAGE) num = 1;
            if (nd.object == TOWN) num = 2;

            while (num > 0)
                {
                //более 8 карт рыб на руках не может быть, и то надо 1 скидывать
                if (player[nd.owner].resurs[(int)RESURS::FISH1] + player[nd.owner].resurs[(int)RESURS::FISH2] + player[nd.owner].resurs[(int)RESURS::FISH3] >= 8)  break;
                player[nd.owner].resurs[(int)FishCards.back()] += 1;
                ChangeBANK[nd.owner][(int)FishCards.back()] += 1;
                FishCards.pop_back();       //удаляем из основного банка
                num--;
                }

            i++;
            }//if dice2

          }
    }  // for (auto& nd : *nodePtr)

    return;
}

//========================================================================================
// получить ресурсы 1 игроку на 3 шаге с окружающих переданную точку гексов
//========================================================================================
void Get_Resurs(int village_node,int pl)
{
    int x=0,y=0;
    
    if (village_node < 0)     {   std::cout << " ERR Ask resurs near  " << village_node  << std::endl;      return;   }
    
    //определить координаты узла
    x = nodePtr->at(village_node).getNode_x();
    y = nodePtr->at(village_node).getNode_y();

    //std::cout << " Village Node  " << village_node << " x " << x << " y " << y << "  Player " << player_num << std::endl;

    //со основных гексов , до которых расстояние менее 3, получить по 1 ресурсу
    for (auto &g : *gecsPtr)
       {
        if (abs(x - g.x) < 25 && abs(y - g.y) < 25 && g.type != RESURS::PIRATE)
           {
            if(g.type != RESURS::FISH1)   //не озеро
                {
                CARD_Bank[(int)g.type] -= 1;
                player[pl].resurs[(int)g.type] += 1;
                }
            else 
                {
                player[pl].resurs[(int)FishCards.back()] += 1;
                FishCards.pop_back();
                }
           }
       }
    //гексы отмелей
    for (auto& f : FishGecs)
       {
        if (abs(x - f.x) < 25 && abs(y - f.y) < 25)
            {
            player[pl].resurs[(int)FishCards.back()] += 1;
            FishCards.pop_back();
            }
       }

return;
}

//==================================================================================================
// добавление узла в вектор поля
//==================================================================================================
int AddNewNode(int x, int y, std::vector<NODE>* PtrNode)
{
  //проверить есть ли такой узел в векторе
    for (auto& elem : *PtrNode)
      {
      if (x == elem.getNode_x() && y == elem.getNode_y())   return 0;
      }

    //добавляем узел в вектор
    PtrNode->push_back(NODE(x, y));
    PtrNode->back().owner = -1;
    PtrNode->back().object = -1;

    return 1;
}

//==================================================================================================
// добавление узла в вектор поля
//==================================================================================================
int AddNewRoad(int start, int end, std::vector<ROAD>* PtrRoad)
{
    int index = 0;
    //проверить есть ли дорога  в векторе
    for (auto& elem : *PtrRoad)
    {
      if (start == elem.getRoad_start() && end == elem.getRoad_end())   return 0;
      if (end == elem.getRoad_start() && start == elem.getRoad_end())   return 0;
    }

    //добавляем дорогу в вектор
    PtrRoad->push_back(ROAD(start, end));
    index++;

    return 1;
}

//=================================================================================================
//конкретно для CATAN 19  рандомное расположение значений броска кубика - по часовой стрелке по алфавиту
//рандомно выбирается угол с которого раскладывается последовательность
//=================================================================================================
void SetRandomGecsNumber19(std::vector<GECS>* PtrGecs)
{
 //значений броска кубика 18, так как в 1 поле нет значения где стоят бандиты
 int game_number[18]{ 5,2,6,3,8,10,9,12,11,4,8,10,9,4,5,6,3,11 };  //по алфавиту как на кружочках в игре

 int i, ii;

 int coner = Random_Number(0, 5);   //рандомно выбирается стартовый угол обхода для присвоения номера
 //std::cout << "  Присвоение номеров с угла   " << coner << std::endl;

 int position;
 for (i = 0, ii = 0; i < 19; i++)
    {
     position = GECS_POSITION[coner][i];
     if (PtrGecs->at(position).type != RESURS::PIRATE && PtrGecs->at(position).type != RESURS::FISH1)
        {
         PtrGecs->at(position).gecs_game_number = game_number[ii++];  //ii индексируется только при присвоении
        }
    }  //end for

    return;
}

//=================================================================================================
//конкретно для CATAN 19  рандомное расположение полей гексов
//=================================================================================================
void SetRandomGecsType19(std::vector<GECS>* PtrGecs)
{
    std::vector<RESURS>* vPtr = nullptr;
    if (Game_type == 1) vPtr = &Standart_Game;
    if (Game_type == 2) vPtr = &Fish_Game;

    //алгоритм библиотеки STL - сортировка массива случайным образом
    random_shuffle(vPtr->begin(), vPtr->end());

    //гекс рыбалки не может быть у края поля
    bool flag_OK = false;
    while (Game_type == 2 && flag_OK == false)
        {
        random_shuffle(vPtr->begin(), vPtr->end());
        int i = 0;
        for(auto elem : *vPtr)
           {
            if (elem == RESURS::FISH1 && (i == 4 || i == 5 || i == 8 || i == 9 || i == 10  || i == 13 || i == 14))   flag_OK = true;
            i++;
           }
       }

    int i = 0;
    //проверить номера гексов у полей рират и рыбалка
    for(auto elem : *vPtr)
       {
        PtrGecs->at(i).type  =  elem;
        if (elem == RESURS::PIRATE) 
            {
            bandit_Gecs = i;  
            PtrGecs->at(i).gecs_game_number = -1;
            }
        if (elem == RESURS::FISH1)
        {
            bandit_Gecs = -1;
            PtrGecs->at(i).gecs_game_number = -1;
        }
        ++i;
       }

    //выдать игровые номера гексам для сравнения с броском кубика
    SetRandomGecsNumber19(PtrGecs);
    return;
}

//=========================================================================
// истина к узлу примыкает своя дорога
//=========================================================================
bool isRoadNear(int index,int player_num)
{
 for (auto& r : *roadPtr)
    {
        if (r.Node_num_start == index || r.Node_num_end == index)
            if (r.owner == player_num)  return true;               
    }

    return false;
}

//=========================================================================
// истина если в соседних узлах есть деревня
//=========================================================================
bool isVillageNear(int index)
{
    int nx = nodePtr->at(index).n_x;
    int ny = nodePtr->at(index).n_y;

    for (auto& n : *nodePtr)
      {
        if (abs(n.n_x - nx) <= 20 && abs(n.n_y - ny) <= 20) 
               if(n.owner != -1)  return true;               //близкий узел кем-то занят
      }

 return false;
}

//==================================================================================================
// Инициализация основных классов 
// вызывается из main серверного приложения
//==================================================================================================
int Init_CATAN_Field(std::vector<GECS>* PtrGecs, std::vector<NODE>* PtrNode, std::vector<ROAD>* PtrRoad)
{

    srand(time(NULL));
    int nn = rand() % 20;
    for (int i = 0; i < nn; i++)  rand();

   roadPtr->clear();	nodePtr->clear();	gecsPtr->clear();
   FishGecs.clear();    FishCards.clear();

    //рыбные отмели перемешать
    std::vector<int> fish_num = { 4,5,6,8,9,10 };
    random_shuffle(fish_num.begin(), fish_num.end());

    //инициализация сетки расположения гексов   присвоение координат   ---------------------
    int n = 0;
    for (int ii = 0; ii < 15; ii++)
    {
        for (int i = 0; i < 20; i++)
        {
         //пропускаем номера полей с нулевыми координатами
         if (CATAN19[ii][i] == 0)  continue;    //по координате X == 0 не может быть расположения гекса
         //По номеру гекса рассчитать его координаты
         int x = ((CATAN19[ii][i]) * 4 - 2 * ((ii-1) % 2)) * 10;
         int y = ((ii-1) * 3 + 2) * 10;     //номер ряда выступает в качестве координаты y для гекса
         PtrGecs->push_back(GECS(x, y));  //добавляем в конец вектора основных гексов
         //std::cout << "  Gecs x = " << PtrGecs->at(i).getGecs_x() << "   Gecs y = " << PtrGecs->at(i).getGecs_y() << std::endl;
         //std::cout << "  Vector MAIN GECS size =  " << PtrGecs->size() << std::endl;
        }
        //рыбные
        for (int i = 0; i < 20; i++)
        {
            //пропускаем номера полей с нулевыми координатами
            if (CATAN19_Fish[ii][i] == 0)  continue;    //по координате X == 0 не может быть расположения гекса
            int x = ((CATAN19_Fish[ii][i]) * 4 - 2 * ((ii - 1) % 2)) * 10;
            int y = ((ii - 1) * 3 + 2) * 10;     //номер ряда выступает в качестве координаты y для гекса
            FishGecs.push_back(GECS(x, y));  //добавляем в конец вектора рыбных отмелей
            FishGecs.back().gecs_game_number = fish_num[n++];
        }   
    }
    //рандом расположение разных видов гексов для поля в 19 элементов
    SetRandomGecsType19(PtrGecs);

    //вывод инфо по гексам -----------------------
    int i = 0;
    for (auto& elem : *PtrGecs)
        {
        //std::cout << i++ << "\tGecs x = " << elem.getGecs_x() <<  "\t|  y = " <<  elem.getGecs_y() << "  " << resurs_name[(int)elem.type] << std::endl;
        }

    //по массиву гексов создать массив узлов поля --------------------------------------------------
    int nx, ny;
    for (auto& elem : *PtrGecs)
    {
        //каждому гексу рассчитать 6 вершин
        nx = elem.getGecs_x();        ny = elem.getGecs_y() - 20;   AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x() + 20;   ny = elem.getGecs_y() - 10;   AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x() + 20;   ny = elem.getGecs_y() + 10;   AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x();        ny = elem.getGecs_y() + 20;   AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x() - 20;   ny = elem.getGecs_y() + 10;   AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x() - 20;   ny = elem.getGecs_y() - 10;   AddNewNode(nx, ny, PtrNode);

    }

    //std::cout << "\n ===============Поле узлов CATAN FIELD ================ " << std::endl;
    i = 0;
    //пронумеруем индексы узлов 
    for (auto& n : *PtrNode)
        {
        n.number = i;
        //std::cout << i << "\tNode x = " << n.getNode_x() << "\t|  y = " << n.getNode_y() << " index = " << n.number << std::endl;
        i++;
        }

    //по массиву узлов заполнить дороги ----------------------------------------------------------------------
    int road_num = 0;
    int x_next, y_next;
    int start = 0, end = 0;
    std::vector<NODE>* PtrNode2;
    PtrNode2 = PtrNode;
    for (auto& n : *PtrNode)
    {
        //если находим узел с координатам до которого расстояние менее 2.02 единиц
        //то считаем это узел соседним и регистрируем путь от нашего узла до найденного
        nx = n.getNode_x(); ny = n.getNode_y();   //текущий узел
        end = 0;
        for (auto& next : *PtrNode2)
        {
            x_next = next.getNode_x(); y_next = next.getNode_y();
            if (x_next == nx && y_next == ny)   continue;         //если сравниваем узел сам с собой
            if (abs(x_next - nx) <= 20 && abs(y_next - ny) <= 20)
            {
                //add road
                start = n.getNode_number();
                end = next.getNode_number();
                AddNewRoad(start, end, PtrRoad);
            }
        } //foreach in
    }  //foreach out

   //начальные условия игроков - фишки, пустые вектора карт развития
    for (int i = 1; i < 5; i++)
    {
        for (int ii = 0; ii < 10; ii++)   player[i].resurs[ii] = 0;
        player[i].road = 15;
        player[i].town = 4;
        player[i].village = 5;
        player[i].last_dice = 0;
        develop_CARDS[i].clear();
    }

    //-----------------------------  бонусы узлов  -------------------------------------------
    PtrNode->at(1).type = 0;     PtrNode->at(6).type = 0;   PtrNode->at(10).type = 0;    PtrNode->at(11).type = 0;  // 3:1
    PtrNode->at(52).type = 0;    PtrNode->at(53).type = 0;  PtrNode->at(27).type = 0;    PtrNode->at(28).type = 0; // 3:1
    PtrNode->at(37).type = 1;    PtrNode->at(45).type = 1;   //wood
    PtrNode->at(22).type = 2;    PtrNode->at(23).type = 2;   //bricks
    PtrNode->at(47).type = 3;    PtrNode->at(51).type = 3;   //bread
    PtrNode->at(39).type = 4;    PtrNode->at(40).type = 4;   //stone
    PtrNode->at(4).type = 5;     PtrNode->at(17).type = 5;   //ovca

    //================ карты развития ================================================================
    //число карт развития в базовой игре - рыцари - 14, победное очко - 5, стр дорог - 2 , монополия - 2, 2 карты ресурса - 2
    improve_CARDS.clear();
    int pos;
    IMP_CARD ttt;
    //все карты имеют статус неактивных
    ttt.status = -1;
    ttt.type = IMP_TYPE::KNIGHT;
    for (pos = 0; pos < 14; pos++)  improve_CARDS.insert(improve_CARDS.begin(), ttt);    
    ttt.type = IMP_TYPE::POINT1;
    for (pos = 0; pos < 5; pos++)  improve_CARDS.insert(improve_CARDS.begin(), ttt);
    ttt.type = IMP_TYPE::ROAD2;
    improve_CARDS.insert(improve_CARDS.begin(), ttt);
    improve_CARDS.insert(improve_CARDS.begin(), ttt);
    ttt.type = IMP_TYPE::RESURS_CARD2;
    improve_CARDS.insert(improve_CARDS.begin(), ttt);
    improve_CARDS.insert(improve_CARDS.begin(), ttt);
    ttt.type = IMP_TYPE::MONOPOLIA;
    improve_CARDS.insert(improve_CARDS.begin(), ttt);
    improve_CARDS.insert(improve_CARDS.begin(), ttt);

    //алгоритм библиотеки STL - сортировка массива случайным образом
    random_shuffle(improve_CARDS.begin(),improve_CARDS.end());

    //========================  карточки с рыбами =================================================
    //ботинок -1  по 1 рыбе - 11, по 2 рыбы 10, по 3 рыбы - 8
    FishCards.clear();
    FishCards.push_back(RESURS::BOOT);
    for (pos = 0; pos < 11; pos++)  FishCards.push_back(RESURS::FISH1);
    for (pos = 0; pos < 10; pos++)  FishCards.push_back(RESURS::FISH2);
    for (pos = 0; pos < 8; pos++)  FishCards.push_back(RESURS::FISH3);

    random_shuffle(FishCards.begin(), FishCards.end());
    

   //================ Банк ресурсов ================================================================
   CARD_Bank[(int)RESURS::WOOD] = 19; 
   CARD_Bank[(int)RESURS::STONE] = 19; 
   CARD_Bank[(int)RESURS::OVCA] = 19;
   CARD_Bank[(int)RESURS::BREAD] = 19; 
   CARD_Bank[(int)RESURS::BRICKS] = 19;

   max_road_owner = 0;
   max_army = 0;

   //std::cout << "  Vector Fish size =  " << FishGecs.size() << std::endl;
   //std::cout << "  Vector road size =  " << roadPtr->size() << std::endl;
   //std::cout << "  Vector node size =  " << nodePtr->size() << std::endl;
   //std::cout << "  Vector Gect size =  " << gecsPtr->size() << std::endl;
    return 1;
}

//===============================================================================
//содержит описание одного узла карты
//class NODE
    //int n_x = 0;
    //int n_y = 0;
    //int owner = -1;      //№ игрока владельца узла    ==  -1  - нет владельца 
    //int object = -1;     //тип постройки в узле       ==  -1  - нет посторойки
    //int number = -1;     //уникальный номер узла
NODE::NODE() {};
 NODE::NODE(int x = 0, int y = 0) : n_x(x), n_y(y)
        {};

//вернет истину если узлом никто не владеет
 bool NODE::isNode_free(int index)
 {
     if (owner == -1) return true;
     return false;
 };

 bool NODE::isNode_infocus(int mouse_x, int mouse_y)
 {
     mouse_x = Game_x(mouse_x);  mouse_y = Game_y(mouse_y);
     if (mouse_x > n_x - 9 && mouse_x < n_x + 9 && mouse_y > n_y - 9 && mouse_y < n_y + 9)   return true;
     return false;
 };
 int NODE::getNode_number()  { return number; }
 int NODE::getNode_x() { return n_x; }
 int NODE::getNode_y() { return n_y; }
//=============================== end NODE ==============================

//============================= класс GECS =====================================
//class GECS
//public:
//    int x = 0;
//    int y = 0;
//    RESURS  type = RESURS::EMPTY;
//    int gecs_game_number;       //номер для сравнения с броском кубика  - присваивается в начале игры

    //конструктор по умолчанию 
    GECS::GECS() {};
    GECS::GECS(int x = 0, int y = 0) : x(x), y(y)
           {};
    int GECS::getGecs_x()     {  return  x; }
    int GECS::getGecs_y()     {  return  y; }
    bool GECS::isGecs_infocus(int mouse_x, int mouse_y)
        {
        mouse_x = Game_x(mouse_x);  mouse_y = Game_y(mouse_y);
        if (abs(mouse_x - x) < 15 && abs(mouse_y - y) < 15)  return true;
        return false;
        }
//============================= GECS   end =============================================

//============================= класс дороги =====================================
//class ROAD
    
        //int Node_num_start = -1;
        //int Node_num_end = -1;
        //int owner = -1;      //== 0  - нет владельца
        //int type = 0;      // в будущих играх на пути может быть или дорога или мост

    ROAD::ROAD() {};
    ROAD::ROAD(int start, int end)
        {
        if (start < end) { Node_num_start = start;  Node_num_end = end; }
            else { Node_num_start = end;  Node_num_end = start; }
        }

    bool ROAD::isYourLastVillageNear(int node)
        {
        if (Node_num_start == node || Node_num_end == node)  return true;
        return false;
        }

   bool ROAD::isYourRoadNear(int player_num,int index)
    { 
    for (auto& r : *roadPtr)
           {
           if (r.Node_num_start == Node_num_start || r.Node_num_start == Node_num_end || r.Node_num_end == Node_num_start || r.Node_num_end == Node_num_end)
               if(r.owner == player_num)  return true;
           }
        return false;
    }
    
    bool ROAD::isYourNodeNear(int player, std::vector<NODE>* PtrNode)
        {
        if (PtrNode->at(Node_num_start).owner == player || PtrNode->at(Node_num_end).owner == player)  return true;
        return false;
        }

    bool ROAD::isRoad_infocus(int mouse_x, int mouse_y, std::vector<NODE>* PtrNode)
        {
        mouse_x = Game_x(mouse_x);  
        mouse_y = Game_y(mouse_y);
        int r_x = (PtrNode->at(Node_num_start).n_x + PtrNode->at(Node_num_end).n_x)/2;
        int r_y = (PtrNode->at(Node_num_start).n_y + PtrNode->at(Node_num_end).n_y)/2;
        if (mouse_x > r_x - 10 && mouse_x < r_x + 10 && mouse_y > r_y - 10 && mouse_y < r_y + 10)  return true;
        return false;
        };

    int ROAD::getRoad_start() { return  Node_num_start; }
    int ROAD::getRoad_end()   { return  Node_num_end; }
    int ROAD::getRoad_owner() { return  owner;  }

    ROAD::~ROAD() {};

 
 //============================= ROAD   end =============================================