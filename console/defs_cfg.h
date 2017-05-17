#ifndef DEFS_CFG_H_
#define DEFS_CFG_H_

#include <cstdint>
#include <string>

typedef struct
{
    int             zoom_steps;     ///< number of steps to use for digital zoom
    int             zoom_max;       ///< maximum digital zoom
    std::string     com_port;       ///< name of COM port
    std::string     rec_path;       ///< video frame recording path
    std::string     cascade_path;   ///< feature recognition cascade file path
} tAppCfg;

typedef struct
{
    int         ext_on_ct;  ///< number of 10hz ticks that output will be on
    int         min_level;  ///< rail for maximum output level (zero-based)
    int         max_level;  ///< rail for maximum output level (zero-based)
    int         inh_time;   ///< (sec) inhibit before starting
    int         norm_time;  ///< (sec) no face/eyes in this time, goes to WARN
    int         warn_time;  ///< (sec) no face/eyes in this time, goes to ACT
    int         act_time;   ///< (sec) duration of ACT
    int         smile_thr;  ///< default smile threshold
} tLoopCfg;

typedef struct
{
    int         wait_time;  ///< (sec) interval between end of rec and start of spk
    int         rec_time;   ///< (sec) at least 10 sec longer than rec timeout
    int         spk_time;   ///< (sec) make it longer than time to say longest phrase
    std::string file_name;  ///< name of file with phrases
} tPhraseCfg;

#endif // DEFS_CFG_H_
