#ifndef SYSTEMPROCESS_H
#define SYSTEMPROCESS_H

#include <unistd.h>

#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/List.tcc"
#include "Bang/Math.h"
#include "Bang/String.h"
#include "process.hpp"

namespace TinyProcessLib {
class Process;
}  // namespace TinyProcessLib

NAMESPACE_BANG_BEGIN

class SystemProcess
{
public:
    SystemProcess();
    ~SystemProcess();

    bool Start(const String &command,
               const List<String> &extraArgs = {});
    bool StartDettached(const String &command,
                        const List<String> &extraArgs = {});
    void WaitUntilFinished(float seconds = Math::Infinity<float>(),
                           bool *finished = nullptr,
                           int *status = nullptr);
    void Close();

    void Write(const String &str);
    void CloseWriteChannel();

    String ReadStandardOutput();
    String ReadStandardError();

    void Kill(bool force = false);
    int GetExitCode() const;

private:
    static constexpr int MaxBuffSize = 4096;

    String m_out = "";
    String m_err = "";
    TinyProcessLib::Process *m_process = nullptr;

    void ReadOutErr(String *buffer, const char *str, int size);
    void ReadOut(const char *str, int size);
    void ReadErr(const char *str, int size);
};

NAMESPACE_BANG_END

#endif // SYSTEMPROCESS_H
