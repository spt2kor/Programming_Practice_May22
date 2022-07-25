#include "TaskScheduler.h"
#include <iostream>
using namespace std;

TaskScheduler* TaskScheduler::m_instance = nullptr;
once_flag TaskScheduler::m_flag;


//=========================================================================
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
			
			//NOTE: while above task getting executed, and TaskScheduler::StopTaskScheduler() is called 
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
				InsertNewTask(task); //new time is already got calculated during RepeatTask::Execute()
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
			if (taskQueue.empty())
				next_time_point = time_point::max();
			else
				next_time_point = taskQueue.top()->GetNextExecTime();

			if (QueueUpdateCV.wait_until(ulk, next_time_point) == std::cv_status::timeout)
			{
				cout << "## LaunchSchedulerThread(), Wakeup Due to TimeOut, Processing TopTask now. " << endl;
				processTask = taskQueue.top();
				taskQueue.pop();
				processTask->SetThreadID(thread(&TaskScheduler::ExecuteTask, this, processTask));
			}
			else // may be new task got added, calculate the next_time_point again
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
	if (tId)
	{
		ITask* aTask = reinterpret_cast<ITask*> (tId.GetTaskId());
		{
			lock_guard<mutex> lg(setUpdateMutex);
			auto itr = taskSet.find(aTask);
			if (itr != taskSet.end())
			{
				aTask->SetTaskStatus(TaskStatus::MarkDelete);
				return true;
			}
		}
	}
	return false;
}


//=========================================================================
void TaskScheduler::ClearTasks()
{
	{
		Task_Priority_Queue qToDelete;
		lock_guard<mutex> lg(queueUpdateMutex);
		taskQueue.swap(qToDelete);
	}
	{
		lock_guard<mutex> lg(setUpdateMutex);
		for_each(taskSet.begin(), taskSet.end(), [](auto*& task) {
			if (TaskStatus::Running == task.GetTaskStatus())
			{
				auto& threadId = task.GetThreadID();
				threadId.join();

				delete task; 
				task = nullptr;
			}
		});

		taskSet.clear();
	}
}

//=========================================================================
void TaskScheduler::StopTaskScheduler()
{
	m_isSchedulerRunning.store(false);
	QueueUpdateCV.notify_all(); // can be notify_one
	ClearTasks();
	scheduler_thread.join();
}
//=========================================================================