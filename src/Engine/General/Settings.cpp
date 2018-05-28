#include "Bang/Settings.h"

#include "Bang/Application.h"

USING_NAMESPACE_BANG

Settings *Settings::GetInstance()
{
    return Application::GetInstance()->GetSettings();
}

Settings::Settings()
{
}

Settings::~Settings()
{
}

