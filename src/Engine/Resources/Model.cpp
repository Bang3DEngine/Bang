#include "Bang/Model.h"

#include "Bang/Mesh.h"
#include "Bang/ModelIO.h"
#include "Bang/Animator.h"
#include "Bang/Material.h"
#include "Bang/Resources.h"
#include "Bang/Transform.h"
#include "Bang/Extensions.h"
#include "Bang/GameObject.h"
#include "Bang/GUIDManager.h"
#include "Bang/MeshRenderer.h"
#include "Bang/ShaderProgram.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/SkinnedMeshRenderer.h"
#include "Bang/ShaderProgramFactory.h"

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
    if (!modelTree)
    {
        return gameObject;
    }

    const ModelIONode &modelNode = modelTree->GetData();

    // Set name
    gameObject->SetName( modelNode.name );

    // Set transform
    gameObject->GetTransform()->FillFromMatrix( modelNode.localToParent );

    // Add mesh renderers
    bool addAnimator = false;
    for (uint i = 0; i < modelNode.meshIndices.Size(); ++i)
    {
        Mesh *mesh = modelScene.meshes[ modelNode.meshIndices[i] ].Get();
        Material *material = modelScene.materials[ modelNode.meshIndices[i] ].Get();

        MeshRenderer *mr = nullptr;
        if (mesh->GetBonesPool().IsEmpty())
        {
            mr = gameObject->AddComponent<MeshRenderer>();
        }
        else
        {
            addAnimator = true;
            mr = gameObject->AddComponent<SkinnedMeshRenderer>();
            // gameObject->GetTransform()->FillFromMatrix( Matrix4::Identity );
        }

        mr->SetMesh(mesh);
        mr->SetMaterial(material);
    }

    if (addAnimator && modelScene.animations.Size() >= 1)
    {
        Animator *animator = gameObject->AddComponent<Animator>(1);
        for (const RH<Animation> &animationRH : modelScene.animations)
        {
            animator->AddAnimation( animationRH.Get() );
        }
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
    GameObject *modelGo = CreateGameObjectFromModelNodeTree(
                                                    m_modelScene,
                                                    m_modelScene.modelTree);

    // If skinned mesh renderer, set bone references to gameObjects
    Array<SkinnedMeshRenderer*> smrs =
            modelGo->GetComponentsInDescendantsAndThis<SkinnedMeshRenderer>();
    for (SkinnedMeshRenderer *smr : smrs)
    {
        smr->RetrieveBonesBindPoseFromCurrentHierarchy();
    }
    return modelGo;
}

const String& Model::GetRootGameObjectName() const
{
    return m_modelScene.rootGameObjectName;
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

const Map<String, Mesh::Bone> &Model::GetAllBones() const
{
    return m_modelScene.allBones;
}

void Model::Import(const Path &modelFilepath)
{
    m_modelScene.Clear();
    ClearEmbeddedResources();
    if (!ModelIO::ImportModel(modelFilepath, this, &m_modelScene))
    {
        Debug_Error("Can not load model " << modelFilepath << ". " <<
                    "Look for errors above.");
    }
}

void Model::ImportMeta(const MetaNode &metaNode)
{
    Asset::ImportMeta(metaNode);
}

void Model::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);
}
