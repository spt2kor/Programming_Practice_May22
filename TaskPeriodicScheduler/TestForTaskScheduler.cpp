#include "TestForTaskScheduler.h"

//===============================================================================================
//===============================================================================================

TestForTaskScheduler::TestForTaskScheduler() 
		:taskScheduler(TaskScheduler::GetInstance())
{
	//vector<FNPTR> testFuncs = { &TestForTaskScheduler::TestSingleOneTimeTask
	//							,& TestForTaskScheduler::TestSingleRepeatTask
	//							,& TestForTaskScheduler::Test10OneTimeTaskAnd10RepeatTaskWithTaskStop };

	//swap(_allTestFunc, testFuncs);
}
//===============================================================================================
//===============================================================================================
/*
void TestForTaskScheduler::RunAllTests()
{
	for (auto itr = _allTestFunc.begin(); itr != _allTestFunc.end(); ++itr) 
	{
		FNPTR func = *itr;
		(* this).(*func)();
	};
}
*/

//===============================================================================================
//===============================================================================================
void TestForTaskScheduler::TestSingleOneTimeTask()
{
	cout << "\n===============================================" << endl;
	cout << "@@@@ starting test TestForTaskScheduler::TestSingleOneTimeTask" << endl;
	cout << "\n===============================================" << endl;
	taskScheduler.StartTaskScheduler();
	this_thread::sleep_for(1s);

	cout << "\n **** Adding One Time Task id= 10 , delay = 3s" << endl;
	TaskId onceTask= taskScheduler.AddOneTimeTask(10, OneTimeTaskFunc
				, std::chrono::system_clock::now(), 3s );
	

	cout << "\n TestForTaskScheduler::TestSingleOneTimeTask going on wait for 10s" << endl;
	this_thread::sleep_for(10s);
	taskScheduler.StopTaskScheduler();
	cout << "\n===============================================" << endl;
}
//===============================================================================================
//===============================================================================================
void TestForTaskScheduler::TestSingleRepeatTask()
{
	cout << "\n===============================================" << endl;
	cout << "@@@@ starting test TestForTaskScheduler::TestSingleRepeatTask" << endl;
	cout << "\n===============================================" << endl;
	taskScheduler.StartTaskScheduler();
	this_thread::sleep_for(1s);

	cout << "\n **** Adding Repeat Task id= 20 , delay = 3s" << endl;
	TaskId reptTask = taskScheduler.AddRepeatTask(20, RepeatTaskFunc
		, std::chrono::system_clock::now()
		, 3s , 2s) ;

	cout << "\n TestForTaskScheduler::TestSingleRepeatTask going on wait for 30s" << endl;
	this_thread::sleep_for(30s);
	taskScheduler.StopTaskScheduler();
	cout << "\n===============================================" << endl;
}

//===============================================================================================
//===============================================================================================
void TestForTaskScheduler::Test10OneTimeTaskAnd10RepeatTaskWithTaskStop()
{
	cout << "\n===============================================" << endl;
	cout << "@@@@ starting test TestForTaskScheduler::Test10OneTimeTaskAnd10RepeatTaskWithTaskStop" << endl;
	cout << "\n===============================================" << endl;

	taskScheduler.StartTaskScheduler();
	this_thread::sleep_for(2s);

	//add one time tasks
	vector<TaskId> onceTasksVec;
	for (int i = 0; i < 10; ++i) {
		cout << "\n **** Adding One Time Task i= " << i << endl;
		onceTasksVec.push_back(
			taskScheduler.AddOneTimeTask(i, OneTimeTaskFunc
				, std::chrono::system_clock::now(), 3s * (i + 1)));
	}
	this_thread::sleep_for(10s);
	//try stoppping one time task
	cout << "\n ****  try stopping One Time Task i= " << 5 << endl;
	taskScheduler.StopATask(onceTasksVec[5]);
	cout << "\n ****  try stopping One Time Task i= " << 9 << endl;
	taskScheduler.StopATask(onceTasksVec[9]);

	this_thread::sleep_for(10s);
	//add Repeat time tasks
	vector<TaskId> reptTasksVec;
	for (int i = 0; i < 10; ++i) {
		cout << "\n ****  Adding Repeat Time Task i= " << i << endl;
		reptTasksVec.push_back(
			taskScheduler.AddRepeatTask(i, RepeatTaskFunc
				, std::chrono::system_clock::now()
				, 4s * (i + 1), 5s * (i % 4 + 1)));
	}

	//try stoppping Repeat task
	std::this_thread::sleep_for(5s);
	cout << "\n ****  try stopping Repeat Time Task i= " << 0 << endl;
	taskScheduler.StopATask(onceTasksVec[0]);

	std::this_thread::sleep_for(5s);
	cout << "\n ****  try stopping Repeat Time Task i= " << 1 << endl;
	taskScheduler.StopATask(onceTasksVec[1]);


	this_thread::sleep_for(50s);
	//closing the scheduler
	cout << "\n ****  try StopTaskScheduler() " << endl;
	taskScheduler.StopTaskScheduler();

	taskScheduler.StopTaskScheduler();
	cout << "\n===============================================" << endl;
}
//===============================================================================================
//===============================================================================================