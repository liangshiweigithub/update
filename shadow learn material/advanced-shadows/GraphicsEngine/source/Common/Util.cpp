#include "stdafx.h"

#include "Util.h"

const std::wstring Util::StrToWStr(const std::string& str)
{
	int len;
	int slength = static_cast<int>(str.length() + 1);
	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
	std::wstring temp(buf);
	delete[] buf;

	return temp;
}


const std::string Util::WStrToStr(const std::wstring& wstr)
{
	int slength = static_cast<int>(wstr.length() + 1);
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, 0, 0, 0, 0);
	char* buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, buf, len, 0, 0);
	std::string temp(buf);
	delete[] buf;
	
	return temp;
}
