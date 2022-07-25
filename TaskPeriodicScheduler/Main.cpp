#include <iostream>
#include <vector>
#include "TaskScheduler.h"
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
	auto& taskScheduler = TaskScheduler::GetInstance();
	cout << "\n **** MAin()  started " << endl;

	//add one time tasks
	vector<TaskId> onceTasksVec;
	for (int i = 0; i < 10; ++i) {
		cout << "\n **** Adding One Time Task i= " << i << endl;
		onceTasksVec.push_back(
			taskScheduler.AddOneTimeTask(OneTimeTaskFunc
				, std::chrono::system_clock::now(), 3s * (i + 1)));
	}
	//try stoppping one time task
	cout << "\n **** try stopping One Time Task i= " << 5 << endl;
	taskScheduler.StopATask(onceTasksVec[5]);
	cout << "\n **** try stopping One Time Task i= " << 9 << endl;
	taskScheduler.StopATask(onceTasksVec[9]);

	//add Repeat time tasks
	vector<TaskId> reptTasksVec;
	for (int i = 0; i < 10; ++i) {
		cout << "\n **** Adding Repeat Time Task i= " << i << endl;
		reptTasksVec.push_back(
			taskScheduler.AddRepeatTask(RepeatTaskFunc
				, std::chrono::system_clock::now()
				, 4s * (i + 1) , 5s*(i%4 + 1 )  )  );
	}

	//try stoppping Repeat task
	std::this_thread::sleep_for(5s);
	cout << "\n **** try stopping Repeat Time Task i= " << 0 << endl;
	taskScheduler.StopATask(onceTasksVec[0]);

	std::this_thread::sleep_for(5s);
	cout << "\n **** try stopping Repeat Time Task i= " << 1 << endl;
	taskScheduler.StopATask(onceTasksVec[1]);
	

	//closing the scheduler
	cout << "\n **** try StopTaskScheduler() "<< endl;
	taskScheduler.StopTaskScheduler();

	cout << "\n **** try DistroyInstance() " << endl;
	TaskScheduler::DistroyInstance();

	cout << "\n **** Main() Ended" << endl;
	return 0;
}

//=================================================================================