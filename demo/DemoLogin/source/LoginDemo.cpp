
/*!
* \file LoginDemo.cpp
* \date 03-31-2016 17:54:01
*
*
* \author cs 04nycs@gmail.com
*/

#include "Windows.h"
#include <iostream>
#include "../../../lib-asyn-script/source/Function.h"

asys::FunctionCode* createAsysFunction()
{
	asys::FunctionCode* f{};

	auto platformLogin = f = new asys::FunctionCode();
	{
		f->ASSIGN("$device_id", asys::getInputVariableName(0));
		f->ASSIGN("$index", "0")_;

		f->EXPRESS([](asys::Executable* executable){
			auto index = executable->getValue("$index")->toInt();
			if (index < 5)
			{
				std::cout << "platform  login... " << "device_id " << executable->getValue("$device_id")->content() << std::endl;
				executable->setValue("$index", index + 1);
				return asys::RetCode::code_continue;
			}
			else
			{
				std::cout << "platform login succeeded " << "device_id " << executable->getValue("$device_id") << std::endl;
				executable->setValue("$index", index + 1);
				executable->setValue("$user_id", (executable->getValue("$device_id")->content() + "-user_id"));
				executable->setValue("$access_token", (executable->getValue("$device_id")->content() + "-acccessToken"));
				return asys::RetCode::code_done;
			}
		})_;
		
		f->RETURN({"true", "$user_id", "$access_token"})_;
	}

	auto loginServerLogin = f = new asys::FunctionCode();
	{
		f->ASSIGN("$user_id", asys::getInputVariableName(0));
		f->ASSIGN("$access_token", asys::getInputVariableName(1));
		f->ASSIGN("$index", "0")_;

		f->EXPRESS([](asys::Executable* executable){
			auto index = executable->getValue("$index")->toInt();
			if (index < 5)
			{
				std::cout << "login-server login... " << "$user_id " << executable->getValue("$user_id") << std::endl
					<< "$access_token"  << executable->getValue("$access_token")->content() << std::endl;
				executable->setValue("$index", index + 1);
				return asys::RetCode::code_continue;
			}
			else
			{
				std::cout << "login-server login succeeded " << std::endl;
				executable->setValue("$index", index + 1);
				executable->setValue("$session_id", "123456");
				return asys::RetCode::code_done;
			}
		})_;

		f->RETURN({ "true", "$session_id", "127.0.0.1:3697" })_;
	}

	auto gameServerLogin = f = new asys::FunctionCode();
	{
		f->ASSIGN("$user_id", asys::getInputVariableName(0))_;
		f->ASSIGN("$session_id", asys::getInputVariableName(1))_;
		f->ASSIGN("$gameserver_ip", asys::getInputVariableName(1))_;

		f->ASSIGN("$index", "0")_;

		f->EXPRESS([](asys::Executable* executable){
			auto index = executable->getValue("$index")->toInt();
			if (index < 5)
			{
				std::cout << "game-server login... " << "$user_id " << executable->getValue("$user_id")->content() << std::endl
					<< "$session_id " << executable->getValue("$session_id")->content() << std::endl;
				executable->setValue("$gameserver_ip", index + 1);
				executable->setValue("$index", index + 1);
				return asys::RetCode::code_continue;
			}
			else
			{
				std::cout << "game-server login succeeded " << std::endl;
				executable->setValue("$player_info", "player-123");
				return asys::RetCode::code_done;
			}
		})_;

		f->RETURN({ "true", "$player_info"})_;
	}

	auto mainFun = f = new asys::FunctionCode();
	{
		f->WHILE_NOT("$game_end")_
		{
			f->CALL({ "$success", "$user_id", "$access_token" }, { "device_id=test123" }, platformLogin)_;
			f->IF_NOT("$success")_
			{
				f->CONTINUE()_;
			}f->END_IF()_;

			f->CALL({ "$success", "$session_id", "$gameserver_ip" }, { "$user_id", "$access_token" }, loginServerLogin)_;
			f->IF_NOT("$success")_
			{
				f->CONTINUE()_;
			}f->END_IF()_;

			f->CALL({ "$success", "$player_info" }, { "$user_id", "$session_id", "$gameserver_ip" }, gameServerLogin)_;
			f->IF_NOT("$success")_
			{
				f->CONTINUE()_;
			}f->END_IF()_;

			f->EXPRESS([](asys::Executable* executable){
				std::cout << "login-success: " << executable->getValue("$player_info")->content() << std::endl;
				return asys::RetCode::code_done;
			})_;

			f->EXPRESS([](asys::Executable* executable){
				//return continue to indicate that next time executable is run, it will continue here.
				std::cout << executable->getValue("$player_info")->content() << " is playing the game." << std::endl;
				return asys::RetCode::code_continue;
			})_;

		}f->END_WHILE()_;
	}

	return mainFun;
}

bool update(asys::Executable* executable)
{
	auto retcode = executable->run();
	return (retcode == asys::RetCode::code_continue) ? true: false;
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
	time.wSecond += 3;        // Wait 3 sec

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


	auto fun = createAsysFunction();
	auto executable = fun->compile();
	delete fun; fun = nullptr;

	while (WaitForSingleObject(hTimer, 5000) == WAIT_OBJECT_0){    // Wait for timer event

		auto retCode = executable->run();
		if (retCode == asys::RetCode::code_done) break;
	}

	executable->release();

	CancelWaitableTimer(hTimer);    // Stop timer
	CloseHandle(hTimer);            // Delete handle

	return 0;
}