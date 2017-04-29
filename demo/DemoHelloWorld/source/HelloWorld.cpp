/*!
 * \file HelloWorld.cpp
 * \date 04-29-2017 13:18:59
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#include "Windows.h"
#include <iostream>
#include "../../../lib-asyn-script/source/AsynScript.h"

asys_reg_funs;

//The "Hello World!" string will be printed by one char per "frame",
//which demonstrates a core feature of asyn-script and the usage of
//the asys_redo keyword.
//In this demo, the virtual machine is invoked once per second, so
//it will take one second to output a character to the console. 
asys::FunctionCode* helloWorld()
{
	BEGIN_FUN(){
		ASYS_VAR(std::string, str);
		_CC{
			str = "Hello World!";
		}CC_;

		ASYS_VAR(size_t, index);
		_CC{
			if (index < str.r().length())
			{
				std::cout << str.r()[index];
				index = index + 1;
				asys_redo;
			}
		}CC_;
	}END_FUN;
}

int main()
{
	int count = 0;    // Iteration counter

	HANDLE hTimer = NULL;    // WaitableTimer handle
	hTimer = CreateWaitableTimer(    // Create waitable timer
		NULL,
		FALSE,    // Autoreset --> timer restarts counting after event fired
		NULL);

	SYSTEMTIME time;        // System time structure
	GetSystemTime(&time);    // Curren time
	time.wSecond += 1;     

	FILETIME ftime;    // File time (expected by SetWaitableTimer)
	SystemTimeToFileTime(&time, &ftime);    // Convert system to file time

	if (!SetWaitableTimer(    // Set waitable timer
		hTimer,    // Timer handle
		reinterpret_cast<LARGE_INTEGER*>(&ftime),    // Convert file time to large integer
		1000,    // Period time, fire again after 2 sec
		NULL,
		NULL,
		0))
	{
		std::cout << "SetWaitableTimer failed" << GetLastError() << std::endl;
	};


	asys::Machine machine;
	machine.installCode(helloWorld());

	while (WaitForSingleObject(hTimer, 5000) == WAIT_OBJECT_0){    // Wait for timer event
		if (machine.run() != asys::CodeFlow::redo_) break;
	}

	CancelWaitableTimer(hTimer);    // Stop timer
	CloseHandle(hTimer);            // Delete handle

	return 0;
}