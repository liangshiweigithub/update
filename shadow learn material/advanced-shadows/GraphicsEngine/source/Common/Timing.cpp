#include "stdafx.h"

#include "Timing.h"
#include "PerformanceCounter.h"

Timing::Timing() :
	_cycleTime(.0),
	_cycleTriggerTime(.0)
{
}

double Timing::GetCycleTime() const
{
	return _cycleTime;
}

void Timing::TriggerCycle()
{
	double prevTriggerTime = _cycleTriggerTime;

	_cycleTriggerTime = PerformanceCounter::Time();

	_cycleTime = _cycleTriggerTime - prevTriggerTime;
}

