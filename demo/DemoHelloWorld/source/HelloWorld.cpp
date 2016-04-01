/*!
* \file HelloWorld.cpp
* \date 03-31-2016 19:32:27
*
*
* \author cs 04nycs@gmail.com
*/

#include <iostream>
#include "../../../lib-asyn-script/source/Function.h"

asys::FunctionCode* createAsysFunction()
{
	asys::FunctionCode* f{};
	auto pSumFun = f = new asys::FunctionCode();
	{
		f->ASSIGN("$n", asys::getInputVariableName(0))_;
		f->ASSIGN("$sum", "0")_;
		f->ASSIGN("$i", "0")_;
		f->WHILE_NOT_EQUAL("$i", "$n")_
		{
			f->OPERATE("$sum", "$sum", "$i", asys::Operator::plus)_;
			f->OPERATE("$i", "$i", "1", asys::Operator::plus)_;
		}f->END_WHILE()_;
		f->RETURN({ "$sum" })_;
	}

	auto pMain = f =new asys::FunctionCode();
	{
		f->ASSIGN("$index", "0")_;
		f->WHILE_NOT_EQUAL("$index", "100")_;
		{
			f->CALL({ "$sum" }, { "$index" }, pSumFun)_;

			f->EXPRESS([](asys::Executable* executable){
				std::cout << "sum = " << executable->getValue("$sum")->content().c_str() << std::endl;
				return asys::RetCode::code_done;
			})_;

			f->OPERATE("$index", "$index", "1", asys::Operator::plus)_;
		}f->END_WHILE()_;
	}

	return pMain;
}

int main()
{
	auto fun = createAsysFunction();
	auto executable = fun->compile();
	delete fun; fun = nullptr;

	while (executable->run() == asys::RetCode::code_continue);
	executable->release();

	char c{};
	std::cin >> c;

	return 0;
}