
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
	for (auto instruction : m_instructions)
	{
		delete instruction;
	}

	m_instructions.clear();
}



void asys::FunctionCode::clear()
{
	for (auto instruction : m_instructions)
	{
		delete instruction;
	}

	m_instructions.clear();
}

asys::BreakPoint& asys::FunctionCode::Do(const std::function<void(Executable*)>& express)
{
	auto instruction = new DoInstruction(express);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::Call(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, Code* code)
{
	return Call_ex(outputParams, inputParams, code, "");
}

asys::BreakPoint&  asys::FunctionCode::Call(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, const std::string& codeName)
{
	return Call_ex(outputParams, inputParams, nullptr, codeName);
}

asys::BreakPoint&  asys::FunctionCode::Call_ex(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, Code* code, const std::string& codeName)
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

	return Call_ex(outputPairs, inputPairs, code, codeName);
}

asys::BreakPoint& asys::FunctionCode::Call_ex(const std::vector<std::pair<std::string, std::string>>& outputParams, const std::vector<std::pair<std::string, std::string>>&inputParams, Code* code, const std::string& codeName)
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

	auto instruction = new CallInstruction(outputParams, inputParams, code, codeName);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::Input(const std::vector<std::string>& inputParams)
{
	return Do([inputParams](Executable* executable){

		for (size_t i = 0; i < inputParams.size(); ++i)
		{
			auto& param = inputParams[i];
			assert(isValidVariableName(param));
			executable->setValue(param, executable->getValue(asys::getInputVariableName(i)));
		}
	});
}

asys::BreakPoint& asys::FunctionCode::Assign(const std::string& var1, const std::string& var2)
{
	// can not assign to const variable.
	assert(isValidVariableName(var1));

	return Do([var1, var2](Executable* executable){
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
	});
}

asys::BreakPoint& asys::FunctionCode::Operate(const std::string& output, const std::string& var1, const std::string& var2, Operator eOperator)
{
	assert(isValidVariableName(output));

	return Do([output, var1, eOperator, var2](Executable* executable){
		const auto& callback = Value::getBinaryOperator(eOperator);
		if (!callback) return;

		//none of the operands allows to be null.
		executable->setValue(output, callback(isValidVariableName(var1) ? *executable->getValue(var1) : var1,
			isValidVariableName(var2) ? *executable->getValue(var2) : var2));
	});
}

asys::BreakPoint& asys::FunctionCode::Operate(const std::string& output, const std::string& var, Operator eOperator)
{
	assert(isValidVariableName(output));

	return Do([output, eOperator, var](Executable* executable){
		const auto& callback = Value::getUnaryOperator(eOperator);
		if (!callback) return;

		//the operand must not be null.
		executable->setValue(output, callback(isValidVariableName(var) ? *executable->getValue(var) : var));
	});
}

asys::BreakPoint& asys::FunctionCode::If(const std::string& var)
{
	return If_ex([var](Executable* executable){
		if (asys::isValidVariableName(var))
		{
			auto val = executable->getValue(var);
			if (!val) return false;

			return val->toBool();
		}

		return Value(var).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::If_not(const std::string& var)
{
	return If_ex([var](Executable* executable){
		if (asys::isValidVariableName(var))
		{
			auto val = executable->getValue(var);
			if (!val) return true;
			return !val->toBool();
		}

		return !Value(var).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::If_equal(const std::string& var1, const std::string& var2)
{
	return If_ex([var1, var2](Executable* executable){

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

asys::BreakPoint& asys::FunctionCode::If_not_equal(const std::string& var1, const std::string& var2)
{
	return If_ex([var1, var2](Executable* executable){

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

asys::BreakPoint& asys::FunctionCode::If_ex(const std::function<bool(Executable*)>& express)
{
	int ip = static_cast<int>(m_instructions.size());
	auto instruction = new IfInstruction(express);
	m_instructions.push_back(instruction);
	m_unmatchedIfIps.push_back(ip);

	return instruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::Else()
{
	assert(m_unmatchedIfIps.size() > 0);//  "Asynscript compile error, there is no unmatched if expression for this else-expression";

	int unMatchedIfIp = m_unmatchedIfIps.back();
	auto ifInstruction = dynamic_cast<IfInstruction*>(m_instructions[unMatchedIfIp]);

	assert(ifInstruction->elseIp == INVALID_IP);// "Asynscript compile error, there is already an else expression for if.");

	int ip = static_cast<int>(m_instructions.size());
	auto elseInstruction = new ElseInstruction();
	m_instructions.push_back(elseInstruction);

	ifInstruction->elseIp = ip;
	elseInstruction->ifIp = unMatchedIfIp;

	return elseInstruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::End_if()
{
	assert(m_unmatchedIfIps.size() > 0);// "Asynscript compile error, there is no unmatched if expression for this endif-expression");

	int unMatchedIfIp = m_unmatchedIfIps.back();
	auto ifInstruction = dynamic_cast<IfInstruction*>(m_instructions[unMatchedIfIp]);

	assert(ifInstruction->endIfIp == INVALID_IP);// "Asynscript compile error, there is already an endif expression for if.");

	int ip = static_cast<int>(m_instructions.size());
	auto endIfInstruction = new EndIfInstruction();
	m_instructions.push_back(endIfInstruction);

	ifInstruction->endIfIp = ip;
	endIfInstruction->ifIp = unMatchedIfIp;

	int elseIp = ifInstruction->elseIp;
	if (elseIp != INVALID_IP)
	{
		auto elseInstruction = dynamic_cast<ElseInstruction*>(m_instructions[elseIp]);
		elseInstruction->endIfIp = ip;
		endIfInstruction->elseIp = elseIp;
	}

	m_unmatchedIfIps.pop_back();

	return endIfInstruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::While(const std::string& var)
{
	return While_ex([var](Executable* executable){
		if (asys::isValidVariableName(var)){
			auto val = executable->getValue(var);
			if (!val) return false;
			return val->toBool();
		}

		return Value(var).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::While_not(const std::string& var)
{
	return While_ex([var](Executable* executable){
		if (asys::isValidVariableName(var)){
			auto val = executable->getValue(var);
			if (!val) return true;
			return !val->toBool();
		}

		return !Value(var).toBool();
	});
}

asys::BreakPoint& asys::FunctionCode::While_equal(const std::string& var1, const std::string& var2)
{
	return While_ex([var1, var2](Executable* executable){

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

asys::BreakPoint& asys::FunctionCode::While_not_equal(const std::string& var1, const std::string& var2)
{
	return While_ex([var1, var2](Executable* executable){

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

asys::BreakPoint& asys::FunctionCode::While_ex(const std::function<bool(Executable*)>& express)
{
	int ip = static_cast<int>(m_instructions.size());
	auto instruction = new WhileInstruction(express);
	m_instructions.push_back(instruction);
	m_unmatchedWhileIps.push_back(ip);

	return instruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::End_while()
{
	assert(m_unmatchedWhileIps.size() > 0);// "Asynscript compile error, there is no unmatched while expression for this endwhile-expression");

	int unMatchedWhileIp = m_unmatchedWhileIps.back();
	auto whileInstruction = dynamic_cast<WhileInstruction*>(m_instructions[unMatchedWhileIp]);

	assert(whileInstruction->endWhileIp == INVALID_IP);// "Asynscript compile error, there is already an endwhile expression for while.");

	int ip = static_cast<int>(m_instructions.size());
	auto endWhileInstruction = new EndWhileInstruction();
	m_instructions.push_back(endWhileInstruction);

	whileInstruction->endWhileIp= ip;
	endWhileInstruction->whileIp = unMatchedWhileIp;

	m_unmatchedWhileIps.pop_back();

	return endWhileInstruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::Continue()
{
	assert(m_unmatchedWhileIps.size() > 0);// "Asynscript compile error, there is no unmatched while expression for this continue-expression");

	int unMatchedWhileIp = m_unmatchedWhileIps.back();
	auto whileInstruction = dynamic_cast<WhileInstruction*>(m_instructions[unMatchedWhileIp]);

	auto continueInstruction = new ContinueInstruction();
	m_instructions.push_back(continueInstruction);

	continueInstruction->whileIp = unMatchedWhileIp;

	return continueInstruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::Break()
{
	assert(m_unmatchedWhileIps.size() > 0);// "Asynscript compile error, there is no unmatched while expression for this break-expression");

	int unMatchedWhileIp = m_unmatchedWhileIps.back();
	auto whileInstruction = dynamic_cast<WhileInstruction*>(m_instructions[unMatchedWhileIp]);

	auto breakInstruction = new BreakInstruction();
	m_instructions.push_back(breakInstruction);

	breakInstruction->whileIp = unMatchedWhileIp;

	return breakInstruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::Return(const std::vector<std::string>& vars)
{
	auto instruction = new ReturnInstruction(vars);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}

asys::Executable* asys::FunctionCode::compile()
{
	assert(m_unmatchedIfIps.size() == 0);// "Asynscript compile error: There are unmatched ifs in this function.");
	assert(m_unmatchedWhileIps.size() == 0);// "Asynscript compile error: There are unmatched ifs in this function.");

	decltype(m_instructions) compile_instructions;
	compile_instructions.resize(m_instructions.size());

	for (int i = 0; i < static_cast<int>(m_instructions.size()); ++i)
	{
		compile_instructions[i] = m_instructions[i]->clone();
		auto instruction = compile_instructions[i];

		if (instruction->instructionType() == InstructionType::type_call)
		{
			auto callInstruction = dynamic_cast<CallInstruction*>(instruction);
			if (!callInstruction->codeName.empty()) 
			{
				if (!asys::isValidVariableName(callInstruction->codeName))
				{
					auto itCode = m_dynamicCodes.find(callInstruction->codeName);
					if (itCode != m_dynamicCodes.end())
					{
						callInstruction->code = itCode->second;
					}
				}
			}
		}
	}

	return new FunctionExecutable(compile_instructions, m_dynamicCodes);
}

asys::FunctionExecutable::FunctionExecutable(const std::vector<Instruction*> instructions, const std::map<std::string, Code*> dynamicCodes)
	: m_instructions(instructions)
{
	setDynamicCodes(dynamicCodes);
}

asys::FunctionExecutable::~FunctionExecutable()
{
	for (auto instruction : m_instructions)
	{
		delete instruction;
	}

	m_instructions.clear();
}

asys::CodeFlow asys::FunctionExecutable::run()
{
	while (true)
	{
		if (m_nCurIp >= static_cast<int>(m_instructions.size()))
		{
			break;
		}

		auto instruction = m_instructions[m_nCurIp];

		auto callback = instruction->breakPoint().callback();
		if (callback && m_retCodeFlow != CodeFlow::redo_)
		{
			callback(this, instruction->breakPoint());
		}

		m_retCodeFlow = CodeFlow::next_;

		switch (instruction->instructionType())
		{
		case InstructionType::type_null:
			m_nCurIp = processNullInstruction(m_nCurIp);
			break;
		case InstructionType::type_do:
			m_nCurIp = processDoInstruction(m_retCodeFlow, m_nCurIp, dynamic_cast<DoInstruction*>(instruction));
			break;
		case InstructionType::type_call:
			m_nCurIp = processCallInstruction(m_retCodeFlow, m_nCurIp, dynamic_cast<CallInstruction*>(instruction));
			break;
		case InstructionType::type_if:
			m_nCurIp = processIfInstruction(m_nCurIp, dynamic_cast<IfInstruction*>(instruction));
			break;
		case InstructionType::type_else:
			m_nCurIp = processElseInstruction(m_nCurIp, dynamic_cast<ElseInstruction*>(instruction));
			break;
		case InstructionType::type_endif:
			m_nCurIp = processEndIfInstruction(m_nCurIp, dynamic_cast<EndIfInstruction*>(instruction));
			break;
		case InstructionType::type_while:
			m_nCurIp = processWhileInstruction(m_nCurIp, dynamic_cast<WhileInstruction*>(instruction));
			break;
		case InstructionType::type_endwhile:
			m_nCurIp = processEndWhileInstruction(m_nCurIp, dynamic_cast<EndWhileInstruction*>(instruction));
			break;
		case InstructionType::type_continue:
			m_nCurIp = processContinueInstruction(m_nCurIp, dynamic_cast<ContinueInstruction*>(instruction));
			break;
		case InstructionType::type_break:
			m_nCurIp = processBreakInstruction(m_nCurIp, dynamic_cast<BreakInstruction*>(instruction));
			break;
		case InstructionType::type_return:
			m_nCurIp = processReturnInstruction(m_nCurIp, dynamic_cast<ReturnInstruction*>(instruction));
			break;
		default:
			++m_nCurIp;
			break;
		}

		if (m_retCodeFlow == CodeFlow::redo_)
		{
			break;
		}
	}

	return m_retCodeFlow;
}

int asys::FunctionExecutable::processDoInstruction(CodeFlow& retCode, int curIp, DoInstruction* expressInstruction)
{
	if (!expressInstruction->express)
	{
		retCode = CodeFlow::next_;
		return curIp + 1;
	}

	setReturnCodeFlow(CodeFlow::next_);
	expressInstruction->express(this);

	retCode = getReturnCodeFlow();

	if (retCode == CodeFlow::redo_)
	{
		return curIp;
	}

	if (retCode == CodeFlow::next_)
	{
		return curIp + 1;
	}

	if (retCode == CodeFlow::break_
		|| retCode == CodeFlow::continue_)
	{
		for (int ip = curIp - 1; ip >= 0; ip--)
		{
			auto instruction = m_instructions[ip];
			if (instruction->instructionType() == InstructionType::type_while)
			{
				auto whileInstruction = dynamic_cast<WhileInstruction*>(instruction);
				if (retCode == CodeFlow::continue_)
				{
					return ip;
				}
				return whileInstruction->endWhileIp + 1;
			}
		}
	}

	if (retCode == CodeFlow::return_)
	{
		return static_cast<int>(m_instructions.size());
	}

	return curIp + 1;
}

int asys::FunctionExecutable::processCallInstruction(CodeFlow& retCode, int curIp, CallInstruction* callInstruction)
{
	retCode = CodeFlow::next_;

	if (!callInstruction->executable)
	{
		//get the real code if it's a dynamic call.
		if (!callInstruction->codeName.empty())
		{
			auto& dynamicCodes = getDynamicCodes();
			auto it = dynamicCodes.find(callInstruction->codeName);
			if (it != dynamicCodes.end())
			{
				callInstruction->code = it->second;
			}
		}

		if (!callInstruction->code) return curIp + 1;

		callInstruction->executable = callInstruction->code->compile();
		callInstruction->executable->retain();

		//pass input arguments to the invoked code.
		for (const auto& pair : callInstruction->inputParams)
		{
			if (isValidVariableName(pair.second))
			{
				callInstruction->executable->setValue(pair.first, getValue(pair.second));
			}
			else
			{
				callInstruction->executable->setValue(pair.first, pair.second);
			}
		}
	}

	retCode = callInstruction->executable->run();

	if (retCode == CodeFlow::redo_) return curIp;

	//fetch outputs from invoked code.
	for (const auto& pair : callInstruction->outputParams)
	{
		setValue(pair.first, callInstruction->executable->getValue(pair.second));
	}

	callInstruction->executable->release();
	callInstruction->executable = nullptr;
	if (!callInstruction->codeName.empty())
	{
		callInstruction->code = nullptr;
	}

	return curIp + 1;
}

int asys::FunctionExecutable::processReturnInstruction(int curIp, ReturnInstruction* retInstruction)
{
	for (int i = 0; i < static_cast<int>(retInstruction->outputParams.size()); ++i)
	{
		const auto& param = retInstruction->outputParams[i];
		if (asys::isValidVariableName(param))
		{
			setOutoutValue(i, getValue(retInstruction->outputParams[i]));
		}
		else
		{
			setOutoutValue(i, param);
		}
	}

	return static_cast<int>(m_instructions.size());
}

void asys::BreakPoint::operator()(const std::function<void(Executable*, const BreakPoint& breakpoint)>& callback, const char* fileName /*= nullptr*/, const char* functionName /*= nullptr*/, int lineNumber /*= -1*/)
{
	m_callback = callback;
	if (fileName) m_fileName = fileName;
	if (functionName) m_functionName = functionName;
	m_lineNumber = lineNumber;
}
