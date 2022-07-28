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
	//---------------var to make this class singleton---------------------------
	static TaskScheduler* m_instance;
	static once_flag m_flag;

	//-------------------var required to Run the scheduler_thread -----------------------
	thread scheduler_thread;
	atomic<bool> m_isSchedulerRunning{ false };

	//-------------------var to store tasks in TaskSet and schedule execution in taskQueue-----------------------
	typedef priority_queue< ITask*, vector< ITask* >, TaskComparator> Task_Priority_Queue;
	mutex queueUpdateMutex;
	std::condition_variable QueueUpdateCV;	
	Task_Priority_Queue taskQueue;

	mutex setUpdateMutex;
	unordered_set< ITask*> taskSet;
	//==================================================================
	//remove task from taskQueue and taskSet
	//empty the taskQueue and destroy it.
	//wait for already running tasks to get joined, then delete tasks from taskSet
	void ClearTasks();

	//Init TaskScheduler and launch scheduler_thread = LaunchSchedulerThread() 
	TaskScheduler(); 

	//do cleanup by StopTaskScheduler()
	~TaskScheduler(); 

	//create new TaskScheduler singleton object
	static void Create_Scheduler();
	//==================================================================
	//Thread to read aTask from TaskQueue, then launch Execution in new thread
	void LaunchSchedulerThread();

	//Add first time the New task(of type OneTimeTask/RepeatTask) / reschedul the Repeat Task to TaskQueue
	TaskId InsertNewTask(ITask* task, bool repeatTask = false);

	//check if this task is MarkDelete( applicable for both OneTimeTask/RepeatTask), dont execute, and remove from taskSet
	//check if this is OneTimeTask, then execute, and remove from taskSet
	//check if this is RepeatTask, then execute, and InsertNewTask() with new time
	void ExecuteTask(ITask* task);

public:

	//because its a singlton class
	TaskScheduler(const TaskScheduler&) = delete;
	TaskScheduler(TaskScheduler&&) = delete;
	TaskScheduler& operator= (const TaskScheduler&) = delete;
	TaskScheduler& operator= (TaskScheduler&&) = delete;

	//==================================================================
	//Get me the singlton Instance
	static TaskScheduler& GetInstance();

	//distroy the m_instance explicitly, and free the memory
	static void DistroyInstance();

	//==================================================================
	//add one time task and return the handler wrapped inside TaskId
	TaskId AddOneTimeTask( int taskIntID
						 , TaskFuncPtr  fnptr
						 , time_point firstExecTime
						 , time_duration initialDelay);
	
	//add Repeat task and return the handler wrapped inside TaskId
	TaskId AddRepeatTask(int taskIntID
						, TaskFuncPtr  fnptr
						, time_point firstExecTime 
						, time_duration initialDelay 
						, time_duration repeatTime);

	//validate the task, then mark the task for TaskStatus::MarkDelete
	//at the time of its planned execution, (without executing the task) it will get deleted from taskQueue and TaskSet
	bool StopATask(TaskId tId);
	//==================================================================

	//mark m_isSchedulerRunning=false, and wait for scheduler_thread to join and clean the taskQueue and TaskSet
	void StopTaskScheduler();
	//==================================================================
	//mark m_isSchedulerRunning=true, clean the previous data, and start new thread
	void StartTaskScheduler();
	//==================================================================

};


//=================================================================================
void OneTimeTaskFunc();


void RepeatTaskFunc();
//=================================================================================
