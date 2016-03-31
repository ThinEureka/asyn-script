
/*!
 * \file Simple.h
 * \date 02-10-2016 15:13:36
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include "Core.h"
#include <functional>

namespace asys
{
	class SimpleLamdaExecutable : public Executable
	{
	public:
		SimpleLamdaExecutable(const std::function<RetCode(Executable*)>& callback) : m_callback(callback) {};

		RetCode run() override;

	private:
		std::function<RetCode(Executable*)> m_callback{};
	};

	class SimpleLamdaCode : public Code
	{
	public:
		SimpleLamdaCode(const std::function<RetCode(Executable*)>& callback)
		{
			m_excutable = new SimpleLamdaExecutable(callback);
			m_excutable->retain();
		}

		virtual ~SimpleLamdaCode()
		{
			m_excutable->release();
		}

		Executable* compile() override;

	private:
		SimpleLamdaExecutable* m_excutable{};
	};

	template<typename T>
	class SimpleCode : public Code
	{
	public:
		Executable* compile() override { return new T(); }
	};

	template<typename T, typename P1>
	class Simple1Code : public Code
	{
	public:
		Simple1Code(const P1& p1) : m_p1(p1) {}

		Executable* compile() override { return new T(m_p1); }

	private:
		P1 m_p1;
	};

	template<typename T, typename P1, typename P2>
	class Simple2Code : public Code
	{
	public:
		Simple2Code(const P1& p1, const P2& p2) : m_p1(p1), m_p2(p2) {}

		Executable* compile() override { return new T(m_p1, m_p2); }

	private:
		P1 m_p1;
		P2 m_p2;
	};

	template<typename T, typename P1, typename P2, typename P3>
	class Simple3Code : public Code
	{
	public:
		Simple3Code(const P1& p1, const P2& p2, const P3& p3) : m_p1(p1), m_p2(p2), m_p3(p3) {}

		Executable* compile() override { return new T(m_p1, m_p2, m_p3); }

	private:
		P1 m_p1;
		P2 m_p2;
		P3 m_p3;
	};
}