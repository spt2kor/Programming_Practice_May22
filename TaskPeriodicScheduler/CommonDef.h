#pragma once
#include <cstdint>
#include <cstdint>
#include <chrono>

using namespace std;

typedef std::chrono::system_clock::time_point		time_point;
typedef std::chrono::system_clock::duration			time_duration;

typedef void ( * TaskFuncPtr) (void);
//------------------------------------

struct TaskId
{
	uint64_t taskID{0};
	TaskId()
	{}
	TaskId(uint64_t id) :taskID(id)
	{}
	uint64_t GetTaskId()	{
		return taskID;
	}
};

//------------------------------------

enum class ExecutionFrequency
{
	OneTimeExecution,
	RepeatedExecution,
};
//------------------------------------
enum class TaskStatus
{
	Created,
	Scheduled,
	MarkDeleted,
};
//------------------------------------
