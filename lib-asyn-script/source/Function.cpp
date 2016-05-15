
/*!
 * \file Function.cpp
 * \date 02-10-2016 15:29:51
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#include "Function.h"
#include <assert.h>

asys::FunctionCode::FunctionCode()
{

}

asys::FunctionCode::~FunctionCode()
{
	for (auto instructor : m_instructors)
	{
		delete instructor;
	}

	m_instructors.clear();
}

void asys::FunctionCode::clear()
{
	for (auto instructor : m_instructors)
	{
		delete instructor;
	}

	m_instructors.clear();
}

asys::BreakPoint& asys::FunctionCode::EXPRESS(const std::function<RetCode(Executable*)>& express)
{
	auto instructor = new ExpressInstructor(express);
	m_instructors.push_back(instructor);

	return instructor->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::CALL(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, Code* code)
{
	return CALL_EX(outputParams, inputParams, code, "");
}

asys::BreakPoint&  asys::FunctionCode::CALL(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, const std::string& codeName)
{
	return CALL_EX(outputParams, inputParams, nullptr, codeName);
}

asys::BreakPoint&  asys::FunctionCode::CALL_EX(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, Code* code, const std::string& codeName)
{
	std::vector<std::pair<std::string, std::string>> outputPairs;
	auto inputPairs = outputPairs;

	outputPairs.resize(outputParams.size());
	inputPairs.resize(inputParams.size());

	for (int i = 0; i < static_cast<int>(outputParams .size()); ++i)
	{
		auto& pair = outputPairs[i];
		pair.first = outputParams[i];
		pair.second = asys::getOutputVariableName(i);
	}

	for (int i = 0; i < static_cast<int>(inputParams.size()); ++i)
	{
		auto& pair = inputPairs[i];
		pair.first = asys::getInputVariableName(i);
		pair.second = inputParams[i];
	}

	return CALL_EX(outputPairs, inputPairs, code, codeName);
}

asys::BreakPoint& asys::FunctionCode::CALL_EX(const std::vector<std::pair<std::string, std::string>>& outputParams, const std::vector<std::pair<std::string, std::string>>&inputParams, Code* code, const std::string& codeName)
{
	for (auto& param : outputParams)
	{
		//there's not point to have any of the 2 assignment operands being const
		assert(isValidVariableName(param.first) && isValidVariableName(param.second));
	}

	for (auto& param : inputParams)
	{
		assert(isValidVariableName(param.first));
	}

	auto instructor = new CallInstructor(outputParams, inputParams, code, codeName);
	m_instructors.push_back(instructor);

	return instructor->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::INPUT(const std::vector<std::string>& inputParams)
{
	return EXPRESS([inputParams](Executable* executable){

		for (size_t i = 0; i < inputParams.size(); ++i)
		{
			auto& param = inputParams[i];
			assert(isValidVariableName(param));
			executable->setValue(param, executable->getValue(asys::getInputVariableName(i)));
		}

		return RetCode::code_done;
	});
}

asys::BreakPoint& asys::FunctionCode::ASSIGN(const std::string& var1, const std::string& var2)
{
	// can not assign to const variable.
	assert(isValidVariableName(var1));

	return EXPRESS([var1, var2](Executable* executable){
		if (isValidVariableName(var2))
		{	
			// the second argument allows to be null, thus its effect
			//is to erase var1 from variables table.
			executable->setValue(var1, executable->getValue(var2));
		}
		else
		{
			executable->setValue(var1, var2);
		}
		return RetCode::code_done;
	});
}

asys::BreakPoint& asys::FunctionCode::OPERATE(const std::string& output, const std::string& var1, const std::string& var2, Operator eOperator)
{
	assert(isValidVariableName(output));

	return EXPRESS([output, var1, eOperator, var2](Executable* executable){
		const auto& callback = Value::getBinaryOperator(eOperator);
		if (!callback) return RetCode::code_done;

		//none of the operands allows to be null.
		executable->setValue(output, callback(isValidVariableName(var1) ? *executable->getValue(var1) : var1,
			isValidVariableName(var2) ? *executable->getValue(var2) : var2));

		return RetCode::code_done;
	});
}

asys::BreakPoint& asys::FunctionCode::OPERATE(const std::string& output, const std::string& var, Operator eOperator)
{
	assert(isValidVariableName(output));

	return EXPRESS([output, eOperator, var](Executable* executable){
		const auto& callback = Value::getUnaryOperator(eOperator);
		if (!callback) return RetCode::code_done;

		//the operand must not be null.
		executable->setValue(output, callback(isValidVariableName(var) ? *executable->getValue(var) : var));

		return RetCode::code_done;
	});
}

asys::BreakPoint& asys::FunctionCode::IF(const std::string& var)
{
	return IF_EX([var](Executable* executable){
		if (asys::isValidVariableName(var))
		{
			auto val = executable->getValue(var);
			if (!val) return false;

			return val->toBool();
		}

		return Value(var).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::IF_NOT(const std::string& var)
{
	return IF_EX([var](Executable* executable){
		if (asys::isValidVariableName(var))
		{
			auto val = executable->getValue(var);
			if (!val) return true;
			return !val->toBool();
		}

		return !Value(var).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::IF_EQUAL(const std::string& var1, const std::string& var2)
{
	return IF_EX([var1, var2](Executable* executable){

		Value leftValue{ var1 };
		Value rightValue{ var2 };

		const Value* left = &leftValue;
		const Value* right = &rightValue;

		if (asys::isValidVariableName(var1))
		{
			left = executable->getValue(var1);

			if (left)
			{
				leftValue = *left;
			}
			else
			{
				left = nullptr;
			}
		}

		if (asys::isValidVariableName(var2))
		{
			right = executable->getValue(var2);
			if (right)
			{
				rightValue = *right;
			}
			else
			{
				right = nullptr;
			}
		}

		if (!left || !right) return left == right;

		return Value::equal(leftValue, rightValue).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::IF_NOT_EQUAL(const std::string& var1, const std::string& var2)
{
	return IF_EX([var1, var2](Executable* executable){

		Value leftValue{ var1 };
		Value rightValue{ var2 };

		const Value* left = &leftValue;
		const Value* right = &rightValue;

		if (asys::isValidVariableName(var1))
		{
			left = executable->getValue(var1);

			if (left)
			{
				leftValue = *left;
			}
			else
			{
				left = nullptr;
			}
		}

		if (asys::isValidVariableName(var2))
		{
			right = executable->getValue(var2);
			if (right)
			{
				rightValue = *right;
			}
			else
			{
				right = nullptr;
			}
		}

		if (!left || !right) return left != right;

		return !Value::equal(leftValue, rightValue).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::IF_EX(const std::function<bool(Executable*)>& express)
{
	int ip = static_cast<int>(m_instructors.size());
	auto instructor = new IfInstructor(express);
	m_instructors.push_back(instructor);
	m_unmatchedIfIps.push_back(ip);

	return instructor->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::ELSE()
{
	assert(m_unmatchedIfIps.size() > 0);//  "Asynscript compile error, there is no unmatched if expression for this else-expression";

	int unMatchedIfIp = m_unmatchedIfIps.back();
	auto ifInstructor = dynamic_cast<IfInstructor*>(m_instructors[unMatchedIfIp]);

	assert(ifInstructor->elseIp == INVALID_IP);// "Asynscript compile error, there is already an else expression for if.");

	int ip = static_cast<int>(m_instructors.size());
	auto elseInstructor = new ElseInstructor();
	m_instructors.push_back(elseInstructor);

	ifInstructor->elseIp = ip;
	elseInstructor->ifIp = unMatchedIfIp;

	return elseInstructor->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::END_IF()
{
	assert(m_unmatchedIfIps.size() > 0);// "Asynscript compile error, there is no unmatched if expression for this endif-expression");

	int unMatchedIfIp = m_unmatchedIfIps.back();
	auto ifInstructor = dynamic_cast<IfInstructor*>(m_instructors[unMatchedIfIp]);

	assert(ifInstructor->endIfIp == INVALID_IP);// "Asynscript compile error, there is already an endif expression for if.");

	int ip = static_cast<int>(m_instructors.size());
	auto endIfInstructor = new EndIfInstructor();
	m_instructors.push_back(endIfInstructor);

	ifInstructor->endIfIp = ip;
	endIfInstructor->ifIp = unMatchedIfIp;

	int elseIp = ifInstructor->elseIp;
	if (elseIp != INVALID_IP)
	{
		auto elseInstructor = dynamic_cast<ElseInstructor*>(m_instructors[elseIp]);
		elseInstructor->endIfIp = ip;
		endIfInstructor->elseIp = elseIp;
	}

	m_unmatchedIfIps.pop_back();

	return endIfInstructor->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::WHILE(const std::string& var)
{
	return WHILE_EX([var](Executable* executable){
		if (asys::isValidVariableName(var)){
			auto val = executable->getValue(var);
			if (!val) return false;
			return val->toBool();
		}

		return Value(var).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::WHILE_NOT(const std::string& var)
{
	return WHILE_EX([var](Executable* executable){
		if (asys::isValidVariableName(var)){
			auto val = executable->getValue(var);
			if (!val) return true;
			return !val->toBool();
		}

		return !Value(var).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::WHILE_EQUAL(const std::string& var1, const std::string& var2)
{
	return WHILE_EX([var1, var2](Executable* executable){

		Value leftValue{ var1 };
		Value rightValue{ var2 };

		const Value* left = &leftValue;
		const Value* right = &rightValue;

		if (asys::isValidVariableName(var1))
		{
			left = executable->getValue(var1);

			if (left)
			{
				leftValue = *left;
			}
			else
			{
				left = nullptr;
			}
		}

		if (asys::isValidVariableName(var2))
		{
			right = executable->getValue(var2);
			if (right)
			{
				rightValue = *right;
			}
			else
			{
				right = nullptr;
			}
		}

		if (!left || !right) return left == right;

		return Value::equal(leftValue, rightValue).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::WHILE_NOT_EQUAL(const std::string& var1, const std::string& var2)
{
	return WHILE_EX([var1, var2](Executable* executable){

		Value leftValue{ var1 };
		Value rightValue{ var2 };

		const Value* left = &leftValue;
		const Value* right = &rightValue;

		if (asys::isValidVariableName(var1))
		{
			left = executable->getValue(var1);

			if (left)
			{
				leftValue = *left;
			}
			else
			{
				left = nullptr;
			}
		}

		if (asys::isValidVariableName(var2))
		{
			right = executable->getValue(var2);
			if (right)
			{
				rightValue = *right;
			}
			else
			{
				right = nullptr;
			}
		}

		if (!left || !right) return left != right;

		return !Value::equal(leftValue, rightValue).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::WHILE_EX(const std::function<bool(Executable*)>& express)
{
	int ip = static_cast<int>(m_instructors.size());
	auto instructor = new WhileInstructor(express);
	m_instructors.push_back(instructor);
	m_unmatchedWhileIps.push_back(ip);

	return instructor->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::END_WHILE()
{
	assert(m_unmatchedWhileIps.size() > 0);// "Asynscript compile error, there is no unmatched while expression for this endwhile-expression");

	int unMatchedWhileIp = m_unmatchedWhileIps.back();
	auto whileContructor = dynamic_cast<WhileInstructor*>(m_instructors[unMatchedWhileIp]);

	assert(whileContructor->endWhileIp == INVALID_IP);// "Asynscript compile error, there is already an endwhile expression for while.");

	int ip = static_cast<int>(m_instructors.size());
	auto endWhileInstructor = new EndWhileInstructor();
	m_instructors.push_back(endWhileInstructor);

	whileContructor->endWhileIp= ip;
	endWhileInstructor->whileIp = unMatchedWhileIp;

	m_unmatchedWhileIps.pop_back();

	return endWhileInstructor->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::CONTINUE()
{
	assert(m_unmatchedWhileIps.size() > 0);// "Asynscript compile error, there is no unmatched while expression for this continue-expression");

	int unMatchedWhileIp = m_unmatchedWhileIps.back();
	auto whileContructor = dynamic_cast<WhileInstructor*>(m_instructors[unMatchedWhileIp]);

	auto continueInstructor = new ContinueInstructor();
	m_instructors.push_back(continueInstructor);

	continueInstructor->whileIp = unMatchedWhileIp;

	return continueInstructor->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::BREAK()
{
	assert(m_unmatchedWhileIps.size() > 0);// "Asynscript compile error, there is no unmatched while expression for this break-expression");

	int unMatchedWhileIp = m_unmatchedWhileIps.back();
	auto whileContructor = dynamic_cast<WhileInstructor*>(m_instructors[unMatchedWhileIp]);

	auto breakInstructor = new BreakInstructor();
	m_instructors.push_back(breakInstructor);

	breakInstructor->whileIp = unMatchedWhileIp;

	return breakInstructor->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::RETURN(const std::vector<std::string>& vars)
{
	auto instructor = new ReturnInstructor(vars);
	m_instructors.push_back(instructor);

	return instructor->breakPoint();
}

asys::Executable* asys::FunctionCode::compile()
{
	assert(m_unmatchedIfIps.size() == 0);// "Asynscript compile error: There are unmatched ifs in this function.");
	assert(m_unmatchedWhileIps.size() == 0);// "Asynscript compile error: There are unmatched ifs in this function.");

	decltype(m_instructors) compile_instructors;
	compile_instructors.resize(m_instructors.size());

	for (int i = 0; i < static_cast<int>(m_instructors.size()); ++i)
	{
		compile_instructors[i] = m_instructors[i]->clone();
		auto instructor = compile_instructors[i];

		if (instructor->instructorType() == InstructorType::type_call)
		{
			auto callInstructor = dynamic_cast<CallInstructor*>(instructor);
			if (!callInstructor->codeName.empty()) 
			{
				if (!asys::isValidVariableName(callInstructor->codeName))
				{
					auto itCode = m_dynamicCodes.find(callInstructor->codeName);
					if (itCode != m_dynamicCodes.end())
					{
						callInstructor->code = itCode->second;
					}
				}
			}
		}
	}

	return new FunctionExecutable(compile_instructors, m_dynamicCodes);
}

asys::FunctionExecutable::FunctionExecutable(const std::vector<Instructor*> instructors, const std::map<std::string, Code*> dynamicCodes)
	: m_instructors(instructors)
	, m_dynamicCodes(dynamicCodes)
{

}

asys::FunctionExecutable::~FunctionExecutable()
{
	for (auto instructor : m_instructors)
	{
		delete instructor;
	}

	m_instructors.clear();
}

asys::RetCode asys::FunctionExecutable::run()
{
	RetCode retCode = RetCode::code_done;

	while (true)
	{
		if (m_nCurIp >= static_cast<int>(m_instructors.size()) || 
			retCode == RetCode::code_continue) break;

		auto instructor = m_instructors[m_nCurIp];

		auto callback = instructor->breakPoint().callback();
		if (callback)
		{
			callback(this, instructor->breakPoint());
		}

		switch (instructor->instructorType())
		{
		case InstructorType::type_null:
			m_nCurIp = processNullInstructor(m_nCurIp);
			break;
		case InstructorType::type_express:
			m_nCurIp = processExpressInstructor(retCode, m_nCurIp, dynamic_cast<ExpressInstructor*>(instructor));
			break;
		case InstructorType::type_call:
			m_nCurIp = processCallInstructor(retCode, m_nCurIp, dynamic_cast<CallInstructor*>(instructor));
			break;
		case InstructorType::type_if:
			m_nCurIp = processIfInstructor(m_nCurIp, dynamic_cast<IfInstructor*>(instructor));
			break;
		case InstructorType::type_else:
			m_nCurIp = processElseInstructor(m_nCurIp, dynamic_cast<ElseInstructor*>(instructor));
			break;
		case InstructorType::type_endif:
			m_nCurIp = processEndIfInstructor(m_nCurIp, dynamic_cast<EndIfInstructor*>(instructor));
			break;
		case InstructorType::type_while:
			m_nCurIp = processWhileInstructor(m_nCurIp, dynamic_cast<WhileInstructor*>(instructor));
			break;
		case InstructorType::type_endwhile:
			m_nCurIp = processEndWhileInstructor(m_nCurIp, dynamic_cast<EndWhileInstructor*>(instructor));
			break;
		case InstructorType::type_continue:
			m_nCurIp = processContinueInstructor(m_nCurIp, dynamic_cast<ContinueInstructor*>(instructor));
			break;
		case InstructorType::type_break:
			m_nCurIp = processBreakInstructor(m_nCurIp, dynamic_cast<BreakInstructor*>(instructor));
			break;
		case InstructorType::type_return:
			m_nCurIp = processReturnInstructor(m_nCurIp, dynamic_cast<ReturnInstructor*>(instructor));
			break;
		default:
			++m_nCurIp;
			break;
		}
	}

	return retCode;
}

int asys::FunctionExecutable::processExpressInstructor(RetCode& retCode, int curIp, ExpressInstructor* expressInstructor)
{
	if (!expressInstructor->express)
	{
		retCode = RetCode::code_done;
		return curIp + 1;
	}

	retCode = expressInstructor->express(this);

	if (retCode == RetCode::code_continue) return curIp;

	return curIp + 1;
}

int asys::FunctionExecutable::processCallInstructor(RetCode& retCode, int curIp, CallInstructor* callInstructor)
{
	retCode = RetCode::code_done;

	if (!callInstructor->executable)
	{
		//get the real code if it's a dynamic call.
		if (!callInstructor->code)
		{
			auto it = m_dynamicCodes.find(callInstructor->codeName);
			if (it != m_dynamicCodes.end())
			{
				callInstructor->code = it->second;
			}
		}

		if (!callInstructor->code) return curIp + 1;

		callInstructor->executable = callInstructor->code->compile();
		callInstructor->executable->retain();

		//pass input arguments to the invoked code.
		for (const auto& pair : callInstructor->inputParams)
		{
			if (isValidVariableName(pair.second))
			{
				callInstructor->executable->setValue(pair.first, getValue(pair.second));
			}
			else
			{
				callInstructor->executable->setValue(pair.first, pair.second);
			}
		}
	}

	retCode = callInstructor->executable->run();

	if (retCode == RetCode::code_continue) return curIp;

	//fetch outputs from invoked code.
	for (const auto& pair : callInstructor->outputParams)
	{
		setValue(pair.first, callInstructor->executable->getValue(pair.second));
	}

	callInstructor->executable->release();
	callInstructor->executable = nullptr;

	return curIp + 1;
}

int asys::FunctionExecutable::processReturnInstructor(int curIp, ReturnInstructor* retInstructor)
{
	for (int i = 0; i < static_cast<int>(retInstructor->outputParams.size()); ++i)
	{
		const auto& param = retInstructor->outputParams[i];
		if (asys::isValidVariableName(param))
		{
			setOutoutValue(i, getValue(retInstructor->outputParams[i]));
		}
		else
		{
			setOutoutValue(i, param);
		}
	}

	return static_cast<int>(m_instructors.size());
}

void asys::BreakPoint::operator()(const std::function<void(Executable*, const BreakPoint& breakpoint)>& callback, const char* fileName /*= nullptr*/, const char* functionName /*= nullptr*/, int lineNumber /*= -1*/)
{
	m_callback = callback;
	if (fileName) m_fileName = fileName;
	if (functionName) m_functionName = functionName;
	m_lineNumber = lineNumber;
}
