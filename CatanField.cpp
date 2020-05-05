#include <iostream>
#include <cstdlib>
#include <vector>
#include <typeinfo>
#include <time.h>
#include "CatanField.h"
#include "Catan_View.h"


extern std::vector<GECS>* gecsPtr;    //��������� �� ������ ������
extern std::vector<NODE>* nodePtr;    //��������� �� ������ ����� ����
extern std::vector<ROAD>* roadPtr;    //��������� �� ������ �����


//������ 15 �� 20 - ������������� ��� �������� ����
int CATAN19[15][20] =
{
    {   2,3,4   },   //   0, 1, 2
    {  2,3,4,5  },   //  3, 4, 5, 6
    { 1,2,3,4,5 },   // 7, 8, 9,10,11
    {  2,3,4,5  },   //  12,13,14,15
    {   2,3,4   }    //   16,17,18
};

//��� 19 ������ ���������� ���� �������� � ����
int GECS_POSITION[6][19] =
{
{ 0,1,2,6,11,15,18,17,16,12,7,3,4,5,10,14,13,8,9 },
{ 2,6,11,15,18,17,16,12,7,3,0,1,5,10,14,13,8,4,9 },
{ 11,15,18,17,16,12,7,3,0,1,2,6,10,14,13,8,4,5,9 },
{ 18,17,16,12,7,3,0,1,2,6,11,15,14,13,8,4,5,10,9 },
{ 16,12,7,3,0,1,2,6,11,15,18,17,13,8,4,5,10,14,9 },
{ 7,3,0,1,2,6,11,15,18,17,16,12,8,4,5,10,14,13,9 }
};

//� ���� - ����� 3.������� 3, ����� - 4. ����� 4 ������ 4 ��������� 1
RESURS Simple_Game[19] = { RESURS::PIRATE,
                           RESURS::STONE,RESURS::STONE,RESURS::STONE,
                           RESURS::BRICKS,RESURS::BRICKS,RESURS::BRICKS,
                           RESURS::BREAD,RESURS::BREAD,RESURS::BREAD,RESURS::BREAD,
                           RESURS::OVCA,RESURS::OVCA,RESURS::OVCA,RESURS::OVCA,
                           RESURS::WOOD,RESURS::WOOD,RESURS::WOOD,RESURS::WOOD };



//============================= ����� PLAYER =====================================
//class PLAYER

    //int village = 5;
    //int town = 4;
    //int road = 15;
    //int resurs[10];
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


//==================================================================================================
// ���������� ���� � ������ ����
//==================================================================================================
int AddNewNode(int x, int y, std::vector<NODE>* PtrNode)
{
  //��������� ���� �� ����� ���� � �������
    for (auto& elem : *PtrNode)
      {
      if (x == elem.getNode_x() && y == elem.getNode_y())   return 0;
      }

    //��������� ���� � ������
    PtrNode->push_back(NODE(x, y));

    return 1;
}

//==================================================================================================
// ���������� ���� � ������ ����
//==================================================================================================
int AddNewRoad(int start, int end, std::vector<ROAD>* PtrRoad)
{
    int index = 0;
    //��������� ���� �� ������  � �������
    for (auto& elem : *PtrRoad)
    {
      if (start == elem.getRoad_start() && end == elem.getRoad_end())   return 0;
      if (end == elem.getRoad_start() && start == elem.getRoad_end())   return 0;
    }

    //��������� ������ � ������
    PtrRoad->push_back(ROAD(start, end));
    index++;

    return 1;
}

//=================================================================================================
//��������� ��� CATAN 19  ��������� ������������ �������� ������ ������ - �� ������� ������� �� ��������
//�������� ���������� ���� � �������� �������������� ������������������
//=================================================================================================
void SetRandomGecsNumber19(std::vector<GECS>* PtrGecs)
{
 //�������� ������ ������ 18, ��� ��� � 1 ���� ��� �������� ��� ����� �������
 int game_number[18]{ 5,2,6,3,8,10,9,12,11,4,8,10,9,4,5,6,3,11 };  //�� �������� ��� �� ��������� � ����

 int i, ii;

 srand(time(0)); 
 int coner = rand() % 6;   //�������� ���������� ��������� ���� ������ ��� ���������� ������
 std::cout << "  ���������� ������� � ����   " << coner << std::endl;

 int position;
 for (i = 0, ii = 0; i < 19; i++)
    {
     position = GECS_POSITION[coner][i];
     if (PtrGecs->at(position).type != RESURS::PIRATE)
        {
         PtrGecs->at(position).gecs_game_number = game_number[ii++];  //ii ������������� ������ ��� ����������
        }
    }  //end for

    return;
}

//=================================================================================================
//��������� ��� CATAN 19  ��������� ������������ ����� ������
//=================================================================================================
void SetRandomGecsType19(std::vector<GECS>* PtrGecs)
{
 int i, ii;

    srand(time(0)); // �������������� ������������

    //������ � ����� � ���-��� �������� � ������ ����� ����� Simple_Game[19]
    for (i = 0; i < 19; i++)
    {
        //�������� ��������� �����
        ii = rand() % 19;
        while (true)   //���� ���� �� ������� � ��������� ����� ��� ����
        {
           if (PtrGecs->at(ii).type == RESURS::EMPTY)
            {
                PtrGecs->at(ii).type = Simple_Game[i];
                //std::cout << "  Gecs " << i << "  put in " << ii << " sell  \n";
                break;
            }
            else { if (++ii >= 19)   ii = 0; }
        }
    }  //end for
    return;
}

//=========================================================================
// ������ ���� � �������� ����� ���� �������
//=========================================================================
bool isVillageNear(int index)
{
    int nx = nodePtr->at(index).n_x;
    int ny = nodePtr->at(index).n_y;

    for (auto& n : *nodePtr)
      {
        if (abs(n.n_x - nx) <= 20 && abs(n.n_y - ny) <= 20) 
               if(n.owner != -1)  return true;               //������� ���� ���-�� �����
      }

 return false;
}

//==================================================================================================
// ������������� �������� ������� 
// ���������� �� main ���������� ����������
//==================================================================================================
int Init_CATAN_Field(std::vector<GECS>* PtrGecs, std::vector<NODE>* PtrNode, std::vector<ROAD>* PtrRoad)
{
//������������� ����� ������������ ������ ---------------------------------------------------------
    for (int ii = 0; ii < 15; ii++)
    {
        for (int i = 0; i < 20; i++)
        {
            //���������� ������ ����� � �������� ������������
            if (CATAN19[ii][i] == 0)  continue;    //�� ���������� X == 0 �� ����� ���� ������������ �����

            //�� ������ ����� ���������� ��� ����������
            int x = (CATAN19[ii][i] * 4 - 2*(ii%2)) * 10;
            int y = (ii * 3 + 2) * 10;     //����� ���� ��������� � �������� ���������� y ��� �����

            PtrGecs->push_back(GECS(x, y));  //��������� � ����� �������

            //std::cout << "  Gecs x = " << PtrGecs->at(i).getGecs_x() << "   Gecs y = " << PtrGecs->at(i).getGecs_y() << std::endl;
            //std::cout << "  Vector size =  " << PtrGecs->size() << std::endl;
        }
    }
    //������ ������������ ������ ����� ������ ��� ���� � 19 ���������
    SetRandomGecsType19(PtrGecs);
    SetRandomGecsNumber19(PtrGecs);

    //����� ���� �� ������ -----------------------
    int i = 0;
    for(auto &elem : *PtrGecs)
       {
        //std::cout << i++ << "\tGecs x = " << elem.getGecs_x() <<  "\t|  y = " <<  elem.getGecs_y() <<std::endl;
       }

//�� ������� ������ ��������� ������ ����� ���� --------------------------------------------------
    int nx,ny;
    for (auto& elem : *PtrGecs)
    {
        //������� ����� ���������� 6 ������
        nx = elem.getGecs_x();      ny = elem.getGecs_y()  - 20;  AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x()+20;   ny = elem.getGecs_y() - 10;   AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x()+20;   ny = elem.getGecs_y() + 10;   AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x();      ny = elem.getGecs_y()  + 20;  AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x()-20;   ny = elem.getGecs_y() + 10;   AddNewNode(nx, ny, PtrNode);
        nx = elem.getGecs_x()-20;   ny = elem.getGecs_y() - 10;   AddNewNode(nx, ny, PtrNode);
    }

    std::cout <<  "\n ===============���� ����� CATAN FIELD ================ "  << std::endl;
    i = 0;
    //����������� ������� ����� 
    for (auto& n : *PtrNode)
    {
     n.number = i;
     //std::cout << i << "\tNode x = " << n.getNode_x() << "\t|  y = " << n.getNode_y() << " index = " << n.number << std::endl;
     i++;
    }

//�� ������� ����� ��������� ������ ----------------------------------------------------------------------
    int road_num = 0;
    int x_next,y_next;
    int start = 0,end = 0;
    std::vector<NODE>* PtrNode2;
    PtrNode2 = PtrNode;
    for (auto& n : *PtrNode)
    {
    //���� ������� ���� � ����������� �� �������� ���������� ����� 2.02 ������
    //�� ������� ��� ���� �������� � ������������ ���� �� ������ ���� �� ����������
        nx = n.getNode_x(); ny = n.getNode_y();   //������� ����
        end = 0;
        for (auto& next : *PtrNode2)
          {
           x_next = next.getNode_x(); y_next = next.getNode_y();
           if (x_next == nx && y_next == ny)   continue;         //���� ���������� ���� ��� � �����
           if (abs(x_next - nx) <= 20 && abs(y_next - ny) <= 20)
                    {
                    //add road
                    start = n.getNode_number();
                    end = next.getNode_number();
                    AddNewRoad(start, end, PtrRoad);
                    }
          } //foreach in
    }  //foreach out

    //���������� �����
    /*
    std::cout << "\n =============== ������ ����� ================ " << std::endl;
    i = 0;
    for (auto& rr : *PtrRoad)
    {
        std::cout << i++ << "\tROAD from = " << rr.getRoad_start() << "\t|  to = " << rr.getRoad_end() << std::endl;
    }*/

    return 1;
}

//===============================================================================
//�������� �������� ������ ���� �����
//class NODE
    //int n_x = 0;
    //int n_y = 0;
    //int owner = -1;      //� ������ ��������� ����    ==  -1  - ��� ��������� 
    //int object = -1;     //��� ��������� � ����       ==  -1  - ��� ����������
    //int number = -1;     //���������� ����� ����
NODE::NODE() {};
 NODE::NODE(int x = 0, int y = 0) : n_x(x), n_y(y)
        {};

 
 //������ ������ ���� ����� ����� �� �������
 bool NODE::isNode_free(int index)
 {
     if (owner == -1) return true;
     return false;
 };

 bool NODE::isNode_infocus(int mouse_x, int mouse_y)
 {
     mouse_x = Game_x(mouse_x);  mouse_y = Game_y(mouse_y);
     if (mouse_x > n_x - 10 && mouse_x < n_x + 10 && mouse_y > n_y - 10 && mouse_y < n_y + 10)   return true;
     return false;
 };
 int NODE::getNode_number()  { return number; }
 int NODE::getNode_x() { return n_x; }
 int NODE::getNode_y() { return n_y; }
//=============================== end NODE ==============================

//============================= ����� GECS =====================================
//class GECS
//public:
//    int x = 0;
//    int y = 0;
//    RESURS  type = RESURS::EMPTY;
//    int gecs_game_number;       //����� ��� ��������� � ������� ������  - ������������� � ������ ����

    //����������� �� ��������� 
    GECS::GECS() {};
    GECS::GECS(int x = 0, int y = 0) : x(x), y(y)
           {};
    int GECS::getGecs_x()     {  return  x; }
    int GECS::getGecs_y()     {  return  y; }
   //============================= GECS   end =============================================

//============================= ����� ������ =====================================
//class ROAD
    
        //int Node_num_start = -1;
        //int Node_num_end = -1;
        //int owner = -1;      //== 0  - ��� ���������
        //int type = 0;      // � ������� ����� �� ���� ����� ���� ��� ������ ��� ����

    ROAD::ROAD() {};
    ROAD::ROAD(int start, int end)
        {
        if (start < end) { Node_num_start = start;  Node_num_end = end; }
            else { Node_num_start = end;  Node_num_end = start; }
        }

    bool ROAD::isYourNodeNear(int player, std::vector<NODE>* PtrNode)
        {
        if (PtrNode->at(Node_num_start).owner == player || PtrNode->at(Node_num_end).owner == player)  return true;
        return false;
        }

    bool ROAD::isRoad_infocus(int mouse_x, int mouse_y, std::vector<NODE>* PtrNode)
        {
        mouse_x = Game_x(mouse_x);  mouse_y = Game_y(mouse_y);
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