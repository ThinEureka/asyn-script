/*!
 * \file AsysVariable.cpp
 * \date 12-22-2016 16:43:15
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#include "AsysVariable.h"
#include "Machine.h"

asys::AsysValue* asys::AsysVariable::getAsysValue(Machine* asys_this) const
{
	return asys_this->getAsysValue(*this);
}

asys::AsysValue* asys::AsysVariable::getAsysValueFromCurMainThreadMachine() const
{
	return getAsysValue(Machine::getCurMainThreadMachine());
}
