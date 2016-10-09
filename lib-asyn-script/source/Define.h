
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

#define ASYS_VAR(name) const std::string name("$"#name)
#define ASYS_PARAM(name) const std::string& name = "$"#name
#define D_ASYS_PARAM(name) const std::string& name
#define asys_value(name) const auto& tmp_##name = name; auto& name = (*executable)[tmp_##name];
#define asys_const(name) asys::toString(name) 
#define asys_null ""

#define asys_redo {executable->setReturnCodeFlow(asys::CodeFlow::redo_); return;}
#define asys_next {executable->setReturnCodeFlow(asys::CodeFlow::next_); return;}
#define asys_return(...) {executable->setOutputValues({__VA_ARGS__}); executable->setReturnCodeFlow(asys::CodeFlow::return_); return;}
#define asys_break {executable->setReturnCodeFlow(asys::CodeFlow::break_); return;}
#define asys_continue {executable->setReturnCodeFlow(asys::CodeFlow::continue_); return;}

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

	enum class Operator
	{
		plus,
		minus,
		multiply,
		divide,
		equal,
		not_equal,
		greater_than,
		greater_than_equal,
		less_than,
		less_than_equal,
		and_,
		or_,
		not_,
	};

	extern const std::string OUTPUT_VARIABLE_PREFIX;
	extern const std::string INPUT_VARIABLE_PREFIX;

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

	inline bool isValidVariableName(const std::string& var) { return var.size() > 0 && var[0] == '$'; }
	inline bool isSystemReservedVariableName(const std::string& var) { return var.size() > 1 && var[0] == '$' && var[1] == '$'; }

	inline std::string getOutputVariableName(int index) { return OUTPUT_VARIABLE_PREFIX + toString(index); }
	inline std::string getInputVariableName(int index) { return INPUT_VARIABLE_PREFIX + toString(index); }

	inline std::string toAsysVarName(const char* name) { return std::string("$") + name; }

}
