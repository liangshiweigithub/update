#include "stdafx.h"
#include "Logger.h"

/* Logger::Stream ************************************************************/
Logger::Stream::Stream(Logger &logger, Logger::Level level) :
	_logger(logger),
	_level(level)
{
}

Logger::Stream::~Stream()
{
	_logger.Log(_level, _stream.str());
}

std::stringstream& Logger::Stream::GetStream()
{
	return _stream;
}


/* Logger ********************************************************************/
const char* Logger::LevelNames[] = { "Debug", "Info", "Warn", "Error", "Fatal" };

Logger::Logger(const std::string &name) :
	_name(name),
	_level(Debug)
{
}

Logger::Logger(const std::string &name, Level level) :
	_name(name),
	_level(level)
{
}


Logger::~Logger()
{
}

Logger::Level Logger::GetLevel() const
{
	return _level;
}

void Logger::SetLevel(Level level)
{
	_level = level;
}

bool Logger::IsEnabled(Level level) const
{
	return GetLevel() <= level;
}

void Logger::Log(Level level, const std::string& msg)
{
	if (level < GetLevel())
		return;

	std::cout << "[" << LevelNames[level] << "] " << _name << " - " << msg << std::endl;
}