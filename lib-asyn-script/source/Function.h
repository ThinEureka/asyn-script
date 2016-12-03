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
#include "AsysVariableT.h"
#include "Debug.h"

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
		const char* fileName() const { return m_fileName; }
		const char* functionName() const { return m_functionName; }
		int lineNumber() const { return m_lineNumber; }

		void operator ()(const std::function<void(FunctionExecutable*, const BreakPoint&, Context*)>& callback,
			const char* fileName = nullptr,
			const char* functionName = nullptr,
			int lineNumber = -1);

		const std::function<void(FunctionExecutable*, const BreakPoint&, Context*)>& callback() const { return m_callback; }

	private:
		Instruction* m_instruction{ nullptr };
		const char* m_fileName{};
		const char* m_functionName{};
		int m_lineNumber{ -1 };
		std::function<void(FunctionExecutable*, const BreakPoint&, Context*)> m_callback;
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
		CallInstruction(const std::function<void(Executable* caller, Executable* callee)> outputCallback, const std::function<void(Executable* caller, Executable* callee)> inputCallback, Code* code, const std::function<Code*(Executable* caller)> getCodeCallback= nullptr)
			: Instruction(InstructionType::type_call)
			, outputCallback(outputCallback)
			, inputCallback(inputCallback)
			, getCodeCallback(getCodeCallback)
			, code(code)
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
			auto instruction = new CallInstruction(outputCallback, inputCallback, code, getCodeCallback);
			instruction->setBreakPoint(breakPoint());
			return instruction;
		}

	public:
		std::function<void(Executable* caller, Executable* callee)> outputCallback{};
		std::function<void(Executable* caller, Executable* callee)> inputCallback{};
		std::function<Code*(Executable* caller)> getCodeCallback{};
		Executable* executable{};
		Code* code{};
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
		ReturnInstruction(const std::function<void(asys::Executable*)>& returnCallback) : Instruction(InstructionType::type_return), returnCallback(returnCallback){};

		Instruction* clone() const override
		{
			auto copy = new ReturnInstruction(returnCallback);

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		std::function<void(asys::Executable*)> returnCallback{};
	};

	class FunctionCode : public Code
	{
	public:
		FunctionCode();
		virtual ~FunctionCode();

		BreakPoint& Do(const std::function<void(Executable*)>& express);
		BreakPoint& Declare(AsysVariable& var);

		template<typename ...Args>
		BreakPoint& Declare(AsysVariable& var, Args&... args)
		{
			auto& breakPoint = Declare(var);
			Declare(args...);

			return breakPoint;
		}

		template<typename ...Args>
		BreakPoint& Input(Args&... args)
		{
			auto& breakPoint = Declare(args...);
			Input_ex(0, args...);
			return breakPoint;
		}

		BreakPoint& Input()
		{
			return Do(nullptr);
		}

		//CALL returns multiple variable from the called function, which in turn are assigned to the outputParams.
		BreakPoint& Call(const VariableList& outputs, const ValueList& inputs, Code* code);
		BreakPoint& Call(const VariableList& outputs, const ValueList& inputs, const AsysVariable& code);

		template<typename V>
		BreakPoint& If(const AsysVariableT<V>& var)
		{
			return If_ex([=](Executable* executable){
				auto pCastValue = dynamic_cast<AsysValueT<V>*>(executable->getAsysValue(var));
				return pCastValue->getNativeValue();
			});
		}

		BreakPoint& If_ex(const std::function<bool(Executable*)>& express);

		template<typename V>
		BreakPoint& If_not(const AsysVariableT<V>& var)
		{
			return If_ex([=](Executable* executable){
				auto pCastValue = dynamic_cast<AsysValueT<V>*>(executable->getAsysValue(var));
				return !pCastValue->getNativeValue();
			});
		}

		BreakPoint& Else();
		BreakPoint& End_if();

		template<typename V>
		BreakPoint& While(const AsysVariableT<V>& var)
		{
			return While_ex([=](Executable* executable){
				auto pCastValue = dynamic_cast<AsysValueT<V>*>(executable->getAsysValue(var));
				return pCastValue->getNativeValue();
			});
		}

		BreakPoint& While_ex(const std::function<bool(Executable*)>& express);

		template<typename V>
		BreakPoint& While_not(const AsysVariableT<V>& var)
		{
			return While_ex([=](Executable* executable){
				auto pCastValue = dynamic_cast<AsysValueT<V>*>(executable->getAsysValue(var));
				return !pCastValue->getNativeValue();
			});
		}

		BreakPoint& End_while();
		BreakPoint& Continue();
		BreakPoint& Break();

		BreakPoint& Return();
		BreakPoint& Return(const ValueList& vars);

		template<typename V1, typename V2>
		BreakPoint& Assign(const AsysVariableT<V1>& var1, const AsysVariableT<V2>& var2)
		{
			return Do([=](asys::Executable* executable){
				executable->setAsysValue(var1, executable->getAsysValue(var2));
			});
		}

		template<typename V, typename C>
		BreakPoint& Assign(const AsysVariableT<V>& var, const C& constValue)
		{
			return Do([=](asys::Executable* executable){
				auto* pCastValue = dynamic_cast<AsysValueT<V>*>(executable->getAsysValue(var));
				pCastValue->getNativeValueReference() = constValue;
			});
		}

		Executable* compile() override;

		void clear();

	private:
		BreakPoint& Call_ex(const std::function<void(Executable* caller, Executable* callee)> outputCallback, const std::function<void(Executable* caller, Executable* callee)> inputCallback, Code* code, const std::function<Code*(Executable* caller)> getCodeCallback = nullptr);
		BreakPoint& Return_ex(const std::function<void(asys::Executable*)>& returnCallback);

		template<typename ...Args>
		void Input_ex(int inputIndex, AsysVariable& var, Args&... args)
		{
			Do([=](asys::Executable* executable){
				auto pAsysValue = executable->getInput(inputIndex);
				if (pAsysValue)
				{
					executable->setAsysValue(var, pAsysValue);
				}
			});

			Input_ex(inputIndex + 1, args...);
		}

		template<typename ...Args>
		void Input_ex(int)
		{}

	private:
		std::vector<Instruction*> m_instructions;
		std::list<int> m_unmatchedIfIps;
		std::list<int> m_unmatchedWhileIps;

		std::string m_fileNameForCurInstruction;
		int m_lineNumerForCurInstruction{ -1 };
		std::string m_functionNameForCurInstruction;
		bool m_bCompiled{};
	};

	class FunctionExecutable : public Executable
	{
	public:
		friend class Debugger;

	public:
		FunctionExecutable(const std::vector<Instruction*> instructions, const StackStructure& stackStructure);
		virtual ~FunctionExecutable();

		CodeFlow run(Context* context = nullptr) override;

		void attachDebugger(Debugger* debugger, DebugInfo* parentDebugInfo = nullptr);
		void detachDebugger();

	private:
		int processNullInstruction(int curIp, Context* context) {return curIp + 1;}

		int processDoInstruction(CodeFlow& retCode, int curIp, DoInstruction* expressInstruction, Context* context);
		int processCallInstruction(CodeFlow& retCode, int curIp, CallInstruction* callInstruction, Context* context);

		int processIfInstruction(int curIp, IfInstruction* ifInstruction, Context* context)
		{
			bool condition = ifInstruction->express(this);
			if (condition) return curIp + 1;

			if (ifInstruction->elseIp != INVALID_IP) return ifInstruction->elseIp + 1;

			return ifInstruction->endIfIp + 1;
		}

		int processElseInstruction(int curIp, ElseInstruction* elseInstruction, Context* context) { return elseInstruction->endIfIp + 1; }
		int processEndIfInstruction(int curIp, EndIfInstruction* endIfInstruction, Context* context) { return curIp + 1; }

		int processWhileInstruction(int curIp, WhileInstruction* whileInstruction, Context* context)
		{
			bool condition = whileInstruction->express(this);
			if (condition) return curIp + 1;

			return whileInstruction->endWhileIp + 1;
		}

		int processEndWhileInstruction(int curIp, EndWhileInstruction* endWhileInstruction, Context* context) { return endWhileInstruction->whileIp; }
		int processContinueInstruction(int curIp, ContinueInstruction* continueInstruction, Context* context) { return continueInstruction->whileIp; }
		int processBreakInstruction(int curIp, BreakInstruction* breakInstruction, Context* context)
		{
			auto whileIp = breakInstruction->whileIp;
			auto whileInstruction = dynamic_cast<WhileInstruction*>(m_instructions[whileIp]);
			return whileInstruction->endWhileIp + 1;
		}

		int processReturnInstruction(int curIp, ReturnInstruction* retInstruction, Context* context);

		CodeFlow processBreakpoint(const BreakPoint& breakpoint, Context* context);

	private:
		std::vector<Instruction*> m_instructions;
		DebugInfo* m_pDebugInfo{};
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
