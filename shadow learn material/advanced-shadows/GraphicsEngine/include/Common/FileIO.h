#pragma once

#include <string>

#include "CommonExports.h"

class COMMON_API FileIO
{
public:
	static bool LoadFromFile(const std::string& filename, std::string& dest);
	static unsigned int GetNbrOfLines(const std::string& str);
	static const std::string GetLine(const std::string& str, unsigned int nbr);
	static bool Write(const std::string& filename, const std::string& input);
	static bool Load(const std::string& filename, std::string& output);
	static bool LoadValue(const std::string& filename, const std::string& id, int& value);
};