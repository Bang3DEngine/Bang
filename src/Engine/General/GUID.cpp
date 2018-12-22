#include "Bang/GUID.h"

#include <stdio.h>
#include <cctype>

#include "Bang/Math.h"
#include "Bang/Random.h"
#include "Bang/Time.h"

using namespace Bang;

GUID::GUIDType GUID::EmptyGUID()
{
    return 0;
}

const GUID &GUID::Empty()
{
    static GUID emptyGUID;
    return emptyGUID;
}

bool GUID::IsEmpty() const
{
    return *this == Empty();
}

void GUID::SetEmbeddedAssetGUID(const GUIDType &guid)
{
    m_embeddedAssetGUID = guid;
}

GUID GUID::GetRandomGUID()
{
    GUID guid;
    guid.m_timeGUID = Time::GetNow().GetNanos();
    guid.m_randGUID = Random::GetRange<GUIDType>(1, Math::Max<GUIDType>());
    guid.m_embeddedAssetGUID = GUID::EmptyGUID();
    return guid;
}

const GUID::GUIDType &GUID::GetTimeGUID() const
{
    return m_timeGUID;
}
const GUID::GUIDType &GUID::GetRandGUID() const
{
    return m_randGUID;
}
const GUID::GUIDType &GUID::GetEmbeddedAssetGUID() const
{
    return m_embeddedAssetGUID;
}

GUID GUID::WithEmbeddedAssetGUID(GUID::GUIDType embeddedFileGUID) const
{
    GUID guid = *this;
    guid.SetEmbeddedAssetGUID(embeddedFileGUID);
    return guid;
}

GUID GUID::WithoutEmbeddedAssetGUID() const
{
    GUID guid = *this;
    guid.SetEmbeddedAssetGUID(GUID::EmptyGUID());
    return guid;
}

std::istream &GUID::operator>>(std::istream &is)
{
    if (is.peek() != EOF && std::isdigit(is.peek()))
    {
        is >> m_timeGUID;
    }
    is >> std::ws;
    if (is.peek() != EOF && std::isdigit(is.peek()))
    {
        is >> m_randGUID;
    }
    is >> std::ws;
    if (is.peek() != EOF && std::isdigit(is.peek()))
    {
        is >> m_embeddedAssetGUID;
    }
    return is;
}

bool GUID::operator==(const GUID &rhs) const
{
    return GetTimeGUID() == rhs.GetTimeGUID() &&
           GetRandGUID() == rhs.GetRandGUID() &&
           GetEmbeddedAssetGUID() == rhs.GetEmbeddedAssetGUID();
}

bool GUID::operator!=(const GUID &rhs) const
{
    return !(*this == rhs);
}

bool GUID::operator<(const GUID &rhs) const
{
    if (GetTimeGUID() < rhs.GetTimeGUID())
    {
        return true;
    }
    else if (GetTimeGUID() > rhs.GetTimeGUID())
    {
        return false;
    }
    else
    {
        if (GetRandGUID() < rhs.GetRandGUID())
        {
            return true;
        }
        else if (GetRandGUID() > rhs.GetRandGUID())
        {
            return false;
        }
        else
        {
            return (GetEmbeddedAssetGUID() < rhs.GetEmbeddedAssetGUID());
        }
    }
}
