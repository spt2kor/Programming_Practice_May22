#include <iostream>
#include <vector>
#include "TaskScheduler.h"
#include "TestForTaskScheduler.h"

using namespace std;
//=================================================================================
void OneTimeTaskFunc()
{
	static atomic<int> call_count_o = 0;
	cout << "\n OneTimeTaskFunc() called , call :"<< call_count_o++ << endl;
}

//=================================================================================
void RepeatTaskFunc()
{
	static atomic<int> call_count_r = 0;
	cout << "\n RepeatTaskFunc() called , call :" << call_count_r++ << endl;
}

//=================================================================================
int  main()
{
	cout << "\n **** MAin()  started " << endl;

	TestForTaskScheduler tester;
	tester.TestSingleOneTimeTask();

	tester.TestSingleRepeatTask();

	tester.Test10OneTimeTaskAnd10RepeatTaskWithTaskStop();

	cout << "\n **** Main() Ended" << endl;
	return 0;
}

//=================================================================================