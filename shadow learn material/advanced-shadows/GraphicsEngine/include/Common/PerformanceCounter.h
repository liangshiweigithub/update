#pragma once

#include "CommonExports.h"

class COMMON_API PerformanceCounter
{
public:
	static bool IsSupported();

	static double Frequency();
	static double Time();

private:
	static double _frequency;
};