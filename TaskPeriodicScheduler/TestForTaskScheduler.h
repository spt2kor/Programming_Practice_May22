#pragma once
#include "TaskScheduler.h"
#include <iostream>
using namespace std;

class TestForTaskScheduler
{
private:
	TaskScheduler& taskScheduler;
	typedef void (TestForTaskScheduler::*FNPTR) ();
	vector<FNPTR> _allTestFunc;
public:

	TestForTaskScheduler();
	~TestForTaskScheduler() 
	{
		//taskScheduler.DistroyInstance();
	}

	TestForTaskScheduler(const TestForTaskScheduler&) = delete;
	TestForTaskScheduler(TestForTaskScheduler&& ) = delete;
	TestForTaskScheduler& operator= (const TestForTaskScheduler&) = delete;
	TestForTaskScheduler& operator= (const TestForTaskScheduler&&) = delete;


	//all test functions below
	void TestSingleOneTimeTask();
	void TestSingleRepeatTask();
	void Test10OneTimeTaskAnd10RepeatTaskWithTaskStop();
	
	//void RunAllTests();

};

