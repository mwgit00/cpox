#ifndef DEFS_H_
#define DEFS_H_

#include <map>
#include <string>
#include "FSMEvent.h"
#include "MTQueue.hpp"

#define SCA_BLACK       (cv::Scalar(0, 0, 0))
#define SCA_WHITE       (cv::Scalar(255, 255, 255))
#define SCA_GRAY        (cv::Scalar(128, 128, 128))
#define SCA_CYAN        (cv::Scalar(255, 255, 0))
#define SCA_PINK        (cv::Scalar(192, 0, 192))
#define SCA_DK_GRAY     (cv::Scalar(64, 64, 64))
#define SCA_YELLOW      (cv::Scalar(0, 255, 255))
#define SCA_YELLOW_MED  (cv::Scalar(0, 192, 192))
#define SCA_RED         (cv::Scalar(0, 0, 255))
#define SCA_RED_MED     (cv::Scalar(0, 0, 192))
#define SCA_GREEN       (cv::Scalar(0, 255, 0))
#define SCA_GREEN_MED   (cv::Scalar(0, 192, 0))
#define SCA_BLUE        (cv::Scalar(255, 0, 0))

#define KEY_GO          ('g')
#define KEY_HALT        ('h')
#define KEY_LISTEN      ('L')
#define KEY_QUIT        ('Q')
#define KEY_ESC         ((char)27)
#define KEY_EYES        ('1')
#define KEY_GRIN        ('2')
#define KEY_SAY         ('s')
#define KEY_ZOOMGT      ('>')
#define KEY_ZOOMLT      ('<')
#define KEY_PANL        ('[')
#define KEY_PANR        (']')
#define KEY_TILTU       ('{')
#define KEY_TILTD       ('}')
#define KEY_ZOOM0       (')')
#define KEY_PT0         ('0')

#define MAX_LEVEL       (10u)

typedef std::map<std::string, std::string> tMapStrStr;

typedef MTQueue<std::string> tMsgQueue;
typedef MTQueue<FSMEvent> tEventQueue;

#endif // DEFS_H_
