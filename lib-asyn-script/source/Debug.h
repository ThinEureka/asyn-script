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
	class Executable;
	class BreakPoint;
	class FunctionExecutable;
	class Context;
	class VariableViewer;

	/*class Debugger
	{
	public:
		virtual CodeFlow onBreakPoint(Executable* asys_this, const BreakPoint& breakPoint, Context* context) = 0;
	};

	class DebugInfo
	{
	public:
		virtual ~DebugInfo();

		Debugger* getDebugger()
		{
			return m_pDebugger;
		}

		void setDebugger(Debugger* debugger)
		{
			m_pDebugger = debugger;
		}

		void setParentDebugInfo(DebugInfo* debugInfo)
		{
			m_pParentDebugInfo = debugInfo;
		}

		void initialize(FunctionExecutable* executable);

		void setFileName(const char* fileName)
		{
			m_fileName = fileName;
		}

		void setFunName(const char* funName)
		{
			m_funName = funName;
		}

		void setLineNumber(int lineNumber)
		{
			m_lineNumber = lineNumber;
		}

		const char* fileName() const
		{
			return m_fileName;
		}

		const char* funName() const
		{
			return m_funName;
		}

		int lineNumber() const
		{
			return m_lineNumber;
		}

		void setCurLocation(const char* fileName, const char* funName, int lineNumber)
		{
			m_fileName = fileName;
			m_funName = funName;
			m_lineNumber = lineNumber;
		}

	private:
		const char* m_fileName;
		const char* m_funName;
		int m_lineNumber{ -1 };
		std::map<const char*, VariableViewer*> m_variables;
		DebugInfo* m_pParentDebugInfo{};
		Debugger* m_pDebugger{};
		FunctionExecutable* m_pExecutable{};

	private:
		friend class Debugger;
	};*/
}