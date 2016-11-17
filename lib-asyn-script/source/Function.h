/*!
 * \file Function.h
 * \date 02-10-2016 15:29:48
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include "Core.h"
#include <vector>
#include <list>
#include <functional>


namespace asys
{
	class FunctionExecutable;
	class FunctionCode;

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

	const int INVALID_IP = -1;

	class Instruction;

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
		const std::string& fileName() const { return m_fileName; }
		const std::string& functionName() const { return m_functionName; }
		int lineNumber() const { return m_lineNumber; }

		void operator ()(const std::function<void(Executable*, const BreakPoint& breakpoint)>& callback,
			const char* fileName = nullptr,
			const char* functionName = nullptr,
			int lineNumber = -1);

		const std::function<void(Executable*, const BreakPoint& breakpoint)>& callback() const { return m_callback; }

	private:
		Instruction* m_instruction{ nullptr };
		std::string m_fileName;
		std::string m_functionName;
		int m_lineNumber{ -1 };
		std::function<void(Executable*, const BreakPoint& breakpoint)> m_callback;
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
		DoInstruction(const std::function<void(Executable*)>& express) : Instruction(InstructionType::type_do), express(express){}

		Instruction* clone() const override
		{
			auto instruction = new DoInstruction(express);
			instruction->setBreakPoint(breakPoint());
			return instruction;
		}

	public:
		std::function<void(Executable*)> express{};
	};

	class CallInstruction : public Instruction
	{
	public:
		CallInstruction(const std::vector<std::pair<std::string, std::string>>& outputParams, const std::vector<std::pair<std::string, std::string>>& inputParams, Code* code, const std::string& codeName)
			: Instruction(InstructionType::type_call)
			, outputParams(outputParams)
			, inputParams(inputParams)
			, code(code)
			, codeName(codeName)
		{}

		virtual ~CallInstruction()
		{
			if (executable)
			{
				executable->release();
				executable = nullptr;
			}
		}

		Instruction* clone() const override 
		{
			auto instruction = new CallInstruction(outputParams, inputParams, code, codeName);
			instruction->setBreakPoint(breakPoint());
			return instruction;
		}

	public:
		std::vector<std::pair<std::string, std::string>> outputParams;
		std::vector<std::pair<std::string, std::string>> inputParams;
		Executable* executable{};
		Code* code{};
		std::string codeName;
	};

	class IfInstruction : public Instruction
	{
	public:
		IfInstruction(const std::function<bool(Executable*)>& express) : Instruction(InstructionType::type_if), express(express) {};

		Instruction* clone() const override 
		{ 
			auto copy = new IfInstruction(express);
			copy->elseIp = elseIp;
			copy->endIfIp = endIfIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		std::function<bool(Executable*)> express;
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
		WhileInstruction(const std::function<bool(Executable*)>& express) : Instruction(InstructionType::type_while), express(express) {};

		Instruction* clone() const override
		{
			auto copy = new WhileInstruction(express);
			copy->endWhileIp = endWhileIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		std::function<bool(Executable*)> express;
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
		ReturnInstruction(const std::vector<std::string>& outputParams) : Instruction(InstructionType::type_return), outputParams(outputParams){};

		Instruction* clone() const override
		{
			auto copy = new ReturnInstruction(outputParams);

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		std::vector<std::string> outputParams;
	};

	class FunctionCode : public Code
	{
	public:
		FunctionCode();
		virtual ~FunctionCode();

		//It's easier to debug the code using DO and lamda expression than using ASSIGN or OPERATE
		//and the expressions written in C++ in the lamda expression are more expressive than pure asyn-script codes.
		BreakPoint& Do(const std::function<void(Executable*)>& express);

		//CALL returns multiple variable from the called function, which in turn are assigned to the outputParams.
		BreakPoint& Call(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, Code* code);
		
		//If codeName is const which means it's not a variable name, the real code to be called is determined when FunctionCode is compiled; 
		//In this case, codeName is used to look up the code as key in the dynamic codes table, thus the code should be registered
		//before compile() being called. If codeName is a valid variable name(starts with $), the code invoked would be the one registered with the name 
		//stored in the code Name variable when the call instruction is executed.
		BreakPoint& Call(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, const std::string& codeName);

		BreakPoint& Input(const std::vector<std::string>& inputParams);

		BreakPoint& If(const std::string& var);
		BreakPoint& If_ex(const std::function<bool(Executable*)>& express);
		BreakPoint& If_not(const std::string& var);
		BreakPoint& If_equal(const std::string& var1, const std::string& var2);
		BreakPoint& If_not_equal(const std::string& var1, const std::string& var2);

		BreakPoint& Else();
		BreakPoint& End_if();

		BreakPoint& While(const std::string& var);
		BreakPoint& While_ex(const std::function<bool(Executable*)>& express);
		BreakPoint& While_not(const std::string& var);
		BreakPoint& While_equal(const std::string& var1, const std::string& var2);
		BreakPoint& While_not_equal(const std::string& var1, const std::string& var2);

		BreakPoint& End_while();
		BreakPoint& Continue();
		BreakPoint& Break();

		BreakPoint& Return() { return Return({}); }
		BreakPoint& Return(const std::vector<std::string>& vars);

		//It's easier to debug the code using EXPRESS and lamda expression than using ASSIGN or OPERATE
		//and the expressions written in C++ in the lamda expression are more expressive than pure asyn-script codes.

		//var1 should starts with $, if not it's a const, which can't be assigned.
		//if var2 starts with $, the value in the variables table with key var2 will be 
		//assigned when the generated instruction is invoked, or var2 itself will be assigned instead.
		BreakPoint& Assign(const std::string& var1, const std::string& var2);

		BreakPoint& Operate(const std::string& output, const std::string& var1, const std::string& var2, Operator eOperator);
		BreakPoint& Operate(const std::string& output, const std::string& var, Operator eOperator);

		Executable* compile() override;

		void registerDynamicCode(const std::string& name, Code* code) { m_dynamicCodes[name] = code; }
		void unregisterDynamicCode(const std::string& name) { m_dynamicCodes.erase(name); }
		void unregisterAllDynamicCodes() { m_dynamicCodes.clear(); }

		void clear();

	private:
		BreakPoint& Call_ex(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, Code* code, const std::string& codeName);

		//CALL_EX uses pairs of variable names to pass values when invoking or returning from the sub-function.
		BreakPoint& Call_ex(const std::vector<std::pair<std::string, std::string>>& outputParams, const std::vector<std::pair<std::string, std::string>>&inputParams, Code* code, const std::string& codeName);

	private:
		std::vector<Instruction*> m_instructions;
		std::list<int> m_unmatchedIfIps;
		std::list<int> m_unmatchedWhileIps;
		std::map<std::string, Code*> m_dynamicCodes;
	};

	class FunctionExecutable : public Executable
	{
	public:
		FunctionExecutable(const std::vector<Instruction*> instructions, const std::map<std::string, Code*> dynamicCodes);
		virtual ~FunctionExecutable();

		CodeFlow run() override;

	private:
		int processNullInstruction(int curIp) {return curIp + 1;}

		int processDoInstruction(CodeFlow& retCode, int curIp, DoInstruction* expressInstruction);
		int processCallInstruction(CodeFlow& retCode, int curIp, CallInstruction* callInstruction);

		int processIfInstruction(int curIp, IfInstruction* ifInstruction)
		{
			bool condition = ifInstruction->express(this);
			if (condition) return curIp + 1;

			if (ifInstruction->elseIp != INVALID_IP) return ifInstruction->elseIp + 1;

			return ifInstruction->endIfIp + 1;
		}

		int processElseInstruction(int curIp, ElseInstruction* elseInstruction) { return elseInstruction->endIfIp + 1; }
		int processEndIfInstruction(int curIp, EndIfInstruction* endIfInstruction) { return curIp + 1; }

		int processWhileInstruction(int curIp, WhileInstruction* whileInstruction)
		{
			bool condition = whileInstruction->express(this);
			if (condition) return curIp + 1;

			return whileInstruction->endWhileIp + 1;
		}

		int processEndWhileInstruction(int curIp, EndWhileInstruction* endWhileInstruction) { return endWhileInstruction->whileIp; }
		int processContinueInstruction(int curIp, ContinueInstruction* continueInstruction) { return continueInstruction->whileIp; }
		int processBreakInstruction(int curIp, BreakInstruction* breakInstruction)
		{
			auto whileIp = breakInstruction->whileIp;
			auto whileInstruction = dynamic_cast<WhileInstruction*>(m_instructions[whileIp]);
			return whileInstruction->endWhileIp + 1;
		}

		int processReturnInstruction(int curIp, ReturnInstruction* retInstruction);

	private:
		std::vector<Instruction*> m_instructions;
		int m_nCurIp{ INVALID_IP  + 1};
		CodeFlow m_retCodeFlow{CodeFlow::next_};
	};

	class FunctionMap : public std::map <std::string, asys::FunctionCode*>
	{
	public:
		virtual ~FunctionMap()
		{
			for (auto& pair : *this)
			{
				if (pair.second)
				{
					delete pair.second;
				}
			}

			clear();
		}
	};
}