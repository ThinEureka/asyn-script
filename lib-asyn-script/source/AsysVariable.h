/*!
 * \file AsysVariable.h
 * \date 11-25-2016 14:44:28
 *
 *
 * \author cs (04nycs@gmail.com)
 */

#pragma once

#include <assert.h>
#include <initializer_list>
#include <vector>
#include "AsysValue.h"
#include <functional>
#include "AsysValueT.h"

namespace asys
{
	class Const
	{
	public:
		template <typename T>
		Const(const T& nativeValue)
		{
			auto newAsysValue = new AsysValueT<T>;
			{
				newAsysValue->setNativeValue(nativeValue);
			}

			m_pAsysValue = newAsysValue;
		}

		template <typename T>
		Const(T* nativeValue)
		{
			auto newAsysValue = new AsysValueT<T*>;
			{
				newAsysValue->setNativeValue(nativeValue);
			}

			m_pAsysValue = newAsysValue;
		}

		Const(){}

		Const(const Const& val)
		{
			copyFrom(val);
		}

		virtual ~Const()
		{
			if (m_pAsysValue)
			{
				delete m_pAsysValue;
				m_pAsysValue = nullptr;
			}
		}

		void copyFrom(const Const& value)
		{
			if (this == &value)
			{
				return;
			}

			delete m_pAsysValue;
			if (!value.m_pAsysValue)
			{
				m_pAsysValue = nullptr;
			}
			else
			{
				m_pAsysValue = value.m_pAsysValue->clone();
			}
		}

		AsysValue* getAsysValue() { return m_pAsysValue; }
		AsysValue* getAsysValue() const { return m_pAsysValue; }

	protected:
		AsysValue* m_pAsysValue{ nullptr };
	};

	class Executable;

	class VariableViewer{
	public:
		virtual const char* getName() const = 0;
	};

	template<typename T>
	class VariableViewerT : public VariableViewer
	{
	public:
		void setTarget(const char* varName, T* value)
		{
			m_pValue = value;
			m_varName = varName;
		}

		virtual const char* getName() const override
		{
			return m_varName;
		}

	private:
		T* m_pValue{};
		const char* m_varName{};
	};

	class AsysVariable
	{
	public:
		AsysVariable() = default;
		AsysVariable(const AsysVariable& var)
		{
			shallowCopy(var);
		}

	protected:
		virtual AsysVariable* clone() const
		{
			auto newVar = new AsysVariable();
			newVar->deepCopy(*this);

			return newVar;
		}

		void shallowCopy(const AsysVariable& var)
		{
			m_memoryOffset = var.m_memoryOffset;
			m_pName = var.m_pName;
		}

		void deepCopy(const AsysVariable& var)
		{
			shallowCopy(var);
			//other info
		}

		virtual size_t getAsysValueSize() const
		{
			return 0;
		}

		virtual void construct(void* address)
		{
			//do nothing
		}

		virtual void destruct()
		{
			//do nothing
		}

		virtual void reset()
		{
			//do nothing
		}

		size_t getMemoryOffset() const
		{
			return m_memoryOffset;
		}

		void setMemoryOffset(size_t memoryOffset)
		{
			m_memoryOffset = memoryOffset;
		}

		virtual AsysValue* getAsysValue() const { return m_pAsysValue; }
		virtual AsysValue* getAsysValue() { return m_pAsysValue; }

		void setAsysValue(AsysValue* asysValue) { m_pAsysValue = asysValue; }

		virtual VariableViewer* createVariableViewer() const
		{
			return nullptr;
		}

	protected:
		size_t m_memoryOffset{};
		const char* m_pName{};
		mutable AsysValue* m_pAsysValue{};

	private:
		friend class Stack;
		friend class StackStructure;
		friend class Executable;
		friend class ValueList;
		friend class Debugger;
		friend class DebugInfo;
	};

	class VariableList
	{
	public:
		VariableList(std::initializer_list<AsysVariable> varList)
		{
			m_variables.insert(m_variables.begin(), varList.begin(), varList.end());
		}

		VariableList(){}

		size_t getLength() const
		{
			return m_variables.size();
		}

		const AsysVariable* getAsysVariable(int index) const
		{
			if (index < 0 || index >= static_cast<int>(m_variables.size()))
			{
				return nullptr;
			}

			return &m_variables[index];
		}

	private:
		std::vector<AsysVariable> m_variables;
	};

	class ValueList
	{
	public:
		friend class Executable;

	public:
		struct InnerValue
		{
			AsysVariable asysVarible;
			Const asysConst;
			bool isConst{};
		};

	public:
		template<typename... Args>
		ValueList(const Args&... args)
		{
			m_values.resize(sizeof...(Args));
			init(0, args...);
		}

		size_t getLength() const
		{
			return m_values.size();
		}

	private:
		template<typename... Args>
		void init(int index, const Const& constValue, const Args&... args)
		{
			m_values[index].asysConst.copyFrom(constValue);
			m_values[index].isConst = true;
			init(index + 1, args...);
		}

		template<typename... Args>
		void init(int index, const AsysVariable& variable, const Args&... values)
		{
			m_values[index].asysVarible.shallowCopy(variable);
			m_values[index].isConst = false;
			init(index + 1, values...);
		}

		void init(int){}

	private:
		void forEachValue(const std::function<void(bool, const AsysVariable&, const Const&)>& callback) const
		{
			if (callback)
			{
				for (const auto& val : m_values)
				{
					callback(val.isConst, val.asysVarible, val.asysConst);
				}
			}
		}

	private:
		std::vector<InnerValue> m_values;
	};
}