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

//key words
#define BEGIN_FUN(...) auto& __this_function = m_asynFunctions[__FUNCTION__]; \
					if (__this_function) return __this_function; \
					__this_function = new asys::FunctionCode; \
					__this_function->Input(__VA_ARGS__);

#define END_FUN __this_function->compile(); return __this_function;

#define _CC __this_function->Do(__LINE__, [=](asys::Machine* asys_this){
#define CC_ })_;

//asys::FunctionCode::Call(outputs, inputs, code)
#define CALL(fun, ...)  __this_function->Call(fun, {__VA_ARGS__})_

#define IF(var) __this_function->If(var)_;
//cc means it's wrapped in a lamda expression, like a time capsule that the expression is evaluated at runtime to allow you writing c++ expression directly.
#define IF_CC(var) __this_function->If_ex([=](asys::Machine* asys_this){ return var; })_d_no_callback;

//asys::FunctionCode::If_ex(const std::function<bool(Executable*)>& express)
#define IF_EX(...) __this_function->If_ex(__VA_ARGS__)_;
#define IF_NOT(condition) __this_function->If_not(condition)_;

#define ELSE __this_function->Else()_;
#define END_IF __this_function->End_if()_;

#define WHILE(var) __this_function->While(var)_;

//cc means it's wrapped in a lamda expression, like a time capsule that the expression is evaluated at runtime to allow you writing c++ expression directly.
#define WHILE_CC(var) __this_function->While_ex([=](asys::Machine* asys_this){ return var; })_d_no_callback;

//asys::FunctionCode::While_ex(const std::function<bool(Executable*)>& express)
#define WHILE_EX(express) __this_function->While_ex(express)_;
#define WHILE_NOT(var) __this_function->While_not(var)_;

#define END_WHILE __this_function->End_while()_;
#define CONTINUE __this_function->Continue()_;
#define BREAK __this_function->Break()_;

#define RETURN(...) __this_function->Return(asys::ValueList(__VA_ARGS__))_;
#define ASSIGN(var1, var2) __this_function->Assign(var1, var2)_;

#define ASYS_VAR_F(type, name, f) asys::AsysVariableT<type> name(#name, 0); f->Declare(name)_;
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
