#pragma once
#include "CommonDef.h"
#include <atomic>
#include <mutex>
#include <cassert>  

class TaskScheduler
{
	atomic<bool> m_isSchedulerRunning {true};

	static TaskScheduler* m_instance;
	static once_flag m_flag;

	TaskScheduler() { 	}

	~TaskScheduler()	{
		Clear();
	}

	void Create_Scheduler()	{
		m_instance = new TaskScheduler();
	}
	//==================================================================

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
		Clear();
	}
	//==================================================================
};


