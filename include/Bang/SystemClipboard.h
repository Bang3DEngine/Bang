#ifndef SYSTEMCLIPBOARD_H
#define SYSTEMCLIPBOARD_H

#include "Bang/BangDefines.h"
#include "Bang/String.h"

namespace Bang
{
class SystemClipboard
{
public:
    static void Set(const String &str);
    static String Get();

    SystemClipboard() = delete;
};
}

#endif  // SYSTEMCLIPBOARD_H
