/*!
 * \file AsysVariableT.h
 * \date 12-2-2016 17:24:10
 * 
 * 
 * \author cs (04nycs@gmail.com)
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
			if (!m_pExecutable)
			{
				m_pExecutable = Executable::getMainExecutable();
			}

			auto asysValue = dynamic_cast<AsysValueT<T>*>(m_pExecutable->getAsysValue(*this));
			return asysValue->getNativeValue();
		}

		T& r() const
		{
			if (!m_pExecutable)
			{
				m_pExecutable = Executable::getMainExecutable();
			}

			auto asysValue = dynamic_cast<AsysValueT<T>*>(m_pExecutable->getAsysValue(*this));
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

		virtual void construct(void* address) override
		{
			m_pAsysValue = new (address)AsysValueT<T>;
			m_pAddress = address;
		}

		virtual void destruct() override
		{
			auto* pCastValue = dynamic_cast<AsysVariableT<T>*>(m_pAsysValue);
			pCastValue->~AsysVariableT<T>();
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

		virtual VariableViewer* createVariableViewer() const
		{
			auto viewer = new VariableViewerT<T>;
			auto pCastValue = dynamic_cast<AsysValueT<T>*>(m_pAsysValue);
			viewer->setTarget(m_pName, &pCastValue->getNativeValueReference());

			return viewer;
		}

	private:
		void* m_pAddress{};
		mutable Executable* m_pExecutable{};

		void setExecutable(Executable* executable)
		{
			m_pExecutable = executable;
		}
	};
}