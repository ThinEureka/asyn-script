
/*!
 * \file Instructions.cpp
 * \date 12-21-2016 10:51:17
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#include "Instructions.h"

asys::BreakPoint& asys::BreakPoint::operator()(const std::function<void(Machine*, const BreakPoint&)>& callback, const char* fileName /*= nullptr*/, const char* functionName /*= nullptr*/, int lineNumber /*= -1*/)
{
	m_callback = callback;
	m_fileName = fileName;
	m_functionName = functionName;
	m_lineNumber = lineNumber;

	return *this;
}

asys::BreakPoint& asys::BreakPoint::operator>>=(const VariableList& variableList)
{
	CallInstruction* callInstruction = dynamic_cast<CallInstruction*>(m_instruction);
	if (callInstruction)
	{
		callInstruction->setOutputs(variableList);
	}

	return *this;
}
