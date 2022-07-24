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
void main()
{
	auto& taskScheduler = TaskScheduler::GetInstance();
	cout << "\n **** TaskPeriodicScheduler started " << endl;

	vector<TaskId> onceTasksVec;
	for (int i = 0; i < 10; ++i) {
		onceTasksVec.push_back(
			taskScheduler.AddOneTimeTask(OneTimeTaskFunc
				, std::chrono::system_clock::now(), 3s * (i + 1)));
	}
	taskScheduler.StopATask(onceTasksVec[5]);
	taskScheduler.StopATask(onceTasksVec[9]);

	vector<TaskId> reptTasksVec;
	for (int i = 0; i < 10; ++i) {
		reptTasksVec.push_back(
			taskScheduler.AddRepeatTask(RepeatTaskFunc
				, std::chrono::system_clock::now()
				, 4s * (i + 1) , 5s*(i%4 + 1 )  )  );
	}

	std::this_thread::sleep_for(5s);
	taskScheduler.StopATask(onceTasksVec[0]);
	std::this_thread::sleep_for(5s);
	taskScheduler.StopATask(onceTasksVec[1]);

	cout << "\n **** TaskPeriodicScheduler Ended" << endl;
	taskScheduler.StopTaskScheduler();
}

//=================================================================================