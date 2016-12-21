/*!
 * \file Machine.h
 * \date 12-21-2016 10:23:12
 * 
 * 
 * \author cs (04nycs@gmail.com)
 *  https://github.com/ThinEureka/asyn-script
 */

#pragma  once

#include "Define.h"
#include "Instructions.h"
#include <vector>
#include "Stack.h"

namespace asys
{
	class Machine;

	class FunctionRuntime
	{
	private:
		void destruct(Machine* machine);
		void construct(const FunctionCode* funCode, Stack* stack);

	private:
		int m_curIp{ asys::INVALID_IP };
		const FunctionCode* m_pFunction{ nullptr };
		size_t m_baseFrameOffset{ 0 };
		Stack* m_pStack{ nullptr };
		std::vector<std::function<void(asys::Machine*)>>* m_pDeallocators;

	private:
		friend class Machine;
	};

	class Machine
	{
	public:
		static Machine* getCurMainThreadMachine() { return m_pCurMainThreadMachine; }

	public:
		Machine(StackPool* stackPool = nullptr);
		virtual ~Machine();

		void installCode(const FunctionCode* code, const ValueList& valueList);
		CodeFlow run();

		AsysValue* getAsysValue(const AsysVariable& var);
		void setAsysValue(const AsysVariable& var, const AsysValue* pValue);

		int getThreadId() const { return m_threadId; };
		void setThreadId(int threadId) { m_threadId = threadId; }

	private:
		void cleanupRuntime();

		void popFunctionRuntime();
		void pushFunctionRuntime(const FunctionCode* code, FunctionRuntime* caller, const ValueList& valueList);

		void processNullInstruction();

		void processDoInstruction(const DoInstruction* doInstruction);
		void processCallInstruction(const CallInstruction* callInstruction);

		void processIfInstruction(const IfInstruction* ifInstruction);

		void processElseInstruction(const ElseInstruction* elseInstruction);
		void processEndIfInstruction(const EndIfInstruction* endIfInstruction);

		void processWhileInstruction(const WhileInstruction* whileInstruction);

		void processEndWhileInstruction(const EndWhileInstruction* endWhileInstruction);
		void processContinueInstruction(const ContinueInstruction* continueInstruction);
		void processBreakInstruction(const BreakInstruction* breakInstruction);

		void processReturnInstruction(const ReturnInstruction* retInstruction);

		void construct(const AsysVariable& var)
		{
			construct(m_pCurFunRuntime, var);
		}

		void destruct(const AsysVariable& var)
		{
			destruct(m_pCurFunRuntime, var);
		}

		AsysValue* getAsysValue(FunctionRuntime* funRuntime, const AsysVariable& var);

		void construct(FunctionRuntime* funRuntime, const AsysVariable& var);
		void destruct(FunctionRuntime* funRuntime, const AsysVariable& var);

		Stack* getCurStack()
		{
			if (m_stackIndex < 0 ||
				m_stackIndex >= static_cast<int>(m_stacks.size()))
			{
				return nullptr;
			}

			return m_stacks[m_stackIndex];
		}

	private:
		std::vector<FunctionRuntime> m_funRuntimes;
		FunctionRuntime* m_pCurFunRuntime{};
		CodeFlow m_codeFlow{ CodeFlow::next_ };
		std::vector<AsysValue*> m_outputs;
		int m_threadId;

		std::vector<Stack*> m_stacks;
		int m_stackIndex{ -1 };

		bool m_isUsingBuiltInStackPool{ false };
		StackPool* m_pStackPool{ nullptr };

	private:
		static Machine* m_pCurMainThreadMachine;
	};
}