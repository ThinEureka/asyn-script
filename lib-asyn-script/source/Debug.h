/*!
 * \file Debug.h
 * \date 12-1-2016 10:48:17
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#include <string>
#include <map>
#include "Define.h"

namespace asys
{
	class BreakPoint;
	class VariableViewer;
	class DebugInfo;
	class CallInfo;

	class Debugger
	{
	public:
		Debugger() = default;
		virtual ~Debugger() = default;

		virtual CodeFlow onBreakPoint(const BreakPoint& breakPoint, asys::CodeFlow lastCodeFlow, int callStackIndex) = 0;
		virtual void log(const char* fileName, const char* functionName, int line, const char* format, ...) = 0;

		virtual void onDoInstructionLineNumber(asys::CodeFlow codeFlow, int lineNumber) = 0;

		void setMachine(Machine* machine) { m_pMachine = machine; }
		Machine* getMachine() { return m_pMachine; }

		const DebugInfo* getDebugInfo();
		const CallInfo* getCallInfo(int index);

		int getCallStackSize();

	protected:
		Machine* m_pMachine{};
	};

	class CallInfo
	{
	public:
		CallInfo(){}

		//not to be used as base class
		~CallInfo();

		const std::vector<VariableViewer*>& getVariables() { return m_variables; }
		const BreakPoint* getBreakPoint() { return m_pCurBreakpoint; }

	private:
		void init(Machine* machine, int callIndex);
		void updateBreakPoint(Machine*, int callIndex);

	private:
		std::vector<VariableViewer*> m_variables;
		const BreakPoint* m_pCurBreakpoint{ nullptr };

	private:
		friend class DebugInfo;
	};

	class DebugInfo
	{
	public:
		DebugInfo(Machine* machine);

		//not to be used as base class
		~DebugInfo();

	public:
		void onPushCallStack();

		void onPopCallStack();

		void onBreakPoint(const BreakPoint& breakPoint);

	private:
		void generateCallStack();

		void pushCallInfo(int index);
		void popCallInfo();

	private:
		std::vector<CallInfo*> m_callStack;
		Machine* m_pMachine{};
		const BreakPoint* m_pCurBreakPoint{ nullptr };
		int m_nCurCallIndex{ -1 };
		std::vector<VariableViewer*>* m_pLocalVariables{ nullptr };
	};
}