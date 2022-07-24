#pragma once
#include "CommonDef.h"
#include <atomic>
#include <mutex>
#include <cassert>  
#include <queue>
#include <unordered_set>
#include "Tasks.h"

class TaskScheduler
{
	static TaskScheduler* m_instance;
	static once_flag m_flag;
	//------------------------------------------

	atomic<bool> m_isSchedulerRunning{ false };

	mutex queueUpdateMutex;
	std::condition_variable QueueUpdateCV;
	priority_queue< ITask*, vector< ITask* >, TaskComparator> taskQueue;

	mutex setUpdateMutex;
	unordered_set< ITask*> taskSet;
	//==================================================================
	void ClearTasks();
	TaskScheduler() { 	}

	~TaskScheduler()	{
		ClearTasks();
	}

	void Create_Scheduler()	{
		m_instance = new TaskScheduler();
	}
	//==================================================================

	void StartScheduler();

	void InsertNewTask(ITask* task);

public:
	TaskScheduler(const TaskScheduler&) = delete;
	TaskScheduler(TaskScheduler&&) = delete;
	TaskScheduler& operator= (const TaskScheduler&) = delete;
	TaskScheduler& operator= (TaskScheduler&&) = delete;

	//==================================================================
	static TaskScheduler& GetInstance()
	{
		std::call_once(m_flag, &TaskScheduler::Create_Scheduler);
	}
	//==================================================================
	
	TaskId AddOneTimeTask( TaskFuncPtr  fnptr
						 , time_point firstExecTime
						 , time_duration initialDelay);
		
	TaskId AddRepeatTask( TaskFuncPtr  fnptr
						, time_point firstExecTime 
						, time_duration initialDelay 
						, time_duration repeatTime);


	bool StopATask(TaskId tId);
	//==================================================================


	void StopTaskScheduler()
	{
		m_isSchedulerRunning.store ( false);
		ClearTasks();
	}
	//==================================================================
};


