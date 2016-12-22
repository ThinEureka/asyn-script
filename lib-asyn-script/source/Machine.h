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
	class Variable;
	class FunctionCode;

	void asysRedo(Machine* asys_this);
	void asysNext(Machine* asys_this);
	void asysContinue(Machine* asys_this);
	void asysBreak(Machine* asys_this);

	template<typename ...Args>
	void asysReturn(Machine* asys_this, const Args&... args);

	class FunctionRuntime
	{
	private:
		void destruct(Machine* machine);
		void construct(const FunctionCode* funCode, Stack* stack);

	private:
		int m_curIp{ asys::INVALID_IP };
		const FunctionCode* m_pFunction{ nullptr };
		size_t m_frameOffset{ 0 };
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
		Machine(StackPool* stackPool = nullptr, bool sharingStackPool = false);
		virtual ~Machine();

		void installCode(const FunctionCode* code, const ValueList& valueList)
		{
			cleanupRuntime();
			pushFunctionRuntime(code, nullptr, valueList);
		}

		void installCode(const FunctionCode* code)
		{
			cleanupRuntime();
			pushFunctionRuntime(code, nullptr);
		}

		CodeFlow run();

	private:
		void cleanupRuntime();

		void popFunctionRuntime();
		void pushFunctionRuntime(const FunctionCode* code, FunctionRuntime* caller, const ValueList& valueList)
		{
			pushFunctionRuntime(code, caller);
			setupInputs(valueList, caller);
		}

		void pushFunctionRuntime(const FunctionCode* code, FunctionRuntime* caller);
		void setupInputs(const ValueList& valueList, FunctionRuntime* caller);

		void processNullInstruction()
		{
			m_pCurFunRuntime->m_curIp++;
		}

		void processDoInstruction(const DoInstruction* doInstruction);
		void processCallInstruction(const CallInstruction* callInstruction);

		void processIfInstruction(const IfInstruction* ifInstruction);

		void processElseInstruction(const ElseInstruction* elseInstruction)
		{
			m_pCurFunRuntime->m_curIp = elseInstruction->endIfIp + 1;
		}

		void processEndIfInstruction(const EndIfInstruction* endIfInstruction)
		{
			m_pCurFunRuntime->m_curIp++;
		}

		void processWhileInstruction(const WhileInstruction* whileInstruction);

		void processEndWhileInstruction(const EndWhileInstruction* endWhileInstruction)
		{
			m_pCurFunRuntime->m_curIp = endWhileInstruction->whileIp;
		}

		void processContinueInstruction(const ContinueInstruction* continueInstruction)
		{
			m_pCurFunRuntime->m_curIp = continueInstruction->whileIp;
		}

		void processBreakInstruction(const BreakInstruction* breakInstruction);

		void processReturnInstruction(const ReturnInstruction* retInstruction);

		AsysValue* getAsysValue(FunctionRuntime* funRuntime, const AsysVariable& var);

		void constructValue(FunctionRuntime* funRuntime, const AsysVariable& var);

		void destructValue(FunctionRuntime* funRuntime, const AsysVariable& var);

		Stack* getCurStack()
		{
			if (m_stackIndex < 0 ||
				m_stackIndex >= static_cast<int>(m_stacks.size()))
			{
				return nullptr;
			}

			return m_stacks[m_stackIndex];
		}

		AsysValue* getCallerOutputValue(int index);


		bool hasAsysValue(const AsysVariable& var)
		{
			return getAsysValue(var) != nullptr;
		}

		AsysValue* getAsysValue(const AsysVariable& var)
		{
			if (!m_pCurFunRuntime)
			{
				return nullptr;
			}

			return getAsysValue(m_pCurFunRuntime, var);
		}

		int getThreadId() const { return m_threadId; };
		void setThreadId(int threadId) { m_threadId = threadId; }

		void constructValue(const AsysVariable& var)
		{
			constructValue(m_pCurFunRuntime, var);
		}

		void destructValue(const AsysVariable& var)
		{
			destructValue(m_pCurFunRuntime, var);
		}

		template<typename ...Args>
		void output(const Args&... args)
		{
			if (m_funRuntimes.size() == 1)
			{
				m_outputs.resize(sizeof...(Args));
			}

			output_ex(0, args...);
		} 

		void output()
		{
			//do nothing.
		}

		template<typename ...Args>
		void output_ex(int index, const AsysVariable& var, const Args&... args)
		{
			setOutputByVar(index, var);
			output_ex(index + 1, args...);
		}

		template<typename T, typename ...Args>
		void output_ex(int index, const T& value, const Args&... args)
		{
			AsysValueT<T> asysValue;
			asysValue.setNativeValue(value);

			setOutputByValue(index, asysValue);
			output_ex(index + 1, args...);
		}

		template<typename ...Args>
		void output_ex(int num)
		{
			//do nothing.
		}

		void setOutputByVar(int index, const AsysVariable& var)
		{
			if (m_funRuntimes.size() == 1)
			{
				auto pRetValue = getAsysValue(m_pCurFunRuntime, var);
				m_outputs[index] = pRetValue->clone();
			}
			else
			{
				auto pCallerOutValue = getCallerOutputValue(index);
				if (pCallerOutValue)
				{
					auto pRetValue = getAsysValue(m_pCurFunRuntime, var);
					pCallerOutValue->assign(*pRetValue);
				}
			}
		}

		void setOutputByValue(int index, const AsysValue& value)
		{
			if (m_funRuntimes.size() == 1)
			{
				m_outputs[index] = value.clone();
			}
			else
			{
				auto pCallerOutValue = getCallerOutputValue(index);
				if (pCallerOutValue)
				{
					pCallerOutValue->assign(value);
				}
			}
		}

		void setCodeFlow(CodeFlow codeFlow)
		{
			m_codeFlow = codeFlow;
		}

	private:
		std::vector<FunctionRuntime> m_funRuntimes;
		FunctionRuntime* m_pCurFunRuntime{};

		std::vector<Stack*> m_stacks;
		int m_stackIndex{ -1 };

		StackPool* m_pStackPool{ nullptr };
		bool m_isSharingStackPool{ false };

		std::vector<AsysValue*> m_outputs;

		CodeFlow m_codeFlow{ CodeFlow::next_ };
		int m_threadId{ THREAD_ID_MAIN};

	private:
		static Machine* m_pCurMainThreadMachine;

	private:
		friend class AsysVariable;
		friend class FunctionCode;

		friend void asysRedo(Machine* asys_this);
		friend void asysNext(Machine* asys_this);
		friend void asysBreak(Machine* asys_this);
		friend void asysContinue(Machine* asys_this);

		template<typename ...Args>
		friend void asysReturn(Machine* asys_this, const Args&... args);
	};

	inline void asysContinue(Machine* asys_this)
	{
		asys_this->setCodeFlow(asys::CodeFlow::continue_);
	}

	inline void asysNext(Machine* asys_this)
	{
		asys_this->setCodeFlow(asys::CodeFlow::next_);
	}

	inline void asysRedo(Machine* asys_this)
	{
		asys_this->setCodeFlow(asys::CodeFlow::redo_);
	}

	inline void asysBreak(Machine* asys_this)
	{
		asys_this->setCodeFlow(asys::CodeFlow::break_);
	}

	template<typename ...Args>
	void asysReturn(Machine* asys_this, const Args&... args)
	{
		asys_this->output(args...);
		asys_this->setCodeFlow(asys::CodeFlow::return_);
	}
}

