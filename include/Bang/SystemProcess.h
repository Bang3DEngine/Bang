#ifndef SYSTEMPROCESS_H
#define SYSTEMPROCESS_H

#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/List.tcc"
#include "Bang/Math.h"
#include "Bang/String.h"

namespace TinyProcessLib
{
class Process;
}  // namespace TinyProcessLib

namespace Bang
{
class SystemProcess
{
public:
    SystemProcess();
    ~SystemProcess();

    bool Start(const String &command, const Array<String> &extraArgs = {});
    bool StartDettached(const String &command,
                        const Array<String> &extraArgs = {});
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
}  // namespace Bang

#endif  // SYSTEMPROCESS_H
