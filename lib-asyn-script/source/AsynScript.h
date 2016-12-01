/*!
 * \file AsynScript.h
 * \date 11-16-2016 9:15:54
 * 
 * 
 * \author cs (04nycs@gmail.com)
 */

#pragma once

#include "AsysVariable.h"
#include "Function.h"
#include "Define.h"

//key words
#define BEGIN_FUN(...) auto& __this_function = m_asynFunctions[__FUNCTION__]; \
					if (__this_function) return __this_function; \
					__this_function = new asys::FunctionCode; \
					__this_function->Input(__VA_ARGS__)_; 

#define END_FUN return __this_function;

#define _CC __this_function->Do([=](asys::Executable* asys_this){
#define CC_ })_;

//asys::FunctionCode::Call(outputs, inputs, code)
#define CALL(...)  __this_function->Call(__VA_ARGS__)_;

#define IF(var) __this_function->If(var)_;
//asys::FunctionCode::If_ex(const std::function<bool(Executable*)>& express)
#define IF_EX(...) __this_function->If_ex(__VA_ARGS__)_;
#define IF_NOT(condition) __this_function->If_not(condition)_;
#define IF_EQUAL(var1, var2) __this_function->If_equal(var1, var2)_;
#define IF_NOT_EQUAL(var1, var2) __this_function->If_not_equal(var1, var2)_;

#define ELSE __this_function->Else()_;
#define END_IF __this_function->End_if()_;

#define WHILE(var) __this_function->While(var)_;

//asys::FunctionCode::While_ex(const std::function<bool(Executable*)>& express)
#define WHILE_EX(express) __this_function->While_ex(express)_;

#define WHILE_NOT(var) __this_function->While_not(var)_;
#define WHILE_EQUAL(var1, var2)  __this_function->While_equal(var)_;
#define WHILE_NOT_EQUAL(var1, var2) __this_function->While_not_equal(var1, var2)_;

#define END_WHILE __this_function->End_while()_;
#define CONTINUE __this_function->Continue()_;
#define BREAK __this_function->Break()_;

#define RETURN(...) __this_function->Return({__VA_ARGS__})_;
#define ASSIGN(var1, var2) __this_function->Assign(var1, var2)_;

#define ASYS_VAR_F(type, name, f) asys::AsysVariableT<type> name(#name, 0); f->Declare(name)_;
#define ASYS_VAR(type, name) ASYS_VAR_F(type, name, __this_function);
#define ASYS_PARAM(type, name) asys::AsysVariableT<type> name = asys::AsysVariableT<type>(#name, 0)
#define D_ASYS_PARAM(name) asys::AsysVariableT<type> name(#name, 0)

//keywords used in embedded c++ codes
#define asys_redo {asys_this->setReturnCodeFlow(asys::CodeFlow::redo_); return;}
#define asys_next {asys_this->setReturnCodeFlow(asys::CodeFlow::next_); return;}
#define asys_return(...) {asys_this->setOutput({__VA_ARGS__}); asys_this->setReturnCodeFlow(asys::CodeFlow::return_); return;}
#define asys_break {asys_this->setReturnCodeFlow(asys::CodeFlow::break_); return;}
#define asys_continue {asys_this->setReturnCodeFlow(asys::CodeFlow::continue_); return;}
#define asys_this asys_this

//used to register asyn functions.
#define asys_reg_funs asys::FunctionMap m_asynFunctions;
#define asys_reg_funs_static_impl(class_name) asys::FunctionMap class_name::m_asynFunctions;

// const value
#define asys_null ""

