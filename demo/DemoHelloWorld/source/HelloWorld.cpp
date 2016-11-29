/*!
* \file HelloWorld.cpp
* \date 03-31-2016 19:32:27
*
*
* \author cs 04nycs@gmail.com
*/

#include <iostream>
//#include "../../../lib-asyn-script/source/Function.h"
//#include "../../../lib-asyn-script/source/AsynScript.h"
#include "../../../lib-asyn-script/source/AsysVarible.h"
#include "../../../lib-asyn-script/source/Function.h"

//asys_reg_funs;

//asys::FunctionCode* sum(ASYS_PARAM(n))
//{
//	BEGIN_FUN(n){
//		ASYS_VAR(sum);
//		ASSIGN(sum, asys_const(0));
//
//		ASYS_VAR(i);
//		ASSIGN(i, asys_const(0));
//
//		WHILE_NOT_EQUAL(i, n){
//			OPERATE(sum, sum, i, asys::Operator::plus);
//			OPERATE(i, i, asys_const(1), asys::Operator::plus);
//		}END_WHILE;
//
//		RETURN(sum);
//	}END_FUN;
//}

//asys::FunctionCode* sum2(ASYS_PARAM(n))
//{
//	BEGIN_FUN(n){
//		ASYS_VAR(n);
//		_CC{
//			asys_value(n);
//			int v_sum = 0;
//			for (int i = 0; i != n.toInt(); ++i)
//			{
//				v_sum += i;
//			}
//			asys_return(v_sum);
//		}CC_;
//	}END_FUN;
//}

//asys::FunctionCode* print_sum(ASYS_PARAM(n))
//{
//	BEGIN_FUN(n){
//		ASYS_VAR(index);
//		ASSIGN(index, asys_const(0));
//
//		WHILE_NOT_EQUAL(index, n){
//			ASYS_VAR(v_sum);
//			CALL({ v_sum }, { index }, sum());
//
//			_CC{
//				asys_value(v_sum);
//				std::cout << "sum = " << v_sum.toString() << std::endl;
//			}CC_;
//
//			ASYS_VAR(v_sum2);
//			CALL({ v_sum2 }, { index }, sum2());
//
//			_CC{
//				asys_value(v_sum2);
//				std::cout << "sum2 = " << v_sum2.toString() << std::endl;
//			}CC_;
//
//			OPERATE(index, index, asys_const(1), asys::Operator::plus);
//		}END_WHILE;
//	}END_FUN;
//}

int main()
{
	auto f = new asys::FunctionCode();

	asys::AsysVaribleT<int> i;
	asys::AsysVaribleT<int> sum;

	f->Declare(i);
	f->Declare(sum);

	f->Assign(i, 0);
	f->While(true);
	{
		f->Do([=](asys::Executable* asys_this){
			auto i_value = dynamic_cast<asys::AsysValueT<int>*>(asys_this->getAsysValue(i));
			if (i_value->getNativeValue() == 100)
			{
				asys_this->setReturnCodeFlow(asys::CodeFlow::return_); return;
			}
			
			auto i_sum = dynamic_cast<asys::AsysValueT<int>*>(asys_this->getAsysValue(sum));
			i_sum->getNativeValueReference() = i_sum->getNativeValue() + i_value->getNativeValue();
			asys_this->getAsysValue(i)->toInt();

			i_value->getNativeValueReference() = i_value->getNativeValue() + 1;

			std::cout << "i " << i_value->getNativeValue() << " n " << i_sum->getNativeValue() << std::endl;
		});
	}f->End_while();

	auto executable = f->compile();
	while (executable->run() == asys::CodeFlow::redo_);
	executable->release();

	typedef int int_tmp;
	int x0 = 1;
	asys::AsysValueT<int> g;
	g.setNativeValue(x0);
	g.toBool();

	bool x1 = false;
	asys::AsysValueT<bool> h;
	h.setNativeValue(x1);
	h.toUShort();

	asys::AsysValueT<std::string> k;

	{
		asys::Const valueInt{ 9 };
	}
	
	asys::Const valueString{ std::string("9") };


	asys::Const valueNullPtr{ nullptr };
	const int gg = 0;

	asys::Const valueG(gg);

	asys::Const valueConstInt{ gg };
	asys::Const valueConstInt2{ (const int*)&gg };

	//asys::Const valuePointer{&gg};

	asys::Const vecVec{ std::vector<int>({}) };
	asys::Const cons_str{ "abc" };
	asys::Const str{ std::string("ABC") };
	asys::Const str2{ std::string("DFD") };
	asys::Const ptr_{ nullptr };
	//asys::Const void_ptr{ (void*)(nullptr) };

	str.getAsysValue()->assign(*cons_str.getAsysValue());
	str.getAsysValue()->assign(*str2.getAsysValue());

	//asys::Variable<int> varInt;
	//varInt = 9;
	//varInt = true;
	////varInt = 7.0; // an warning is expected here.
	//int temp = 3 + varInt;

	//asys::Variable<int> varBool;
	//varBool = true;
	//if (varBool)
	//{
	//	int k = 9 + varInt;
	//}

	//asys::FunctionCode* code = new asys::FunctionCode;
	//code->If(varInt);

	//auto executable = print_sum()->compile();
	//executable->setInputValue(0, 100);

	//while (executable->run() == asys::CodeFlow::redo_);
	//executable->release();

	char c{};
	std::cin >> c;

	return 0;
}