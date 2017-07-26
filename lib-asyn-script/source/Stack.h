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
	class AsysValue;
	
	class Stack
	{
	public:
		Stack(size_t size) : m_size(size), m_pRawMem(new char[size]){}

		//not meant to be used as base class.
		~Stack()
		{
			delete[] m_pRawMem;
		}

		Stack(const Stack& stack) = delete;

		size_t getRemainSize() const
		{
			return m_size - m_usedSize;
		}

		void* getAddressByFrameOffset(size_t size) const
		{
			return m_pRawMem + size;
		}

		size_t getCurFrameOffset() const
		{
			return m_usedSize;
		}

		bool allocate(size_t size)
		{
			if (getRemainSize() < size)
			{
				return false;
			}

			m_usedSize += size;
			return true;
		}

		bool dellocate(size_t size)
		{
			if (size > m_usedSize)
			{
				return false;
			}

			m_usedSize -= size;
			return true;
		}

		size_t getSize() const
		{
			return m_size;
		}

		void cleanup()
		{
			m_usedSize = 0;
		}

	private:
		char* const m_pRawMem{};
		const size_t m_size{};
		size_t m_usedSize{};
	};

	class StackFrame
	{
	public:
		StackFrame(){}

		//not meant to be base class.
		~StackFrame()
		{
			clear();
		}

		void clear();

		size_t getSize() const
		{
			return m_curSize;
		}

		void declare(AsysVariable& var);

		void constructFrame(Stack* stack, size_t frameOffset) const;
		void destructFrame(Stack* stack, size_t frameOffset) const;

		AsysValue* getValue(Stack* stack, size_t frameOffset, const AsysVariable& var) const;

		void constructValue(Stack* stack, size_t frameOffset, const AsysVariable& var) const;
		void destructValue(Stack* stack, size_t frameOffset, const AsysVariable& var) const;

		const AsysVariable* getVariableByIndex(int index) const;

		void forEachVariable(const std::function<void(AsysVariable*)>& callback) const;

	private:
		void* getValuePointerAddress(Stack* stack, size_t frameOffset, const AsysVariable& var) const;
		AsysValue** getPointerToAsysValuePointer(Stack* stack, size_t frameOffset, const AsysVariable& var) const
		{
			return static_cast<AsysValue**>(getValuePointerAddress(stack, frameOffset, var));
		}

		AsysValue* getValueAddressByValuePointerAddress(AsysValue** ppAsysValue) const
		{
			return (AsysValue*)((char*)ppAsysValue + sizeof(AsysValue*));
		}

	private:
		std::map<size_t, AsysVariable*>  m_variables;
		size_t m_curSize{};
	};

	class StackPool
	{
	public:
		virtual ~StackPool(){};

		virtual Stack* checkout(size_t size) = 0;
		virtual void checkIn(Stack* stack) = 0;
		virtual size_t getDefaultStackSize() = 0;
	};

	class DefaultStackPool : public StackPool
	{
	public:
		DefaultStackPool(size_t defaultStackSize = 4 * 1024, size_t poolSize = 1) 
			: m_defaultStackSize(defaultStackSize)
			, m_poolSize(poolSize){}

		virtual ~DefaultStackPool();

		virtual Stack* checkout(size_t size) override;
		virtual void checkIn(Stack* stack) override;
		virtual size_t getDefaultStackSize() override { return m_defaultStackSize; }

		void setDefaultStackSize(size_t stackSize)
		{
			m_defaultStackSize = stackSize;
		}

		void setPoolSize(size_t poolSize)
		{
			m_poolSize = poolSize;
		}

		size_t getPoolSize() const { return m_poolSize; }

	private:
		size_t m_defaultStackSize;
		size_t m_poolSize;
		std::vector<Stack*> m_stacks;
	};
}
