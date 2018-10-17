#ifndef SETTINGS_H
#define SETTINGS_H

#include "Bang/BangDefines.h"

namespace Bang
{
class Settings
{
public:
    static Settings *GetInstance();

private:
    Settings();
    virtual ~Settings();

    friend class Application;
};
}

#endif  // SETTINGS_H
