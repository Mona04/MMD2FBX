#pragma once
#include <cstdio>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ILogger.h"


#define LOG_INFO(...)	 { { Framework::Log::_callerName = __FUNCTION__; Framework::Log::_line = __LINE__; Framework::Log::Info(__VA_ARGS__);    } }
#define LOG_WARNING(...) { { Framework::Log::_callerName = __FUNCTION__; Framework::Log::_line = __LINE__; Framework::Log::Warning(__VA_ARGS__); } }
#define LOG_ERROR(...)	 { { Framework::Log::_callerName = __FUNCTION__; Framework::Log::_line = __LINE__; Framework::Log::Error(__VA_ARGS__);   } }


namespace Framework
{
	class Log final
	{
	public:
		template<typename... Args> static void Info(const Args&... args);
		template<typename... Args> static void Warning(const Args&... args);
		template<typename... Args> static void Error(const Args&... args);

		static void SetLogger(ILogger* logger) { _logger = logger; }

	protected:
		static void LogToLogger(const char* text, Log_Type type);
		static void Write(const char* text);

	public:
		static ILogger* _logger;
		static std::string _callerName;
		static int _line;
	};

	template<typename... Args>
	void Log::Info(const Args&... args)
	{
		std::stringstream stream;
		((stream << "[Info] [Fun : " << _callerName << "] [Line : " << _line << "] " << args), ...);  // c++ 단항 우측 폴드 적용(22장 참고)

		LogToLogger(stream.str().c_str(), Log_Type::Info);
	}

	template<typename... Args>
	void Log::Warning(const Args&... args)
	{
		std::stringstream stream;
		((stream << "[Warning] [Fun : " << _callerName << "] [Line : " << _line << "] " << args), ...);  // c++ 단항 우측 폴드 적용(22장 참고)

		LogToLogger(stream.str().c_str(), Log_Type::Info);
	}

	template<typename... Args>
	void Log::Error(const Args&... args)
	{
		std::stringstream stream;
		((stream << "[Error] [Fun : " << _callerName << "] [Line : " << _line << "] " << args), ...);  // c++ 단항 우측 폴드 적용(22장 참고)

		LogToLogger(stream.str().c_str(), Log_Type::Info);
	}
}