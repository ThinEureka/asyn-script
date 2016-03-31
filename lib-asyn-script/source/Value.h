
/*!
 * \file Value.h
 * \date 02-10-2016 11:56:44
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include "Define.h"
#include <functional>
#include <limits>
#include <string>

namespace asys
{
	class Value
	{
	public:
		static const Value plus(const Value& val1, const Value& val2);
		static const Value minus(const Value& val1, const Value& val2);
		static const Value multiply(const Value& val1, const Value& val2);
		static const Value divide(const Value& val1, const Value& val2);

		static const Value equal(const Value& val1, const Value& val2);
		static const Value not_equal(const Value& val1, const Value& val2);
		static const Value greater_than(const Value& val1, const Value& val2);
		static const Value greater_than_equal(const Value& val1, const Value& val2);
		static const Value less_than(const Value& val1, const Value& val2);
		static const Value less_than_equal(const Value& val1, const Value& val2);
		static const Value and_(const Value& val1, const Value& val2);
		static const Value or_(const Value& val1, const Value& val2);

		static const Value not_(const Value& val);
		static const Value unary_plus(const Value& val);
		static const Value unary_minus(const Value& val);

		static std::function<const Value(const Value&, const Value&)> getBinaryOperator(Operator eOperator);
		static std::function<const Value(const Value&)> getUnaryOperator(Operator eOperator);

	public:
		Value(){}
		Value(const std::string& str) { m_strContent = str; }

		Value(const char* pchar) { m_strContent = pchar; }

		Value(bool boolValue) { m_strContent = (boolValue ? "true" : "false"); }

		Value(int intValue) { m_strContent = patch::to_string(intValue); }
		Value(long long llValue) { m_strContent = patch::to_string(llValue); }

		Value(float floatValue) { m_strContent = patch::to_string(floatValue); }
		Value(double doubleValue) { m_strContent = patch::to_string(doubleValue); }
		Value(long double ldValue) { m_strContent = patch::to_string(ldValue); }

		virtual ~Value(){}

		const std::string& content() const { return m_strContent; }

		bool toBool() const { return m_strContent != "false" && m_strContent != "0" && m_strContent != "null"; }
		int toInt() const;
		long long toLongLong() const;
		float toFloat() const;
		double toDouble() const;
		long double toLongDouble() const;

		bool isBoolean() const { return m_strContent == "true" || m_strContent == "false"; }
		bool isInteger(long long& llValue) const;
		bool isNumeric(long double& ldValue) const;

	private:
		std::string m_strContent;
	};
}