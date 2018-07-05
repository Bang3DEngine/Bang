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
#include "Bang/SkinnedMeshRenderer.h"

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
        Mesh *mesh = modelScene.meshes[ modelNode.meshIndices[i] ].Get();

        MeshRenderer *mr = nullptr;
        if (!mesh->GetBonesPool().IsEmpty())
        {
            mr = gameObject->AddComponent<MeshRenderer>();
        }
        else
        {
            mr = gameObject->AddComponent<SkinnedMeshRenderer>();
        }

        mr->SetMesh(mesh);
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

const Array<RH<Mesh> > &Model::GetMeshes() const
{
    return m_modelScene.meshes;
}

const Array<RH<Material> > &Model::GetMaterials() const
{
    return m_modelScene.materials;
}

const Array<RH<Animation> > &Model::GetAnimations() const
{
    return m_modelScene.animations;
}

const Array<String> &Model::GetMeshesNames() const
{
    return m_modelScene.meshesNames;
}

const Array<String> &Model::GetMaterialsNames() const
{
    return m_modelScene.materialsNames;
}

const Array<String> &Model::GetAnimationsNames() const
{
    return m_modelScene.animationsNames;
}

void Model::Import(const Path &modelFilepath)
{
    m_modelScene.Clear();
    if (!ModelIO::ImportModel(modelFilepath, this, &m_modelScene))
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
