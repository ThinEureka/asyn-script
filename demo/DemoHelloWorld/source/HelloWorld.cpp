/*!
* \file HelloWorld.cpp
* \date 03-31-2016 19:32:27
*
*
* \author cs 04nycs@gmail.com
*/

#include <iostream>
#include "../../../lib-asyn-script/source/Function.h"
#include "../../../lib-asyn-script/source/AsynScript.h"

asys::FunctionMap m_asynFunctions;

asys::FunctionCode* sum(ASYS_PARAM(n))
{
	BEGIN_FUN(n){
		ASYS_VAR(sum);
		ASSIGN(sum, asys_const(0));

		ASYS_VAR(i);
		ASSIGN(i, asys_const(0));

		WHILE_NOT_EQUAL(i, n){
			OPERATE(sum, sum, i, asys::Operator::plus);
			OPERATE(i, i, asys_const(1), asys::Operator::plus);
		}END_WHILE;

		RETURN(sum);
	}END_FUN;
}

asys::FunctionCode* sum2(ASYS_PARAM(n))
{
	BEGIN_FUN(n){
		__C{
			asys_value(n);
			int v_sum = 0;
			for (int i = 0; i != n.toInt(); ++i)
			{
				v_sum += i;
			}
			asys_return(v_sum);
		}C__;
	}END_FUN;
}

asys::FunctionCode* print_sum(ASYS_PARAM(n))
{
	BEGIN_FUN(n){
		ASYS_VAR(index);
		ASSIGN(index, asys_const(0));

		WHILE_NOT_EQUAL(index, n){
			ASYS_VAR(v_sum);
			CALL({ v_sum }, { index }, sum());

			__C{
				asys_value(v_sum);
				std::cout << "sum = " << v_sum.toString() << std::endl;
			}C__;

			ASYS_VAR(v_sum2);
			CALL({ v_sum2 }, { index }, sum2());

			__C{
				asys_value(v_sum2);
				std::cout << "sum2 = " << v_sum2.toString() << std::endl;
			}C__;

			OPERATE(index, index, asys_const(1), asys::Operator::plus);
		}END_WHILE;
	}END_FUN;
}

int main()
{
	auto executable = print_sum()->compile();
	executable->setInputValue(0, 100);

	while (executable->run() == asys::CodeFlow::redo_);
	executable->release();

	char c{};
	std::cin >> c;

	return 0;
}