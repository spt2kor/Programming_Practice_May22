#pragma once

#include "CommonDef.h"
#include <thread>

//================================================================================
class ITask
{
protected:
	TaskStatus		tStatus{ TaskStatus::Created };

	TaskId			tId{ 0 };

	TaskFuncPtr		fnptr;
	time_point		firstExecTime;
	time_duration	firstDelayDuration;

	time_point		nextExecTime;

	ExecutionFrequency execFreqType {ExecutionFrequency::OneTimeExecution};

	thread			threadID;
public:
	ITask(TaskFuncPtr fnptr, time_point firstExecTime
		, time_duration	firstDelayDuration, ExecutionFrequency Tasktype)
		: fnptr(fnptr), firstExecTime(firstExecTime)
		, firstDelayDuration(firstDelayDuration), nextExecTime(firstExecTime)
		, execFreqType(Tasktype)
	{
		CalculateNextExecTime();
	}

	virtual ~ITask() = 0
	{	}

	thread& GetThreadID() {
		return threadID;
	}
	void SetThreadID(thread&& trId) {
		swap(threadID,trId);
	}

	virtual void Execute();

	virtual void CalculateNextExecTime() {
		nextExecTime = firstExecTime + firstDelayDuration;
	}

	time_point GetNextExecTime() {
		return nextExecTime;
	}
	//==================================================================
	void SetTaskStatus(TaskStatus aStatus)	{ // Scheduled or MarkedDeleted
		tStatus = aStatus;
	}

	TaskStatus GetTaskStatus() {
		return tStatus;
	}

	void SetTaskID(TaskId aTaskId) {
		tId = aTaskId;
	}

	ExecutionFrequency GetExecutionFrequency()
	{
		return execFreqType;
	}

	friend bool operator < (const ITask& a, const ITask& b)
	{
		return a.nextExecTime < b.nextExecTime;
	}
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

	void Execute();

	void CalculateNextExecTime() {
		nextExecTime = std::chrono::system_clock::now() + firstDelayDuration;
	}

	~RepeatTask() = default;
};


//===============================================================

