#include "Bang/Prefab.h"

#include "Bang/Scene.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/SceneManager.h"
#include "Bang/XMLMetaReader.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

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

    if (!GetInfoContent().IsEmpty())
    {
        MetaNode metaNode = XMLMetaReader::FromString(GetInfoContent());
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

const String &Prefab::GetInfoContent() const
{
    return m_gameObjectMetaInfoContent;
}

void Prefab::Import(const Path &prefabFilepath)
{
    ImportMetaFromFile( ImportFilesManager::GetImportFilepath(prefabFilepath) );
}

void Prefab::ImportMeta(const MetaNode &metaNode)
{
    Asset::ImportMeta(metaNode);

    String newMetaInfo = metaNode.ToString();
    if (newMetaInfo != GetInfoContent())
    {
        m_gameObjectMetaInfoContent = newMetaInfo;
    }
}

void Prefab::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);

    *metaNode = XMLMetaReader::FromString(GetInfoContent());
}
