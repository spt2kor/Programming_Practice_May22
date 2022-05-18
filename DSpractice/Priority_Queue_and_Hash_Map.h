#pragma once
#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

struct Priority_Queue_and_Hash_Map
{
	struct Node
	{
		int dist;
		int id;
	};
	struct comp
	{
		bool min_heap = true;
		comp(bool min_heap = true) : min_heap(min_heap) {}
		bool operator() (int d1, int d2)
		{
			if (min_heap)
				return d1 < d2;
			else
				return d1 > d2;
		}
	};
	unordered_map<int, Node*>  um;
	priority_queue<Node> pq;
	bool min_heap = true;
	comp comparator;

	Priority_Queue_and_Hash_Map(bool min_heap = true) 
							: min_heap(min_heap)
							, comparator(min_heap)
	{}

	void insert(int id,int dist);

	void update(int id, int dist);

	void remove(int id);

	int get_top();
};


