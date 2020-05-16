#include <iostream>
#include <typeinfo>
#include <string.h>
#include <cstdlib>
#include <string.h>
#include <vector>
#include <iterator>

#include "CatanField.h"
#include "Catan_View.h"

//===================================================================================
//= ���������� �� �������� ����� � ����� CATAN2   7 may 
//===================================================================================

extern std::vector<GECS>* gecsPtr;    //��������� �� ������ ������
extern std::vector<NODE>* nodePtr;    //��������� �� ������ ����� ����
extern std::vector<ROAD>* roadPtr;    //��������� �� ������ �����

extern int player_num;


//===========================================================================
// ���������� true ���� ���� ����� � ���� � ������� 3 � 1
//===========================================================================
bool bonus31(int  player)
{
	for (auto node : *nodePtr)
	    {
		if (node.owner == player && node.type == 0)  return true;
        }
	return false;
}

//===========================================================================
// ���������� true ���� ���� ����� � ���� � ������� 2 � 1 c ��������� �����
//===========================================================================
bool bonus21(int  player,int type)
{
	for (auto node : *nodePtr)
	   {
	   if (node.owner == player && node.type == type)  return true;
	   }
	return false;
}

//================================================================
//���������� ����� ���� ��������� ������ ��������� �� node
//���������� -1 ���� ���� �� ����� �������� ������� ����
//�������� �������, ����� � ���� ��������� 2 ����� ������, � ���� �����,
// ����� ���� ����� ���� ������� 2 ����� !!!!
//================================================================
void isRoad_Start_Node(int node,int player, int* first, int* second, int* third)
{
 int num_roads = 0;

   for (auto& r : *roadPtr)
    {
	 //���� ������ ���� � ��������� � ����
	 if(r.owner == player  &&  (r.Node_num_start == node || r.Node_num_end == node))
	       {
		   //if(player_num == 0)  
			//   std::cout << " Node " << node << "  start = " << r.Node_num_start << " end =  " << r.Node_num_end << " owner =  " << r.owner  <<std::endl;

		    if (*first == -1) { node == r.Node_num_start ? *first = r.Node_num_end : *first = r.Node_num_start; }
			      else if(*second == -1) { node == r.Node_num_start ? *second = r.Node_num_end : *second = r.Node_num_start; }
				        else { node == r.Node_num_start ? *third = r.Node_num_end : *third = r.Node_num_start; }
	       }

    }  //for

   return;
}

//=========================================================================
//���������� true  ���� ������ ��������� � ����
// ���� �� ��������� ��������� ��������� first  second  ��� ��������� �����
//=========================================================================
bool isFinishWay(std::vector<int> road,int  player, int* first, int* second)
{
	int node;
	int node_prev;
	int new_node;

	*first = -1;
	*second = -1;
	node = road.at(road.size() - 1);
	node_prev = road.at(road.size() - 2);
	
	//���� ���� �����, �� ������ ������������
	if (nodePtr->at(node).owner != player && nodePtr->at(node).owner != -1)   return true;

	//���� � ���� ��������� ��� ���� 1 ������ ������ �� ��� �� ����� ������
	for (auto& r : *roadPtr)
	{
		if (r.owner == player && (r.Node_num_start == node || r.Node_num_end == node))
		{
			//���� ��� ������ �� �� ������� ������� � ����� ����
			if (r.Node_num_start != node_prev && r.Node_num_end != node_prev)
			    {
				r.Node_num_start != node ? new_node = r.Node_num_start : new_node = r.Node_num_end;
				if (*first != -1 && *second != -1) 
				       {
					   std::cout << "ERROR in isFinishWay  - more than 2 new way" << std::endl;
					   return  true; 
				       }
				if (*first == -1)  *first = new_node;
				   else *second = new_node;
			    }
		}
	}  //for

	if (*first != -1) return false;
	return true;
}

std::vector<std::vector<int>>  WAYS;
//=============================================================
//   ���������� ����� ������ ������
//   pl = ����� ������
//=============================================================
int Count_Road_Length(int pl)
{
	bool flag = false;

	std::vector<int> road;
	//std::vector<std::vector<int>>  WAYS;
	std::vector<std::vector<int>>  WAYS_TMP;
	const size_t road_size = 15;
	int first_way, second_way, third_way;

	int max_way = 0;
	int stack = 0;       //���-�� ������������� ����������� �����

	if (pl < 1)  return 0;
	WAYS.clear();

	//���� �� ����� ������
	for (size_t node = 0; node < nodePtr->size(); node++)
	{
	road.clear();      //������ ������ ������
	WAYS_TMP.clear();  //������ ���� ������������� �����������

	    //���� �� ���� �������� ������, � ����� ���������� �� 2 ���� ���� ���� ����� !!!!!!
	    second_way = -1;  first_way = -1;  third_way = -1;
	    isRoad_Start_Node(node, pl, &first_way, &second_way, &third_way);
		if (first_way == -1)    continue;  // ��� ������ ����� � ���� ����

		//������� ������ ���� 
		road.push_back(node);    road.push_back(first_way);    //������ ��������� ���� ��������� ����
		WAYS_TMP.push_back(road);    //�������� ������ � ������ ������������� �����
		road.clear();

		if(second_way != -1)        //���� ���� 2-� ������, �� �� ���� �������� � �������������
		   {
			road.push_back(node);	road.push_back(second_way);    //����� ���� � 2 ����
			WAYS_TMP.push_back(road);   
			road.clear();
		   }

		if (third_way != -1)       
		    {
			road.push_back(node);	road.push_back(third_way);     //����� ���� � 3 ����
			WAYS_TMP.push_back(road);
			road.clear();
		    }

		//std::cout << " Node= " << node << "\t1 = " << first_way << "\t2 =  " << second_way << "\t3 =  " << third_way << std::endl;

		//-------------------------------------------------------------------------------------------
		//���� ������ �� ���������� � ���� ����������� �� ���������
		while (WAYS_TMP.size() > 0)
		{
			//�������� ������ � ������� �������� ����� ������
			//if (WAYS.size() > 120)    { 	std::cout << " WAY.size() > 120  " << std::endl;	return 0;   }
			//if (WAYS_TMP.size() > 30)   {	std::cout << " WAYS_TMP.size() > 30  " << std::endl;  return 0;    }
			if (road.size() > 16)
			   { 
				std::cout << " road.size() > 16  " << " and TMP.size() =  " << WAYS_TMP.size() << std::endl;
				for (auto node : road) std::cout << " | " << node;
				std::cout << " | " << std::endl;

				std::cout << " ======== TMP road vector  =============== " << WAYS_TMP.size() << std::endl;
					for (auto rrr : WAYS_TMP)
					  {
				      for (auto node : rrr)  std::cout << " | " << node;
					  std::cout << " | " << std::endl;
					  }
			    return 0;
			   }

		road.clear();
		//������� ��������� ������������� ������    ( WAYS_TMP - ������ �� �������� )
	    //for (auto elem : WAYS_TMP.at(WAYS_TMP.size() - 1))   road.push_back(elem);     //����������� ���������� � ������
		for (auto elem : WAYS_TMP.back())   road.push_back(elem);     //����������� ���������� � ������
		WAYS_TMP.pop_back();   //������� ����������� ������

	     //�������� ��� ����� ������ �� 6 ��������� - ���� ��������� ���� ��� ����������� � �������,
		 //������ ������ ���������� �� ���� � �� ���� ���������
		if (road.size() > 6)
		    {
			flag = false;
			for (int i = 0; i < road.size() - 1; i++)
			   {	
				if (road.back() == road.at(i))
				        {
					    if (max_way < road.size() - 1)   max_way = road.size() - 1;
					    //WAYS.push_back(road); 
						flag = true;
						break;       //������ ����������� ������ � ����
				        }
			   }
			if (flag == true) continue;   //������� ���������� �� FOR � ����� ������������ �� ������ WHILE
		    }
			
		//std::cout << " road.size() =  " << road.size() << " TMP.size() =  " << WAYS_TMP.size() << std::endl;

		second_way = -1;  first_way = -1;
		if (isFinishWay(road,pl, &first_way, &second_way) == true)
			    {
			    if (max_way < road.size() - 1)  max_way = road.size()-1;
				//WAYS.push_back(road);
				continue;                //������ ���������
			    }
		//std::cout << " first_way  " << first_way << " second  " << second_way << std::endl;

		//������ ��������� � ����������� �������� ������ � ���� 
		if (second_way != -1)   {  road.push_back(second_way);  WAYS_TMP.push_back(road);	road.pop_back(); }

		road.push_back(first_way);	WAYS_TMP.push_back(road);   //������ ������ � �������������
		}  //while

	}  //for
	   
	return max_way;
}

//============================================================


//=============================================================
//   ���������� ����� ������ ������  TEST !!!
//   pl = ����� ������
//=============================================================
int Count_Road_LengthTest(int pl)
{
	int max_way = Count_Road_Length(pl);

	//----------------------------------------------------------------------------------
	//  ������ �������� ����� ��� ��������
	//----------------------------------------------------------------------------------
	std::cout << " == ��������� ������  ==    max_way = " <<  max_way << std::endl;
	for (auto rrr : WAYS)
	   {
	   for (auto node : rrr)
		  {
		   std::cout << " | " << node;
		  }
	   std::cout << " | " << std::endl;
	   }
	 

	return max_way;
}