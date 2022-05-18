#include <iostream>
#include "interval_map.h"
using namespace std;

void IntervalMapTest();

void main()
{
	map<int, char> m;
	cout << endl << "map.begin() == map.end() =  " << (m.begin() ==m.end() ? "TRUE" : "FALSE") << endl;

	cout << "\n Program Started" << endl;

	interval_map<int, char> intvMap('A');
	intvMap.assign(1, 3, 'B');
	
	cout << "\n------------------------" << endl;

	for (int i = -2; i < 6; ++i)
	{
		cout << "\n key = " << i << " , val = " << intvMap[i];
	}

	cout << "\n------------------------" << endl;

	IntervalMapTest();

	cout << "\n Program Ended" << endl;
}

//========================

void IntervalMapTest()
{
	cout << "\n IntervalMapTest Started" << endl;

}

//========================
