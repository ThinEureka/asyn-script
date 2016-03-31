/*!
 * \file Concurrency.h
 * \date 02-10-2016 12:46:40
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include "CompoundCode.h"
#include "Core.h"
#include <vector>

namespace asys
{
	class ConcurrencyAllCode : public CompoundCode
	{
	public:
		ConcurrencyAllCode(){};
		ConcurrencyAllCode(const std::vector<Code*>& codes) : CompoundCode(codes) {};

		Executable* compile() override;
	};

	class ConcurrencyAllExecutable : public Executable
	{
	public:
		struct ExcutableState
		{
			bool finished{ false };
			Executable* extutable{};
			Code* code{};
		};

	public:
		ConcurrencyAllExecutable(const std::vector<Code*>& codes);
		virtual ~ConcurrencyAllExecutable();

		RetCode run() override;

	private:
		std::vector<ExcutableState> m_excutableStates;
	};

	class ConcurrencyAnyCode : public CompoundCode
	{
	public:
		ConcurrencyAnyCode(){};
		ConcurrencyAnyCode(const std::vector<Code*>& codes) : CompoundCode(codes) {};

		Executable* compile() override;
	};

	class ConcurrencyAnyExecutable : public Executable
	{
	public:
		struct ExcutableState
		{
			Executable* executable{};
			Code* code{};
		};

	public:
		ConcurrencyAnyExecutable(const std::vector<Code*>& codes);
		virtual ~ConcurrencyAnyExecutable();

		RetCode run() override;

	private:
		std::vector<ExcutableState> m_excutableStates;
	};
}