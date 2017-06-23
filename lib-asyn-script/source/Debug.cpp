
/*!
 * \file Debug.cpp
 * \date 12-1-2016 15:24:50
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#include "Machine.h"
#include "Debug.h"
#include "Function.h"

asys::CallInfo::~CallInfo()
{
	for (auto& var : m_variables)
	{
		delete var;
	}
	m_variables.clear();
}

void asys::CallInfo::init(Machine* machine, int callIndex)
{
	auto pFunRuntime = &machine->m_funRuntimes[callIndex];

	pFunRuntime->m_pFunction->m_stackFrame.forEachVariable([=](AsysVariable* var){
		if (!machine->hasAsysValue(*var))
		{
			machine->constructValue(pFunRuntime, *var);
		}

		auto viewer = var->createVariableViewer(machine->getAsysValue(pFunRuntime, *var));
		m_variables.push_back(viewer);
	});

	updateBreakPoint(machine, callIndex);
}

void asys::CallInfo::updateBreakPoint(Machine* machine, int callIndex)
{
	auto pFunRuntime = &machine->m_funRuntimes[callIndex];
	const auto& instructions = pFunRuntime->m_pFunction->m_instructions;
	if (pFunRuntime->m_curIp >= static_cast<int>(instructions.size()))
	{
		m_pCurBreakpoint = nullptr;
	}
	else
	{
		auto instruction = instructions[pFunRuntime->m_curIp];
		m_pCurBreakpoint = &instruction->breakPoint();
	}
}

asys::DebugInfo::DebugInfo(Machine* machine)
	: m_pMachine(machine)
{
	generateCallStack();
}

asys::DebugInfo::~DebugInfo()
{
	for (auto callDebugInfo : m_callStack)
	{
		delete callDebugInfo;
	}
	m_callStack.clear();
}

void asys::DebugInfo::onPushCallStack()
{
	pushCallInfo(m_pMachine->m_funRuntimes.size() - 1);
}

void asys::DebugInfo::onPopCallStack()
{
	popCallInfo();
}

void asys::DebugInfo::onBreakPoint(const BreakPoint& breakPoint)
{
	if (m_pCurBreakPoint != &breakPoint)
	{
		m_callStack.back()->updateBreakPoint(m_pMachine, m_pMachine->m_funRuntimes.size() - 1);
		m_pCurBreakPoint = &breakPoint;
	}
}

void asys::DebugInfo::generateCallStack()
{
	for (int index = 0; index < static_cast<int>(m_pMachine->m_funRuntimes.size()); ++index)
	{
		pushCallInfo(index);
	}
}

void asys::DebugInfo::pushCallInfo(int index)
{
	CallInfo* pCallInfo = new CallInfo;
	pCallInfo->init(m_pMachine, index);
	m_callStack.push_back(pCallInfo);

	m_pLocalVariables = &pCallInfo->m_variables;
}

void asys::DebugInfo::popCallInfo()
{
	CallInfo* pCallInfo = m_callStack.back();
	delete pCallInfo;
	m_callStack.pop_back();

	if (m_callStack.size() > 0)
	{
		m_callStack.back()->updateBreakPoint(m_pMachine, m_callStack.size() - 1);

		m_pLocalVariables = &m_callStack.back()->m_variables;
	}
	else
	{
		m_pLocalVariables = nullptr;
	}
}

const asys::DebugInfo* asys::Debugger::getDebugInfo()
{
#if ASYS_BREAKPOINT == 1
	if (!m_pMachine)
	{
		return nullptr;
	}

	return m_pMachine->getDebugInfo();
#endif

	return nullptr;
	
}
