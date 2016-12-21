/*!
 * \file Function.h
 * \date 02-10-2016 15:29:48
 *
 *
 * \author cs 04nycs@gmail.com
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#include "Core.h"
#include <vector>
#include <list>
#include <functional>
#include "AsysVariableT.h"
#include "Debug.h"
#include "Instructions.h"

namespace asys
{
	class FunctionExecutable;
	class FunctionCode;
	class Machine;

	class FunctionCode
	{
	public:
		FunctionCode();
		virtual ~FunctionCode();

		BreakPoint& Do(const std::function<void(Machine*)>& express);
		BreakPoint& Declare(AsysVariable& var);

		template<typename ...Args>
		BreakPoint& Declare(AsysVariable& var, Args&... args)
		{
			auto& breakPoint = Declare(var);
			Declare(args...);

			return breakPoint;
		}

		template<typename ...Args>
		BreakPoint& Input(Args&... args)
		{
			auto& breakPoint = Declare(args...);
			Input_ex(0, args...);
			return breakPoint;
		}

		BreakPoint& Input()
		{
			return Do(nullptr);
		}

		//CALL returns multiple variable from the called function, which in turn are assigned to the outputParams.
		BreakPoint& Call_ex(const VariableList& outputs, const ValueList& inputs, FunctionCode* code);
		BreakPoint& Call_ex(const VariableList& outputs, const ValueList& inputs, const AsysVariable& code);


		BreakPoint& Call(const AsysVariable& code, const ValueList& inputs);
		BreakPoint& Call(FunctionCode* code, const ValueList& inputs);

		template<typename V>
		BreakPoint& If(const AsysVariableT<V>& var)
		{
			return If_ex([=](asys::Machine* asys_this){
				auto pCastValue = dynamic_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return pCastValue->getNativeValue();
			});
		}

		BreakPoint& If_ex(const std::function<bool(asys::Machine*)>& express);

		template<typename V>
		BreakPoint& If_not(const AsysVariableT<V>& var)
		{
			return If_ex([=](asys::Machine* asys_this){
				auto pCastValue = dynamic_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return !pCastValue->getNativeValue();
			});
		}

		BreakPoint& Else();
		BreakPoint& End_if();

		template<typename V>
		BreakPoint& While(const AsysVariableT<V>& var)
		{
			return While_ex([=](asys::Machine* asys_this){
				auto pCastValue = dynamic_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return pCastValue->getNativeValue();
			});
		}

		BreakPoint& While_ex(const std::function<bool(asys::Machine*)>& express);

		template<typename V>
		BreakPoint& While_not(const AsysVariableT<V>& var)
		{
			return While_ex([=](asys::Machine* asys_this){
				auto pCastValue = dynamic_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				return !pCastValue->getNativeValue();
			});
		}

		BreakPoint& End_while();
		BreakPoint& Continue();
		BreakPoint& Break();

		BreakPoint& Return();
		BreakPoint& Return(const ValueList& vars);

		template<typename V1, typename V2>
		BreakPoint& Assign(const AsysVariableT<V1>& var1, const AsysVariableT<V2>& var2)
		{
			return Do([=](asys::Machine* asys_this){
				auto* pCastValue1 = dynamic_cast<AsysValueT<V1>*>(asys_this->getAsysValue(var1));
				auto* pCastValue2 = dynamic_cast<AsysValueT<V2>*>(asys_this->getAsysValue(var2));
				pCastValue1->getNativeValueReference() = pCastValue2->getNativeValue();
			});
		}

		template<typename V, typename C>
		BreakPoint& Assign(const AsysVariableT<V>& var, const C& constValue)
		{
			return Do([=](asys::Executable* executable){
				auto* pCastValue = dynamic_cast<AsysValueT<V>*>(asys_this->getAsysValue(var));
				pCastValue->getNativeValueReference() = constValue;
			});
		}

		void clear();

	private:
		BreakPoint& Return_ex(const std::function<void(asys::Executable*)>& returnCallback);
		const AsysVariable* getInputVariable(int index) const;
		int getNumInputs() const;

		template<typename ...Args>
		void Input_ex(int inputIndex, AsysVariable& var, Args&... args);

		template<typename ...Args>
		void Input_ex(int)
		{}

	private:
		std::vector<Instruction*> m_instructions;
		std::list<int> m_unmatchedIfIps;
		std::list<int> m_unmatchedWhileIps;

		std::string m_fileNameForCurInstruction;
		int m_lineNumerForCurInstruction{ -1 };
		std::string m_functionNameForCurInstruction;

		int m_numInputs{};

		StackFrame m_stackFrame;

	private:
		friend class Machine;
		friend class FunctionRuntime;
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
