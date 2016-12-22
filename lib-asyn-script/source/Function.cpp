
/*!
 * \file Function.cpp
 * \date 02-10-2016 15:29:51
 *
 *
 * \author cs 04nycs@gmail.com
 * https://github.com/ThinEureka/asyn-script
 */

#include "Function.h"
#include <assert.h>
#include "Debug.h"
#include "Machine.h"

asys::BreakPoint& asys::FunctionCode::Do(const std::function<void(Machine*)>& express)
{
	auto instruction = new DoInstruction(express);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::Declare(AsysVariable& var)
{
	m_stackFrame.declare(var);

	return Do([=](asys::Machine* asys_this){
		if (asys_this->hasAsysValue(var))
		{
			asys_this->destructValue(var);
		}

		asys_this->constructValue(var);
	});
}

asys::BreakPoint& asys::FunctionCode::Call(const AsysVariable& code, const ValueList& inputs)
{
	auto instruction = new CallInstruction(code, inputs);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::Call(FunctionCode* code, const ValueList& inputs)
{
	auto instruction = new CallInstruction(code, inputs);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::If_ex(const std::function<bool(Machine*)>& express)
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

asys::BreakPoint& asys::FunctionCode::While_ex(const std::function<bool(Machine*)>& express)
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

	whileInstruction->endWhileIp = ip;
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

asys::BreakPoint& asys::FunctionCode::Return()
{
	auto instruction = new ReturnInstruction();
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}

void asys::FunctionCode::clear()
{
	for (auto instruction : m_instructions)
	{
		delete instruction;
	}

	m_instructions.clear();
	m_stackFrame.clear();

	m_unmatchedIfIps.clear();
	m_unmatchedWhileIps.clear();

	m_numInputs = 0;
}

void asys::FunctionCode::compile()
{
	assert(m_unmatchedIfIps.size() == 0);// "Asynscript compile error: There are unmatched ifs in this function.");
	assert(m_unmatchedWhileIps.size() == 0);// "Asynscript compile error: There are unmatched ifs in this function.");

#if ASYS_BREAKPOINT == 1
	BreakPoint* pLastValidBreakPoint = nullptr;
	for (size_t i = 0; i < m_instructions.size(); ++i)
	{
		if (!pLastValidBreakPoint)
		{
			pLastValidBreakPoint = &m_instructions[i]->breakPoint();
		}
		else
		{
			if (m_instructions[i]->breakPoint().lineNumber() == -1)
			{
				m_instructions[i]->breakPoint()(nullptr, pLastValidBreakPoint->fileName(), pLastValidBreakPoint->functionName(), pLastValidBreakPoint->lineNumber());
			}
			else
			{
				pLastValidBreakPoint = &m_instructions[i]->breakPoint();
			}
		}
	}
#endif
}

const asys::AsysVariable* asys::FunctionCode::getInputVariable(int index) const
{
	if (index < 0 || index >= m_numInputs)
	{
		return nullptr;
	}

	return m_stackFrame.getVariableByIndex(index);
}

asys::BreakPoint& asys::FunctionCode::Return(const ValueList& vars)
{
	auto instruction = new ReturnInstruction(vars);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}
