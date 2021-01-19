#include "Framework.h"
#include "Log.h"
#include <mutex>

using namespace Framework;

ILogger* Log::_logger = nullptr;
std::string Log::_callerName = "";
int Log::_line = 0;

void Log::LogToLogger(const char* text, Log_Type type)
{
	_logger ? _logger->Log(std::string(text), type) : Write(text);
}

void Log::Write(const char* text)
{
	static std::mutex m_mutex;
	std::unique_lock<std::mutex> lock(m_mutex);
	std::ofstream logfile("log.txt", std::ios_base::app);
	if (logfile.fail())
	{
		std::cerr << "Unable to open debug file" << std::endl;
	}

	logfile << text << std::endl;
}
