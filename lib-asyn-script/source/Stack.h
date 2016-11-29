/*!
 * \file Stack.h
 * \date 11-29-2016 12:32:10
 * 
 * 
 * \author cs (04nycs@gmail.com)
 */

#pragma once

#include <vector>
#include <functional>
#include <map>

namespace asys
{
	class AsysVariable;

	class StackStructure
	{
	public:
		StackStructure(const StackStructure& structure);

		StackStructure()
		{}

		virtual ~StackStructure();

		void clear();

		void declare(AsysVariable& var);

		size_t getCurSize() const
		{
			return m_curSize;
		}

		void forEachVariable(const std::function<void(AsysVariable*)>& callback);

		AsysVariable* getVariable(size_t memoryOffset);
		const AsysVariable* getVariable(size_t memoryOffset) const;
		

	private:
		std::map<size_t, AsysVariable*>  m_variables;
		size_t m_curSize{};
	};

	class Stack
	{
	public:
		Stack(const StackStructure& structure);
		Stack() = delete;

		virtual ~Stack();

		bool hasAsysValue(const AsysVariable& var) const;

		const AsysVariable* getMetaVariable(const AsysVariable& var) const;

		AsysVariable* getMetaVariable(const AsysVariable& var);

		void construct(const AsysVariable& var);
		void destruct(const AsysVariable& var);

	private:
		void* getAddress(const AsysVariable& var) const;

	private:
		StackStructure m_structure;
		char* const m_pRawMem{};
	};
}
