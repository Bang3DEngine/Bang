#include "Bang/Serializable.h"

#include "Bang/File.h"
#include "Bang/GUID.h"
#include "Bang/GUIDManager.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Path.h"
#include "Bang/String.h"

using namespace Bang;

Serializable::Serializable()
{
    SetGUID(GUIDManager::GetNewGUID());
}

Serializable::~Serializable()
{
}

Serializable::Serializable(const Serializable &)
{
    // Don't copy GUID, intentionally left in blank
}

MetaNode Serializable::GetMeta() const
{
    MetaNode metaNode;
    ExportMeta(&metaNode);
    return metaNode;
}

String Serializable::GetSerializedString() const
{
    MetaNode metaNode;
    ExportMeta(&metaNode);
    return metaNode.ToString();
}

void Serializable::ImportMeta(const String &metaNodeString)
{
    MetaNode metaNode;
    metaNode.Import(metaNodeString);
    ImportMeta(metaNode);
}

void Serializable::ImportMeta(const MetaNode &metaNode)
{
    if (metaNode.Contains("GUID"))
    {
        SetGUID(metaNode.Get<GUID>("GUID"));
    }
}

void Serializable::ExportMeta(MetaNode *metaNode) const
{
    metaNode->SetName(GetClassName());
    metaNode->Set<GUID>("GUID", GetGUID());
}

bool Serializable::ImportMetaFromFile(const Path &path)
{
    if (path.Exists())
    {
        String fileContents = File::GetContents(path);
        ImportMeta(fileContents);
        return true;
    }
    return false;
}

bool Serializable::ExportMetaToFile(const Path &path) const
{
    File::Write(path, GetSerializedString());
    return true;
}

HideFlags &Serializable::GetHideFlags()
{
    return m_hideFlags;
}

const HideFlags &Serializable::GetHideFlags() const
{
    return m_hideFlags;
}
