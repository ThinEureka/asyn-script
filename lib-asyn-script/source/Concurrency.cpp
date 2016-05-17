
/*!
 * \file Concurrency.cpp
 * \date 02-10-2016 12:47:50
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#include "Concurrency.h"

asys::Executable* asys::ConcurrencyAllCode::compile()
{
	return new ConcurrencyAllExecutable(m_codes);
}

asys::ConcurrencyAllExecutable::ConcurrencyAllExecutable(const std::vector<Code*>& codes)
{
	for (auto code : codes)
	{
		ExcutableState state;
		state.code = code;

		m_excutableStates.push_back(state);
	}
}

asys::ConcurrencyAllExecutable::~ConcurrencyAllExecutable()
{
	for (auto& state : m_excutableStates)
	{
		if (state.extutable)
		{
			state.extutable->release();
		}
	}

	m_excutableStates.clear();
}

asys::CodeFlow asys::ConcurrencyAllExecutable::run()
{
	auto retCode = CodeFlow::next_;
	for (auto& state : m_excutableStates)
	{
		if (!state.finished)
		{
			if (!state.extutable)
			{
				state.extutable = state.code->compile();
				state.extutable->retain();
			}

			auto ret = state.extutable->run();

			if (ret == CodeFlow::next_)
			{
				state.extutable->release();
				state.extutable = nullptr;
				state.finished = true;
			}
			else
			{
				retCode = CodeFlow::yield_;
			}
		}
	}

	return retCode;
}

asys::Executable* asys::ConcurrencyAnyCode::compile()
{
	return new ConcurrencyAnyExecutable(m_codes);
}

asys::ConcurrencyAnyExecutable::ConcurrencyAnyExecutable(const std::vector<Code*>& codes)
{
	for (auto code : codes)
	{
		ExcutableState state;
		state.code = code;

		m_excutableStates.push_back(state);
	}
}

asys::ConcurrencyAnyExecutable::~ConcurrencyAnyExecutable()
{
	for (auto& state : m_excutableStates)
	{
		if (state.executable)
		{
			state.executable->release();
		}
	}

	m_excutableStates.clear();
}

asys::CodeFlow asys::ConcurrencyAnyExecutable::run()
{
	auto retCode = CodeFlow::next_;
	for (auto& state : m_excutableStates)
	{
		if (!state.executable)
		{
			state.executable = state.code->compile();
			state.executable->retain();
		}

		auto ret = state.executable->run();

		if (ret == CodeFlow::next_)
		{
			state.executable->release();
			state.executable = nullptr;

			return CodeFlow::next_;
		}
		else
		{
			retCode = CodeFlow::yield_;
		}
	}

	return retCode;
}
