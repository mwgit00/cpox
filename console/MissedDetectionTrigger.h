#ifndef MISSED_DETECTION_TRIGGER_H_
#define MISSED_DETECTION_TRIGGER_H_

#include <chrono>

class MissedDetectionTrigger
{
public:

    enum
    {
        STATE_IDLE,
        STATE_INHIBIT,
        STATE_DETECT,
    };
    
    MissedDetectionTrigger();
    virtual ~MissedDetectionTrigger();

    bool update(bool is_ok);
    
    bool isTriggered(void) const;
    bool isDetecting(void) const;
    double getDetectMsecRemaining(void) const;
    int getInhibitSecondsRemaining(void) const;
    
    void reset();
    void start();
    void set(const double t_miss_ms, const double t_inh_ms);

public:
    
    int state;
    double detect_timeout_ms;
    double inhibit_timeout_ms;
    bool is_triggered;
    bool is_detecting;

    std::chrono::time_point<std::chrono::steady_clock> tp_update;
    std::chrono::time_point<std::chrono::steady_clock> tp_detect_start;
    std::chrono::time_point<std::chrono::steady_clock> tp_inhibit_start;
};

#endif // MISSED_DETECTION_TRIGGER_H_
