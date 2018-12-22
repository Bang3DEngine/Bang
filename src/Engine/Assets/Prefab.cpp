#include "Bang/Prefab.h"

#include "Bang/GameObject.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/MetaNode.h"
#include "Bang/Scene.h"
#include "Bang/SceneManager.h"

namespace Bang
{
class Path;
}

using namespace Bang;

Prefab::Prefab()
{
}

Prefab::Prefab(GameObject *go)
{
    SetGameObject(go);
}

Prefab::Prefab(const String &gameObjectMetaInfoContent)
{
    m_gameObjectMetaInfoContent = gameObjectMetaInfoContent;
}

Prefab::~Prefab()
{
}

GameObject *Prefab::Instantiate() const
{
    GameObject *go = InstantiateRaw();
    go->SetParent(SceneManager::GetActiveScene());
    return go;
}

GameObject *Prefab::InstantiateRaw() const
{
    GameObject *go = GameObjectFactory::CreateGameObject(false);

    if (!GetMetaContent().IsEmpty())
    {
        MetaNode metaNode;
        metaNode.Import(GetMetaContent());
        go->ImportMeta(metaNode);
    }
    return go;
}

void Prefab::SetGameObject(GameObject *go)
{
    if (go)
    {
        m_gameObjectMetaInfoContent = go->GetSerializedString();
    }
    else
    {
        m_gameObjectMetaInfoContent = "";
    }
}

const String &Prefab::GetMetaContent() const
{
    return m_gameObjectMetaInfoContent;
}

void Prefab::Import(const Path &prefabFilepath)
{
    ImportMetaFromFile(MetaFilesManager::GetMetaFilepath(prefabFilepath));
}

void Prefab::ImportMeta(const MetaNode &metaNode)
{
    Asset::ImportMeta(metaNode);

    String newMetaInfo = metaNode.ToString();
    if (newMetaInfo != GetMetaContent())
    {
        m_gameObjectMetaInfoContent = newMetaInfo;
    }
}

void Prefab::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);

    MetaNode prefabMetaNode;
    prefabMetaNode.Import(GetMetaContent());

    *metaNode = prefabMetaNode;
}
