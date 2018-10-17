#ifndef ICLONEABLE_H
#define ICLONEABLE_H

#include "Bang/Assert.h"
#include "Bang/BangDefines.h"

namespace Bang
{
#define ICLONEABLE(CLASS)                 \
public:                                   \
    virtual CLASS *Clone() const override \
    {                                     \
        CLASS *c = new CLASS();           \
        CloneInto(c);                     \
        return c;                         \
    }

class ICloneable
{
public:
    virtual ICloneable *Clone() const
    {
        ASSERT_MSG(false, "Method not implemented");
        return nullptr;
    }

protected:
    ICloneable()
    {
    }
    virtual ~ICloneable()
    {
    }
    virtual void CloneInto(ICloneable *clone) const = 0;
};
}

#endif  // ICLONEABLE_H
