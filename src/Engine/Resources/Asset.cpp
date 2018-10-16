#include "Bang/Asset.h"

FORWARD NAMESPACE_BANG_BEGIN
FORWARD class MetaNode;
FORWARD NAMESPACE_BANG_END

USING_NAMESPACE_BANG

String Asset::ToString() const
{
    String res = GetClassName();
    res +=  "Asset(" + String::ToString((void*)this) + ")";
    return res;
}

void Asset::ImportMeta(const MetaNode &metaNode)
{
    Resource::ImportMeta(metaNode);
}

void Asset::ExportMeta(MetaNode *metaNode) const
{
    Resource::ExportMeta(metaNode);
}
