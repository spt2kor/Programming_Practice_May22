#include "TaskScheduler.h"
#include <iostream>
using namespace std;

TaskScheduler* TaskScheduler::m_instance = nullptr;
once_flag TaskScheduler::m_flag;

//=========================================================================
//Init TaskScheduler and launch the TaskScheduler thread
TaskScheduler::TaskScheduler() 
{
	scheduler_thread = thread(&TaskScheduler::LaunchSchedulerThread, this);
}

TaskScheduler::~TaskScheduler() {
	StopTaskScheduler();
}
//--------------------------------------------------------------------------
void TaskScheduler::Create_Scheduler() {
	m_instance = new TaskScheduler();
}

TaskScheduler& TaskScheduler::GetInstance()
{
	std::call_once(m_flag, &TaskScheduler::Create_Scheduler);
	return *m_instance;
}

//=========================================================================
//check if this task is MarkDelete( applicable for both OneTimeTask/RepeatTask), dont execute, and remove from taskSet
//check if this is OneTimeTask, then execute, and remove from taskSet
//check if this is RepeatTask, then execute, and InsertNewTask() with new time
// 
// entering task is either TaskStatus::MarkDelete or TaskStatus::Scheduled
void TaskScheduler::ExecuteTask(ITask* task)
{
	cout << "## starting ExecuteTask() " << endl;
	try
	{
		bool isMarkDelete = false;
		if (TaskStatus::MarkDelete == task->GetTaskStatus())
		{
			isMarkDelete = true;
		}
		else if(true == m_isSchedulerRunning )//TaskStatus::Scheduled
		{	
			task->SetTaskStatus(TaskStatus::Running);
			
			task->Execute();
			
			//NOTE: while above task getting executed, and if TaskScheduler::StopTaskScheduler() is called 
			//then return after execution is finished and let this thread joinable by main thread.
			if (false == m_isSchedulerRunning)
				return;

			if (task->GetExecutionFrequency() == ExecutionFrequency::OneTimeExecution)
			{
				task->SetTaskStatus(TaskStatus::MarkDelete);
				isMarkDelete = true;
			}
			else //if(task->GetExecutionFrequency() == ExecutionFrequency::RepeatedExecution)
			{
				//NOTE: new time is already got calculated during RepeatTask::Execute()
				InsertNewTask(task, true); 
			}
		}
		if (isMarkDelete)
		{
			{
				lock_guard<mutex> lg(setUpdateMutex);
				taskSet.erase(task);
			}
		}
	}
	catch (exception& e)
	{
		cout << "## Exception Caught in ExecuteTask() , e= " << e.what() << endl;
	}
	catch (...)
	{
		cout << "## Unknown Exception Caught in ExecuteTask() " << endl;
	}
	cout << "## Closing ExecuteTask() " << endl;

	task->SetThreadID(thread()); // release the thread handle from ITask.
	return;
}

//=========================================================================
//Thread to read aTask from TaskQueue, then launch Execution in new thread
void TaskScheduler::LaunchSchedulerThread()
{
	cout << "## starting LaunchSchedulerThread() " << endl;
	try
	{
		m_isSchedulerRunning.store(true);

		ITask* processTask = nullptr;
		time_point next_time_point;

		unique_lock<mutex> ulk(queueUpdateMutex, defer_lock);

		while (true == m_isSchedulerRunning)
		{
			processTask = nullptr;
			ulk.lock();
			if (taskQueue.empty()) // if taskQueue is empty wait for long time
				next_time_point = time_point::max();
			else   // if taskQueue is not empty, then take first top task and wait for its time_point
				next_time_point = taskQueue.top()->GetNextExecTime();

			//if timeout, means its time to execute taskQueue.top() task
			if (QueueUpdateCV.wait_until(ulk, next_time_point) == std::cv_status::timeout)
			{
				cout << "## LaunchSchedulerThread(), Wakeup Due to TimeOut, Processing TopTask now. " << endl;
				processTask = taskQueue.top();
				taskQueue.pop();

				cout << " ### LaunchSchedulerThread() start execution of current task :"<< processTask->GetTaskID().GetTaskId() << endl;

				processTask->SetThreadID(thread(&TaskScheduler::ExecuteTask, this, processTask));
			}
			else // may be new task got added(or surious wake), calculate the next_time_point again
				cout << "## LaunchSchedulerThread(), Wakeup without TimeOut." << endl;

			ulk.unlock();
		}
	}
	catch (exception& e)
	{
		cout << "## Exception Caught in LaunchSchedulerThread() , e= " <<e.what() << endl;
	}
	catch (...)
	{
		cout << "## Unknown Exception Caught in LaunchSchedulerThread() "  << endl;
	}
	cout << "## Closing LaunchSchedulerThread() " << endl;
}

//=========================================================================
//add one time task and return the handler wrapped inside TaskId
TaskId TaskScheduler::AddOneTimeTask(TaskFuncPtr fnptr
	, time_point firstExecTime
	, time_duration initialDelay)
{
	if (false == m_isSchedulerRunning)
		return TaskId();

	OneTimeTask* aTask = new OneTimeTask(fnptr, firstExecTime, initialDelay);
	auto tID = InsertNewTask(aTask);
	cout << " ### Added new One Time task, successfully tID = " << tID.GetTaskId() << endl;
	return tID;
}

//=========================================================================
//add Repeat task and return the handler wrapped inside TaskId
TaskId TaskScheduler::AddRepeatTask(TaskFuncPtr  fnptr
	, time_point firstExecTime
	, time_duration initialDelay
	, time_duration repeatTime)
{
	if (false == m_isSchedulerRunning)
		return TaskId();

	RepeatTask* aTask = new RepeatTask(fnptr, firstExecTime, initialDelay, repeatTime);
	auto tID = InsertNewTask(aTask);
	cout << " ### Added Repeat Time task, successfully tID = " << tID.GetTaskId() << endl;
	return tID;
}

//=========================================================================
//Add first time the New task(of type OneTimeTask/RepeatTask) / reschedul the Repeat Task to TaskQueue
TaskId TaskScheduler::InsertNewTask(ITask* task, bool repeatTask ) //, bool repeatTask = false
{
	TaskId tID(reinterpret_cast<uint64_t> (task));
	//if scheduler is stopped then dont insert new task.
	if (false == m_isSchedulerRunning)
	{
		cout << " ### Scehduler is stopped , unable to add new task tID = " << tID.GetTaskId() << endl;
		return tID;
	}

	if(!repeatTask) // if repeat task then dont add into taskSet
	{
		lock_guard<mutex> lg(setUpdateMutex);
		taskSet.insert(task);
	}

	task->SetTaskStatus(TaskStatus::Scheduled);
	//schedule/push this task into taskQueue
	{
		lock_guard<mutex> lg(queueUpdateMutex);
		taskQueue.push(task);
	}
	cout << " ### Added task to Queue, successfully tID = " << tID.GetTaskId() << endl;
	QueueUpdateCV.notify_all(); // notify to scheduler_thread

	return tID;
}

//=========================================================================
//validate the task, then mark the task for TaskStatus::MarkDelete
//at the time of its planned execution, (without executing the task) it will get deleted from taskQueue and TaskSet

bool TaskScheduler::StopATask(TaskId tId)
{
	if (tId) ////validate the task,
	{
		ITask* aTask = reinterpret_cast<ITask*> (tId.GetTaskId());
		{
			lock_guard<mutex> lg(setUpdateMutex);
			auto itr = taskSet.find(aTask);
			if (itr != taskSet.end())
			{
				cout << " ### StopATask() mark for delete successfully tID = " << tId.GetTaskId() << endl;
				aTask->SetTaskStatus(TaskStatus::MarkDelete);
				return true;
			}
		}
	}
	return false;
}

//=========================================================================
//remove task from taskQueue and taskSet
//empty the taskQueue and destroy it.
//wait for already running tasks to get joined, then delete tasks from taskSet
void TaskScheduler::ClearTasks()
{
	cout << " ### request to ClearTasks() " << endl;
	//empty the taskQueue and destroy it.
	{
		Task_Priority_Queue qToDelete;
		lock_guard<mutex> lg(queueUpdateMutex);
		taskQueue.swap(qToDelete);
	}
	//wait for already running tasks to get joined, then delete tasks from taskSet
	{
		lock_guard<mutex> lg(setUpdateMutex);
		for_each(taskSet.begin(), taskSet.end(), [](auto* task) {
				// NOTE: will try to join the task in DTOR ITask::~ITask(), then cleanup the memory
				delete task; 
				task = nullptr;
		});

		taskSet.clear();
	}
	cout << " ### ClearTasks() successfully "<< endl;
}

//=========================================================================
//mark m_isSchedulerRunning=false, and wait for scheduler_thread to join and clean the taskQueue and TaskSet
void TaskScheduler::StopTaskScheduler()
{
	cout << " ### request to StopTaskScheduler() " << endl;
	m_isSchedulerRunning.store(false);
	QueueUpdateCV.notify_all(); // can be notify_one
	ClearTasks();

	if (scheduler_thread.joinable())
	{
		cout << " ### StopTaskScheduler() , waiting to join the scheduler_thread" << endl;
		scheduler_thread.join();
	}
	cout << " ### StopTaskScheduler() successfully " << endl;
}

//distroy the instance explicitly, and free the memory
void TaskScheduler::DistroyInstance()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

//=========================================================================