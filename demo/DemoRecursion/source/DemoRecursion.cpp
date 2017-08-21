/*!
* \file Recursion.cpp
* \date 7-7-2016 20:00:00
*
*
* \author cs (04nycs@gmail.com)
*/


#include <iostream>
#include "../../../lib-asyn-script/source/Function.h"
#include "../../../lib-asyn-script/source/AsynScript.h"

asys_reg_funs;

asys::FunctionCode* sum(ASYS_P(long long, n))
{
	BEGIN_FUN(n){
		_CC{
			if (n <= 0)
			{
				asys_return(0);
			}
		}CC_;

		ASYS_VAR(long long, sub_result);
		CALL(sum(), CC(long long, n - 1))
			>>= {sub_result};

		RETURN(CC(long long, n + sub_result));
	}END_FUN;
}

asys::FunctionCode* print()
{
	BEGIN_FUN(){
		ASYS_VAR(long long, n);

		WHILE_CC(true){
			ASYS_VAR(long long, result);
			CALL(sum(), n) >>= {result};

			_CC{
				std::cout << "s(" << n << ") = " 
					<< result << std::endl;
				++n;
			}CC_;
		}END_WHILE;
	}END_FUN;
}

int main()
{
	asys::Machine machine;
	machine.installCode(print());
	while (machine.run() == asys::CodeFlow::redo_);

	char c{};
	std::cin >> c;

	return 0;
}