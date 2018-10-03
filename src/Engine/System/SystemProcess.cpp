#include "Bang/SystemProcess.h"

#include <chrono>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "Bang/Array.h"
#include "Bang/Debug.h"
#include "Bang/Mutex.h"
#include "Bang/Paths.h"
#include "Bang/Thread.h"
#include "Bang/SystemUtils.h"

using namespace TinyProcessLib;

constexpr int SystemProcess::MaxBuffSize;

USING_NAMESPACE_BANG

SystemProcess::SystemProcess()
{
}

SystemProcess::~SystemProcess()
{
    Close();
}

void SystemProcess::ReadOutErr(String *buffer, const char *str, int size)
{
    constexpr int MaxBuffSize = 4096;
    char buff[MaxBuffSize+1];
    int sizeToRead = Math::Min(MaxBuffSize, size);
    memcpy(buff, str, static_cast<std::size_t>(sizeToRead));
    buff[sizeToRead] = 0;
    *buffer += String(buff);
}

void SystemProcess::ReadOut(const char *str, int size)
{
    ReadOutErr(&m_out, str, size);
}

void SystemProcess::ReadErr(const char *str, int size)
{
    ReadOutErr(&m_err, str, size);
}

bool SystemProcess::Start(const String &command, const List<String> &extraArgs)
{
    String fullCommand =
          "LD_PRELOAD=\"\" ; " + command + " " + String::Join(extraArgs, " ");

    m_process =
       new Process(fullCommand,
                   Paths::GetExecutableDir().GetAbsolute(),
                   [this](const char *str, int size) { ReadOut(str, size); },
                   [this](const char *str, int size) { ReadErr(str, size); },
                   true,
                   SystemProcess::MaxBuffSize);

    return true;
}

void SystemProcess::WaitUntilFinished(float seconds,
                                      bool *finishedOut,
                                      int *statusOut)
{
    if (m_process)
    {
        int status = 0;
        bool finished = false;
        Time beginning = Time::GetNow();
        while ( !finished &&
                (Time::GetPassedTimeSince(beginning).GetSeconds() < seconds) )
        {
            Thread::SleepCurrentThread( Math::Max(seconds / 10.0f, 0.1f) );
            finished = m_process->try_get_exit_status(status);
        }

        if (finishedOut)
        {
            *finishedOut = finished;
        }

        if (statusOut)
        {
            *statusOut = status;
        }
    }
}

void SystemProcess::Close()
{
    if (m_process)
    {
        Kill(true);
        m_process = nullptr;
        delete m_process;
    }
}

void SystemProcess::Write(const String &str)
{
    if (m_process)
    {
        m_process->write(str);
    }
}

void SystemProcess::CloseWriteChannel()
{
    if (m_process)
    {
        m_process->close_stdin();
    }
}

String SystemProcess::ReadStandardOutput()
{
    WaitUntilFinished();
    return m_out;
}
String SystemProcess::ReadStandardError()
{
    WaitUntilFinished();
    return m_err;
}

void SystemProcess::Kill(bool force)
{
    if (m_process)
    {
        m_process->kill(force);
    }
}

int SystemProcess::GetExitCode() const
{
    return m_process ? m_process->get_exit_status() : 0;
}
