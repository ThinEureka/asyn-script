/*!
* \file HelloWorld.cpp
* \date 03-31-2016 19:32:27
*
*
* \author cs 04nycs@gmail.com
*/

#include <iostream>
#include "../../../lib-asyn-script/source/Function.h"

asys::FunctionMap m_asynFunctions;

asys::FunctionCode* sum(ASYS_PARAM(n))
{
	auto& f = m_asynFunctions[__FUNCTION__];
	if (f) return f;

	f = new asys::FunctionCode;
	{
		f->INPUT({n})_;

		ASYS_VAR(sum);
		f->ASSIGN(sum, "0")_;

		ASYS_VAR(i);
		f->ASSIGN(i, "0")_;

		f->WHILE_NOT_EQUAL(i, n)_
		{
			f->OPERATE(sum, sum, i, asys::Operator::plus)_;
			f->OPERATE(i, i, "1", asys::Operator::plus)_;
		}f->END_WHILE()_;
		f->RETURN({ sum })_;
	}

	return f;
}

asys::FunctionCode* print_sum(ASYS_PARAM(n))
{
	auto& f = m_asynFunctions[__FUNCTION__];
	if (f) return f;

	f = new asys::FunctionCode;
	{
		f->INPUT({n})_;

		ASYS_VAR(index);
		f->ASSIGN(index, "0")_;

		f->WHILE_NOT_EQUAL(index, n)_;
		{
			ASYS_VAR(v_sum);
			f->CALL({v_sum}, { index }, sum())_;

			f->EXPRESS([v_sum](asys::Executable* executable){
				asys_value(v_sum);
				std::cout << "sum = " << v_sum.toString() << std::endl;
				return asys::CodeFlow::next_;
			})_;

			f->OPERATE(index, index, "1", asys::Operator::plus)_;
		}f->END_WHILE()_;
	}

	return f;
}

int main()
{
	auto executable = print_sum()->compile();
	executable->setInputValue(0, 100);

	while (executable->run() == asys::CodeFlow::yield_);
	executable->release();

	char c{};
	std::cin >> c;

	return 0;
}