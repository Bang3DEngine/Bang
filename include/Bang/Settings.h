#ifndef SETTINGS_H
#define SETTINGS_H

#include "Bang/BangDefines.h"
#include "Bang/IReflectable.h"
#include "Bang/Path.h"

namespace Bang
{
class Settings
{
protected:
    Settings();
    virtual ~Settings();

    virtual void Init();

    static Settings *GetInstance();

protected:
    friend class Application;
};
}  // namespace Bang

#endif  // SETTINGS_H
