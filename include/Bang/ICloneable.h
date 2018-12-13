#ifndef ICLONEABLE_H
#define ICLONEABLE_H

#include "Bang/Assert.h"
#include "Bang/BangDefines.h"

namespace Bang
{
#define ICLONEABLE(CLASS)                               \
public:                                                 \
    virtual CLASS *Clone(bool cloneGUID) const override \
    {                                                   \
        CLASS *c = new CLASS();                         \
        CloneInto(c, cloneGUID);                        \
        return c;                                       \
    }

class ICloneable
{
public:
    virtual ICloneable *Clone(bool cloneGUID) const
    {
        BANG_UNUSED(cloneGUID);
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
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const = 0;
};
}

#endif  // ICLONEABLE_H
