#include "TaskScheduler.h"
#include <iostream>
using namespace std;

TaskScheduler* TaskScheduler::m_instance = nullptr;
once_flag TaskScheduler::m_flag;

//=========================================================================
//Init TaskScheduler and launch the TaskScheduler thread
TaskScheduler::TaskScheduler()  
{
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
	cout << "## starting ExecuteTask(): GetTaskIntID =  "<< task->GetTaskIntID() << endl;
	try
	{
		if(true == m_isSchedulerRunning && (TaskStatus::MarkDelete != task->GetTaskStatus()))//TaskStatus::Scheduled
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
			}
			else //if(task->GetExecutionFrequency() == ExecutionFrequency::RepeatedExecution)
			{
				//NOTE: new time is already got calculated during RepeatTask::Execute()
				InsertNewTask(task, true); 
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
	cout << "## Closing ExecuteTask() GetTaskIntID =  " << task->GetTaskIntID() << endl;

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

				//Note: remove the lock on the queue, sothat we dont hold onto both(mapUpdateMutex) the lock at same time 
				ulk.unlock();

				if (taskToThreadMap[processTask].joinable())
					taskToThreadMap[processTask].join();

				cout << " ### LaunchSchedulerThread() start execution of current task :"<< processTask->GetTaskIntID() << endl;
				if (TaskStatus::MarkDelete != processTask->GetTaskStatus())
				{	
					lock_guard<mutex> lg(mapUpdateMutex);
					auto newThread = thread(&TaskScheduler::ExecuteTask, this, processTask);
					taskToThreadMap[processTask] = move(newThread);
				}
				else 
				{
					lock_guard<mutex> lg(mapUpdateMutex);
					taskToThreadMap.erase(processTask);
				}
			}
			else // may be new task got added(or surious wake), calculate the next_time_point again
			{
				ulk.unlock();
				cout << "\n## TaskScheduler::LaunchSchedulerThread(), Wakeup without TimeOut." << endl;
			}

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
TaskId TaskScheduler::AddOneTimeTask(int taskIntID
	, TaskFuncPtr fnptr
	, time_point firstExecTime
	, time_duration initialDelay)
{
	if (false == m_isSchedulerRunning)
		return TaskId();

	OneTimeTask* aTask = new OneTimeTask(taskIntID, fnptr, firstExecTime, initialDelay);
	auto tID = InsertNewTask(aTask);
	cout << " ### TaskScheduler::AddOneTimeTask() - Added new One Time task, successfully tID = " << taskIntID << endl;
	return tID;
}

//=========================================================================
//add Repeat task and return the handler wrapped inside TaskId
TaskId TaskScheduler::AddRepeatTask(int taskIntID
	, TaskFuncPtr  fnptr
	, time_point firstExecTime
	, time_duration initialDelay
	, time_duration repeatTime)
{
	if (false == m_isSchedulerRunning)
		return TaskId();

	RepeatTask* aTask = new RepeatTask(taskIntID, fnptr, firstExecTime, initialDelay, repeatTime);
	auto tID = InsertNewTask(aTask);
	cout << " ### TaskScheduler::AddRepeatTask() - Added Repeat Time task, successfully tID = " << taskIntID << endl;
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
		cout << " ### TaskScheduler::InsertNewTask()- Scehduler is stopped , unable to add new task tID = " << task->GetTaskIntID() << endl;
		return tID;
	}

	if(!repeatTask) // if repeat task then dont add into taskSet
	{
		lock_guard<mutex> lg(mapUpdateMutex);
		taskToThreadMap.emplace(  task , thread()  );
	}

	task->SetTaskStatus(TaskStatus::Scheduled);
	//schedule/push this task into taskQueue
	{
		lock_guard<mutex> lg(queueUpdateMutex);
		taskQueue.push(task);
	}
	cout << " ### ### TaskScheduler::InsertNewTask() - Added task to Queue, successfully tID = " << task->GetTaskIntID() << endl;
	QueueUpdateCV.notify_all(); // notify to scheduler_thread

	return tID;
}

//=========================================================================
//validate the task, then mark the task for TaskStatus::MarkDelete
//at the time of its planned execution, (without executing the task) it will get deleted from taskQueue and TaskSet

bool TaskScheduler::StopATask(TaskId tId)
{
	cout << " ### TaskScheduler::StopATask() requested  tId = " << tId.GetTaskId() << endl;
	if (tId) ////validate the task,
	{
		ITask* aTask = reinterpret_cast<ITask*> (tId.GetTaskId());
		cout << " ### TaskScheduler::StopATask() requested  aTask = " << aTask << endl;
		{
			lock_guard<mutex> lg(mapUpdateMutex);
			auto itr = taskToThreadMap.find(aTask);
			if (itr != taskToThreadMap.end())
			{
				cout << " ### TaskScheduler::StopATask() mark for delete successfully tID = " << aTask->GetTaskIntID() << endl;
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
	cout << " ### request to TaskScheduler::ClearTasks() " << endl;
	//empty the taskQueue and destroy it.
	{
		Task_Priority_Queue qToDelete;
		lock_guard<mutex> lg(queueUpdateMutex);
		taskQueue.swap(qToDelete);
	}
	//wait for already running tasks to get joined, then delete tasks from taskSet
	{
		lock_guard<mutex> lg(mapUpdateMutex);
		for(auto itr = taskToThreadMap.begin(); itr != taskToThreadMap.end(); ++itr ) 
		{
				// NOTE: will try to join the task in DTOR ITask::~ITask(), then cleanup the memory
				if (itr->second.joinable())
					itr->second.join();

				itr->second = move( thread());
				delete itr->first;
		};
		
		taskToThreadMap.clear();
	}
	cout << " ### TaskScheduler::ClearTasks() successfully "<< endl;
}

//=========================================================================
//mark m_isSchedulerRunning=false, and wait for scheduler_thread to join and clean the taskQueue and TaskSet
void TaskScheduler::StopTaskScheduler()
{
	cout << " ### request to TaskScheduler::StopTaskScheduler() " << endl;
	m_isSchedulerRunning.store(false);
	QueueUpdateCV.notify_all(); // can be notify_one
	ClearTasks();

	if (scheduler_thread.joinable())
	{
		cout << " ### TaskScheduler::StopTaskScheduler() , waiting to join the scheduler_thread" << endl;
		scheduler_thread.join();
	}
	cout << " ### TaskScheduler::StopTaskScheduler() successfully " << endl;
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

//mark m_isSchedulerRunning=true, clean the previous data, and start new thread
void TaskScheduler::StartTaskScheduler()
{
	cout << " ### request to TaskScheduler::StartTaskScheduler() " << endl;
	int stopCount=5;
	while (true == m_isSchedulerRunning && stopCount>0) {
		cout << " ### TaskScheduler::StartTaskScheduler() - ERROR Old scheduler thread is already running, trying to stop now :tryCount = "<< (5- stopCount +1)<< endl;
		StopTaskScheduler();
	}

	auto newThread = thread(&TaskScheduler::LaunchSchedulerThread, this);
	swap(scheduler_thread, newThread);
}

//=========================================================================