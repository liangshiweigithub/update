#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "CommonExports.h"

class COMMON_API Logger
{
public:
	enum Level
	{
		Debug = 0,
		Info,
		Warn,
		Error,
		Fatal
	};

	class COMMON_API Stream
	{
	public:
		Stream(Logger& logger, Logger::Level level);
		~Stream();

		std::stringstream& GetStream();

	private:
		Logger& _logger;
		Logger::Level _level;

		std::stringstream _stream;
	};

	Logger(const std::string& name);
	Logger(const std::string& name, Level level);

	~Logger();

	Level GetLevel() const;
	void SetLevel(Level level);

	bool IsEnabled(Level level) const;

	void Log(Level level, const std::string& msg);

private:
	static const char* LevelNames[];

	Level _level;
	std::string _name;
};

template<class T>
const Logger::Stream& operator<<(const Logger::Stream& logStream, const T& msg)
{
	// const_cast because of c4239
	const_cast<Logger::Stream&>(logStream).GetStream() << msg;

	return logStream;
}