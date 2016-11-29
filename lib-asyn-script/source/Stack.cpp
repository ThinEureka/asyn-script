

/*!
 * \file Stack.cpp
 * \date 11-29-2016 12:32:07
 * 
 * 
 * \author cs (04nycs@gmail.com)
 */

#include "Stack.h"
#include "AsysVarible.h"

asys::StackStructure::StackStructure(const StackStructure& structure)
{
	for (const auto& pair : structure.m_variables)
	{
		m_variables[pair.first] = pair.second->clone();
	}
	m_curSize = structure.m_curSize;
}

asys::StackStructure::~StackStructure()
{
	clear();
}

void asys::StackStructure::clear()
{
	for (auto& pair : m_variables)
	{
		delete pair.second;
	}

	m_variables.clear();
	m_curSize = 0;
}

void asys::StackStructure::declare(AsysVariable& var)
{
	var.setMemoryOffset(m_curSize);
	m_variables[m_curSize] = var.clone();
	m_curSize += var.getAsysValueSize();
}

void asys::StackStructure::forEachVariable(const std::function<void(AsysVariable*)>& callback)
{
	if (callback)
	{
		for (const auto& pair : m_variables)
		{
			callback(pair.second);
		}
	}
}

asys::AsysVariable* asys::StackStructure::getVariable(size_t memoryOffset)
{
	auto it = m_variables.find(memoryOffset);
	if (it == m_variables.end())
	{
		return nullptr;
	}

	return it->second;
}

const asys::AsysVariable* asys::StackStructure::getVariable(size_t memoryOffset) const
{
	auto it = m_variables.find(memoryOffset);
	if (it == m_variables.end())
	{
		return nullptr;
	}

	return it->second;
}

asys::Stack::Stack(const StackStructure& structure)
	: m_structure(structure)
	, m_pRawMem(new char[m_structure.getCurSize()])
{

}

asys::Stack::~Stack()
{
	m_structure.forEachVariable([=](AsysVariable* pVar){
		if (pVar->getAsysValue())
		{
			destruct(*pVar);
		}
	});

	delete[] m_pRawMem;
}

void* asys::Stack::getAddress(const AsysVariable& var) const
{
	return m_pRawMem + var.getMemoryOffset();
}

bool asys::Stack::hasAsysValue(const AsysVariable& var) const
{
	auto pMetaVar = m_structure.getVariable(var.getMemoryOffset());
	return pMetaVar->getAsysValue() != nullptr;
}

const asys::AsysVariable* asys::Stack::getMetaVariable(const AsysVariable& var) const
{
	return m_structure.getVariable(var.getMemoryOffset());
}

asys::AsysVariable* asys::Stack::getMetaVariable(const AsysVariable& var)
{
	return m_structure.getVariable(var.getMemoryOffset());
}

void asys::Stack::construct(const AsysVariable& var)
{
	auto pMetaVar = m_structure.getVariable(var.getMemoryOffset());
	auto address = getAddress(var);
	pMetaVar->construct(address);
}

void asys::Stack::destruct(const AsysVariable& var)
{
	auto pMetaVar = m_structure.getVariable(var.getMemoryOffset());
	auto address = getAddress(var);
	pMetaVar->destruct();
}
