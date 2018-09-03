#include "Bang/Serializable.h"

#include <istream>
#include <ostream>
#include <sstream>
#include <fstream>

#include "Bang/Path.h"
#include "Bang/File.h"
#include "Bang/String.h"
#include "Bang/MetaNode.h"
#include "Bang/GUIDManager.h"

USING_NAMESPACE_BANG

Serializable::Serializable()
{
    SetGUID( GUIDManager::GetNewGUID() );
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
    MetaNode metaNode = MetaNode::FromString(metaNodeString);
    ImportMeta(metaNode);
}

void Serializable::ImportMeta(const MetaNode &metaNode)
{
    if (metaNode.Contains("GUID"))
    { SetGUID(metaNode.Get<GUID>("GUID")); }
}

void Serializable::ExportMeta(MetaNode *metaNode) const
{
    metaNode->SetName( GetClassName() );
    metaNode->Set<GUID>( "GUID", GetGUID() );
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

HideFlags &Serializable::GetHideFlags() { return m_hideFlags; }
const HideFlags &Serializable::GetHideFlags() const { return m_hideFlags; }

