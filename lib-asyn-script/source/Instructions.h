﻿/*!
 * \file Instructions.h
 * \date 12-21-2016 10:35:10
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#include <functional>
#include "AsysVariable.h"
#include "Define.h"

namespace asys
{
	class FunctionCode;
	class Machine;
	class CallInstruction;

	enum class InstructionType
	{
		type_null,
		type_do,
		type_call,
		type_if,
		type_else,
		type_endif,
		type_while,
		type_endwhile,
		type_continue,
		type_break,
		type_return,
	};

	class Instruction;
	class FunctionCode;

	class BreakPoint
	{
	public:
		BreakPoint(Instruction* instruction) : m_instruction(instruction) {}

		BreakPoint& operator = (const BreakPoint& breakPoint)
		{
			//m_instruction does not change.
			m_fileName = breakPoint.fileName();
			m_functionName = breakPoint.functionName();
			m_lineNumber = breakPoint.lineNumber();
			m_callback = breakPoint.callback();
			return *this;
		}

		Instruction* instruction() { return m_instruction; }
		const char* fileName() const { return m_fileName; }
		const char* functionName() const { return m_functionName; }
		int lineNumber() const { return m_lineNumber; }

		void setLineNumber(int lineNumber) { m_lineNumber = lineNumber; }

		BreakPoint& operator ()(const std::function<void(Machine*, const BreakPoint&)>& callback,
			const char* fileName = nullptr,
			const char* functionName = nullptr,
			int lineNumber = -1);

		BreakPoint& operator >>= (const VariableList& variableList);

		const std::function<void(Machine*, const BreakPoint&)>& callback() const { return m_callback; }

	private:
		Instruction* m_instruction{ nullptr };
		const char* m_fileName{};
		const char* m_functionName{};
		int m_lineNumber{ -1 };
		std::function<void(Machine*, const BreakPoint&)> m_callback;
	};

	class Instruction
	{
	public:
		Instruction(InstructionType instructionType) : m_instructionType(instructionType) {}
		virtual ~Instruction() {}

		InstructionType instructionType() const { return m_instructionType; }

		const BreakPoint& breakPoint() const { return m_breakPoint; }
		BreakPoint& breakPoint(){ return m_breakPoint; }

		void setBreakPoint(const BreakPoint& breakPoint) { m_breakPoint = breakPoint; }

	private:
		InstructionType m_instructionType;
		BreakPoint m_breakPoint{ this };
	};

	class DoInstruction : public Instruction
	{
	public:
		DoInstruction(const std::function<void(Machine*)>& express) : Instruction(InstructionType::type_do), express(express){}

	public:
		std::function<void(Machine*)> express{};
	};

	class CallInstruction : public Instruction
	{
	public:
		CallInstruction(const FunctionCode* code, const ValueList& inputs)
			: Instruction(InstructionType::type_call)
			, inputs(inputs)
			, code(code)
		{}

		CallInstruction(const AsysVariable& codeVar, const ValueList& inputs)
			: Instruction(InstructionType::type_call)
			, inputs(inputs)
			, codeVar(codeVar)
		{}

		virtual ~CallInstruction()
		{}

		void setOutputs(const VariableList& outputs)
		{
			this->outputs = outputs;
		}

	public:
		VariableList outputs;
		ValueList inputs;
		const FunctionCode* code{};
		AsysVariable codeVar;
	};

	class IfInstruction : public Instruction
	{
	public:
		IfInstruction(const std::function<bool(Machine*)>& express) : Instruction(InstructionType::type_if), express(express) {};

	public:
		std::function<bool(Machine*)> express;
		int elseIp{ INVALID_IP };
		int endIfIp{ INVALID_IP };
	};

	class ElseInstruction : public Instruction
	{
	public:
		ElseInstruction() : Instruction(InstructionType::type_else) {};

	public:
		int ifIp{ INVALID_IP };
		int endIfIp{ INVALID_IP };
	};

	class EndIfInstruction : public Instruction
	{
	public:
		EndIfInstruction() : Instruction(InstructionType::type_endif) {};

	public:
		int ifIp{ INVALID_IP };
		int elseIp{ INVALID_IP };
	};

	class WhileInstruction : public Instruction
	{
	public:
		WhileInstruction(const std::function<bool(Machine*)>& express) : Instruction(InstructionType::type_while), express(express) {};

	public:
		std::function<bool(Machine*)> express;
		int endWhileIp{ INVALID_IP };
	};

	class EndWhileInstruction : public Instruction
	{
	public:
		EndWhileInstruction() : Instruction(InstructionType::type_endwhile) {};

	public:
		int whileIp{ INVALID_IP };
	};

	class ContinueInstruction : public Instruction
	{
	public:
		ContinueInstruction() : Instruction(InstructionType::type_continue) {};

	public: 
		int whileIp{ INVALID_IP };
	};

	class BreakInstruction : public Instruction
	{
	public:
		BreakInstruction() : Instruction(InstructionType::type_break) {};

	public:
		int whileIp{ INVALID_IP };
	};

	class ReturnInstruction : public Instruction
	{
	public: 
		ReturnInstruction(const ValueList& valueList) 
			: Instruction(InstructionType::type_return),
			valueList(valueList){}

		ReturnInstruction() : Instruction(InstructionType::type_return){}

	public:
		ValueList valueList;
	};
}
