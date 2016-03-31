

/*!
 * \file Core.cpp
 * \date 02-10-2016 12:03:58
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#include "Core.h"

void asys::Executable::setOutputValues(const std::vector<const Value*>& values)
{
	for (int i = 0; i < static_cast<int>(values.size()); ++i)
	{
		auto pValue = values[i];
		setValue(getOutputVariableName(i), pValue);
	}
}

void asys::Executable::setOutputValues(const std::vector<Value>& values)
{
	for (int i = 0; i < static_cast<int>(values.size()); ++i)
	{
		auto& value = values[i];
		setValue(getOutputVariableName(i), value);
	}
}
