#pragma once

namespace Framework
{
	enum class Log_Type : int { Error, Warning, Info };

	class ILogger
	{
	public:
		virtual ~ILogger() = default;
		virtual void Log(const std::string& log, const Log_Type& type) = 0;
	};
}