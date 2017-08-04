
/*!
* \file LoginDemo.cpp
* \date 03-31-2016 17:54:01
*
*
* \author cs 04nycs@gmail.com
*/

#include "Windows.h"
#include <iostream>
#include "../../../lib-asyn-script/source/AsynScript.h"

class Game
{
public:
	Game(){}
	virtual ~Game(){}

	bool run(const std::string& deviceId)
	{
		if (!m_bStarted)
		{
			m_machine.installCode(login(), { deviceId });
			m_bStarted = true;
		}

		auto retCode = m_machine.run();
		return retCode == asys::CodeFlow::redo_;
	}

private:
	//ret success user_id access_token
	asys::FunctionCode* platformLogin(ASYS_P(std::string, device_id))
	{
		BEGIN_FUN(device_id){
			ASYS_VAR(int, index);
			ASSIGN(index, CC(int, 0));

			ASYS_VAR(std::string, user_id);
			ASYS_VAR(std::string, access_token);

			_CC{
				if (index < 5)
				{
					std::cout << "user-server  login... " <<
						"device_id=" << device_id.r() << std::endl;
					index++;
					asys_redo;
				}
				else
				{
					std::cout << "platform login succeeded " <<
						"device_id=" << device_id.r() << std::endl;
					index++;
					user_id = "user_172034";
					access_token = "6534B029C4FA65";
				}
			}CC_;
			RETURN(CC(bool, true), user_id, access_token);
		}END_FUN;
	}

	//ret success session_id gameserver_ip
	asys::FunctionCode* userServerLogin(ASYS_P(std::string, user_id), ASYS_P(std::string, access_token))
	{
		BEGIN_FUN(user_id, access_token){
			ASYS_VAR(int, index);
			ASSIGN(index, CC(int, 0));

			ASYS_VAR(std::string, session_id);
			_CC{
				if (index < 5)
				{
					std::cout << "login-server login... "
						<< "$user_id=" << user_id.r() << std::endl
						<< "$access_token=" << access_token.r() << std::endl;
					index++;
					asys_redo;
				}
				else
				{
					std::cout << "login-server login succeeded "
						<< std::endl;
					index++;
					session_id = "123456";
				}
			}CC_;
			RETURN(CC(bool, true), session_id, CC(std::string, "127.0.0.1:3697"));
		}END_FUN;
	}

	//ret success player_info
	asys::FunctionCode* gameServerLogin(ASYS_P(std::string, user_id), ASYS_P(std::string, session_id), ASYS_P(std::string, gameserver_ip))
	{
		BEGIN_FUN(user_id, session_id, gameserver_ip){

			ASYS_VAR(int, index);
			ASSIGN(index, CC(int, 0));

			ASYS_VAR(std::string, player_info);
			_CC{

				if (index < 5)
				{
					std::cout << "game-server login... " << "$user_id=" 
						<< user_id.r() << std::endl
						<< "$session_id=" << session_id.r() << std::endl;
					index++;
					asys_redo;
				}
				else
				{
					std::cout << "game-server login succeeded " << std::endl;
					player_info = "player-123";
				}
			}CC_;

			RETURN(CC(bool, true), player_info);
		}END_FUN;
	}

	//ret int success std::string player_info
	asys::FunctionCode* login(ASYS_P(std::string, device_id))
	{
		BEGIN_FUN(device_id){
			ASYS_VAR(bool, game_end);
			WHILE_NOT(game_end){
				ASYS_VAR(bool, success);
				ASYS_VAR(std::string, user_id);
				ASYS_VAR(std::string, access_token);

				CALL(platformLogin(), device_id) 
					>>= {success, user_id, access_token};

				IF_NOT(success){
					CONTINUE;
				}END_IF;

				ASYS_VAR(std::string, session_id);
				ASYS_VAR(std::string, gameserver_ip);
				CALL(userServerLogin(), user_id, access_token) 
						>>= { success, session_id, gameserver_ip };

				IF_NOT(success){
					CONTINUE;
				}END_IF;

				ASYS_VAR(std::string, player_info);
				CALL(gameServerLogin(), user_id, session_id, gameserver_ip) 
					>>= { success, player_info };

				IF_NOT(success){
					CONTINUE;
				}END_IF;

				_CC{
					std::cout << "login-success: " 
						<< player_info.r() << std::endl;
				}CC_;

				_CC{
					std::cout << player_info.r() 
						<< " is playing the game." << std::endl;
					asys_redo;
				}CC_;
			}END_WHILE;
		}END_FUN;
	}

private:
	asys_reg_funs;
	asys::Machine m_machine;
	bool m_bStarted{ false };
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
	time.wSecond += 1;        // Wait 1 sec

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
	std::cout << "Start game" << std::endl;
	while (WaitForSingleObject(hTimer, 5000) == WAIT_OBJECT_0){    // Wait for timer event
		if (!game.run("SAMSUM_NT7100__123")) break;;
	}

	CancelWaitableTimer(hTimer);    // Stop timer
	CloseHandle(hTimer);            // Delete handle

	return 0;
}