
/*!
 * \file Core.cpp
 * \date 02-10-2016 12:03:58
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#include "Core.h"

asys::Executable* asys::Executable::m_pMainExecutable = nullptr;

void asys::Executable::setInput(const ValueList& vars, Executable* executable)
{
	clearInputs();
	m_inputs.resize(vars.getLength());
	int index = 0;
	vars.forEachValue([&](bool isConst, const AsysVariable& var, const Const& constValue){
		if (isConst)
		{
			m_inputs[index++] = constValue.getAsysValue()->clone();
		}
		else
		{
			m_inputs[index++] = executable->getAsysValue(var)->clone();
		}
	});
}

void asys::Executable::setOutput(const ValueList& vars, Executable* executable)
{
	clearOutputs();
	m_outputs.resize(vars.getLength());
	int index = 0;
	vars.forEachValue([&](bool isConst, const AsysVariable& var, const Const& constValue){
		if (isConst)
		{
			m_outputs[index++] = constValue.getAsysValue()->clone();
		}
		else
		{
			m_outputs[index] = executable->getAsysValue(var)->clone();
		}
	});
}

void asys::Executable::fetchOutput(const VariableList& vars, Executable* callee)
{
	for (size_t i = 0; i < vars.getLength(); ++i)
	{
		auto output = callee->getOutput(i);
		if (output)
		{
			getAsysValue(*vars.getAsysVariable(i))->assign(*output);
		}
		else
		{
			AsysVariable* asysVar = m_stack.getMetaVariable(*vars.getAsysVariable(i));
			asysVar->reset();
		}
	}
}

const asys::AsysValue* asys::Executable::getOutput(int index) const
{
	if (index < 0 || index >= static_cast<int>(m_outputs.size()))
	{
		return nullptr;
	}

	return m_outputs[index];
}

const asys::AsysValue* asys::Executable::getInput(int index) const
{
	if (index < 0 || index >= static_cast<int>(m_inputs.size()))
	{
		return nullptr;
	}

	return m_inputs[index];
}
