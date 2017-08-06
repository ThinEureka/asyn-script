
/*!
* \file Sum.cpp
* \date 03-31-2016 19:32:27
*
*
* \author cs 04nycs@gmail.com
*/

#include <iostream>
#include "../../../lib-asyn-script/source/AsynScript.h"

asys_reg_funs;

asys::FunctionCode* sum_cc(ASYS_P(int, n))
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

asys::FunctionCode* sum_while(ASYS_P(int, n))
{
	BEGIN_FUN(n){
		ASYS_VAR(int, sum);
		ASYS_VAR(int, i);

		WHILE_CC(i <= n){
			_CC{
				sum += i++;
			}CC_;
		}END_WHILE;

		RETURN(sum);
	}END_FUN;
}

asys::FunctionCode* sum_for(ASYS_P(int, n))
{
	BEGIN_FUN(n){
		ASYS_VAR(int, sum);
		ASYS_VAR(int, i);

		FOR_CC(i = 0, i <= n, ++i){
			_CC{
				sum += i;
			}CC_;
		}END_FOR;

		RETURN(sum);
	}END_FUN;
}

asys::FunctionCode* print_sum(ASYS_P(int, n))
{
	BEGIN_FUN(n){

		ASYS_VAR(int, index);
		ASYS_VAR(int, sum_int);
		ASYS_VAR(double, sum_double);
		ASYS_VAR(short, sum_short);

		WHILE_CC(index <= n){

			_CC{
				sum_double = sum_int;
				std::cout << "index, sum_int, sum_double = " << index << " " << sum_int << " " << sum_double << std::endl;
			}CC_;

			ASYS_VAR(int, sum_cc_int);
			CALL(sum_cc(), index) >>= {sum_cc_int};

			ASYS_VAR(int, sum_for_int);
			CALL(sum_for(), index) >>= {sum_for_int};

			_CC{
				sum_double = sum_cc_int;
				sum_short = static_cast<short>(sum_double);
				std::cout << "index, int_sum_cc, sum_short, int_sum_for = "
					<< index << " "
					<< sum_cc_int << " "
					<< sum_short << " "
					<< sum_for_int << " "
					<< std::endl;
				index++;
			}CC_;
		}END_WHILE;
	}END_FUN;
}

int main()
{
	asys::Machine machine;
	machine.installCode(print_sum(), { 100 });

	while (machine.run() == asys::CodeFlow::redo_);

	char c{};
	std::cin >> c;

	return 0;
}
