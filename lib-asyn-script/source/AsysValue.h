/*!
 * \file AsysValue.h
 * \date 11-29-2016 14:13:01
 * 
 * 
 * \author cs (04nycs@gmail.com)
 * https://github.com/ThinEureka/asyn-script
 */

#pragma once

#include <string>

namespace asys
{
	class AsysValue
	{
	public:
		virtual ~AsysValue() {}

		virtual bool toBool() const = 0;

		virtual char toChar() const = 0;
		virtual unsigned char toUchar() const = 0;

		virtual short toShort() const = 0;
		virtual unsigned short toUShort() const = 0;

		virtual int toInt() const = 0;
		virtual unsigned int toUInt() const = 0;

		virtual long toLong() const = 0;
		virtual unsigned long toULong() const = 0;

		virtual long long toLongLong() const = 0;
		virtual unsigned long long toULongLong() const = 0;

		virtual float toFloat() const = 0;
		virtual double toDouble() const = 0;
		virtual long double toLongDouble() const = 0;

		virtual void* toVoidPointer() const = 0;

		virtual AsysValue* clone() const = 0;

		virtual std::string toString() const = 0;

		virtual void assign(const AsysValue& value) = 0;
	};
}