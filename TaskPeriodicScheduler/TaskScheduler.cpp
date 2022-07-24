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
	if (false == m_isSchedulerRunning)
		return TaskId();

	OneTimeTask* aTask = new OneTimeTask(fnptr, firstExecTime, initialDelay);
	return  InsertNewTask(aTask);
}

//=========================================================================
TaskId TaskScheduler::AddRepeatTask(TaskFuncPtr  fnptr
	, time_point firstExecTime
	, time_duration initialDelay
	, time_duration repeatTime)
{
	if (false == m_isSchedulerRunning)
		return TaskId();

	RepeatTask* aTask = new RepeatTask(fnptr, firstExecTime, initialDelay, repeatTime);
	return  InsertNewTask(aTask);
}

//=========================================================================
TaskId TaskScheduler::InsertNewTask(ITask* task)
{
	TaskId tID(reinterpret_cast<uint64_t> (task));

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

	return tID;
}

//=========================================================================
bool TaskScheduler::StopATask(TaskId tId)
{
	bool isValidTask = false;
	if ( ! tId )
		return isValidTask;

	ITask *aTask =  reinterpret_cast<ITask*> (tId.GetTaskId());
	{
		lock_guard<mutex> lg(setUpdateMutex);
		auto itr = taskSet.find(aTask);
		if ( itr != taskSet.end())
		{
			taskSet.erase(itr);
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