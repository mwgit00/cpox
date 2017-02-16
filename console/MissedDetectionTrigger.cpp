#include "MissedDetectionTrigger.h"

MissedDetectionTrigger::MissedDetectionTrigger() :
	miss_ct(0),
	max_miss_ct(0),
	inhibit_ct(0),
	is_triggered(0)
{
	// will start in a disabled state
	// update will have no effect until reset is called
}

MissedDetectionTrigger::~MissedDetectionTrigger()
{
}

bool MissedDetectionTrigger::isTriggered(void) const
{
	return is_triggered;
}

int MissedDetectionTrigger::getInhibitCt(void) const
{
	return inhibit_ct;
}

int MissedDetectionTrigger::getMissCount(void) const
{
	return miss_ct;
}

void MissedDetectionTrigger::reset(const int n, const int inh)
{
	// start in non-triggered state with inhibit counter set
	miss_ct = 0;
	max_miss_ct = n;
	inhibit_ct = inh;
	is_triggered = false;
}

bool MissedDetectionTrigger::update(bool is_ok)
{
	if (inhibit_ct > 0)
	{
		// decrement inhibit counter
		// do nothing until inhibit period is over
		inhibit_ct--;
	}
	else
	{
		// otherwise check input state
		if (is_ok)
		{
			// OK so reset miss counter and remain untriggered
			miss_ct = 0;
			is_triggered = false;
		}
		else
		{
			// not OK so increment miss counter
			if (miss_ct < max_miss_ct)
			{
				miss_ct++;
				if (miss_ct == max_miss_ct)
				{
					// too many misses so enter triggered state
					is_triggered = true;
				}
			}
		}
	}

	// return latest state
	return is_triggered;
}

