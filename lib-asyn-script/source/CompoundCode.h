/*!
 * \file CompoundCode.h
 * \date 02-10-2016 12:59:57
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include "Core.h"
#include <vector>

namespace asys
{
	class CompoundCode : public Code
	{
	public:
		CompoundCode(){};
		CompoundCode(const std::vector<Code*>& codes);
		virtual ~CompoundCode();

		void pushCode(Code* code);
		void popCode();

		void pushCodes(const std::vector<Code*>& codes);

		void clearCodes();

		virtual Executable* compile() override = 0;

	protected:
		std::vector<Code*> m_codes;
	};
}