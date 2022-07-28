#include "Tasks.h"
#include <iostream>
using namespace std;


//=========================================================================================
//=========================================================================================
// launch new thread, calculate next Exec time , insert next task into Scheduler
ITask::ITask(int taskIntID, TaskFuncPtr fnptr, time_point firstExecTime
	, time_duration	firstDelayDuration, ExecutionFrequency Tasktype)
	: taskIntID(taskIntID) , fnptr(fnptr), firstExecTime(firstExecTime)
	, firstDelayDuration(firstDelayDuration), nextExecTime(firstExecTime)
	, execFreqType(Tasktype)
{
	CalculateNextExecTime();
}

//-------------------------------------------------------------------------------------------
void ITask::Execute() {
	cout << "\txxx starting ITask::Execute() " << endl;
	cout << "\n -------- --------- -------- --------- -------- " << endl;
	try
	{
		PrintTask();
		fnptr();
	}
	catch (exception& e)
	{
		cout << "\txxx Exception Caught in ITask::Execute() , e= " << e.what() << endl;
	}
	catch (...)
	{
		cout << "\txxx Unknown Exception Caught in ITask::Execute() " << endl;
	}
	cout << "\n -------- --------- -------- --------- -------- " << endl;
	cout << "\txxx Closing ITask::Execute() " << endl;	
}

//-------------------------------------------------------------------------------------------
// 
// NOTE: will try to join the task in DTOR ITask::~ITask(), then cleanup the memory
ITask::~ITask()
{
}

//-------------------------------------------------------------------------------------------
auto ITask::GetExecFreqAsStr() const
{
	return string((ExecutionFrequency::OneTimeExecution == execFreqType) ? "OneTimeExecution" : "RepeatedExecution");
}


void ITask::PrintTask() const
{
	cout << "\n \txxx ITask int ID : [[ " << taskIntID <<" ]], [[" << GetExecFreqAsStr() << "]] "<< endl;
}

//=========================================================================================
//=========================================================================================
// launch new thread, calculate next Exec time , insert next task into Scheduler
void RepeatTask::Execute() {
	cout << "\txxx starting RepeatTask::Execute() . repeatExecCount = " << ++repeatExecCount << endl;
	ITask::Execute();
	CalculateNextExecTime();
	cout << "\txxx Closing RepeatTask::Execute() " << endl;
}

