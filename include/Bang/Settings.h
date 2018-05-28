#ifndef SETTINGS_H
#define SETTINGS_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

class Settings
{
public:

    static Settings *GetInstance();

private:
	Settings();
	virtual ~Settings();

    friend class Application;
};

NAMESPACE_BANG_END

#endif // SETTINGS_H

