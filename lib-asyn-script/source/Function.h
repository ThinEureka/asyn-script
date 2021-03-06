﻿/*!
 * \file Function.h
 * \date 02-10-2016 15:29:48
 *
 *
 * \author cs 04nycs@gmail.com
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#include <vector>
#include <list>
#include <functional>
#include "AsysVariableT.h"
#include "Debug.h"
#include "Instructions.h"
#include "Stack.h"
#include "Machine.h"

namespace asys
{
	class FunctionCode;
	class Machine;
	class Debugger;
	class CallInfo;
	class DebugInfo;

	class FunctionCode
	{
	public:
		FunctionCode(){}
		virtual ~FunctionCode()
		{
			clear();
		}

		FunctionCode(const FunctionCode& code) = delete;

		BreakPoint& Do(const std::function<void(Machine*)>& express);
		BreakPoint& Do(int lineNumber, const std::function<void(Machine*)>& express);

		BreakPoint& Declare(AsysVariable& var);

		template<typename ...Args>
		BreakPoint& Declare(AsysVariable& var, Args&... args)
		{
			auto& breakPoint = Declare(var);
			Declare(args...);

			return breakPoint;
		}

		template<typename ...Args>
		void Input(Args&... args)
		{
			Input_ex(0, args...);
		}

		void Input()
		{
			//do nothing.
		}

		BreakPoint& Call(const AsysVariable& code, const VariableList& inputs);
		BreakPoint& Call(FunctionCode* code, const VariableList& inputs);

		template<typename V>
		BreakPoint& If(const AsysVariableT<V>& var)
		{
			assert(var.getVariableType() != VariableType::output);
			return If_ex([=](asys::Machine* asys_this){
				auto pCastValue = static_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return pCastValue->getNativeValue();
			});
		}

		BreakPoint& If_ex(const std::function<bool(asys::Machine*)>& express);

		template<typename V>
		BreakPoint& If_not(const AsysVariableT<V>& var)
		{
			assert(var.getVariableType() != VariableType::output);
			return If_ex([=](asys::Machine* asys_this){
				auto pCastValue = static_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return !pCastValue->getNativeValue();
			});
		}

		template<typename V>
		BreakPoint& Else_if(const AsysVariableT<V>& var)
		{
			assert(var.getVariableType() != VariableType::output);
			return Else_if_ex([=](asys::Machine* asys_this){
				auto pCastValue = static_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return pCastValue->getNativeValue();
			});
		}

		BreakPoint& Else_if_ex(const std::function<bool(asys::Machine*)>& express);

		template<typename V>
		BreakPoint& Else_if_not(const AsysVariableT<V>& var)
		{
			assert(var.getVariableType() != VariableType::output);
			return Else_if_ex([=](asys::Machine* asys_this){
				auto pCastValue = static_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return !pCastValue->getNativeValue();
			});
		}

		BreakPoint& Else();
		BreakPoint& End_if();

		template<typename V>
		BreakPoint& While(const AsysVariableT<V>& var)
		{
			assert(var.getVariableType() != VariableType::output);
			return While_ex([=](asys::Machine* asys_this){
				auto pCastValue = static_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return pCastValue->getNativeValue();
			});
		}

		BreakPoint& While_ex(const std::function<bool(asys::Machine*)>& express);

		template<typename V>
		BreakPoint& While_not(const AsysVariableT<V>& var)
		{
			assert(var.getVariableType() != VariableType::output);
			return While_ex([=](asys::Machine* asys_this){
				auto pCastValue = static_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return !pCastValue->getNativeValue();
			});
		}

		BreakPoint& End_while();
		BreakPoint& Continue();
		BreakPoint& Break();

		BreakPoint& Return();
		BreakPoint& Return(const VariableList& vars);

		template<typename V1, typename V2>
		BreakPoint& Assign(const AsysVariableT<V1>& var1, const AsysVariableT<V2>& var2)
		{
			//do not use CC or OUT_CC in the left side of ASSIGN statements, only normal variables are accepted here.
			assert(var1.getVariableType() != VariableType::input);
			assert(var1.getVariableType() != VariableType::output);
			assert(var2.getVariableType() != VariableType::output);

			return Do([=](asys::Machine* asys_this){
				auto* pCastValue1 = static_cast<AsysValueT<V1>*>(asys_this->getAsysValue(var1));
				auto* pCastValue2 = static_cast<AsysValueT<V2>*>(asys_this->getAsysValue(var2));
				pCastValue1->getNativeValueReference() = pCastValue2->getNativeValue();
			});
		}

		void clear();
		void compile();

	private:
		const AsysVariable* getInputVariable(int index) const;
		int getNumInputs() const { return m_numInputs; }

		template<typename ...Args>
		void Input_ex(int inputIndex, AsysVariable& var, Args&... args)
		{
			m_stackFrame.declare(var);
			Input_ex(inputIndex + 1, args...);
		}

		template<typename ...Args>
		void Input_ex(int num)
		{
			m_numInputs = num;
		}

		void movePendingOutputInstructions(int numOutputVariable, int realLineNumber);

	private:
		std::vector<Instruction*> m_instructions;
		std::list<int> m_unmatchedIfIps;
		std::list<int> m_unmatchedWhileIps;

		int m_numInputs{};
		StackFrame m_stackFrame;

	private:
		friend class Machine;
		friend class FunctionRuntime;
		friend class Debugger;
		friend class DebugInfo;
		friend class CallInfo;
		friend class BreakPoint;
	};

	class FunctionMap : public std::map <std::string, asys::FunctionCode*>
	{
	public:
		virtual ~FunctionMap()
		{
			for (auto& pair : *this)
			{
				if (pair.second)
				{
					delete pair.second;
				}
			}

			clear();
		}
	};
}
