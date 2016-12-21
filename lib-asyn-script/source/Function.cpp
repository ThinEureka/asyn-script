
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

asys::FunctionCode::FunctionCode()
{

}

asys::FunctionCode::~FunctionCode()
{
	clear();
}

asys::BreakPoint& asys::FunctionCode::Do(const std::function<void(Machine*)>& express)
{
	auto instruction = new DoInstruction(express);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
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
	return Return_ex(nullptr);
}

asys::BreakPoint& asys::FunctionCode::Return_ex(const std::function<void(asys::Executable*)>& returnCallback)
{
	auto instruction = new ReturnInstruction(returnCallback);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}
