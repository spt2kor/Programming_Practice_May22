#pragma once

#include "CommonDef.h"
#include <thread>

//================================================================================
class ITask
{
protected:
	TaskStatus		tStatus{ TaskStatus::Created };

	TaskId			taskId{ 0 };

	TaskFuncPtr		fnptr;
	time_point		firstExecTime;
	time_duration	firstDelayDuration;

	time_point		nextExecTime;

	ExecutionFrequency execFreqType {ExecutionFrequency::OneTimeExecution};

	thread			threadID;

public:
	ITask(TaskFuncPtr fnptr, time_point firstExecTime
		, time_duration	firstDelayDuration, ExecutionFrequency Tasktype);

	// NOTE: will try to join the task in DTOR ITask::~ITask(), then cleanup the memory
	virtual ~ITask() = 0;

	thread& GetThreadID()  {
		return threadID;
	}
	void SetThreadID(thread&& trId) {
		swap(threadID,trId);
	}

	//default func , execute the fnptr, supress if exception happens
	virtual void Execute();

	virtual void CalculateNextExecTime() {
		nextExecTime = firstExecTime + firstDelayDuration;
	}

	time_point GetNextExecTime() const{
		return nextExecTime;
	}
	//==================================================================
	void SetTaskStatus(TaskStatus aStatus)	{ // Scheduled or MarkedDeleted
		tStatus = aStatus;
	}

	TaskStatus GetTaskStatus() const{
		return tStatus;
	}

	void SetTaskID(TaskId aTaskId) {
		taskId = aTaskId;
	}
	TaskId GetTaskID() {
		return taskId;
	}

	ExecutionFrequency GetExecutionFrequency() const
	{
		return execFreqType;
	}

	friend bool operator < (const ITask& a, const ITask& b)
	{
		return a.nextExecTime < b.nextExecTime;
	}

	void PrintTask() const;
};

//===============================================================
struct TaskComparator
{
	bool operator () (ITask* a, ITask* b)
	{
		return (*a < *b);
	}
};

//===============================================================
class OneTimeTask : public ITask
{
public:
	OneTimeTask(TaskFuncPtr fnptr, time_point	firstExecTime, time_duration firstDelayDuration)
		: ITask(fnptr, firstExecTime, firstDelayDuration, ExecutionFrequency::OneTimeExecution)
	{}

	~OneTimeTask() = default;
};


//===============================================================
//===============================================================
class RepeatTask : public ITask
{
	time_duration	repeatTimeDuration;

public:
	RepeatTask(TaskFuncPtr fnptr, time_point firstExecTime
		, time_duration	firstDelayDuration, time_duration repeatTimeDuration)
		: ITask(fnptr, firstExecTime, firstDelayDuration, ExecutionFrequency::RepeatedExecution)
		, repeatTimeDuration(repeatTimeDuration)
	{
	}

	//default func , execute the fnptr, supress if exception happens
	//NOTE: also calculat the next execution time for the repeat task
	void Execute();

	void CalculateNextExecTime() {
		nextExecTime = std::chrono::system_clock::now() + firstDelayDuration;
	}

	~RepeatTask() = default;
};
//===============================================================

