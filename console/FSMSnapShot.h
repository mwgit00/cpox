#ifndef FSM_SNAP_SHOT_H_
#define FSM_SNAP_SHOT_H_

#include "opencv2/imgproc/imgproc.hpp"

#include "defs.h"

class FSMSnapShot
{
public:
    FSMSnapShot();
    virtual ~FSMSnapShot();
public:
    cv::Scalar color;
    std::string label;
    int prog;
};

#endif // FSM_SNAP_SHOT_H_
