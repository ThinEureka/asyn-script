/*!
 * \file AsysValue.h
 * \date 11-29-2016 14:13:01
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#include "AsysValue.h"
#include <assert.h>
#include <string>

namespace asys
{
	template<typename T>
	class AsysValueGeneralT : public AsysValue
	{
	public:
		virtual bool toBool() const override
		{
			assert(false); //assert("no default conversion to boolean value for custom class");
			return false;
		}

		virtual char toChar() const override
		{
			assert(false); //"no default conversion to char value for custom class"
			return '\0';
		}

		virtual unsigned char toUchar() const override
		{
			assert(false); // "no default conversion to unsigned char for custom class");
			return '\0';
		}

		virtual short toShort() const override
		{
			assert(false);// "no default conversion to short for custom class");
			return 0;
		}

		virtual unsigned short toUShort() const override
		{
			assert(false); // "no default conversion to ushort for custom class");
			return 0;
		}

		virtual int toInt() const override
		{
			assert(false); // "no default conversion to int for custom class");
			return 0;
		}

		virtual unsigned int toUInt() const override
		{
			assert(false); // "no default conversion to unit for custom class");
			return 0;
		}

		virtual long toLong() const override
		{
			assert(false); // "no default conversion to long for custom class");
			return 0;
		}

		virtual unsigned long toULong() const override
		{
			assert(false); // "no default conversion to unsigned long for custom class");
			return 0;
		}

		virtual long long toLongLong() const override
		{
			assert(false); // "no default conversion to long long for custom class");
			return 0;
		}

		virtual unsigned long long toULongLong() const override
		{
			assert(false); // "no default conversion to unsigned long long for custom class");
			return 0;
		}

		virtual float toFloat() const override
		{
			assert(false); // "no default conversion to unsigned long long for custom class");
			return 0.0f;
		}

		virtual double toDouble() const override
		{
			assert(false); // "no default conversion to double for custom class");
			return 0.0;
		}

		virtual long double toLongDouble() const override
		{
			assert(false); // "no default conversion to long double for custom class"
			return 0.0;
		}

		virtual void* toVoidPointer() const override
		{
			assert(false); // "no default conversion to void pointer for custom class");
			return nullptr;
		}

		virtual std::string toString() const override
		{
			assert(false);
			//"no default conversion to string for custom class");
			return "";
		}

		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueGeneralT<T>();
			newValue->m_nativeValue = this->m_nativeValue;
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			auto pCastValue = dynamic_cast<const AsysValueGeneralT*>(&value);
			if (!pCastValue)
			{
				//the assignments of custom-defined types variable only accept 
				//values with exactly the same type, only variable with built-in
				//types and pointers can be assigned values of convertible types.
				//because template mechanism does not reflect the original class inheritance 
				//relationship, so assigning a sub-class value to a base-class variable
				//forbidden as well. It's better to use pointer variable here, since 
				//the pointers can be converted freely.
				assert(false);
			}

			this->m_nativeValue = pCastValue->m_nativeValue;
		}

		const T& getNativeValue() const
		{
			return this->m_nativeValue;
		}

		void setNativeValue(const T& nativeValue)
		{
			this->m_nativeValue = nativeValue;
		}

		T& getNativeValueReference()
		{
			return this->m_nativeValue;
		}

	protected:
		T m_nativeValue{};
	};

	template <typename T>
	class AsysValueT : public AsysValueGeneralT<T>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<T>();
			newValue->m_nativeValue = this->m_nativeValue;
			return newValue;
		}
	};

	template<>
	class AsysValueT<std::nullptr_t> : public AsysValueGeneralT<std::nullptr_t>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<std::nullptr_t>();
			newValue->setNativeValue(nullptr);
			return newValue;
		}

		virtual void* toVoidPointer() const override
		{
			return nullptr;
		}

		virtual bool toBool() const override
		{
			return false;
		}

		virtual void assign(const AsysValue& value) override
		{
			auto pAsysValue = dynamic_cast<const AsysValueT<std::nullptr_t>*>(&value);
			if (!pAsysValue)
			{
				//not allowed to assigned other type to nullptr_t;
				assert(false);
			}

			this->m_nativeValue = nullptr;
		}
	};

	template<typename T>
	class AsysValueT<T*> : public AsysValueGeneralT<T*>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<T*>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual bool toBool() const override
		{
			return this->m_nativeValue != nullptr;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = (T*)(value.toVoidPointer());
		}

		virtual void* toVoidPointer() const override
		{
			return (void*)(this->m_nativeValue);
		}

		virtual std::string toString() const override
		{
			assert(false); // "no default conversion to string for general pointer type");
			return "";
		}
	};

	template<>
	class AsysValueT<const char*> : public AsysValueGeneralT<const char*>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<const char*>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual std::string toString() const override
		{
			auto castValue = static_cast<const char*>(this->m_nativeValue);
			return castValue;
		}

		virtual bool toBool() const override
		{
			return this->m_nativeValue != nullptr;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = static_cast<const char*>(value.toVoidPointer());
		}
	};

	template<typename T>
	class AsysValueBuildInT : public AsysValueGeneralT<T>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueBuildInT<T>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual bool toBool() const override
		{
			return this->m_nativeValue != 0;
		}

		virtual char toChar() const override
		{
			return static_cast<char>(this->m_nativeValue);
		}

		virtual unsigned char toUchar() const override
		{
			return static_cast<unsigned char>(this->m_nativeValue);
		}

		virtual short toShort() const override
		{
			return static_cast<short>(this->m_nativeValue);
		}

		virtual unsigned short toUShort() const override
		{
			return static_cast<unsigned short>(this->m_nativeValue);
		}

		virtual int toInt() const override
		{
			return static_cast<int>(this->m_nativeValue);
		}

		virtual unsigned int toUInt() const override
		{
			return static_cast<unsigned int>(this->m_nativeValue);
		}

		virtual long toLong() const override
		{
			return static_cast<long>(this->m_nativeValue);
		}

		virtual unsigned long toULong() const override
		{
			return static_cast<unsigned long>(this->m_nativeValue);
		}

		virtual long long toLongLong() const override
		{
			return static_cast<long long>(this->m_nativeValue);
		}

		virtual unsigned long long toULongLong() const override
		{
			return static_cast<unsigned long long>(this->m_nativeValue);
		}

		virtual float toFloat() const override
		{
			return static_cast<float>(this->m_nativeValue);
		}

		virtual double toDouble() const override
		{
			return static_cast<double>(this->m_nativeValue);
		}

		virtual long double toLongDouble() const override
		{
			return static_cast<long double>(this->m_nativeValue);
		}

		virtual void* toVoidPointer() const override
		{
			assert("no default conversion to double for custom class");
			return nullptr;
		}
	};

	template<>
	class AsysValueT<bool> : public AsysValueBuildInT<bool>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<bool>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual bool toBool() const override
		{
			return this->m_nativeValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toBool();
		}
	};

	template<>
	class AsysValueT<char> : public AsysValueBuildInT<char>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<char>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toChar();
		}
	};

	template<>
	class AsysValueT<unsigned char> : public AsysValueBuildInT<unsigned char>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<unsigned char>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toUchar();
		}
	};

	template<>
	class AsysValueT<short> : public AsysValueBuildInT<short>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<short>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toShort();
		}
	};

	template<>
	class AsysValueT<unsigned short> : public AsysValueBuildInT<unsigned short>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<unsigned short>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toUShort();
		}
	};

	template<>
	class AsysValueT<int> : public AsysValueBuildInT<int>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<int>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toInt();
		}
	};

	template<>
	class AsysValueT<unsigned int> : public AsysValueBuildInT<unsigned int>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<unsigned int>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toUInt();
		}
	};

	template<>
	class AsysValueT<long> : public AsysValueBuildInT<long>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<long>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toLong();
		}
	};

	template<>
	class AsysValueT<unsigned long> : public AsysValueBuildInT<unsigned long>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<unsigned long>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toULong();
		}
	};

	template<>
	class AsysValueT<long long> : public AsysValueBuildInT<long long>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<long long>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toLongLong();
		}
	};

	template<>
	class AsysValueT<unsigned long long> : public AsysValueBuildInT<unsigned long long>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<unsigned long long>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toULongLong();
		}
	};

	template<>
	class AsysValueT<float> : public AsysValueBuildInT<float>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<float>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toFloat();
		}
	};

	template<>
	class AsysValueT<double> : public AsysValueBuildInT<double>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<double>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toDouble();
		}
	};

	template<>
	class AsysValueT<long double> : public AsysValueBuildInT<long double>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<long double>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toLongDouble();
		}
	};

	template<>
	class AsysValueT<std::string> : public AsysValueGeneralT<std::string>
	{
	public:
		virtual AsysValue* clone() const override
		{
			auto newValue = new AsysValueT<std::string>();
			newValue->setNativeValue(this->m_nativeValue);
			return newValue;
		}

		virtual void assign(const AsysValue& value) override
		{
			this->m_nativeValue = value.toString();
		}

		virtual std::string toString() const override
		{
			return this->m_nativeValue;
		}
	};
}