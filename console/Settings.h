#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <string>
#include "defs_cfg.h"

class Settings
{
public:
    
    Settings();
    virtual ~Settings();

    bool Read(const std::string& rs);
    bool Write(const std::string& rs) const;
    void ApplyDefaults(void);

public:
    
    tAppCfg     app;
    tLoopCfg    loop;
    tPhraseCfg  phrase;
};

#endif // SETTINGS_H_
