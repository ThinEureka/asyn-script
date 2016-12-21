/*!
 * \file Stack.h
 * \date 11-29-2016 12:32:10
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
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

	class StackFrame
	{
	public:
		size_t getSize() const;
		void declare(AsysVariable& var);

		void constructFrame(Stack* stack, size_t frameOffset) const;
		void destructFrame(Stack* stack, size_t frameOffset) const;

		AsysVariable* getVariable(Stack* stack, size_t frameOffset, const AsysVariable& var) const;
	};

	class Stack
	{
	public:
		Stack();
		virtual ~Stack();

		size_t getRemainSize() const;
		void* getAddressByFrameOffset(size_t size) const;
		size_t getCurFrameOffset() const;

		bool allocate(size_t size);
		bool dellocate(size_t size);

	private:
		void* getAddress(const AsysVariable& var) const;

	private:
		char* const m_pRawMem{};
	};

	class StackPool
	{
	public:
		virtual Stack* checkout(size_t size) = 0;
		virtual void checkIn(Stack* stack) = 0;
		virtual size_t getDefaultSize() = 0;
	};

	class DefaultStackPool : public StackPool
	{
	public:
		DefaultStackPool(size_t defaultSize = 4*1024);

		virtual Stack* checkout(size_t size) override;
		virtual void checkIn(Stack* stack) override;
		virtual size_t getDefaultSize() override;
	};
}
