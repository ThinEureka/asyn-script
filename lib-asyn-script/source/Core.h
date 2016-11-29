
/*!
 * \file Core.h
 * \date 02-10-2016 11:37:20
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include "Value.h"
#include <map>
#include <vector>
#include "Define.h"
#include <functional>
#include "AsysVarible.h"
#include "Stack.h"

namespace asys
{
	class Executable;

	class Code
	{
	public:
		virtual Executable* compile() = 0;

	protected:
		StackStructure m_stackStructure;
	};

	class Executable
	{
	public:
		Executable(const StackStructure& stackStructure)
			: m_stack(stackStructure)
		{

		}

		asys::Executable::~Executable()
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
		void setOutput(const ValueList& vars, Executable* executable);

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

	protected:
		int m_nReferenceCount{ 0 };
		CodeFlow m_retCodeFlow{ CodeFlow::next_ };
		std::vector<std::function<void(asys::Executable*)>> m_deallocators;
		std::vector<AsysValue*> m_inputs;
		std::vector<AsysValue*> m_outputs;
		Stack m_stack;
	};
	
}