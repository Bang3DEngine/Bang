#include "Bang/Model.h"

#include "Bang/Mesh.h"
#include "Bang/ModelIO.h"
#include "Bang/Material.h"
#include "Bang/Resources.h"
#include "Bang/Transform.h"
#include "Bang/Extensions.h"
#include "Bang/GameObject.h"
#include "Bang/GUIDManager.h"
#include "Bang/MeshRenderer.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

// Model
Model::Model()
{
}

Model::~Model()
{
}

GameObject *CreateGameObjectFromModelNodeTree(const ModelIOScene &modelScene,
                                              const Tree<ModelIONode>* modelTree)
{
    GameObject *gameObject = GameObjectFactory::CreateGameObject();
    if (!modelTree) { return gameObject; }

    const ModelIONode &modelNode = modelTree->GetData();

    // Set name
    gameObject->SetName( modelNode.name );

    // Set transform
    gameObject->GetTransform()->SetLocalPosition(
                Transform::GetPositionFromMatrix4(modelNode.transformation) );
    gameObject->GetTransform()->SetLocalRotation(
                Transform::GetRotationFromMatrix4(modelNode.transformation) );
    gameObject->GetTransform()->SetLocalScale(
                Transform::GetScaleFromMatrix4(modelNode.transformation) );

    // Add mesh renderers
    for (int i = 0; i < SCAST<int>(modelNode.meshIndices.Size()); ++i)
    {
        MeshRenderer *mr = gameObject->AddComponent<MeshRenderer>();
        mr->SetMesh( modelScene.meshes[ modelNode.meshIndices[i] ].Get() );
        mr->SetMaterial( modelScene.materials[ modelNode.meshIndices[i] ].Get() );
    }

    // Add children
    for (const Tree<ModelIONode> *childTree : modelTree->GetChildren())
    {
        GameObject *childGo = CreateGameObjectFromModelNodeTree(modelScene,
                                                                childTree);
        childGo->SetParent(gameObject);
    }

    return gameObject;
}

GameObject *Model::CreateGameObjectFromModel() const
{
    return CreateGameObjectFromModelNodeTree(m_modelScene,
                                             m_modelScene.modelTree);
}

void Model::AddMesh(Mesh *mesh,
                    Material *material,
                    const String &meshName,
                    const String &materialName)
{
    String newMeshName = Model::GetNewName(meshName, GetMeshesNames());
    m_modelScene.meshesNames.PushBack(
            newMeshName + "." + Extensions::GetMeshExtension());
    m_modelScene.meshes.PushBack( RH<Mesh>(mesh) );
    mesh->SetParentResource(this);

    String newMaterialName = Model::GetNewName(materialName, GetMaterialsNames());
    m_modelScene.materialsNames.PushBack(
            newMaterialName + "." + Extensions::GetMaterialExtension());
    m_modelScene.materials.PushBack( RH<Material>(material) );
    material->SetParentResource(this);
}

RH<Mesh> Model::GetMeshByName(const String &meshName)
{
    RH<Mesh> mesh;
    for (uint i = 0; i < GetMeshesNames().Size(); ++i)
    {
        if (meshName == GetMeshesNames()[i])
        {
            mesh.Set(GetMeshes()[i].Get());
            break;
        }
    }
    return mesh;
}

RH<Material> Model::GetMaterialByName(const String &materialName)
{
    RH<Material> material;
    for (uint i = 0; i < GetMaterialsNames().Size(); ++i)
    {
        if (materialName == GetMaterialsNames()[i])
        {
            material.Set(GetMaterials()[i].Get());
            break;
        }
    }
    return material;
}


const Array<RH<Mesh> > &Model::GetMeshes() const
{
    return m_modelScene.meshes;
}

const Array<RH<Material> > &Model::GetMaterials() const
{
    return m_modelScene.materials;
}

const Array<String> &Model::GetMeshesNames() const
{
    return m_modelScene.meshesNames;
}

const Array<String> &Model::GetMaterialsNames() const
{
    return m_modelScene.materialsNames;
}

GUID::GUIDType Model::GetNextEmbeddedFileGUID() const
{
    return m_modelScene.meshes.Size() + m_modelScene.materials.Size();
}

Resource *Model::GetEmbeddedResource(GUID::GUIDType embeddedFileGUID) const
{
    auto pair = GetEmbeddedFileResourceAndName(embeddedFileGUID);
    return pair.first;
}

String Model::GetEmbeddedFileResourceName(GUID::GUIDType embeddedFileGUID) const
{
    auto pair = GetEmbeddedFileResourceAndName(embeddedFileGUID);
    return pair.second;
}

void Model::Import(const Path &modelFilepath)
{
    // Clear previous
    m_modelScene.Clear();

    // Load new
    ModelIOScene modelScene;

    // Copy
    if (ModelIO::ImportModel(modelFilepath, GetGUID(), &modelScene))
    {
        ASSERT(modelScene.modelTree);
        m_modelScene.modelTree = modelScene.modelTree->GetDeepCopy();
        for (uint i = 0; i < modelScene.meshes.Size(); ++i)
        {
            AddMesh(modelScene.meshes[i].Get(), modelScene.materials[i].Get(),
                    modelScene.meshesNames[i],  modelScene.materialsNames[i]);
        }
    }
    else
    {
        Debug_Error("Can not load model " << modelFilepath << ". " <<
                    "Look for errors above.");
    }
}

void Model::ImportXML(const XMLNode &xmlInfo)
{
    Asset::ImportXML(xmlInfo);
}

void Model::ExportXML(XMLNode *xmlInfo) const
{
    Asset::ExportXML(xmlInfo);
}

std::pair<Resource *, String>
Model::GetEmbeddedFileResourceAndName(GUID::GUIDType embeddedFileGUID) const
{
    std::pair<Resource*, String> pair;
    pair.first = nullptr;
    pair.second = "NameNotFound";

    Array< String > names;
    Array< Resource* > resources;
    for (uint i = 0; i < GetMeshes().Size(); ++i)
    {
        names.PushBack(GetMeshesNames()[i]);
        names.PushBack(GetMaterialsNames()[i]);
        resources.PushBack(GetMeshes()[i].Get());
        resources.PushBack(GetMaterials()[i].Get());
    }

    for (uint i = 0; i < resources.Size(); ++i)
    {
        if (resources[i]->GetGUID().GetEmbeddedFileGUID() == embeddedFileGUID)
        {
            pair.first = resources[i];
            pair.second = names[i];
            break;
        }
    }

    return pair;
}

String Model::GetNewName(const String &originalName,
                         const Array<String> &existingNames)
{
    int auxIndex = 0;
    String newName = originalName;
    while (existingNames.Contains(newName))
    {
        newName = originalName + "_" + String(auxIndex);
        ++auxIndex;
    }
    return newName;
}
