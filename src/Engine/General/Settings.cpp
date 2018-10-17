#include "Bang/Settings.h"

#include "Bang/Application.h"

using namespace Bang;

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
