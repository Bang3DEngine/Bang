#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/List.tcc"
#include "Bang/Mutex.h"
#include "Bang/String.h"

namespace Bang
{
class SystemUtils
{
public:
    static void System(const String &command,
                       const Array<String> &argsArray = {},
                       String *output = nullptr,
                       bool *success = nullptr);

    static Mutex *GetMutex();

private:
    Mutex m_mutex;

    SystemUtils();

    static SystemUtils *GetInstance();

    friend class Application;
};
}  // namespace Bang

#endif  // SYSTEMUTILS_H
