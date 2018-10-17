#ifndef GUID_H
#define GUID_H

#include <stdint.h>
#include <istream>
#include <system_error>
#include <unordered_map>

#include "Bang/BangDefines.h"

NAMESPACE_BANG_BEGIN

class GUID
{
public:
    using GUIDType = uint64_t;
    static const GUIDType EmptyGUID;

    GUID() = default;

    static const GUID& Empty();
    bool IsEmpty() const;

    static GUID GetRandomGUID();

    const GUIDType& GetTimeGUID() const;
    const GUIDType& GetRandGUID() const;
    const GUIDType& GetEmbeddedResourceGUID() const;

    GUID WithEmbeddedResourceGUID(GUID::GUIDType embeddedFileGUID) const;
    GUID WithoutEmbeddedResourceGUID() const;

    std::istream &operator>>(std::istream &is);
    bool operator==(const GUID &rhs) const;
    bool operator!=(const GUID &rhs) const;
    bool operator<(const GUID &rhs) const;

private:
    GUIDType m_timeGUID             = GUID::EmptyGUID;
    GUIDType m_randGUID             = GUID::EmptyGUID;
    GUIDType m_embeddedResourceGUID = GUID::EmptyGUID;

    void SetEmbeddedResourceGUID(const GUIDType &guid);

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
            return std::hash<Bang::GUID::GUIDType>()(guid.GetTimeGUID()) ^
                   std::hash<Bang::GUID::GUIDType>()(guid.GetRandGUID()) ^
                   std::hash<Bang::GUID::GUIDType>()(guid.GetEmbeddedResourceGUID());
        }
    };
}

#endif // GUID_H
