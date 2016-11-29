﻿/*!
 * \file Variable.h
 * \date 11-25-2016 14:44:28
 *
 *
 * \author cs (04nycs@gmail.com)
 */

#pragma once
#include <assert.h>
#include <initializer_list>
#include <vector>
#include <string>
#include "AsysValue.h"
#include <functional>

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
		AsysValue* m_pAsysValue{nullptr};
	};

	class Executable;

	class AsysVariable
	{
	public:
		friend class Stack;
		friend class StackStructure;
		friend class Executable;

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

	protected:
		size_t m_memoryOffset{};
		AsysValue* m_pAsysValue{};
	};

	template<typename T>
	class AsysVaribleT : public AsysVariable
	{
	public:
		AsysVaribleT()
		{

		}

		virtual AsysVariable* clone() const override
		{
			auto newVar = new AsysVaribleT<T>();
			newVar->deepCopy(*this);

			return newVar;
		}

		virtual size_t getAsysValueSize() const override
		{
			return sizeof(AsysValueT<T>);
		}

		virtual void construct(void* address) override
		{
			m_pAsysValue = new (address)AsysValueT<T>;
			m_pAddress = address;
		}

		virtual void destruct() override
		{
			auto* pCastValue = dynamic_cast<AsysVaribleT<T>*>(m_pAsysValue);
			pCastValue->~AsysVaribleT<T>();
			m_pAsysValue = nullptr;
		}

		virtual void reset() override
		{
			if (m_pAsysValue)
			{
				destruct();
			}

			construct(m_pAddress);
		}

	private:
		void* m_pAddress{};

		/*	template<typename T1>
			AsysVaribleT& operator = (const AsysVaribleT<T1>& var)
			{
			AsysValueT<T>* nativeValue = dynamic_cast<AsysValueT<T>*>(m_pNativeValue);
			if (!nativeValue)
			{
			m_pNativeValue = nativeValue = new AsysValueT<T>;
			}

			AsysValueT<T1>* nativeValue1 = dynamic_cast<AsysValueT<T1>*>(var.m_pNativeValue);
			nativeValue->getValue() = nativeValue1->getValue();

			return *this;
			}*/

		/*operator T()
		{
			AsysValueT<T>* nativeValue = dynamic_cast<AsysValueT<T>*>(m_pNativeValue);
			return nativeValue->getValue();
		}*/

		/*template<typename T1>
		AsysVaribleT& operator = (const T1& var)
		{
			AsysValueT<T>* nativeValue = dynamic_cast<AsysValueT<T>*>(m_pNativeValue);
			if (!nativeValue)
			{
				m_pNativeValue = nativeValue = new AsysValueT<T>;
			}

			nativeValue->getValue() = var;

			return *this;
		}*/
	};

	class VariableList
	{
	public:
		VariableList(std::initializer_list<AsysVariable> varList);

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
		template<typename... ValueTypes>
		ValueList(ValueTypes... values)
		{

		}

		template<typename... ValueTypes>
		ValueList(const Const& constValue, ValueTypes... values)
		{
			m_values.resize(m_values.size() + 1);
			m_values[m_values.size() - 1].asysConst.copyFrom(asysValue);
			m_values[m_values.size() - 1].isConst = true;
			ValueList(values...);
		}

		template<typename... ValueTypes>
		ValueList(const AsysVariable& variable, ValueTypes... values)
		{
			m_values.resize(m_values.size() + 1);
			m_values[m_values.size() - 1].asysVarible.shallowCopy(variable);
			m_values[m_values.size() - 1].isConst = false;
			ValueList(values...);
		}

		size_t getLength() const
		{
			return m_values.size();
		}

	private:
		void forEachValue(const std::function<void(bool, const AsysVariable&, const Const&)>& callback) const
		{
			if (callback)
			{
				for (auto val : m_values)
				{
					callback(val.isConst, val.asysVarible, val.asysConst);
				}
			}
		}

	private:
		std::vector<InnerValue> m_values;
	};
}