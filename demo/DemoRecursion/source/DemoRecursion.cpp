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

asys::FunctionCode* sum(ASYS_PARAM(n))
{
	BEGIN_FUN(n){
		ASYS_VAR(n_minus_one);
		_CC{
			asys_value(n);
			asys_value(n_minus_one);
			if (n.toInt() <= 0)
			{
				asys_return(0);
			}

			n_minus_one = n.toInt() - 1;
		}CC_;

		ASYS_VAR(sub_result);
		CALL({ sub_result }, { n_minus_one }, sum());

		_CC{
			asys_value(sub_result);
			asys_value(n);
			asys_return(n.toInt() + sub_result.toInt());
		}CC_;
	}END_FUN;
}

asys::FunctionCode* print()
{
	BEGIN_FUN(){
		ASYS_VAR(n);
		ASSIGN(n, asys_const(0));

		WHILE(asys::True){
			ASYS_VAR(result);
			CALL({ result }, { n }, sum());

			_CC{
				asys_value(result);
				asys_value(n);
				std::cout << "s(" << n.toInt() << ") = " << result.toInt() << std::endl;

				n = n.toInt() + 1;
			}CC_;
		}END_WHILE;
	}END_FUN;
}

int main()
{
	auto executable = print()->compile();
	while (executable->run() == asys::CodeFlow::redo_);
	executable->release();

	char c{};
	std::cin >> c;

	return 0;
}