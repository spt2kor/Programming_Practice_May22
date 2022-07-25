#pragma once
#include <cstdint>
#include <cstdint>
#include <chrono>

using namespace std;
typedef std::chrono::system_clock		system_clock;
typedef system_clock::time_point		time_point;
typedef system_clock::duration			time_duration;

typedef void ( * TaskFuncPtr) (void);
//------------------------------------

struct TaskId
{
	uint64_t taskID{0};
	TaskId()
	{}
	TaskId(uint64_t id) :taskID(id)
	{}
	uint64_t GetTaskId() const {
		return taskID;
	}
	 operator bool() const{
		return 0 != taskID;
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
	Running,
	MarkDelete
};
//------------------------------------
