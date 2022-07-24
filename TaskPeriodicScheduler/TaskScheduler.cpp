#include "TaskScheduler.h"

static TaskScheduler::TaskScheduler* m_instance{nullptr};
static TaskScheduler::once_flag m_flag;

//=========================================================================
void TaskScheduler::StartScheduler()
{
	static_assert(false, " need to implement this function");
}


//=========================================================================
TaskId TaskScheduler::AddOneTimeTask(TaskFuncPtr fnptr
	, time_point firstExecTime
	, time_duration initialDelay)
{
	TaskId dummy;
	if (m_isSchedulerRunning == true)
	{
		OneTimeTask* aTask = new OneTimeTask(fnptr, firstExecTime, initialDelay);

		TaskId tID(reinterpret_cast<uint64_t> (aTask));

		InsertNewTask(aTask);

		return tID;
	}

	return dummy;
}

//=========================================================================
TaskId TaskScheduler::AddRepeatTask(TaskFuncPtr  fnptr
	, time_point firstExecTime
	, time_duration initialDelay
	, time_duration repeatTime)
{
	RepeatTask* aTask = new RepeatTask(fnptr, firstExecTime, initialDelay, repeatTime);

	TaskId tID(reinterpret_cast<uint64_t> (aTask));

	InsertNewTask(aTask);

	return tID;
}


//=========================================================================
void TaskScheduler::InsertNewTask(ITask* task)
{
	{
		lock_guard<mutex> lg(setUpdateMutex);
		taskSet.insert(task);
	}
	
	task->SetTaskStatus(TaskStatus::Scheduled);

	{
		lock_guard<mutex> lg(queueUpdateMutex);
		taskQueue.push(task);
	}

	QueueUpdateCV.notify_all(); // can be notify_one
}

//=========================================================================
bool TaskScheduler::StopATask(TaskId tId)
{
	ITask *aTask =  reinterpret_cast<ITask*> (tId.GetTaskId());
	bool isValidTask = false;
	{
		lock_guard<mutex> lg(setUpdateMutex);
		if (taskSet.find(aTask) != taskSet.end())
		{
			isValidTask = true;
		}
	}
	if (isValidTask)
	{
		aTask->SetTaskStatus(TaskStatus::MarkDeleted);
	}
	return isValidTask;
}


//=========================================================================
void ClearTasks()
{
	static_assert(false , " need to implement this function");
}

//=========================================================================