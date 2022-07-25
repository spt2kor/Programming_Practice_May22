#include "Tasks.h"
#include <iostream>
using namespace std;

// launch new thread, calculate next Exec time , insert next task into Scheduler
void ITask::Execute() {
	cout << "## starting ITask::Execute() " << endl;
	try
	{
		fnptr();
	}
	catch (exception& e)
	{
		cout << "## Exception Caught in ITask::Execute() , e= " << e.what() << endl;
	}
	catch (...)
	{
		cout << "## Unknown Exception Caught in ITask::Execute() " << endl;
	}
	cout << "## Closing ITask::Execute() " << endl;	
}



// launch new thread, calculate next Exec time , insert next task into Scheduler
void RepeatTask::Execute() {
	cout << "## starting RepeatTask::Execute() " << endl;
	ITask::Execute();
	CalculateNextExecTime();
	cout << "## Closing RepeatTask::Execute() " << endl;
}

