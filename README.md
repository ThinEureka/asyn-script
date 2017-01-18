# asyn-script
A library which implements a virtual machine to achieve continuation-like behavior in C++ and provides a scripting language in the form of C++ function calls.

wiki: https://github.com/ThinEureka/asyn-script/wiki

faq: https://github.com/ThinEureka/asyn-script/wiki/FAQ

AsynScriptAssist: https://github.com/ThinEureka/AsynScriptAssist

login demo code

    asys::FunctionCode* login(ASYS_P(std::string, device_id))
	{
		BEGIN_FUN(device_id){
    
			ASYS_VAR(bool, game_end);
      
			WHILE_NOT(game_end){ 
				ASYS_VAR(bool, success);    
				ASYS_VAR(std::string, user_id);   
				ASYS_VAR(std::string, access_token);

				CALL(platformLogin(), device_id) >>= {success, user_id, access_token};

				IF_NOT(success){ 
					CONTINUE;
				}END_IF;

				ASYS_VAR(std::string, session_id);       
				ASYS_VAR(std::string, gameserver_ip);
				CALL(userServerLogin(), user_id, access_token) >>= { success, session_id, gameserver_ip };

				IF_NOT(success){
					CONTINUE;
				}END_IF;

				ASYS_VAR(std::string, player_info);   
				CALL(gameServerLogin(), user_id, session_id, gameserver_ip) >>= { success, player_info };

				IF_NOT(success){
					CONTINUE;
				}END_IF;

				_CC{ 
					std::cout << "login-success: " << player_info.r() << std::endl;
				}CC_;

				_CC{       
					std::cout << player_info.r() << " is playing the game." << std::endl;
					asys_redo;
				}CC_;
			}END_WHILE;
		}END_FUN;
	}
