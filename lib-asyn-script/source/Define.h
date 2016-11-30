
/*!
 * \file Define.h
 * \date 02-11-2016 18:31:20
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include <string>
#include <sstream>

namespace asys
{
	enum class CodeFlow
	{
		redo_, // the virtual machine will start from the same instruction next time it's invoked. 
		next_, // the virtual machine moves to the next instruction
		break_, // function as break in a while loop
		continue_, //function as continue in a while loop
		return_, //function as return in a function
	};

	template < typename T > std::string toString(const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}

	template <typename T> T toType(const std::string& str)
	{
		T value;
		std::istringstream stm(str);
		stm >> value;

		return value;
	}
}
