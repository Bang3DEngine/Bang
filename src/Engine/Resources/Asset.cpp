#include "Bang/Asset.h"

namespace Bang
{
class MetaNode;
}

using namespace Bang;

String Asset::ToString() const
{
    String res = GetClassName();
    res += "Asset(" + String::ToString((void *)this) + ")";
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
