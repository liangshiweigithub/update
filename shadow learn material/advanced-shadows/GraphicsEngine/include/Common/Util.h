#pragma once

#include "CommonExports.h"

#include <string>

class COMMON_API Util
{
public:
	static const std::wstring StrToWStr(const std::string& str);
	static const std::string WStrToStr(const std::wstring& wstr);
};