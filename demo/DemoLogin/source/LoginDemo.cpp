﻿
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

class Game
{
public:
	Game(){}
	virtual ~Game(){}

	bool run(const std::string& deviceId)
	{
		if (!m_pExecutable)
		{
			m_pExecutable = login()->compile();
			m_pExecutable->setInputValue(0, deviceId);
		}

		auto retCode = m_pExecutable->run();
		return retCode == asys::CodeFlow::retry;
	}

private:
	//ret success user_id access_token
	asys::FunctionCode* platformLogin(ASYS_PARAM(device_id))
	{
		auto& f = m_asynFunctions[__FUNCTION__];
		if (f) return f;
		
		f = new asys::FunctionCode;
		{
			f->INPUT({device_id})_;

			ASYS_VAR(index);
			f->ASSIGN(index, "0")_;

			ASYS_VAR(user_id);
			ASYS_VAR(access_token);

			f->EXPRESS([index, user_id, access_token, device_id](asys::Executable* executable){
				asys_value(index);
				asys_value(user_id);
				asys_value(access_token);
				asys_value(device_id);

				if (index.toInt() < 5)
				{
					std::cout << "user-server  login... " << "device_id=" << device_id.toString() << std::endl;
					index = index.toInt() + 1;
					return asys::CodeFlow::retry;
				}
				else
				{
					std::cout << "platform login succeeded " << "device_id=" << device_id.toString() << std::endl;
					index = index.toInt() + 1;
					user_id = "user_172034";
					access_token = "6534B029C4FA65";
					return asys::CodeFlow::next;
				}
			})_;

			f->RETURN({ asys::True, user_id, access_token })_;
		}

		return f;
	}

	//ret success session_id gameserver_ip
	asys::FunctionCode* userServerLogin(ASYS_PARAM(user_id), ASYS_PARAM(access_token))
	{
		auto& f = m_asynFunctions[__FUNCTION__];
		if (f) return f;

		f = new asys::FunctionCode;
		{
			f->INPUT({user_id, access_token})_;

			ASYS_VAR(index);
			f->ASSIGN(index, "0")_;

			ASYS_VAR(session_id);
			f->EXPRESS([index, session_id, user_id, access_token](asys::Executable* executable){
				asys_value(index);
				asys_value(session_id);
				asys_value(user_id);
				asys_value(access_token);

				if (index.toInt() < 5)
				{
					std::cout << "login-server login... " << "$user_id=" << user_id.toString() << std::endl
						<< "$access_token=" << access_token.toString() << std::endl;
					index = index.toInt() + 1;
					return asys::CodeFlow::retry;
				}
				else
				{
					std::cout << "login-server login succeeded " << std::endl;
					index = index.toInt() + 1;
					session_id = "123456";
					return asys::CodeFlow::next;
				}
			})_;

			f->RETURN({ asys::True, session_id, "127.0.0.1:3697" })_;
		}

		return f;
	}

	//ret success player_info
	asys::FunctionCode* gameServerLogin(ASYS_PARAM(user_id), ASYS_PARAM(session_id), ASYS_PARAM(gameserver_ip))
	{
		auto& f = m_asynFunctions[__FUNCTION__];
		if (f) return f;

		f = new asys::FunctionCode;
		{
			f->INPUT({ user_id, session_id, gameserver_ip })_;

			ASYS_VAR(index);
			f->ASSIGN(index, "0")_;

			ASYS_VAR(player_info);
			f->EXPRESS([index, user_id, session_id, player_info](asys::Executable* executable){
				asys_value(index);
				asys_value(user_id);
				asys_value(session_id);
				asys_value(player_info);

				if (index.toInt() < 5)
				{
					std::cout << "game-server login... " << "$user_id=" << user_id.toString() << std::endl
						<< "$session_id=" << session_id.toString() << std::endl;
					index = index.toInt() + 1;
					return asys::CodeFlow::retry;
				}
				else
				{
					std::cout << "game-server login succeeded " << std::endl;
					player_info = "player-123";
					return asys::CodeFlow::next;
				}
			})_;

			f->RETURN({ asys::True, player_info })_;
		}

		return f;
	}

	//ret success player_info
	asys::FunctionCode* login(ASYS_PARAM(device_id))
	{
		auto& f = m_asynFunctions[__FUNCTION__];
		if (f) return f;

		f = new asys::FunctionCode;
		{
			f->INPUT({ device_id })_;

			ASYS_VAR(game_end);
			f->WHILE_NOT(game_end)_
			{
				ASYS_VAR(success);
				ASYS_VAR(user_id);
				ASYS_VAR(access_token);
				f->CALL({ success, user_id, access_token }, { device_id }, platformLogin())_;

				f->IF_NOT(success)_
				{
					f->CONTINUE()_;
				}f->END_IF()_;

				ASYS_VAR(session_id);
				ASYS_VAR(gameserver_ip);
				f->CALL({ success, session_id, gameserver_ip }, { user_id, access_token }, userServerLogin())_;

				f->IF_NOT(success)_
				{
					f->CONTINUE()_;
				}f->END_IF()_;

				ASYS_VAR(player_info);
				f->CALL({ success, player_info }, { user_id, session_id, gameserver_ip }, gameServerLogin())_;

				f->IF_NOT(success)_
				{
					f->CONTINUE()_;
				}f->END_IF()_;

				f->EXPRESS([player_info](asys::Executable* executable){
					asys_value(player_info);
					std::cout << "login-success: " << player_info.toString() << std::endl;
					return asys::CodeFlow::next;
				})_;

				f->EXPRESS([player_info](asys::Executable* executable){
					//return continue to indicate that next time executable is run, it will continue here.
					asys_value(player_info);
					std::cout << player_info.toString() << " is playing the game." << std::endl;
					return asys::CodeFlow::retry;
				})_;

			}f->END_WHILE()_;
		}

		return f;
	}

private:
	asys::FunctionMap m_asynFunctions;
	asys::Executable* m_pExecutable{};
};

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


	Game game;
	std::cout << "Start game" << GetLastError() << std::endl;
	while (WaitForSingleObject(hTimer, 5000) == WAIT_OBJECT_0){    // Wait for timer event
		if (!game.run("SAMSUM_NT7100__123")) break;;
	}

	CancelWaitableTimer(hTimer);    // Stop timer
	CloseHandle(hTimer);            // Delete handle

	return 0;
}