#ifndef ITOSTRING_H
#define ITOSTRING_H

#include "Bang/Bang.h"

namespace Bang
{
class IToString
{
protected:
    IToString() = default;
    virtual ~IToString() = default;

public:
    virtual String ToString() const = 0;
};
}

#endif  // ITOSTRING_H
