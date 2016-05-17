/*!
 * \file Sequence.cpp
 * \date 02-10-2016 12:23:17
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#include "Sequence.h"

asys::Executable* asys::SequenceCode::compile()
{
	return new SequenceExecutable(m_codes);
}

asys::SequenceExecutable::SequenceExecutable(const std::vector<Code*>& codes)
{
	m_codes = codes;
}

asys::SequenceExecutable::~SequenceExecutable()
{
	if (m_currentExe)
	{
		m_currentExe->release();
		m_currentExe = nullptr;
	}

	m_codes.clear();
}

asys::CodeFlow asys::SequenceExecutable::run()
{
	while (true)
	{
		if (!m_currentExe)
		{
			++m_nCurCodeIndex;

			if (m_nCurCodeIndex < static_cast<int>(m_codes.size()))
			{
				auto code = m_codes[m_nCurCodeIndex];
				m_currentExe = code->compile();
				m_currentExe->retain();
			}
			else
			{
				return CodeFlow::next_;
			}
		}

		auto retValue = m_currentExe->run();
		if (retValue == CodeFlow::next_)
		{
			m_currentExe->release();
			m_currentExe = nullptr;
		}
		else
		{
			return CodeFlow::yield_;
		}
	}

	return CodeFlow::next_;
}
