#include "Tasks.h"
#include <iostream>
using namespace std;


//=========================================================================================
//=========================================================================================
// launch new thread, calculate next Exec time , insert next task into Scheduler
ITask::ITask(TaskFuncPtr fnptr, time_point firstExecTime
	, time_duration	firstDelayDuration, ExecutionFrequency Tasktype)
	: fnptr(fnptr), firstExecTime(firstExecTime)
	, firstDelayDuration(firstDelayDuration), nextExecTime(firstExecTime)
	, execFreqType(Tasktype)
{
	CalculateNextExecTime();
}

//-------------------------------------------------------------------------------------------
void ITask::Execute() {
	cout << "## starting ITask::Execute() " << endl;
	try
	{
		PrintTask();
		fnptr();
	}
	catch (exception& e)
	{
		cout << "## Exception Caught in ITask::Execute() , e= " << e.what() << endl;
	}
	catch (...)
	{
		cout << "## Unknown Exception Caught in ITask::Execute() " << endl;
	}
	cout << "## Closing ITask::Execute() " << endl;	
}

//-------------------------------------------------------------------------------------------
// 
// NOTE: will try to join the task in DTOR ITask::~ITask(), then cleanup the memory
ITask::~ITask()
{
	if (TaskStatus::Running == tStatus)
	{
		if (threadID.joinable())
			threadID.join();
		tStatus = TaskStatus::MarkDelete;
	}
}

//-------------------------------------------------------------------------------------------
/*TaskStatus		tStatus{TaskStatus::Created};
TaskId			taskId{ 0 };
TaskFuncPtr		fnptr;
time_point		firstExecTime;
time_duration	firstDelayDuration;
time_point		nextExecTime;
ExecutionFrequency execFreqType{ ExecutionFrequency::OneTimeExecution };
thread			threadID;
*/
void ITask::PrintTask() const
{
	cout << "\n $$ ITask: " << taskId.GetTaskId()<< ", threadID: " << threadID.get_id() << endl;
}

//=========================================================================================
//=========================================================================================
// launch new thread, calculate next Exec time , insert next task into Scheduler
void RepeatTask::Execute() {
	cout << "## starting RepeatTask::Execute() " << endl;
	ITask::Execute();
	CalculateNextExecTime();
	cout << "## Closing RepeatTask::Execute() " << endl;
}

