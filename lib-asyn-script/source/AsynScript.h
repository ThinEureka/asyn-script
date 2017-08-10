/*!
 * \file AsynScript.h
 * \date 11-16-2016 9:15:54
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#include "AsysVariable.h"
#include "Function.h"
#include "Define.h"
#include "Machine.h"
#include "Instructions.h"

namespace asys
{
	template<typename T>
	AsysVariableT<T> tmpInputVar(FunctionCode* f, const std::function<T()>&callback, const char* varName, const char* fileName, const char* funcName, int line)
	{
		AsysVariableT<T> tmp(varName, 0);
		tmp.setVaribaleType(VariableType::input);
		f->Declare(tmp)(nullptr, fileName, funcName, line);
		f->Do(UNDEFINED_LINE, [=](asys::Machine* asys_this){
			tmp.sr(asys_this) = callback();
		})(nullptr, fileName, funcName, line);
		return tmp;
	}

	template<typename T>
	AsysVariableT<T> tmpOutputVar(FunctionCode* f, const std::function<T&()>&callback, const char* varName, const char* fileName, const char* funcName, int line)
	{
		AsysVariableT<T> tmp(varName, 0);
		tmp.setVaribaleType(VariableType::output);
		f->Declare(tmp)(nullptr, fileName, funcName, line);
		f->Do(UNDEFINED_LINE, [=](asys::Machine* asys_this){
			callback() = tmp.sr(asys_this);
		})(nullptr, fileName, funcName, PENDING_OUTPUT_LINE);
		return tmp;
	}
}

//key words
#define BEGIN_FUN(...) auto& __this_function = m_asynFunctions[__FUNCTION__]; \
					if (__this_function) return __this_function; \
					__this_function = new asys::FunctionCode; \
					__this_function->Input(__VA_ARGS__);

#define END_FUN __this_function->compile(); return __this_function;

#define _CC __this_function->Do(__LINE__, [=](asys::Machine* asys_this){
#define CC_ })___;

//The variable returned by CC is an input variable, which can not be used to accept function 
//return values or used as in left operand of assign operation. If you want to output 
//return values to C++ variables, use OUT_CC instead. Otherwise the error will be reported
//when compiling asyn-script.
#define CC(T, express) asys::tmpInputVar<T>(__this_function, [=](){ return (express);}, "__asys_tmp_right_line_" __S__LINE__, __FILE__, __FUNCTION__, __LINE__)

//Can only be used to output return values directly into C++ left values, can not be used as 
//arguments to function calls or in an ASSIGN statement. It returns an output variable.
#define OUT_CC(T, express) asys::tmpOutputVar<T>(__this_function, [=]()->T&{ return (express);}, "__asys_tmp_output_line_" __S__LINE__, __FILE__, __FUNCTION__, __LINE__)

//asys::FunctionCode::Call(outputs, inputs, code)
#define CALL(fun, ...)  __this_function->Call(fun, {__VA_ARGS__})___

#define IF(var) __this_function->If(var)___;
//cc means it's wrapped in a lamda expression, like a time capsule that the expression is evaluated at runtime to allow you writing c++ expression directly.
#define IF_CC(var) __this_function->If_ex([=](asys::Machine* asys_this){ return var; })_d_no_callback;

//asys::FunctionCode::If_ex(const std::function<bool(Executable*)>& express)
#define IF_EX(...) __this_function->If_ex(__VA_ARGS__)___;
#define IF_NOT(condition) __this_function->If_not(condition)___;

#define ELSE_IF(var) __this_function->Else_if(var)___;
#define ELSE_IF_NOT(var) __this_function->Else_if_not(var)___;
//cc means it's wrapped in a lamda expression, like a time capsule that the expression is evaluated at runtime to allow you writing c++ expression directly.
#define ELSE_IF_CC(var) __this_function->Else_if_ex([=](asys::Machine* asys_this){ return var; })_d_no_callback;

#define ELSE __this_function->Else()___;
#define END_IF __this_function->End_if()___;

#define WHILE(var) __this_function->While(var)___;

//cc means it's wrapped in a lamda expression, like a time capsule that the expression is evaluated at runtime to allow you writing c++ expression directly.
#define WHILE_CC(var) __this_function->While_ex([=](asys::Machine* asys_this){ return var; })_d_no_callback;

//asys::FunctionCode::While_ex(const std::function<bool(Executable*)>& express)
#define WHILE_EX(cond_expression) __this_function->While_ex(cond_expression)___;
#define WHILE_NOT(var) __this_function->While_not(var)___;

#define END_WHILE __this_function->End_while()___;
#define CONTINUE __this_function->Continue()___;
#define BREAK __this_function->Break()___;

#define FOR_CC(init_expression, cond_expression, loop_expression) asys::AsysVariableT<bool> __C__LINE__(__asys_tmp_for_init_line_)("__asys_tmp_for_init_line_" __S__LINE__, 0); \
	__this_function->Declare(__C__LINE__(__asys_tmp_for_init_line_))___;\
	WHILE_CC(__C__LINE__(__asys_tmp_for_init_line_)? ((loop_expression),(cond_expression)) : ((init_expression),(__C__LINE__(__asys_tmp_for_init_line_) = true),(cond_expression)))
#define END_FOR END_WHILE

#define RETURN(...) __this_function->Return(asys::VariableList(__VA_ARGS__))___;
#define ASSIGN(var1, var2) __this_function->Assign(var1, var2)___;

#define ASYS_VAR_F(type, name, f) asys::AsysVariableT<type> name(#name, 0); f->Declare(name)___;
#define ASYS_VAR(type, name) ASYS_VAR_F(type, name, __this_function);
#define ASYS_P(type, name) asys::AsysVariableT<type> name = asys::AsysVariableT<type>(#name, 0)
#define D_ASYS_P(type, name) asys::AsysVariableT<type> name

//keywords used in embedded c++ codes
#define asys_redo { asysRedo(asys_this, __LINE__); return;}
#define asys_next { asysNext(asys_this, __LINE__); return;}
#define asys_return(...) { asys_this->output(__VA_ARGS__); asysReturn(asys_this, __LINE__); return; }
#define asys_break { asysBreak(asys_this, __LINE__); return; }
#define asys_continue { asysContinue(asys_this, __LINE__); return;}
#define asys_this asys_this

//log to debugger
#if ASYS_BREAKPOINT == 1
	#define asys_log(...) if (asys_this->getDebugger()) { asys_this->getDebugger()->log(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); }
#else
	#define asys_log(...)
#endif

//used to register asyn functions.
#define asys_reg_funs asys::FunctionMap m_asynFunctions;
#define asys_reg_funs_static_impl(class_name) asys::FunctionMap class_name::m_asynFunctions;
