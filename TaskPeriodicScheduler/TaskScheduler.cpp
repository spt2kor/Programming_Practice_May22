#include "TaskScheduler.h"

static TaskScheduler::TaskScheduler* m_instance{nullptr};
static TaskScheduler::once_flag m_flag;


TaskId TaskScheduler::AddOneTimeTask(TaskFuncPtr  fnptr
	, time_point firstExecTime
	, time_duration initialDelay)
{
	static_assert(false, " need to implement this function");
}

TaskId TaskScheduler::AddRepeatTask(TaskFuncPtr  fnptr
	, time_point firstExecTime
	, time_duration initialDelay
	, time_duration repeatTime)
{
	static_assert(false, " need to implement this function");
}

bool TaskScheduler::StopATask(TaskId tId)
{
	static_assert(false, " need to implement this function");
}


void Clear()
{
	static_assert(false , " need to implement this function");
}