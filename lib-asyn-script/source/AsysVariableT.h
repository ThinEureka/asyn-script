/*!
 * \file AsysVariableT.h
 * \date 12-2-2016 17:24:10
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#include "AsysVariable.h"
#include "AsysValueT.h"

namespace asys
{
	template<typename T>
	class AsysGeneralVariableT : public AsysVariable
	{
	public:
		AsysGeneralVariableT()
		{

		}

		AsysGeneralVariableT(const char* name, int)
		{
			this->m_pName = name;
		}

		operator T() const
		{
			return r();
		}

		//not thread-safe
		T& r() const
		{
			auto pAsysValue = getAsysValueFromCurMainThreadMachine();
			auto asysValue = dynamic_cast<AsysValueT<T>*>(pAsysValue);

			return asysValue->getNativeValueReference();
		}

		//for thread-safe
		T& sr(Machine* machine) const
		{
			auto pAsysValue = getAsysValue(machine);
			auto pCastValue = dynamic_cast<AsysValueT<T>*>(pAsysValue);
			return pCastValue->getNativeValueReference();
		}

		// prefix ++
		T& operator ++ () const
		{
			return ++r();
		}

		// postfix ++
		T operator ++ (int) const
		{
			return r()++;
		}

		// prefix --
		T& operator -- () const
		{
			return --r();
		}

		// postfix ++
		T operator -- (int) const
		{
			return r()--;
		}

	private:
		virtual AsysVariable* clone() const override
		{
			auto newVar = new AsysGeneralVariableT<T>();
			newVar->deepCopy(*this);

			return newVar;
		}

		virtual size_t getAsysValueSize() const override
		{
			return sizeof(AsysValueT<T>);
		}

		virtual void construct(AsysValue* address) const override
		{
			new (address)AsysValueT<T>;
		}

		virtual void destruct(AsysValue* address) const override
		{
			auto* pCastValue = static_cast<AsysValueT<T>*>(address);
			pCastValue->~AsysValueT<T>();
		}

		virtual VariableViewer* createVariableViewer(void* address) const override
		{
			auto viewer = new VariableViewerT<T>;
			auto pCastValue = static_cast<AsysValueT<T>*>(address);
			viewer->setTarget(m_pName, &pCastValue->getNativeValueReference());

			return viewer;
		}
	};


	template<typename T>
	class AsysVariableT : public AsysGeneralVariableT<T>
	{
	public:
		AsysVariableT()
		{

		}

		AsysVariableT(const char* name, int)
			: AsysGeneralVariableT<T>(name, 0)
		{}

		template<typename T1>
		T& operator = (const T1& var) const
		{
			return this->r() = var;
		}

		template<typename T1>
		T& operator += (const T1& var) const
		{
			return this->r() += var;
		}

		template<typename T1>
		T& operator -= (const T1& var) const
		{
			return this->r() -= var;
		}

		template<typename T1>
		T& operator *= (const T1& var) const
		{
			return this->r() *= var;
		}

		template<typename T1>
		T& operator /= (const T1& var) const
		{
			return this->r() /= var;
		}

		template<typename T1>
		T& operator %= (const T1& var) const
		{
			return this->r() %= var;
		}

		template<typename T1>
		T& operator &= (const T1& var) const
		{
			return this->r() &= var;
		}

		template<typename T1>
		T& operator |= (const T1& var) const
		{
			return this->r() |= var;
		}

		template<typename T1>
		T& operator ^= (const T1& var) const
		{
			return this->r() ^= var;
		}

		template<typename T1>
		T& operator <<= (const T1& var) const
		{
			return this->r() <<= var;
		}

		template<typename T1>
		T& operator >>= (const T1& var) const
		{
			return this->r() >>= var;
		}

	private:
		virtual AsysVariableT* clone() const override
		{
			auto newVar = new AsysVariableT<T>();
			newVar->deepCopy(*this);

			return newVar;
		}
	};

	template<typename T>
	class AsysVariableT<T*> : public AsysGeneralVariableT<T*>
	{
	public:
		AsysVariableT()
		{

		}

		AsysVariableT(const char* name, int)
			: AsysGeneralVariableT<T*>(name, 0)
		{
		}

		T& operator *() const
		{
			return *this->r();
		}

		T* operator ->() const
		{
			return this->r();
		}

		template<typename T1>
		T*& operator = (const T1& var) const
		{
			return this->r() = var;
		}

		template<typename T1>
		T*& operator += (const T1& var) const
		{
			return this->r() += var;
		}

		template<typename T1>
		T*& operator -= (const T1& var) const
		{
			return this->r() -= var;
		}

	private:
		virtual AsysVariableT* clone() const override
		{
			auto newVar = new AsysVariableT<T*>();
			newVar->deepCopy(*this);

			return newVar;
		}
	};
}