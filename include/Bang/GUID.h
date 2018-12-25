#ifndef GUID_H
#define GUID_H

#include <stdint.h>
#include <istream>
#include <system_error>
#include <unordered_map>

#include "Bang/BangDefines.h"

namespace Bang
{
class GUID
{
public:
    using GUIDType = uint64_t;
    static GUIDType EmptyGUID();
    static const GUID &Empty();

    GUID() = default;

    bool IsEmpty() const;

    static GUID GetRandomGUID();

    const GUIDType &GetTimeGUID() const;
    const GUIDType &GetRandGUID() const;
    const GUIDType &GetEmbeddedAssetGUID() const;

    GUID WithEmbeddedAssetGUID(GUID::GUIDType embeddedFileGUID) const;
    GUID WithoutEmbeddedAssetGUID() const;

    std::istream &operator>>(std::istream &is);
    bool operator==(const GUID &rhs) const;
    bool operator!=(const GUID &rhs) const;
    bool operator<(const GUID &rhs) const;

private:
    GUIDType m_timeGUID = GUID::EmptyGUID();
    GUIDType m_randGUID = GUID::EmptyGUID();
    GUIDType m_embeddedAssetGUID = GUID::EmptyGUID();

    void SetEmbeddedAssetGUID(const GUIDType &guid);

    friend class GUIDManager;
};
}

// Hash for GUID
namespace std
{
template <>
struct hash<Bang::GUID>
{
    std::size_t operator()(const Bang::GUID &guid) const
    {
        return std::hash<Bang::GUID::GUIDType>()(guid.GetTimeGUID()) ^
               std::hash<Bang::GUID::GUIDType>()(guid.GetRandGUID()) ^
               std::hash<Bang::GUID::GUIDType>()(guid.GetEmbeddedAssetGUID());
    }
};
}

#endif  // GUID_H
