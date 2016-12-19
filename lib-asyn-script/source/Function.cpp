
/*!
 * \file Function.cpp
 * \date 02-10-2016 15:29:51
 *
 *
 * \author cs 04nycs@gmail.com
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

void asys::FunctionCode::clear()
{
	for (auto instruction : m_instructions)
	{
		delete instruction;
	}

	m_instructions.clear();
	m_stackStructure.clear();
}

asys::BreakPoint& asys::FunctionCode::Do(const std::function<void(Executable*)>& express)
{
	auto instruction = new DoInstruction(express);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}

asys::BreakPoint& asys::FunctionCode::Declare(AsysVariable& var)
{
	m_stackStructure.declare(var);

	return Do([=](asys::Executable* executable){
		if (executable->hasAsysValue(var))
		{
			executable->desstruct(var);
		}

		executable->construct(var);
	});
}

asys::BreakPoint& asys::FunctionCode::Call(Code* code, const ValueList& inputs)
{
	return Call_ex(nullptr,
		[=](Executable* caller, Executable* callee)->void{
		callee->setInput(inputs, caller);
	},
		code,
		nullptr);
}

asys::BreakPoint& asys::FunctionCode::Call(const AsysVariable& code, const ValueList& inputs)
{
	return Call_ex(nullptr,
		[=](Executable* caller, Executable* callee)->void{
		callee->setInput(inputs, caller);
	},
		nullptr,
		[=](Executable* caller)->Code*{
		return static_cast<Code*>(caller->getAsysValue(code)->toVoidPointer());
	});
}

asys::BreakPoint& asys::FunctionCode::Call_ex(const VariableList& outputs, const ValueList& inputs, Code* code)
{
	return Call_ex([=](Executable* caller, Executable* callee)->void{
		caller->fetchOutput(outputs, callee);
	},
		[=](Executable* caller, Executable* callee)->void{
		callee->setInput(inputs, caller);
	},
		code,
		nullptr);
}

asys::BreakPoint& asys::FunctionCode::Call_ex(const VariableList& outputs, const ValueList& inputs, const AsysVariable& code)
{
	return Call_ex([=](Executable* caller, Executable* callee)->void{
		caller->fetchOutput(outputs, callee);
	},
		[=](Executable* caller, Executable* callee)->void{
		callee->setInput(inputs, caller);
	},
		nullptr,
		[=](Executable* caller)->Code*{
		return static_cast<Code*>(caller->getAsysValue(code)->toVoidPointer());
	});
}

asys::BreakPoint& asys::FunctionCode::Call_ex(const std::function<void(Executable* caller, Executable* callee)> outputCallback, const std::function<void(Executable* caller, Executable* callee)> inputCallback, Code* code, const std::function<Code*(Executable* caller)> getCodeCallback /*= nullptr*/)
{
	auto instruction = new CallInstruction(outputCallback, inputCallback, code, getCodeCallback);
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

asys::BreakPoint& asys::FunctionCode::Return(const ValueList& vars)
{
	return Return_ex([=](asys::Executable* executable){
		executable->setOutput(vars);
	});
}

asys::BreakPoint& asys::FunctionCode::Return_ex(const std::function<void(asys::Executable*)>& returnCallback)
{
	auto instruction = new ReturnInstruction(returnCallback);
	m_instructions.push_back(instruction);

	return instruction->breakPoint();
}

asys::Executable* asys::FunctionCode::compile()
{
	assert(m_unmatchedIfIps.size() == 0);// "Asynscript compile error: There are unmatched ifs in this function.");
	assert(m_unmatchedWhileIps.size() == 0);// "Asynscript compile error: There are unmatched ifs in this function.");

	return new FunctionExecutable(m_instructions, m_stackStructure);
}

asys::FunctionExecutable::FunctionExecutable(const std::vector<Instruction*> instructions, const StackStructure& stackStructure)
	: Executable(stackStructure)
{
	m_instructions.resize(instructions.size());
	BreakPoint* pLastValidBreakPoint = nullptr;
	for (size_t i = 0; i < instructions.size(); ++i)
	{
		m_instructions[i] = instructions[i]->clone();

#if ASYS_DEBUG == 1
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
#endif
	}

#if ASYS_DEBUG == 1
	m_pDebugInfo = new DebugInfo;
	m_pDebugInfo->initialize(this);
#endif
}

asys::FunctionExecutable::~FunctionExecutable()
{
#if ASYS_DEBUG == 1
	delete m_pDebugInfo;
#endif

	for (auto instruction : m_instructions)
	{
		auto callInstruction = dynamic_cast<CallInstruction*>(instruction);
		if (callInstruction && callInstruction->executable)
		{
			callInstruction->executable->release();
			callInstruction->executable = nullptr;
		}

		delete instruction;
	}
	m_instructions.clear();
}

asys::CodeFlow asys::FunctionExecutable::run()
{
	if (isInMainThread(m_context))
	{
		Executable::m_pMainExecutable = this;
	}

	while (true)
	{
		if (m_nCurIp >= static_cast<int>(m_instructions.size()))
		{
			break;
		}

		auto instruction = m_instructions[m_nCurIp];
#if ASYS_BREAKPOINT == 1
		auto codeFlow = processBreakpoint(instruction->breakPoint());
		if (codeFlow == CodeFlow::redo_)
		{
			return m_retCodeFlow = CodeFlow::redo_;
		}
#endif
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

	if (isInMainThread(m_context))
	{
		Executable::m_pMainExecutable = nullptr;
	}

	return m_retCodeFlow;
}

void asys::FunctionExecutable::attachDebugger(Debugger* debugger, DebugInfo* parentDebugInfo /*= nullptr*/)
{
	if (!m_pDebugInfo)
	{
		return;
	}

	m_pDebugInfo->setDebugger(debugger);
	m_pDebugInfo->setParentDebugInfo(parentDebugInfo);

	if (m_nCurIp >= 0 && m_nCurIp < static_cast<int>(m_instructions.size()))
	{
		auto callInstruction = dynamic_cast<CallInstruction*>(m_instructions[m_nCurIp]);
		if (callInstruction && callInstruction->executable)
		{
			auto funExe = dynamic_cast<FunctionExecutable*>(callInstruction->executable);
			if (funExe)
			{
				funExe->attachDebugger(debugger, m_pDebugInfo);
			}
		}
	}
}

void asys::FunctionExecutable::detachDebugger()
{
	if (!m_pDebugInfo)
	{
		return;
	}

	m_pDebugInfo->setDebugger(nullptr);
	if (m_nCurIp >= 0 && m_nCurIp < static_cast<int>(m_instructions.size()))
	{
		auto callInstruction = dynamic_cast<CallInstruction*>(m_instructions[m_nCurIp]);
		if (callInstruction && callInstruction->executable)
		{
			auto funExe = dynamic_cast<FunctionExecutable*>(callInstruction->executable);
			if (funExe)
			{
				funExe->detachDebugger();
			}
		}
	}
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
		if (callInstruction->getCodeCallback)
		{
			callInstruction->code = callInstruction->getCodeCallback(this);
		}

		if (!callInstruction->code) return curIp + 1;

		callInstruction->executable = callInstruction->code->compile();
		callInstruction->executable->setContext(m_context);
		callInstruction->executable->retain();

#if ASYS_DEBUG == 1
		if (m_pDebugInfo && m_pDebugInfo->getDebugger())
		{
			auto funExe = dynamic_cast<FunctionExecutable*>(callInstruction->executable);
			if (funExe)
			{
				funExe->attachDebugger(m_pDebugInfo->getDebugger());
			}
		}
#endif

		if (callInstruction->inputCallback)
		{
			callInstruction->inputCallback(this, callInstruction->executable);
		}
	}

	retCode = callInstruction->executable->run();

	if (retCode == CodeFlow::redo_) return curIp;

	if (callInstruction->outputCallback)
	{
		callInstruction->outputCallback(this, callInstruction->executable);
	}

	callInstruction->executable->release();
	callInstruction->executable = nullptr;

	if (callInstruction->getCodeCallback)
	{
		callInstruction->code = nullptr;
	}

	if (isInMainThread(m_context))
	{
		Executable::m_pMainExecutable = this;
	}

	return curIp + 1;
}

int asys::FunctionExecutable::processReturnInstruction(int curIp, ReturnInstruction* retInstruction)
{
	if (retInstruction->returnCallback)
	{
		retInstruction->returnCallback(this);
	}
	return static_cast<int>(m_instructions.size());
}

asys::CodeFlow asys::FunctionExecutable::processBreakpoint(const BreakPoint& breakPoint)
{
	auto callback = breakPoint.callback();
	if (callback && m_retCodeFlow != CodeFlow::redo_)
	{
		callback(this, breakPoint, m_context);
	}

	if (m_pDebugInfo)
	{
		if (callback && m_retCodeFlow != CodeFlow::redo_)
		{
			if (breakPoint.lineNumber() != -1)
			{
				m_pDebugInfo->setCurLocation(breakPoint.fileName(), breakPoint.functionName(), breakPoint.lineNumber());
			}
		}

		if (m_pDebugInfo->getDebugger())
		{
			return m_pDebugInfo->getDebugger()->onBreakPoint(this, breakPoint, m_context);
		}
	}

	return CodeFlow::next_;
}

asys::BreakPoint& asys::BreakPoint::operator()(const std::function<void(FunctionExecutable*, const BreakPoint&, Context*)>& callback, const char* fileName /*= nullptr*/, const char* functionName /*= nullptr*/, int lineNumber /*= -1*/)
{
	m_callback = callback;
	m_fileName = fileName;
	m_functionName = functionName;
	m_lineNumber = lineNumber;

	return *this;
}

asys::BreakPoint& asys::BreakPoint::operator >>= (std::initializer_list<AsysVariable> initList)
{
	CallInstruction* callInstruction = dynamic_cast<CallInstruction*>(m_instruction);
	if (callInstruction)
	{
		VariableList varList{ initList };
		callInstruction->setOutputCallback([=](Executable* caller, Executable* callee)->void{
			caller->fetchOutput(varList, callee);
		});
	}

	return *this;
}
