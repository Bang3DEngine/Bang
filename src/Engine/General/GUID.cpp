#include "Bang/GUID.h"

USING_NAMESPACE_BANG

const GUID::GUIDType GUID::EmptyGUID = 0;

const GUID &GUID::Empty() { static GUID emptyGUID; return emptyGUID; }

bool GUID::IsEmpty() const { return *this == Empty(); }

void GUID::SetEmbeddedFileGUID(const GUIDType &guid)
{
    m_embeddedFileGUID = guid;
}

GUID GUID::GetRandomGUID()
{
    GUID guid;
    guid.m_timeGUID = Time::GetNow_Nanos();
    guid.m_randGUID = Random::GetRange<GUIDType>(1, Math::Max<GUIDType>());
    guid.m_embeddedFileGUID = GUID::EmptyGUID;
    return guid;
}

String GUID::ToString() const
{
    return String::ToString( GetTimeGUID() ) + " " +
           String::ToString( GetRandGUID() ) + " " +
           String::ToString( GetEmbeddedFileGUID() );
}

const GUID::GUIDType &GUID::GetTimeGUID() const
{
    return m_timeGUID;
}
const GUID::GUIDType &GUID::GetRandGUID() const
{
    return m_randGUID;
}
const GUID::GUIDType &GUID::GetEmbeddedFileGUID() const
{
    return m_embeddedFileGUID;
}

GUID GUID::WithoutEmbeddedFileGUID() const
{
    GUID guid = *this;
    guid.SetEmbeddedFileGUID( GUID::EmptyGUID );
    return guid;
}

std::istream &GUID::operator>>(std::istream &is)
{
    if (is.peek() != EOF && std::isdigit(is.peek())) { is >> m_timeGUID; }
    is >> std::ws;
    if (is.peek() != EOF && std::isdigit(is.peek())) { is >> m_randGUID; }
    is >> std::ws;
    if (is.peek() != EOF && std::isdigit(is.peek())) { is >> m_embeddedFileGUID; }
    return is;
}

bool GUID::operator==(const GUID &rhs) const
{
    return        GetTimeGUID() ==       rhs.GetTimeGUID() &&
                  GetRandGUID() ==       rhs.GetRandGUID() &&
            GetEmbeddedFileGUID() == rhs.GetEmbeddedFileGUID();
}

bool GUID::operator!=(const GUID &rhs) const
{
    return !(*this == rhs);
}

bool GUID::operator<(const GUID &rhs) const
{
    if (GetTimeGUID() < rhs.GetTimeGUID()) { return true; }
    else if (GetTimeGUID() > rhs.GetTimeGUID()) { return false; }
    else
    {
        if (GetRandGUID() < rhs.GetRandGUID()) { return true; }
        else if (GetRandGUID() > rhs.GetRandGUID()) { return false; }
        else
        {
            return (GetEmbeddedFileGUID() < rhs.GetEmbeddedFileGUID());
        }
    }
}
