
/*!
 * \file Core.h
 * \date 02-10-2016 11:37:20
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include "Value.h"
#include <map>
#include <vector>
#include "Define.h"

namespace asys
{
	class Executable;

	class Code
	{
	public:
		virtual Executable* compile() = 0;
	};

	class Executable
	{
	public:
		virtual RetCode run() = 0;

		void release()
		{
			--m_nReferenceCount;
			if (m_nReferenceCount <= 0)
			{
				delete this;
			}
		}

		void retain() { ++m_nReferenceCount; }

		void setValue(const std::string& name, const Value* pValue) 
		{ 
			if (!pValue)
			{
				m_varTable.erase(name);
			}
			else
			{
				m_varTable[name] = *pValue;
			}
		}

		asys::Value& operator [](const std::string& name) { return m_varTable[name]; }

		void setValue(const std::string& name, const Value& value) { m_varTable[name] = value; }
		void setValue(const std::string& name, const std::string& value) { m_varTable[name] = Value(value); }
		void setValue(const std::string& name, const char* pchar) { m_varTable[name] = Value(std::string(pchar)); }
		void setValue(const std::string& name, int intValue) { m_varTable[name] = Value(intValue); }
		void setValue(const std::string& name, bool boolValue) { m_varTable[name] = Value(boolValue); }
		void setValue(const std::string& name, float floatValue) { m_varTable[name] = Value(floatValue); }
		void setValue(const std::string& name, double doubleValue){ m_varTable[name] = Value(doubleValue); }
		void setValue(const std::string& name, long long llValue){ m_varTable[name] = Value(llValue); }

		const Value* getValue(const std::string& name) const 
		{
			auto it = m_varTable.find(name);
			if (it == m_varTable.end()) return nullptr;
			return &(it->second);
		}

		//void setOutputValues(const std::vector<const Value*>& values);
		void setOutputValues(const std::vector<Value>& values);

		void setOutoutValue(int index, const Value* pValue) { setValue(getOutputVariableName(index), pValue); }
		void setOutoutValue(int index, const Value& value) { setValue(getOutputVariableName(index), value); }

		//void setInputValues(const std::vector<const Value*>& values);
		void setInputValues(const std::vector<Value>& values);

		void setInputValue(int index, const Value* pValue) { setValue(getInputVariableName(index), pValue); }
		void setInputValue(int index, const Value& value) { setValue(getInputVariableName(index), value); }

		const Value* getOutputValue(int index) const { return getValue(getOutputVariableName(index)); }
		const Value* getInputValue(int index) const { return getValue(getInputVariableName(index)); }

	private:
		int m_nReferenceCount{ 0 };
		std::map<std::string, Value> m_varTable;
	};
	
}