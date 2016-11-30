/*!
* \file HelloWorld.cpp
* \date 03-31-2016 19:32:27
*
*
* \author cs 04nycs@gmail.com
*/

#include <iostream>
#include "../../../lib-asyn-script/source/AsynScript.h"

asys_reg_funs;

asys::FunctionCode* sum(ASYS_PARAM(int, n))
{
	BEGIN_FUN(n){
		ASYS_VAR(int, sum);
		ASYS_VAR(int, i);

		WHILE_NOT_EQUAL(i, n){
			PLUS(sum, sum, i);
			PLUS(i, i, 1);
		}END_WHILE;

		RETURN(sum);
	}END_FUN;
}

asys::FunctionCode* sum2(ASYS_PARAM(int, n))
{
	BEGIN_FUN(n){
		_CC{
			int v_sum = 0;
			for (int i = 0; i != n; ++i)
			{
				v_sum += i;
			}
			asys_return(v_sum);
		}CC_;
	}END_FUN;
}

asys::FunctionCode* print_sum(ASYS_PARAM(int, n))
{
	BEGIN_FUN(n){
		ASYS_VAR(int, index);
		ASYS_VAR(int, v_sum);

		WHILE_NOT_EQUAL(index, n){
			CALL({ v_sum }, { index }, sum());

			_CC{
				std::cout << "index, sum = " << index << " " << v_sum << std::endl;
			}CC_;

			ASYS_VAR(int, v_sum2);
			CALL({ v_sum2 }, { index }, sum2());

			_CC{
				std::cout << "index, sum2 = " << index << " " << v_sum2 << std::endl;
			}CC_;

			PLUS(index, index, 1);
		}END_WHILE;
	}END_FUN;
}

int main()
{
	auto executable = print_sum()->compile();
	executable->setInput({100});

	while (executable->run() == asys::CodeFlow::redo_);
	executable->release();

	char c{};
	std::cin >> c;

	return 0;
}