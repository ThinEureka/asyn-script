
/*!
 * \file Define.h
 * \date 02-11-2016 18:31:20
 * 
 * 
 * \author cs 04nycs@gmail.com
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#ifndef ASYS_BREAKPOINT
	#ifdef _DEBUG
		#define ASYS_BREAKPOINT 1
	#else
		#define ASYS_BREAKPOINT 0
	#endif // _DEBUG
#endif // !ASYS_BREAK_POINT

#if ASYS_BREAKPOINT == 0
	#define ASYS_DEBUG 0
#endif

#ifndef ASYS_DEBUG
	#ifdef _DEBUG
		#define ASYS_DEBUG 1
	#else
		#define ASYS_DEBUG 0
	#endif // _DEBUG
#endif // ASYS_DEBUG

// append this macro to the instruction calls to set a break point for ease of debug in C++

#if ASYS_BREAKPOINT == 1
	#define ___ ([](asys::Machine* asys_this, const asys::BreakPoint& breakPoint) {}, __FILE__, __FUNCTION__, __LINE__)
	#define _d_no_callback (nullptr, __FILE__, __FUNCTION__, __LINE__)
#else
	#define ___
	#define _d_no_callback
#endif

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

	const int INVALID_IP = -1;
	const int THREAD_ID_MAIN = 0;
}
