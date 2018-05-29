#ifndef GUID_H
#define GUID_H

#include <istream>

#include "Bang/Time.h"
#include "Bang/String.h"
#include "Bang/Random.h"
#include "Bang/IToString.h"

NAMESPACE_BANG_BEGIN

class GUID : public IToString
{
public:
    using GUIDType = uint64_t;

private: static constexpr GUIDType EmptyGUID = 0;
public:
    GUID() {}

    static const GUID& Empty();
    bool IsEmpty() const;

    static GUID GetRandomGUID();

    // IToString
    String ToString() const override;

    const GUIDType& GetTimeGUID() const;
    const GUIDType& GetRandGUID() const;
    const GUIDType& GetInsideFileGUID() const;

    GUID WithoutInsideFileGUID() const;

    std::istream &operator>>(std::istream &is);
    bool operator==(const GUID &rhs) const;
    bool operator!=(const GUID &rhs) const;
    bool operator<(const GUID &rhs) const;

private:
    GUIDType m_timeGUID       = GUID::EmptyGUID;
    GUIDType m_randGUID       = GUID::EmptyGUID;
    GUIDType m_insideFileGUID = GUID::EmptyGUID;

    void SetInsideFileGUID(const GUIDType &guid);

    friend class GUIDManager;
};

NAMESPACE_BANG_END

// Hash for GUID
namespace std
{
    template <>
    struct hash<Bang::GUID>
    {
        std::size_t operator()(const Bang::GUID& guid) const
        {
            return std::hash<GUID::GUIDType>()(guid.GetTimeGUID()) ^
                   std::hash<GUID::GUIDType>()(guid.GetRandGUID()) ^
                   std::hash<GUID::GUIDType>()(guid.GetInsideFileGUID());
        }
    };
}

#endif // GUID_H
