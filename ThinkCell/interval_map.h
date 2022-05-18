#pragma once
#include <iostream>
#include <map>
using namespace std;

template<typename K, typename V>
class interval_map 
{

	friend void IntervalMapTest();

	V m_valBegin;
	std::map<K, V> m_map;

public:
	// constructor associates whole range of K with val
	interval_map(V const& val)
		: m_valBegin(val)
	{}

	// Assign value val to interval [keyBegin, keyEnd).
	// Overwrite previous values in this interval.
	// Conforming to the C++ Standard Library conventions, the interval
	// includes keyBegin, but excludes keyEnd.
	// If !( keyBegin < keyEnd ), this designates an empty interval,
	// and assign must do nothing.
	// 
	//1. consecutive map entries must not contain the same value: ..., (3,'A'), (5,'A'), ... is not allowed.
	//2. the first entry in m_map must not contain the same value as m_valBegin.
	//3. Initially, the whole range of K is associated with a given initial value, passed to the constructor of the interval_map<K,V> data structure.
	//4. 

	void assign(K const& keyBegin, K const& keyEnd, V const& val) 
	{
		cout<<"\n assign() keyBegin ="<< keyBegin<<" , keyEnd = " << keyEnd<<" , val = "<< val;
		if (!(keyBegin < keyEnd)) // greater or equal to, (keyEnd<= keyBegin)
		{
			cout << "\n ERROR: @@@ (keyBegin < keyEnd) = false, check input" << endl;
			return;
		}

		//====================================================================================
		//add first entry,
		if ( m_map.empty() )
		{
			if (val == m_valBegin)
			{
				cout << "\n ERROR: @@@ (val == m_valBegin) , adding 1st rainge in empty map" << endl;
				return;
			}

			m_map.insert({ keyBegin, val });
			m_map.insert({keyEnd, m_valBegin });
			return;
		}

		//====================================================================================
		//at least one range already present
		//====================================================================================
		auto itr_old_first = m_map.begin();
		auto itr_old_end = --(m_map.end());

		//add first node
		//====================================================================================
		//add before first node
		if (keyBegin < itr_old_first->first)
		{
			if (val == m_valBegin)
			{
				cout << "\n ERROR: @@@ (val == m_valBegin) , adding this range at start " << endl;
				return;
			}
			
			m_map.insert({ keyBegin, val });
			
			//if same value as old first node, then delete old first node, increase the range
			if (val == itr_old_first->second)
			{
				m_map.remove(itr_old_first);
			}
		}
		//overlapping with first node key
		else if ( !(keyBegin < itr_old_first->first) && !(itr_old_first->first < keyBegin) )
		{
			if (val == m_valBegin)
			{
				cout << "\n ERROR: @@@ (val == m_valBegin) , adding this range at start " << endl;
				return;
			}
			// if same value dont insert key, insert if keys are different
			else if (val != itr_old_first->second)
			{
				itr_old_first.first = val;
			}			
		}
		//keyBegin greater than first key then it can be any where in the map
		else if (itr_old_first->first < keyBegin)
		{
			auto itr_upr = m_map.upper_bound(keyBegin);
			if (itr_upr == m_map.end())
			{
				itr_upr--;

			}

		}

		//add 2nd node
		//====================================================================================
	
	}

	// look-up of the value associated with key
	V const& operator[](K const& key) const {
		auto it = m_map.upper_bound(key);
		if (it == m_map.begin()) {
			cout << "\n operator[] :return m_valBegin = for key = " << key << endl;
			return m_valBegin;
		}
		else {
			if(it == m_map.end())
				cout << "\n operator[] :reached to map.end(), returning prev value" << endl;
			return (--it)->second;
		}
	}
};

// Many solutions we receive are incorrect. Consider using a randomized test
// to discover the cases that your implementation does not handle correctly.
// We recommend to implement a test function that tests the functionality of
// the interval_map, for example using a map of int intervals to char.