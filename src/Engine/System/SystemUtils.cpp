#include "Bang/SystemUtils.h"

#include "Bang/Application.h"
#include "Bang/SystemProcess.h"

using namespace Bang;

SystemUtils::SystemUtils()
{
}

void SystemUtils::System(const String &command,
                         const Array<String> &argsArray,
                         String *output,
                         bool *success)
{
    SystemProcess process;
    process.Start(command, argsArray);
    process.WaitUntilFinished();

    String out = process.ReadStandardOutput() + process.ReadStandardError();
    if (output)
    {
        *output = out;
    }

    if (success)
    {
        *success = (process.GetExitCode() == 0);
    }

    process.Close();
}

Mutex *SystemUtils::GetMutex()
{
    return &(SystemUtils::GetInstance()->m_mutex);
}

SystemUtils *SystemUtils::GetInstance()
{
    return Application::GetInstance()->GetSystemUtils();
}
