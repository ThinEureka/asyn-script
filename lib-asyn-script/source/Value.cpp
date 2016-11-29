//
///*!
// * \file Value.cpp
// * \date 02-10-2016 11:56:49
// * 
// * 
// * \author cs 04nycs@gmail.com
// */
//
//#include "Value.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <math.h>
//#include <errno.h>
//
//using namespace std;
//
//const asys::Value asys::Value::plus(const asys::Value& val1, const asys::Value& val2)
//{
//	long long ll1, ll2;
//	if (val1.isInteger(ll1) && val2.isInteger(ll2))
//	{
//		return ll1 + ll2;
//	}
//
//	return val1.toLongDouble() + val2.toLongDouble();
//}
//
//const asys::Value asys::Value::minus(const asys::Value& val1, const asys::Value& val2)
//{
//	long long ll1, ll2;
//	if (val1.isInteger(ll1) && val2.isInteger(ll2))
//	{
//		return ll1 - ll2;
//	}
//
//	return val1.toLongDouble() - val2.toLongDouble();
//}
//
//const asys::Value asys::Value::multiply(const asys::Value& val1, const asys::Value& val2)
//{
//	long long ll1, ll2;
//	if (val1.isInteger(ll1) && val2.isInteger(ll2))
//	{
//		return ll1 * ll2;
//	}
//
//	return val1.toLongDouble() * val2.toLongDouble();
//}
//
//const asys::Value asys::Value::divide(const asys::Value& val1, const asys::Value& val2)
//{
//	long long ll1, ll2;
//	if (val1.isInteger(ll1) && val2.isInteger(ll2))
//	{
//		return ll1 / ll2;
//	}
//
//	return val1.toLongDouble() / val2.toLongDouble();
//}
//
//const asys::Value asys::Value::equal(const asys::Value& val1, const asys::Value& val2)
//{
//	return val1.m_strContent == val2.m_strContent;
//}
//
//const asys::Value asys::Value::not_equal(const asys::Value& val1, const asys::Value& val2)
//{
//	return val1.m_strContent != val2.m_strContent;
//}
//
//const asys::Value asys::Value::greater_than(const asys::Value& val1, const asys::Value& val2)
//{
//	long long ll1, ll2;
//	if (val1.isInteger(ll1) && val2.isInteger(ll2))
//	{
//		return ll1 > ll2;
//	}
//
//	return val1.toLongDouble() > val2.toLongDouble();
//}
//
//const asys::Value asys::Value::greater_than_equal(const asys::Value& val1, const asys::Value& val2)
//{
//	long long ll1, ll2;
//	if (val1.isInteger(ll1) && val2.isInteger(ll2))
//	{
//		return ll1 >= ll2;
//	}
//
//	return val1.toLongDouble() >= val2.toLongDouble();
//}
//
//const asys::Value asys::Value::less_than(const asys::Value& val1, const asys::Value& val2)
//{
//	long long ll1, ll2;
//	if (val1.isInteger(ll1) && val2.isInteger(ll2))
//	{
//		return ll1 < ll2;
//	}
//
//	return val1.toLongDouble() < val2.toLongDouble();
//}
//
//const asys::Value asys::Value::less_than_equal(const asys::Value& val1, const asys::Value& val2)
//{
//	long long ll1, ll2;
//	if (val1.isInteger(ll1) && val2.isInteger(ll2))
//	{
//		return ll1 <= ll2;
//	}
//
//	return val1.toLongDouble() <= val2.toLongDouble();
//}
//
//const asys::Value asys::Value::and_(const asys::Value& val1, const asys::Value& val2)
//{
//	return val1.toBool() && val2.toBool();
//}
//
//const asys::Value asys::Value::or_(const asys::Value& val1, const asys::Value& val2)
//{
//	return val1.toBool() || val2.toBool();
//}
//
//const asys::Value asys::Value::not_(const asys::Value& val)
//{
//	return !val.toBool();
//}
//
//const asys::Value asys::Value::unary_plus(const asys::Value& val)
//{
//	long long llValue;
//	if (val.isInteger(llValue))
//	{
//		return llValue;
//	}
//	else
//	{
//		long double ldValue;
//		if (val.isNumeric(ldValue))
//		{
//			return ldValue;
//		}
//	}
//
//	return "0";
//}
//
//const asys::Value asys::Value::unary_minus(const asys::Value& val)
//{
//	long long llValue;
//	if (val.isInteger(llValue))
//	{
//		return -llValue;
//	}
//	else
//	{
//		long double ldValue;
//		if (val.isNumeric(ldValue))
//		{
//			return -ldValue;
//		}
//	}
//
//	return "0";
//}
//
//std::function<const asys::Value(const asys::Value&, const asys::Value&)> asys::Value::getBinaryOperator(asys::Operator eOperator)
//{
//	switch (eOperator)
//	{
//	case asys::Operator::plus:
//		return Value::plus;
//		break;
//	case asys::Operator::minus:
//		return Value::minus;
//		break;
//	case asys::Operator::multiply:
//		return Value::multiply;
//		break;
//	case asys::Operator::divide:
//		return Value::divide;
//		break;
//	case asys::Operator::equal:
//		return Value::equal;
//		break;
//	case asys::Operator::not_equal:
//		return Value::not_equal;
//		break;
//	case asys::Operator::greater_than:
//		return Value::greater_than;
//		break;
//	case asys::Operator::greater_than_equal:
//		return Value::greater_than_equal;
//		break;
//	case asys::Operator::less_than:
//		return Value::less_than;
//		break;
//	case asys::Operator::less_than_equal:
//		return Value::less_than_equal;
//		break;
//	case asys::Operator::and_:
//		return Value::and_;
//		break;
//	case asys::Operator::or_:
//		return Value::or_;
//		break;
//	default:
//		break;
//	}
//
//	return nullptr;
//}
//
//std::function<const asys::Value(const asys::Value&)> asys::Value::getUnaryOperator(asys::Operator eOperator)
//{
//	switch (eOperator)
//	{
//	case asys::Operator::plus:
//		return Value::unary_plus;
//		break;
//	case asys::Operator::minus:
//		return Value::unary_minus;
//		break;
//	case asys::Operator::not_:
//		return Value::not_;
//		break;
//	default:
//		break;
//	}
//
//	return nullptr;
//}
//
//int asys::Value::toInt() const
//{
//	auto start = m_strContent.c_str();
//
//	errno = 0;
//	char *end{};
//	auto llValue = strtoll(start, &end, 10);
//
//	if (errno == ERANGE ||
//		llValue > std::numeric_limits<int>::max() ||
//		llValue < std::numeric_limits<int>::min() ||
//		(end - start) != m_strContent.length())
//	{
//		return 0;
//	}
//
//	return static_cast<int>(llValue);
//}
//
//long long asys::Value::toLongLong() const
//{
//	auto start = m_strContent.c_str();
//
//	errno = 0;
//	char *end{};
//	auto llValue = strtoll(start, &end, 10);
//
//	if (errno == ERANGE ||
//		(end - start) != m_strContent.length() // Non-numerics?
//		)
//	{
//		return 0;
//	}
//
//	return llValue;
//}
//
//float asys::Value::toFloat() const
//{
//	auto start = m_strContent.c_str();
//
//	errno = 0;
//	char *end{};
//	auto fValue = strtof(start, &end);
//
//	if (errno == ERANGE ||
//		(end - start) != m_strContent.length())
//	{
//		return 0.0f;
//	}
//
//	return fValue;
//}
//
//double asys::Value::toDouble() const
//{
//	auto start = m_strContent.c_str();
//
//	errno = 0;
//	char *end{};
//	double dValue = strtod(start, &end);
//
//	if (errno == ERANGE ||
//		(end - start) != m_strContent.length())
//	{
//		return 0.0;
//	}
//
//	return dValue;
//}
//
//long double asys::Value::toLongDouble() const
//{
//	auto start = m_strContent.c_str();
//
//	errno = 0;
//	char *end{};
//	//strtold is not supported in gcc
//	long double dValue = strtod(start, &end);
//
//	if (errno == ERANGE ||
//		(end - start) != m_strContent.length())
//	{
//		return 0.0;
//	}
//
//	return dValue;
//}
//
//bool asys::Value::isInteger(long long& llValue) const
//{
//	auto start = m_strContent.c_str();
//
//	errno = 0;
//	char *end{};
//	llValue = strtoll(start, &end, 10);
//
//	if (errno == ERANGE ||
//		(end - start) != m_strContent.length())
//	{
//		return false;
//	}
//
//	return true;
//}
//
//bool asys::Value::isNumeric(long double& ldValue) const
//{
//	auto start = m_strContent.c_str();
//
//	errno = 0;
//	char *end{};
//
//	//strtold is not supported in gcc
//	ldValue = strtod(start, &end);
//
//	if (errno == ERANGE ||
//		(end - start) != m_strContent.length())
//	{
//		return false;
//	}
//
//	return true;
//}
//
//asys::Value& asys::Value::operator =(const std::string& str)
//{
//	m_strContent = str; return *this;
//}
//
//asys::Value& asys::Value::operator=(const char* pchar)
//{
//	m_strContent = pchar; return *this;
//}
//
