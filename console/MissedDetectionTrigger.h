#ifndef MISSED_DETECTION_TRIGGER_H_
#define MISSED_DETECTION_TRIGGER_H_

class MissedDetectionTrigger
{
public:
	
	MissedDetectionTrigger();
	virtual ~MissedDetectionTrigger();

	bool update(bool is_ok);
	
	bool isTriggered(void) const;
	int getMissCount(void) const;
	int getInhibitCt(void) const;
	
	void reset(const int n,	const int inh);

private:
	
	int miss_ct;
	int max_miss_ct;
	int inhibit_ct;
	bool is_triggered;
};

#endif // MISSED_DETECTION_TRIGGER_H_
