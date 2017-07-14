﻿
/*!
 * \file Machine.cpp
 * \date 12-21-2016 10:23:18
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#include "Machine.h"
#include "Function.h"

asys::Machine* asys::Machine::m_pCurMainThreadMachine = nullptr;

void asys::FunctionRuntime::construct(const FunctionCode* funCode, Stack* stack)
{
	m_pFunction = funCode;
	m_pStack = stack;
	m_frameOffset = stack->getCurFrameOffset();

	m_pFunction->m_stackFrame.constructFrame(m_pStack, m_frameOffset);
}

void asys::FunctionRuntime::destruct(asys::Machine* machine)
{
	if (m_pDeallocators)
	{
		for (const auto& callback : *m_pDeallocators)
		{
			callback(machine);
		}
		delete m_pDeallocators;
	}

	m_pFunction->m_stackFrame.destructFrame(m_pStack, m_frameOffset);
}

asys::Machine::Machine(StackPool* stackPool/* = nullptr*/, bool sharingStackPool/* = false*/)
{
	if (!stackPool)
	{
		stackPool = new DefaultStackPool();
		sharingStackPool = false;
	}
	m_pStackPool = stackPool;
	m_isSharingStackPool = sharingStackPool;

#if ASYS_DEBUG == 1
	m_pDebugInfo = new DebugInfo(this);
#endif
}

asys::Machine::~Machine()
{
	cleanupRuntime();

	for (auto stack : m_stacks)
	{
		m_pStackPool->checkIn(stack);
	}

	m_stacks.clear();
	m_stackIndex = -1;

	if (!m_isSharingStackPool)
	{
		delete m_pStackPool;
	}

#if ASYS_BREAKPOINT == 1
	delete m_pDebugInfo;
#endif
}

asys::CodeFlow asys::Machine::run()
{
	Machine* pOldMainThreadMacine = nullptr;
	if (getThreadId() == THREAD_ID_MAIN)
	{
		pOldMainThreadMacine = m_pCurMainThreadMachine;
		m_pCurMainThreadMachine = this;
	}

	while (m_pCurFunRuntime)
	{
		auto& instructions = m_pCurFunRuntime->m_pFunction->m_instructions;

		if (m_pCurFunRuntime->m_curIp >= 
			static_cast<int>(instructions.size()))
		{
			popFunctionRuntime();
			continue;
		}

		auto instruction = instructions[m_pCurFunRuntime->m_curIp];

#if ASYS_BREAKPOINT == 1
		auto codeFlow = processBreakpoint(instruction->breakPoint());
		if (codeFlow == CodeFlow::redo_)
		{
			if (getThreadId() == THREAD_ID_MAIN)
			{
				m_pCurMainThreadMachine = pOldMainThreadMacine;
			}

			return m_codeFlow = CodeFlow::redo_;
		}
#endif

		m_codeFlow = CodeFlow::next_;

		switch (instruction->instructionType())
		{
		case InstructionType::type_null:
			processNullInstruction();
			break;
		case InstructionType::type_do:
			processDoInstruction(static_cast<DoInstruction*>(instruction));
			break;
		case InstructionType::type_call:
			processCallInstruction(static_cast<CallInstruction*>(instruction));
			break;
		case InstructionType::type_if:
			processIfInstruction(static_cast<IfInstruction*>(instruction));
			break;
		case InstructionType::type_else:
			processElseInstruction(static_cast<ElseInstruction*>(instruction));
			break;
		case InstructionType::type_endif:
			processEndIfInstruction(static_cast<EndIfInstruction*>(instruction));
			break;
		case InstructionType::type_while:
			processWhileInstruction(static_cast<WhileInstruction*>(instruction));
			break;
		case InstructionType::type_endwhile:
			processEndWhileInstruction(static_cast<EndWhileInstruction*>(instruction));
			break;
		case InstructionType::type_continue:
			processContinueInstruction(static_cast<ContinueInstruction*>(instruction));
			break;
		case InstructionType::type_break:
			processBreakInstruction(static_cast<BreakInstruction*>(instruction));
			break;
		case InstructionType::type_return:
			processReturnInstruction(static_cast<ReturnInstruction*>(instruction));
			break;
		default:
			assert(false); //there should be no other instruction types.
			break;
		}

		if (m_codeFlow == CodeFlow::redo_)
		{
			break;
		}
	}

	if (getThreadId() == THREAD_ID_MAIN)
	{
		m_pCurMainThreadMachine = pOldMainThreadMacine;
	}

	return m_codeFlow;
}

void asys::Machine::processDoInstruction(const DoInstruction* doInstruction)
{
	if (!doInstruction->express)
	{
		m_pCurFunRuntime->m_curIp++;
		return;
	}

	m_codeFlow = CodeFlow::next_;
	doInstruction->express(this);

	if (m_codeFlow == CodeFlow::redo_)
	{
		return;
	}

	if (m_codeFlow == CodeFlow::next_)
	{
		m_pCurFunRuntime->m_curIp++;
		return;
	}

	if (m_codeFlow == CodeFlow::break_
		|| m_codeFlow == CodeFlow::continue_)
	{
		auto curIp = m_pCurFunRuntime->m_curIp;
		auto& instructions = m_pCurFunRuntime->m_pFunction->m_instructions;
		for (int ip = curIp - 1; ip >= 0; ip--)
		{
			auto instruction = instructions[ip];
			if (instruction->instructionType() ==
				InstructionType::type_while)
			{
				auto whileInstruction = static_cast<WhileInstruction*>(instruction);
				if (m_codeFlow == CodeFlow::continue_)
				{
					m_pCurFunRuntime->m_curIp = ip;
				}
				else
				{
					m_pCurFunRuntime->m_curIp = whileInstruction->endWhileIp + 1;
				}
				m_codeFlow = CodeFlow::next_;
				return;
			}
		}
	}

	if (m_codeFlow == CodeFlow::return_)
	{
		popFunctionRuntime();
		m_codeFlow = CodeFlow::next_;
	}
}

void asys::Machine::processCallInstruction(const CallInstruction* callInstruction)
{
	auto code = callInstruction->code;
	if (!code)
	{
		auto pAsysValue = getAsysValue(callInstruction->codeVar);
		if (pAsysValue)
		{
			code = static_cast<const FunctionCode*>(pAsysValue->toVoidPointer());
		}
	}

	if (!code)
	{
		m_pCurFunRuntime->m_curIp++;
	}
	else
	{
		pushFunctionRuntime(code, callInstruction->inputs);
	}
}

void asys::Machine::processIfInstruction(const IfInstruction* ifInstruction)
{
	bool condition = ifInstruction->express(this);
	if (condition)
	{
		m_pCurFunRuntime->m_curIp++;
		return;
	}

	if (ifInstruction->elseIp != INVALID_IP)
	{
		m_pCurFunRuntime->m_curIp = ifInstruction->elseIp + 1;
		return;
	}

	m_pCurFunRuntime->m_curIp = ifInstruction->endIfIp + 1;
}

void asys::Machine::processWhileInstruction(const WhileInstruction* whileInstruction)
{
	bool condition = whileInstruction->express(this);
	if (condition)
	{
		m_pCurFunRuntime->m_curIp++;
	}
	else
	{
		m_pCurFunRuntime->m_curIp = whileInstruction->endWhileIp + 1;
	}
}

void asys::Machine::processBreakInstruction(const BreakInstruction* breakInstruction)
{
	auto& instructions = m_pCurFunRuntime->m_pFunction->m_instructions;
	auto whileInstruction = static_cast<WhileInstruction*>(instructions[breakInstruction->whileIp]);
	m_pCurFunRuntime->m_curIp = whileInstruction->endWhileIp + 1;
}

void asys::Machine::processReturnInstruction(const ReturnInstruction* retInstruction)
{
	//for the case that it's the root function
	if (m_funRuntimes.size() == 1)
	{
		m_outputs.resize(retInstruction->valueList.getLength());
		for (int i = 0; i < static_cast<int>(retInstruction->valueList.getLength()); ++i)
		{
			auto innerValue = retInstruction->valueList.getInnerValueByIndex(i);
			if (innerValue->isConst)
			{
				m_outputs[i] = innerValue->asysConst.getAsysValue()->clone();
			}
			else
			{
				auto pAsynValue = getAsysValue(innerValue->asysVarible);
				m_outputs[i] = pAsynValue->clone();
			}
		}
	}
	else
	{
		//the caller's current instruction should be call-instruction.
		auto pCallerRuntime = &m_funRuntimes[m_funRuntimes.size() - 2];
		auto pCallerFunction = pCallerRuntime->m_pFunction;
		const auto& callerInstructions = pCallerFunction->m_instructions;
		auto callInstruction = static_cast<const CallInstruction*>(callerInstructions[pCallerRuntime->m_curIp]);

		for (size_t i = 0; i < static_cast<int>(retInstruction->valueList.getLength()) &&
			i < callInstruction->outputs.getLength(); ++i)
		{
			auto pOutputVar = callInstruction->outputs.getAsysVariable(i);
			auto pOutputValue = getAsysValue(pCallerRuntime, *pOutputVar);

			auto innerValue = retInstruction->valueList.getInnerValueByIndex(i);
			if (innerValue->isConst)
			{
				pOutputValue->assign(*innerValue->asysConst.getAsysValue());
			}
			else
			{
				auto pRetValue = getAsysValue(m_pCurFunRuntime, innerValue->asysVarible);
				pOutputValue->assign(*pRetValue);
			}
		}
	}

	popFunctionRuntime();
}

asys::AsysValue* asys::Machine::getCallerOutputValue(int index)
{
	if (m_funRuntimes.size() < 2)
	{
		return nullptr;
	}

	auto pCallerRuntime = &m_funRuntimes[m_funRuntimes.size() - 2];
	auto pCallerFunction = pCallerRuntime->m_pFunction;
	const auto& callerInstructions = pCallerFunction->m_instructions;
	auto callInstruction = static_cast<const CallInstruction*>(callerInstructions[pCallerRuntime->m_curIp]);

	if (index < 0 || index >= static_cast<int>(callInstruction->outputs.getLength()))
	{
		return nullptr;
	}

	auto pOutputVar = callInstruction->outputs.getAsysVariable(index);
	return getAsysValue(pCallerRuntime, *pOutputVar);
}

asys::AsysValue* asys::Machine::getAsysValue(FunctionRuntime* funRuntime, const AsysVariable& var)
{
	return funRuntime->m_pFunction->m_stackFrame.getValue(funRuntime->m_pStack, funRuntime->m_frameOffset, var);
}

void asys::Machine::constructValue(FunctionRuntime* funRuntime, const AsysVariable& var)
{
	funRuntime->m_pFunction->m_stackFrame.constructValue(funRuntime->m_pStack, funRuntime->m_frameOffset, var);
}

void asys::Machine::destructValue(FunctionRuntime* funRuntime, const AsysVariable& var)
{
	funRuntime->m_pFunction->m_stackFrame.destructValue(funRuntime->m_pStack, funRuntime->m_frameOffset, var);
}

void asys::Machine::cleanupRuntime()
{
	while (m_funRuntimes.size() > 0)
	{
		popFunctionRuntime();
	}

	for (auto output : m_outputs)
	{
		delete output;
	}
	m_outputs.clear();

	m_codeFlow = CodeFlow::next_;
}

void asys::Machine::pushFunctionRuntime(const FunctionCode* code)
{
	if (!code)
	{
		return;
	}

	m_funRuntimes.resize(m_funRuntimes.size() + 1);
	m_pCurFunRuntime = &m_funRuntimes.back();
	m_pCurFunRuntime->m_curIp = 0;

	if (!getCurStack() || getCurStack()->getRemainSize() < code->m_stackFrame.getSize())
	{
		int i = m_stackIndex + 1;
		for (; i < static_cast<int>(m_stacks.size()); ++i)
		{
			auto newStack = m_stacks[i];
			if (newStack->getRemainSize() >= code->m_stackFrame.getSize())
			{
				std::swap(m_stacks[i], m_stacks[m_stackIndex + 1]);
				++m_stackIndex;
				break;
			}
		}

		if (i == static_cast<int>(m_stacks.size()))
		{
			auto stack = m_pStackPool->checkout(code->m_stackFrame.getSize());
			m_stacks.push_back(stack);
			std::swap(m_stacks[i], m_stacks[m_stackIndex + 1]);
			++m_stackIndex;
		}
	}

	m_pCurFunRuntime->construct(code, getCurStack());

#if ASYS_BREAKPOINT == 1
	if (m_pDebugInfo)
	{
		m_pDebugInfo->onPushCallStack();
	}
#endif
}

void asys::Machine::setupInputs(const ValueList& valueList)
{
	auto pFunction = m_pCurFunRuntime->m_pFunction;
	for (int i = 0; i < pFunction->getNumInputs(); ++i)
	{
		auto pInputVar = pFunction->getInputVariable(i);
		constructValue(m_pCurFunRuntime, *pInputVar);

		if (i < static_cast<int>(valueList.getLength()))
		{
			auto innerValue = valueList.getInnerValueByIndex(i);
			if (innerValue)
			{
				auto pInputValue = getAsysValue(m_pCurFunRuntime, *pInputVar);
				if (innerValue->isConst)
				{
					pInputValue->assign(*innerValue->asysConst.getAsysValue());
				}
				else
				{
					if (m_funRuntimes.size() >= 2)
					{
						auto caller = &m_funRuntimes[m_funRuntimes.size() - 2];
						auto pRealArguemntValue = getAsysValue(caller, innerValue->asysVarible);
						pInputValue->assign(*pRealArguemntValue);
					}
					else
					{
						assert(false);
					}
				}
			}
		}
	}
}

void asys::Machine::popFunctionRuntime()
{
	m_pCurFunRuntime->destruct(this);

	if (m_funRuntimes.size() == 1)
	{
		--m_stackIndex;
	}
	else
	{
		if (m_funRuntimes[m_funRuntimes.size() - 2].m_pStack != getCurStack())
		{
			--m_stackIndex;
		}
	}

	m_funRuntimes.pop_back();

	if (m_funRuntimes.size() > 0)
	{
		m_pCurFunRuntime = &m_funRuntimes.back();
		m_pCurFunRuntime->m_curIp++;
	}
	else
	{
		m_pCurFunRuntime = nullptr;
	}

#if ASYS_BREAKPOINT == 1
	if (m_pDebugInfo)
	{
		m_pDebugInfo->onPopCallStack();
	}
#endif
}

const asys::AsysValue* asys::Machine::getOutput(int index)
{
	if (index < 0 || index >= static_cast<int>(m_outputs.size()))
	{
		return nullptr;
	}

	return m_outputs[index];
}

void asys::Machine::addDeallocator(std::function<void(asys::Machine*)> deallocator)
{
	if (!m_pCurFunRuntime->m_pDeallocators)
	{
		m_pCurFunRuntime->m_pDeallocators = new std::vector<std::function<void(asys::Machine*)>>;
	}

	m_pCurFunRuntime->m_pDeallocators->push_back(deallocator);
}

asys::AsysValue* asys::Machine::getInput(int index)
{
	auto pAsysVar = m_pCurFunRuntime->m_pFunction->getInputVariable(index);
	if (!pAsysVar)
	{
		return nullptr;
	}

	return getAsysValue(*pAsysVar);
}

#if ASYS_BREAKPOINT == 1

asys::CodeFlow asys::Machine::processBreakpoint(const BreakPoint& breakPoint)
{
#if ASYS_DEBUG == 1
	const auto& callback = breakPoint.callback();
	if (callback && m_codeFlow != CodeFlow::redo_)
	{
		callback(this, breakPoint);
	}
#endif

	if (m_pDebugInfo)
	{
		m_pDebugInfo->onBreakPoint(breakPoint);
	}

	if (m_pDebugger)
	{
		return m_pDebugger->onBreakPoint(breakPoint, m_codeFlow, m_funRuntimes.size() - 1);
	}

	return CodeFlow::next_;
}

asys::Debugger* asys::Machine::getDebugger()
{
	return m_pDebugger;
}

void asys::Machine::attachDebugger(Debugger* debugger)
{
	if (m_pDebugger == debugger)
	{
		return;
	}

	delete m_pDebugger;
	m_pDebugger = debugger;

	if (m_pDebugger)
	{
		m_pDebugger->setMachine(this); 
	}

	if (m_pDebugger && !m_pDebugInfo)
	{
		m_pDebugInfo = new DebugInfo(this);
	}
	else if (!m_pDebugger && m_pDebugInfo)
	{
#if ASYS_DEBUG == 0
		delete m_pDebugInfo;
		m_pDebugInfo = nullptr;
#endif
	}
}

void asys::Machine::setDoInstructionLineNumber(asys::CodeFlow codeFlow, int lineNumber)
{
	if (m_pDebugger)
	{
		return m_pDebugger->onDoInstructionLineNumber(codeFlow, lineNumber);
	}
}

#endif