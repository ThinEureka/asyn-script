
/*!
 * \file Core.h
 * \date 02-10-2016 11:37:20
 * 
 * 
 * \author cs 04nycs@gmail.com
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#include <map>
#include <vector>
#include "Define.h"
#include <functional>
#include "AsysVariable.h"
#include "Stack.h"

namespace asys
{
	class Executable;

	/*class Context
	{
	public:
	virtual int threadId() = 0;
	};*/

	/*class Code
	{
	public:
		virtual Executable* compile() = 0;

	protected:
		StackStructure m_stackStructure;
	};*/

	/*class Executable
	{
	public:
		static Executable* getMainExecutable()
		{
			return m_pMainExecutable;
		}

	public:
		Executable(const StackStructure& stackStructure)
			: m_stack(stackStructure)
		{

		}

		virtual ~Executable()
		{
			for (auto& deallocator : m_deallocators)
			{
				deallocator(this);
			}

			clearInputs();
			clearOutputs();
		}

	public:
		virtual CodeFlow run() = 0;

		void release()
		{
			--m_nReferenceCount;
			if (m_nReferenceCount <= 0)
			{
				delete this;
			}
		}

		void construct(const AsysVariable& var)
		{
			m_stack.construct(var);
		}

		void desstruct(const AsysVariable& var)
		{
			m_stack.destruct(var);
		}

		void retain() { ++m_nReferenceCount; }

		void setAsysValue(const AsysVariable& var, const AsysValue* pValue)
		{
			getAsysValue(var)->assign(*pValue);
		}

		const AsysValue* getAsysValue(const AsysVariable& var) const
		{
			return m_stack.getMetaVariable(var)->getAsysValue();
		}

		AsysValue* getAsysValue(const AsysVariable& var)
		{
			return m_stack.getMetaVariable(var)->getAsysValue();
		}

		void setInput(const ValueList& vars, Executable* executable);
		void setInput(const ValueList& vars)
		{
			setInput(vars, nullptr);
		}

		void setOutput(const ValueList& vars);

		void fetchOutput(const VariableList& vars, Executable* callee);

		const AsysValue* getOutput(int index) const;
		const AsysValue* getInput(int index) const;

		bool hasAsysValue(const AsysVariable& var) const
		{
			return m_stack.hasAsysValue(var);
		}

		void addDeallocator(const std::function<void(asys::Executable*)>& deallocator)
		{
			m_deallocators.push_back(deallocator);
		}

		void setReturnCodeFlow(asys::CodeFlow flow) { m_retCodeFlow = flow; }
		CodeFlow getReturnCodeFlow() const { return m_retCodeFlow; }

		void clearInputs()
		{
			for (auto input : m_inputs)
			{
				delete input;
			}
			m_inputs.clear();
		}

		void clearOutputs()
		{
			for (auto output : m_outputs)
			{
				delete output;
			}
			m_outputs.clear();
		}

		void setContext(Context* context) { m_context = context; }
		const Context* getContext() const { return m_context; }

	protected:
		bool isInMainThread(Context* context)
		{
			return !context || context->threadId() == 0;
		}

	protected:
		int m_nReferenceCount{ 0 };
		CodeFlow m_retCodeFlow{ CodeFlow::next_ };
		std::vector<std::function<void(asys::Executable*)>> m_deallocators;
		std::vector<AsysValue*> m_inputs;
		std::vector<AsysValue*> m_outputs;
		Stack m_stack;
		Context* m_context{};

	protected:
		static Executable* m_pMainExecutable;

	private:
		friend class Debugger;
		friend class DebugInfo;
	};*/
	
}