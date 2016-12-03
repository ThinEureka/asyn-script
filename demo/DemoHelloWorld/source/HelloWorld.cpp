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

asys::FunctionCode* sum(ASYS_P(int, n))
{
	BEGIN_FUN(n){
		ASYS_VAR(int, sum);
		ASYS_VAR(int, i);

		WHILE_CC(i <= n){
			_CC{
				sum = sum + i;
				i = i + 1;
			}CC_;
		}END_WHILE;

		RETURN(sum);
	}END_FUN;
}

asys::FunctionCode* sum2(ASYS_P(int, n))
{
	BEGIN_FUN(n){
		_CC{
			int v_sum = 0;
			for (int i = 0; i <= n; ++i)
			{
				v_sum += i;
			}
			asys_return(v_sum);
		}CC_;
	}END_FUN;
}

asys::FunctionCode* print_sum(ASYS_P(int, n))
{
	BEGIN_FUN(n){
		ASYS_VAR(int, index);
		ASYS_VAR(int, v_sum);
		ASYS_VAR(double, sum_double);
		ASYS_VAR(short, sum_short);

		WHILE_CC(index <= n){
			CALL({ v_sum }, { index }, sum());

			_CC{
				sum_double = v_sum;
				std::cout << "index, sum, sum_double = " << index << " " << v_sum << " " << sum_double << std::endl;
			}CC_;

			ASYS_VAR(int, v_sum2);
			CALL({ v_sum2 }, { index }, sum2());

			_CC{
				sum_short = static_cast<short>(sum_double);
				std::cout << "index, sum2, sum_char = " << index << " " << v_sum2 << " " << sum_short << std::endl;
				index = index + 1;
			}CC_;
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