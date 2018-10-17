#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/List.tcc"
#include "Bang/Mutex.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

class SystemUtils
{
public:

    static void System(const String &command,
                       const List<String> &argsList = {},
                       String *output = nullptr,
                       bool *success = nullptr);

    static Mutex* GetMutex();

private:
    Mutex m_mutex;

    SystemUtils();

    static SystemUtils* GetInstance();

    friend class Application;
};

NAMESPACE_BANG_END

#endif // SYSTEMUTILS_H
