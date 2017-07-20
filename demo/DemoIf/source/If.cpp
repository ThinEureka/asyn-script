/*!
 * \file If.cpp
 * \date 07-21-2017 05:59:48
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#include <iostream>
#include "../../../lib-asyn-script/source/AsynScript.h"

asys_reg_funs;

asys::FunctionCode* demo_if()
{
	BEGIN_FUN(){
		ASYS_VAR(int, i);
		FOR_CC(i = 0, i <= 100, ++i){
			IF_CC(i % 5 == 0){
				_CC{
					std::cout << i << ',' << 0 << std::endl;
				}CC_;
			}ELSE_IF_CC(i % 5 == 1){
				_CC{
					std::cout << i << ',' << 1 << std::endl;
				}CC_;
			}ELSE_IF_CC(i % 5 == 2){
				_CC{
					std::cout << i << ',' << 2 << std::endl;
				}CC_;
			}ELSE_IF_CC(i % 5 == 3){
				_CC{
					std::cout << i << ',' << 3 << std::endl;
				}CC_;
			}ELSE{
				_CC{
					std::cout << i << ',' << 4 << std::endl;
				}CC_;
			}END_IF;
		}END_FOR;
	}END_FUN;
}

int main()
{
	asys::Machine machine;
	machine.installCode(demo_if(), {});

	while (machine.run() == asys::CodeFlow::redo_);

	char c{};
	std::cin >> c;

	return 0;
}
