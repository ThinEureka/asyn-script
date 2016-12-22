/*!
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
	class FunctionExecutable;
	class FunctionCode;
	class Machine;

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

		BreakPoint& operator ()(const std::function<void(Machine*, const BreakPoint&)>& callback,
			const char* fileName = nullptr,
			const char* functionName = nullptr,
			int lineNumber = -1);

		BreakPoint& operator >>= (const VariableList& valueList);

		const std::function<void(FunctionExecutable*, const BreakPoint&)>& callback() const { return m_callback; }

	private:
		Instruction* m_instruction{ nullptr };
		const char* m_fileName{};
		const char* m_functionName{};
		int m_lineNumber{ -1 };
		std::function<void(FunctionExecutable*, const BreakPoint&)> m_callback;
	};

	class Instruction
	{
	public:
		Instruction(InstructionType instructionType) : m_instructionType(instructionType) {}
		virtual ~Instruction() {}

		InstructionType instructionType() const { return m_instructionType; }

		virtual Instruction* clone() const { return new Instruction(m_instructionType); }

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

		Instruction* clone() const override
		{
			auto instruction = new DoInstruction(express);
			instruction->setBreakPoint(breakPoint());
			return instruction;
		}

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

		Instruction* clone() const override 
		{
			auto instruction = new CallInstruction(outputs, inputs, code, codeVar);
			instruction->setBreakPoint(breakPoint());
			return instruction;
		}

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

		Instruction* clone() const override 
		{ 
			auto copy = new IfInstruction(express);
			copy->elseIp = elseIp;
			copy->endIfIp = endIfIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		std::function<bool(Machine*)> express;
		int elseIp{ INVALID_IP };
		int endIfIp{ INVALID_IP };
	};

	class ElseInstruction : public Instruction
	{
	public:
		ElseInstruction() : Instruction(InstructionType::type_else) {};

		Instruction* clone() const override 
		{ 
			auto copy = new ElseInstruction();
			copy->ifIp = ifIp;
			copy->endIfIp = endIfIp;

			copy->setBreakPoint(breakPoint());
			return copy;
		}

	public:
		int ifIp{ INVALID_IP };
		int endIfIp{ INVALID_IP };
	};

	class EndIfInstruction : public Instruction
	{
	public:
		EndIfInstruction() : Instruction(InstructionType::type_endif) {};

		Instruction* clone() const override
		{
			auto copy = new EndIfInstruction();
			copy->ifIp = ifIp;
			copy->elseIp = elseIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		int ifIp{ INVALID_IP };
		int elseIp{ INVALID_IP };
	};

	class WhileInstruction : public Instruction
	{
	public:
		WhileInstruction(const std::function<bool(Machine*)>& express) : Instruction(InstructionType::type_while), express(express) {};

		Instruction* clone() const override
		{
			auto copy = new WhileInstruction(express);
			copy->endWhileIp = endWhileIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		std::function<bool(Machine*)> express;
		int endWhileIp{ INVALID_IP };
	};

	class EndWhileInstruction : public Instruction
	{
	public:
		EndWhileInstruction() : Instruction(InstructionType::type_endwhile) {};

		Instruction* clone() const override
		{
			auto copy = new EndWhileInstruction();
			copy->whileIp = whileIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		int whileIp{ INVALID_IP };
	};

	class ContinueInstruction : public Instruction
	{
	public:
		ContinueInstruction() : Instruction(InstructionType::type_continue) {};

		Instruction* clone() const override
		{
			auto copy = new ContinueInstruction();
			copy->whileIp = whileIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public: 
		int whileIp{ INVALID_IP };
	};

	class BreakInstruction : public Instruction
	{
	public:
		BreakInstruction() : Instruction(InstructionType::type_break) {};

		Instruction* clone() const override
		{
			auto copy = new BreakInstruction();
			copy->whileIp = whileIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

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

		Instruction* clone() const override
		{
			auto copy = new ReturnInstruction(valueList);

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		ValueList valueList;
	};
}
