/*!
 * \file Sequence.h
 * \date 02-10-2016 12:23:11
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include "Core.h"
#include <vector>
#include "CompoundCode.h"

namespace asys
{
	class SequenceCode : public CompoundCode
	{
	public:
		SequenceCode(){};
		SequenceCode(const std::vector<Code*>& codes) : CompoundCode(codes){};

		Executable* compile() override;
	};

	class SequenceExecutable : public Executable
	{
	public:
		SequenceExecutable(const std::vector<Code*>& codes);
		virtual ~SequenceExecutable();

		RetCode run() override;

	private:
		std::vector<Code*> m_codes;
		Executable* m_currentExe{ nullptr };
		int m_nCurCodeIndex{ -1 };
	};
}