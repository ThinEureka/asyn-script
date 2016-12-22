
/*!
 * \file Stack.cpp
 * \date 11-29-2016 12:32:07
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#include "Stack.h"
#include "AsysVariable.h"

void asys::StackFrame::clear()
{
	for (auto& pair : m_variables)
	{
		delete pair.second;
	}
	m_variables.clear();
	m_curSize = 0;
}

void asys::StackFrame::declare(AsysVariable& var)
{
	var.setMemoryOffset(m_curSize);
	m_variables[m_curSize] = var.clone();
	m_curSize += var.getAsysValueSize() + sizeof(AsysValue*);
}

void asys::StackFrame::constructFrame(Stack* stack, size_t frameOffset) const
{
	stack->allocate(m_curSize);
	for (const auto& pair : m_variables)
	{
		auto ppAsysValue = getPointerToAsysValuePointer(stack, frameOffset, *pair.second);
		*ppAsysValue = nullptr;
	}
}

void asys::StackFrame::destructFrame(Stack* stack, size_t frameOffset) const
{
	for (const auto& pair : m_variables)
	{
		auto ppAsysValue = getPointerToAsysValuePointer(stack, frameOffset, *pair.second);
		if (*ppAsysValue != nullptr)
		{
			pair.second->destruct(*ppAsysValue);
		}
	}
	stack->dellocate(m_curSize);
}

asys::AsysValue* asys::StackFrame::getValue(Stack* stack, size_t frameOffset, const AsysVariable& var) const
{
	auto ppAsysValue = getPointerToAsysValuePointer(stack, frameOffset, var);
	return *ppAsysValue;
}

void asys::StackFrame::constructValue(Stack* stack, size_t frameOffset, const AsysVariable& var) const
{
	auto ppAsysValue = getPointerToAsysValuePointer(stack, frameOffset, var);
	*ppAsysValue = getValueAddressByValuePointerAddress(ppAsysValue);
	var.construct(*ppAsysValue);
}

void asys::StackFrame::destructValue(Stack* stack, size_t frameOffset, const AsysVariable& var) const
{
	auto ppAsysValue = getPointerToAsysValuePointer(stack, frameOffset, var);
	var.destruct(*ppAsysValue);
}

const asys::AsysVariable* asys::StackFrame::getVariableByIndex(int index) const
{
	if (index < 0 || index >= static_cast<int>(m_variables.size()))
	{
		return nullptr;
	}

	for (auto it = m_variables.begin(); it != m_variables.end(); ++it)
	{
		if (index == 0)
		{
			return it->second;
		}
		--index;
	}

	return nullptr;
}

void* asys::StackFrame::getValuePointerAddress(Stack* stack, size_t frameOffset, const AsysVariable& var) const
{
	return static_cast<char*>(stack->getAddressByFrameOffset(frameOffset)) + var.getMemoryOffset();
}

asys::DefaultStackPool::~DefaultStackPool()
{
	for (auto stack : m_stacks)
	{
		delete stack;
	}
	m_stacks.clear();
}

asys::Stack* asys::DefaultStackPool::checkout(size_t size)
{
	for (auto it = m_stacks.begin(); it != m_stacks.end(); ++it)
	{
		auto stack = *it;
		if (stack->getSize() >= size)
		{
			m_stacks.erase(it);
			return stack;
		}
	}

	size_t stackSize = m_defaultStackSize;
	while (stackSize < size)
	{
		stackSize <<= 1;
	}

	return new Stack(stackSize);
}

void asys::DefaultStackPool::checkIn(Stack* stack)
{
	if (m_stacks.size() < m_poolSize)
	{
		stack->cleanup();
		m_stacks.push_back(stack);
	}
	else
	{
		delete stack;
	}
}
