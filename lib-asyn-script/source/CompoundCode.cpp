///*!
// * \file CompoundCode.cpp
// * \date 02-10-2016 13:00:03
// * 
// * 
// * \author cs 04nycs@gmail.com
// */
//
//#include "CompoundCode.h"
//
//asys::CompoundCode::CompoundCode(const std::vector<Code*>& codes)
//{
//	m_codes = codes;
//}
//
//asys::CompoundCode::~CompoundCode()
//{
//	m_codes.clear();
//}
//
//void asys::CompoundCode::pushCode(Code* code)
//{
//	m_codes.push_back(code);
//}
//
//void asys::CompoundCode::popCode()
//{
//	if (m_codes.size() > 0)
//	{
//		auto code = m_codes.back();
//		m_codes.pop_back();
//	}
//}
//
//void asys::CompoundCode::pushCodes(const std::vector<Code*>& codes)
//{
//	for (auto code : codes)
//	{
//		pushCode(code);
//	}
//}
//
//void asys::CompoundCode::clearCodes()
//{
//	m_codes.clear();
//}
