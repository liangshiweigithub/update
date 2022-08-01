#pragma once

#include "CommonExports.h"

class COMMON_API Timing
{
public:
	Timing();

	double GetCycleTime() const;

	void TriggerCycle();

private:
	double _cycleTime;
	double _cycleTriggerTime;
};