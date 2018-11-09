#include "Bang/Settings.h"

#include "Bang/Application.h"
#include "Bang/Paths.h"

using namespace Bang;

Settings::Settings()
{
}

Settings::~Settings()
{
}

Settings *Settings::GetInstance()
{
    return Application::GetInstance()->GetSettings();
}

void Settings::Init()
{
}
