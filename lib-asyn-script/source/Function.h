/*!
 * \file Function.h
 * \date 02-10-2016 15:29:48
 * 
 * 
 * \author cs 04nycs@gmail.com
 */

#pragma once

#include "Core.h"
#include <vector>
#include <list>
#include <functional>

// append this macro to the instructor calls to set a break point for ease of debug in C++
#ifdef _DEBUG
#define _ ([](asys::Executable* executable, const asys::BreakPoint& breakPoint) \
		{ \
			executable->setValue("$$__FILE__", breakPoint.fileName()); \
			executable->setValue("$$__FUNCTION__", breakPoint.functionName()); \
			executable->setValue("$$__LINE__", breakPoint.lineNumber());},\
			__FILE__, __FUNCTION__, __LINE__);
#else
#define _ ;
#endif

namespace asys
{
	class FunctionExecutable;
	class FunctionCode;

	enum class InstructorType
	{
		type_null,
		type_express,
		type_call,
		type_if,
		type_else,
		type_endif,
		type_while,
		type_endwhile,
		type_continue,
		type_break,
		type_return,
	};

	const int INVALID_IP = -1;

	class Instructor;

	class BreakPoint
	{
	public:
		BreakPoint(Instructor* instructor) : m_instructor(instructor) {}

		BreakPoint& operator = (const BreakPoint& breakPoint)
		{
			//m_instructor does not change.
			m_fileName = breakPoint.fileName();
			m_functionName = breakPoint.functionName();
			m_lineNumber = breakPoint.lineNumber();
			m_callback = breakPoint.callback();
			return *this;
		}

		Instructor* instructor() { return m_instructor; }
		const std::string& fileName() const { return m_fileName; }
		const std::string& functionName() const { return m_functionName; }
		int lineNumber() const { return m_lineNumber; }

		void operator ()(const std::function<void(Executable*, const BreakPoint& breakpoint)>& callback,
			const char* fileName = nullptr,
			const char* functionName = nullptr,
			int lineNumber = -1);

		const std::function<void(Executable*, const BreakPoint& breakpoint)>& callback() const { return m_callback; }

	private:
		Instructor* m_instructor{ nullptr };
		std::string m_fileName;
		std::string m_functionName;
		int m_lineNumber{ -1 };
		std::function<void(Executable*, const BreakPoint& breakpoint)> m_callback;
	};

	class Instructor
	{
	public:
		Instructor(InstructorType instructorType) : m_instructorType(instructorType) {}
		virtual ~Instructor() {}

		InstructorType instructorType() const { return m_instructorType; }

		virtual Instructor* clone() const { return new Instructor(m_instructorType); }

		const BreakPoint& breakPoint() const { return m_breakPoint; }
		BreakPoint& breakPoint(){ return m_breakPoint; }

		void setBreakPoint(const BreakPoint& breakPoint) { m_breakPoint = breakPoint; }

	private:
		InstructorType m_instructorType;
		BreakPoint m_breakPoint{ this };
	};

	class ExpressInstructor : public Instructor
	{
	public:
		ExpressInstructor(const std::function<CodeFlow(Executable*)>& express) : Instructor(InstructorType::type_express), express(express){}

		Instructor* clone() const override 
		{ 
			auto instructor =  new ExpressInstructor(express);
			instructor->setBreakPoint(breakPoint());
			return instructor;
		}

	public:
		std::function<CodeFlow(Executable*)> express{};
	};

	class CallInstructor : public Instructor
	{
	public:
		CallInstructor(const std::vector<std::pair<std::string, std::string>>& outputParams, const std::vector<std::pair<std::string, std::string>>& inputParams, Code* code, const std::string& codeName)
			: Instructor(InstructorType::type_call)
			, outputParams(outputParams)
			, inputParams(inputParams)
			, code(code)
			, codeName(codeName)
		{}

		virtual ~CallInstructor()
		{
			if (executable)
			{
				executable->release();
				executable = nullptr;
			}
		}

		Instructor* clone() const override 
		{
			auto instructor = new CallInstructor(outputParams, inputParams, code, codeName);
			instructor->setBreakPoint(breakPoint());
			return instructor;
		}

	public:
		std::vector<std::pair<std::string, std::string>> outputParams;
		std::vector<std::pair<std::string, std::string>> inputParams;
		Executable* executable{};
		Code* code{};
		std::string codeName;
	};

	class IfInstructor : public Instructor
	{
	public:
		IfInstructor(const std::function<bool(Executable*)>& express) : Instructor(InstructorType::type_if), express(express) {};

		Instructor* clone() const override 
		{ 
			auto copy = new IfInstructor(express);
			copy->elseIp = elseIp;
			copy->endIfIp = endIfIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		std::function<bool(Executable*)> express;
		int elseIp{ INVALID_IP };
		int endIfIp{ INVALID_IP };
	};

	class ElseInstructor : public Instructor
	{
	public:
		ElseInstructor() : Instructor(InstructorType::type_else) {};

		Instructor* clone() const override 
		{ 
			auto copy = new ElseInstructor();
			copy->ifIp = ifIp;
			copy->endIfIp = endIfIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		int ifIp{ INVALID_IP };
		int endIfIp{ INVALID_IP };
	};

	class EndIfInstructor : public Instructor
	{
	public:
		EndIfInstructor() : Instructor(InstructorType::type_endif) {};

		Instructor* clone() const override
		{
			auto copy = new EndIfInstructor();
			copy->ifIp = ifIp;
			copy->elseIp = elseIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		int ifIp{ INVALID_IP };
		int elseIp{ INVALID_IP };
	};

	class WhileInstructor : public Instructor
	{
	public:
		WhileInstructor(const std::function<bool(Executable*)>& express) : Instructor(InstructorType::type_while), express(express) {};

		Instructor* clone() const override
		{
			auto copy = new WhileInstructor(express);
			copy->endWhileIp = endWhileIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		std::function<bool(Executable*)> express;
		int endWhileIp{ INVALID_IP };
	};

	class EndWhileInstructor : public Instructor
	{
	public:
		EndWhileInstructor() : Instructor(InstructorType::type_endwhile) {};

		Instructor* clone() const override
		{
			auto copy = new EndWhileInstructor();
			copy->whileIp = whileIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		int whileIp{ INVALID_IP };
	};

	class ContinueInstructor : public Instructor
	{
	public:
		ContinueInstructor() : Instructor(InstructorType::type_continue) {};

		Instructor* clone() const override
		{
			auto copy = new ContinueInstructor();
			copy->whileIp = whileIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public: 
		int whileIp{ INVALID_IP };
	};

	class BreakInstructor : public Instructor
	{
	public:
		BreakInstructor() : Instructor(InstructorType::type_break) {};

		Instructor* clone() const override
		{
			auto copy = new BreakInstructor();
			copy->whileIp = whileIp;

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		int whileIp{ INVALID_IP };
	};

	class ReturnInstructor : public Instructor
	{
	public: 
		ReturnInstructor(const std::vector<std::string>& outputParams) : Instructor(InstructorType::type_return), outputParams(outputParams){};

		Instructor* clone() const override
		{
			auto copy = new ReturnInstructor(outputParams);

			copy->setBreakPoint(breakPoint());

			return copy;
		}

	public:
		std::vector<std::string> outputParams;
	};

	class FunctionCode : public Code
	{
	public:
		FunctionCode();
		virtual ~FunctionCode();

		//It's easier to debug the code using EXPRESS and lamda expression than using ASSIGN or OPERATE
		//and the expressions written in C++ in the lamda expression are more expressive than pure asyn-script codes.
		BreakPoint& EXPRESS(const std::function<CodeFlow(Executable*)>& express);

		//CALL returns multiple variable from the called function, which in turn are assigned to the outputParams.
		BreakPoint& CALL(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, Code* code);
		
		//If codeName is const which means it's not a variable name, the real code to be called is determined when FunctionCode is compiled; 
		//In this case, codeName is used to look up the code as key in the dynamic codes table, thus the code should be registered
		//before compile() being called. If codeName is a valid variable name(starts with $), the code invoked would be the one registered with the name 
		//stored in the code Name variable when the call instructor is executed.
		BreakPoint& CALL(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, const std::string& codeName);

		BreakPoint& INPUT(const std::vector<std::string>& inputParams);

		BreakPoint& IF(const std::string& var);
		BreakPoint& IF_EX(const std::function<bool(Executable*)>& express);
		BreakPoint& IF_NOT(const std::string& var);
		BreakPoint& IF_EQUAL(const std::string& var1, const std::string& var2);
		BreakPoint& IF_NOT_EQUAL(const std::string& var1, const std::string& var2);

		BreakPoint& ELSE();
		BreakPoint& END_IF();

		BreakPoint& WHILE(const std::string& var);
		BreakPoint& WHILE_EX(const std::function<bool(Executable*)>& express);
		BreakPoint& WHILE_NOT(const std::string& var);
		BreakPoint& WHILE_EQUAL(const std::string& var1, const std::string& var2);
		BreakPoint& WHILE_NOT_EQUAL(const std::string& var1, const std::string& var2);

		BreakPoint& END_WHILE();
		BreakPoint& CONTINUE();
		BreakPoint& BREAK();

		BreakPoint& RETURN() { return RETURN({}); }
		BreakPoint& RETURN(const std::vector<std::string>& vars);

		//It's easier to debug the code using EXPRESS and lamda expression than using ASSIGN or OPERATE
		//and the expressions written in C++ in the lamda expression are more expressive than pure asyn-script codes.

		//var1 should starts with $, if not it's a const, which can't be assigned.
		//if var2 starts with $, the value in the variables table with key var2 will be 
		//assigned when the generated instructor is invoked, or var2 itself will be assigned instead.
		BreakPoint& ASSIGN(const std::string& var1, const std::string& var2);

		BreakPoint& OPERATE(const std::string& output, const std::string& var1, const std::string& var2, Operator eOperator);
		BreakPoint& OPERATE(const std::string& output, const std::string& var, Operator eOperator);

		Executable* compile() override;

		void registerDynamicCodes(const std::string& name, Code* code) { m_dynamicCodes[name] = code; }
		void unregisterDynamicCode(const std::string& name) { m_dynamicCodes.erase(name); }
		void unregisterAllDynamicCodes() { m_dynamicCodes.clear(); }

		void clear();

	private:
		BreakPoint& CALL_EX(const std::vector<std::string>& outputParams, const std::vector<std::string>& inputParams, Code* code, const std::string& codeName);

		//CALL_EX uses pairs of variable names to pass values when invoking or returning from the sub-function.
		BreakPoint& CALL_EX(const std::vector<std::pair<std::string, std::string>>& outputParams, const std::vector<std::pair<std::string, std::string>>&inputParams, Code* code, const std::string& codeName);

	private:
		std::vector<Instructor*> m_instructors;
		std::list<int> m_unmatchedIfIps;
		std::list<int> m_unmatchedWhileIps;
		std::map<std::string, Code*> m_dynamicCodes;
	};

	class FunctionExecutable : public Executable
	{
	public:
		FunctionExecutable(const std::vector<Instructor*> instructors, const std::map<std::string, Code*> dynamicCodes);
		virtual ~FunctionExecutable();

		CodeFlow run() override;

		void registerDynamicCode(const std::string& name, Code* code);

	private:
		int processNullInstructor(int curIp) {return curIp + 1;}

		int processExpressInstructor(CodeFlow& retCode, int curIp, ExpressInstructor* expressInstructor);
		int processCallInstructor(CodeFlow& retCode, int curIp, CallInstructor* callInstructor);

		int processIfInstructor(int curIp, IfInstructor* ifInstructor)
		{
			bool condition = ifInstructor->express(this);
			if (condition) return curIp + 1;

			if (ifInstructor->elseIp != INVALID_IP) return ifInstructor->elseIp + 1;

			return ifInstructor->endIfIp + 1;
		}

		int processElseInstructor(int curIp, ElseInstructor* elseInstructor) { return elseInstructor->endIfIp + 1; }
		int processEndIfInstructor(int curIp, EndIfInstructor* endIfInstructor) { return curIp + 1; }

		int processWhileInstructor(int curIp, WhileInstructor* whileInstructor)
		{
			bool condition = whileInstructor->express(this);
			if (condition) return curIp + 1;

			return whileInstructor->endWhileIp + 1;
		}

		int processEndWhileInstructor(int curIp, EndWhileInstructor* endWhileInstructor) { return endWhileInstructor->whileIp; }
		int processContinueInstructor(int curIp, ContinueInstructor* continueInstructor) { return continueInstructor->whileIp; }
		int processBreakInstructor(int curIp, BreakInstructor* breakInstructor)
		{
			auto whileIp = breakInstructor->whileIp;
			auto whileInstructor = dynamic_cast<WhileInstructor*>(m_instructors[whileIp]);
			return whileInstructor->endWhileIp + 1;
		}

		int processReturnInstructor(int curIp, ReturnInstructor* retInstructor);

	private:
		std::vector<Instructor*> m_instructors;
		int m_nCurIp{ INVALID_IP  + 1};
		CodeFlow m_codeFlow{CodeFlow::next_};
		std::map<std::string, Code*> m_dynamicCodes;
	};

	class FunctionMap : public std::map <std::string, asys::FunctionCode*>
	{
	public:
		virtual ~FunctionMap()
		{
			for (auto& pair : *this)
			{
				if (pair.second)
				{
					delete pair.second;
				}
			}

			clear();
		}
	};
}