
/*!
 * \file Variable.cpp
 * \date 11-25-2016 14:44:32
 * 
 * 
 * \author cs (04nycs@gmail.com)
 */

#include "AsysVariable.h"

asys::VariableList::VariableList(std::initializer_list<AsysVariable> varList)
{
	m_variables.insert(m_variables.begin(), varList.begin(), varList.end());
}
