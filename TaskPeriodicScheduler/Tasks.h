#pragma once
#pragma once
#include "CommonDef.h"


class ITask
{
protected: 
	TaskStatus		tStatus;

	TaskId			tId;

	TaskFuncPtr		fnptr;
	time_point		firstExecTime;
	time_duration	firstDelayDuration;
	time_duration	repeatTimeDuration;

	time_point		nextExecTime;

public:
	virtual void Execute() = 0;
	ITask	(	TaskFuncPtr		fnptr
			, time_point		firstExecTime
			, time_duration		firstDelayDuration
			, time_duration		repeatTimeDuration);

}

class OneTimeTask : public ITask
{
	int ExecTimeInSec;
	TaskId OneTimeTask(FuncPtr  fnptr, int initialDelayInSec, int ExecTimeInSec);
}

class RepeatTaskWithInitDelay : public ITask
{
	int periodicExecTimeInSec;
}

