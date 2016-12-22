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
#include "Core.h"
#include "AsysValueT.h"
#include "Core.h"

namespace asys
{
	template<typename T>
	class AsysVariableT : public AsysVariable
	{
	public:
		AsysVariableT()
		{

		}

		AsysVariableT(const char* name, int)
		{
			m_pName = name;
		}

		operator T() const
		{
			return r();
		}

		//not thread-safe
		T& r() const
		{
			if (m_pMachine != Machine::getCurMainThreadMachine())
			{
				m_pMachine = Machine::getCurMainThreadMachine();
				m_pAsysValue = nullptr;
			}

			if (!m_pAsysValue)
			{
				m_pAsysValue = m_pMachine->getAsysValue(*this);
			}

			auto asysValue = dynamic_cast<AsysValueT<T>*>(m_pAsysValue);
			return asysValue->getNativeValueReference();
		}

		template<typename T1>
		const AsysVariableT& operator = (const T1& var) const
		{
			r() = var;
			return *this;
		}

	private:
		virtual AsysVariable* clone() const override
		{
			auto newVar = new AsysVariableT<T>();
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
			auto* pCastValue = static_cast<AsysValue<T>*>(address);
			pCastValue->~AsysValue<T>();
		}

		//virtual VariableViewer* createVariableViewer() const
		//{
		//	auto viewer = new VariableViewerT<T>;
		//	auto pCastValue = dynamic_cast<AsysValueT<T>*>(m_pAsysValue);
		//	viewer->setTarget(m_pName, &pCastValue->getNativeValueReference());

		//	return viewer;
		//}

	private:
		mutable Machine* m_pMachine{};

		void setMachine(Machine* machine)
		{
			m_pMachine = machine;
		}
	};
}