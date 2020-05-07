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
//= Вычисления по массивам узлов и дорог CATAN2   7 may 
//===================================================================================

extern std::vector<GECS>* gecsPtr;    //указатель на вектор гексов
extern std::vector<NODE>* nodePtr;    //указатель на вектор узлов поля
extern std::vector<ROAD>* roadPtr;    //указатель на вектор дорог

//================================================================
//возвращает номер узла окончания дороги выходящей из node
//возвращает -1 если узел не может являться началом пути
//возможен вариант, когда к узлу примыкает 2 своих дороги, а узел чужой,
// тогда узел может быть началом 2 дорог !!!!
//================================================================
void isRoad_Start_Node(int node,int player, int* first, int* second)
{
 int num_roads = 0;

   for (auto& r : *roadPtr)
    {
	 //если дорога своя и примыкает к узлу
	 if(r.owner == player  && (r.Node_num_start == node || r.Node_num_end == node))
	       {
		   //std::cout << "**** Node " << node << " road start  " << r.Node_num_start << " end =  " << r.Node_num_end << std::endl;
		   //если узел свой или пустой и наберется 2 дороги, то это не стартовый узел
		    if (nodePtr->at(node).owner == player || nodePtr->at(node).owner == -1)
		         {
				 num_roads++;
				 /*
			     if (num_roads > 1 && (nodePtr->at(node).owner == -1 || nodePtr->at(node).owner == player))
			          {  *first = -1;   *second = -1;   return;      }  */
		         }

		    if (*first == -1) { node == r.Node_num_start ? *first = r.Node_num_end : *first = r.Node_num_start; }
			      else { node == r.Node_num_start ? *second = r.Node_num_end : *second = r.Node_num_start; }
	       }

    }  //for

   return;
}


//=========================================================================
//возвращает true  если дорога последняя в пути
// если не последняя заполняет указатели first  second  для найденных дорог
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
	
	//если узел чужой, то дорога оканчивается
	if (nodePtr->at(node).owner != player && nodePtr->at(node).owner != -1)   return true;

	//если к узлу примыкает еще хоть 1 дорога игрока то это не финиш дороги
	for (auto& r : *roadPtr)
	{
		if (r.owner == player && (r.Node_num_start == node || r.Node_num_end == node))
		{
			//если эта дорога не та которая подошла в нашем пути
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

//=============================================================
//   вычисление длины дороги игрока
//   pl = номер игрока
//=============================================================
int Count_Road_Length(int pl)
{
	std::vector<int> road;
	std::vector<std::vector<int>>  WAYS;
	std::vector<std::vector<int>>  WAYS_TMP;
	const size_t road_size = 15;
	int first_way, second_way;

	int max_way = 0;
	int stack = 0;       //кол-во недосчитанных ответвлений дорог

	if (pl < 1)  return 0;

	//цикл по узлам катана
	for (int node = 0; node < nodePtr->size(); node++)
	{
	road.clear();      //чистый вектор дороги
	WAYS_TMP.clear();  //чистый стек незавершенных ответвлений

	    //если из узла стартует дорога, а МОЖЕТ СТАРТОВАТЬ ДО 2 ШТУК если узел чужой !!!!!!
	    second_way = -1;  first_way = -1;
	    isRoad_Start_Node(node, pl, &first_way, &second_way);
		if (first_way == -1)    continue;  // нет начала дорог в этом узле

		//создаем первый путь 
		road.push_back(node);        //первым элементом ушел стартовый узел
		road.push_back(first_way);
		WAYS_TMP.push_back(road);    //загоняем дорогу в вектор незавершенных дорог
		road.clear();

		if(second_way != -1)        //если есть 2-я дорога, то ее тоже загоняем в незавершенные
		   {
			road.push_back(node);   
			road.push_back(second_way);
			WAYS_TMP.push_back(road);   
			road.clear();
		   }

		//-------------------------------------------------------------------------------------------
		//пока дорога не закончится и стек ответвлений не очистится
		while (WAYS_TMP.size() > 0)
		{
			//контроль ошибок в функции подсчета длины дороги
			if (WAYS.size() > 120)    { 	std::cout << " WAY.size() > 120  " << std::endl;	return 0;   }
			if (WAYS_TMP.size() > 30)   {	std::cout << " WAYS_TMP.size() > 30  " << std::endl;  return 0;    }
			if (road.size() > 15)
			   { 
				std::cout << " road.size() > 15  " << " and TMP.size() =  " << WAYS_TMP.size() << std::endl;
				for (auto node : road) std::cout << " | " << node;
				std::cout << " | " << std::endl;

				std::cout << " ======== TMP vector  =============== " << WAYS.size() << std::endl;
					for (auto rrr : WAYS_TMP)
					{
						for (auto node : rrr)  std::cout << " | " << node;
						std::cout << " | " << std::endl;
					}
			    return 0;
			   }

		road.clear();
		//считать последнюю незавершенную дорогу    ( WAYS_TMP - вектор из векторов )
	    for (auto elem : WAYS_TMP.at(WAYS_TMP.size() - 1))   road.push_back(elem);     //поэлементно запихиваем в дорогу
		WAYS_TMP.pop_back();   //удаляем прочитанную дорогу

	     //проверка для дорог длиной от 6 элементов - если последний узел уже встречается в массиве,
		 //значит дорога замкнулась на себя и ее надо завершить
		if (road.size() > 6)
		    {
			auto last = (road.at(road.size() - 1));
			for (int i = 0; i < road.size() - 1; i++)
			   {	
				if (last == road.at(i)) 
				        {
					    if (max_way < road.size() - 1)   max_way = road.size() - 1;
					    //WAYS.push_back(road); 
						last = 20;  
						break;       //дорога закончилась петлей в себя
				        }
			   }
			if (last == 20) continue;   //брейком вывалились из FOR и далее вывадиваемся на начало WHILE
		    }
			
		//std::cout << " road.size() =  " << road.size() << " TMP.size() =  " << WAYS_TMP.size() << std::endl;

		second_way = -1;  first_way = -1;
		if (isFinishWay(road,pl, &first_way, &second_way) == true)
			    {
			    if (max_way < road.size() - 1)  max_way = road.size()-1;
				//WAYS.push_back(road);
				continue;                //дорога закончена
			    }
		//std::cout << " first_way  " << first_way << " second  " << second_way << std::endl;

		//второе ветвление с добавлением элемента дороги в стэк 
		if (second_way != -1)   {  road.push_back(second_way);  WAYS_TMP.push_back(road);	road.pop_back(); }

		road.push_back(first_way);	WAYS_TMP.push_back(road);   //кидаем дорогу в незаконченные
		}  //while

	}  //for

	//----------------------------------------------------------------------------------
	//  печать векторов дорог для контроля
	//----------------------------------------------------------------------------------
	/*
	std::cout << " == Найденные дороги  ==    max_way = " <<  max_way << std::endl;
	for (auto rrr : WAYS)
	   {
	   for (auto node : rrr)
	      {
		   std::cout << " | " << node;
	      }
	   std::cout << " | " << std::endl;
	   }
	   */
	return max_way;
}

//============================================================
