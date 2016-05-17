
/*!
 * \file Simple.cpp
 * \date 02-10-2016 15:13:41
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#include "Simple.h"

asys::CodeFlow asys::SimpleLamdaExecutable::run()
{
	if (m_callback)
	{
		return m_callback(this);
	}

	return CodeFlow::next_;
}

asys::Executable* asys::SimpleLamdaCode::compile()
{
	return m_excutable;
}
