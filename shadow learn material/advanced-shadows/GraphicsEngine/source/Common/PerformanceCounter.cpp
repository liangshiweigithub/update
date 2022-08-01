#include "stdafx.h"
#include "PerformanceCounter.h"

double PerformanceCounter::_frequency = .0;

bool PerformanceCounter::IsSupported()
{
	LARGE_INTEGER frequency;

	bool supported = (0 != ::QueryPerformanceFrequency(&frequency));
	
	_frequency = static_cast<double>(frequency.QuadPart);
	
	return supported;
}

double PerformanceCounter::Frequency()
{
	if (_frequency == .0)
		IsSupported();

	return _frequency;
}

double PerformanceCounter::Time()
{
	LARGE_INTEGER time;

	if ((Frequency() == .0) || (0 == ::QueryPerformanceCounter(&time)))
		return .0;

	return (time.QuadPart / _frequency);
}