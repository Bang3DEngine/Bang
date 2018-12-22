#ifndef IGUIDABLE_H
#define IGUIDABLE_H

#include "Bang/BangDefines.h"
#include "Bang/GUID.h"

namespace Bang
{
class IGUIDable
{
public:
    IGUIDable();
    virtual ~IGUIDable();

    const GUID &GetGUID() const;
    GUID &GetGUID();

protected:
    void SetGUID(const GUID &guid);

private:
    GUID m_GUID;

    friend class Assets;
};
}

#endif  // IGUIDABLE_H
