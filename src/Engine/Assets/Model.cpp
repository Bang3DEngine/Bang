#include "Bang/Model.h"

#include <sys/types.h>
#include <functional>
#include <memory>
#include <ostream>

#include "Bang/Animator.h"
#include "Bang/AnimatorStateMachine.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/AssetHandle.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Debug.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/Map.tcc"
#include "Bang/Material.h"
#include "Bang/Mesh.h"
#include "Bang/MeshRenderer.h"
#include "Bang/ModelIO.h"
#include "Bang/SkinnedMeshRenderer.h"
#include "Bang/StreamOperators.h"
#include "Bang/Transform.h"
#include "Bang/Tree.h"
#include "Bang/Tree.tcc"

namespace Bang
{
class Animation;
class Material;
class MetaNode;
class Path;
}

using namespace Bang;

Model::Model()
{
}

Model::~Model()
{
}

GameObject *CreateGameObjectFromModelNodeTree(
    const ModelIOScene &modelScene,
    const Tree<ModelIONode> *modelTree)
{
    GameObject *gameObject = GameObjectFactory::CreateGameObject();
    if (!modelTree)
    {
        return gameObject;
    }

    const ModelIONode &modelNode = modelTree->GetData();

    // Set name
    gameObject->SetName(modelNode.name);

    // Set transform
    gameObject->GetTransform()->FillFromMatrix(modelNode.localToParent);

    // Add mesh renderers
    bool addAnimator = false;
    for (uint i = 0; i < modelNode.meshIndices.Size(); ++i)
    {
        Mesh *mesh = modelScene.meshes[modelNode.meshIndices[i]].Get();
        Material *material =
            modelScene.materials[modelNode.meshMaterialIndices[i]].Get();

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
        AH<AnimatorStateMachine> animatorSM =
            Assets::Create<AnimatorStateMachine>();

        Animator *animator = gameObject->AddComponent<Animator>(1);
        animator->SetStateMachine(animatorSM.Get());
    }

    // Add children
    for (const Tree<ModelIONode> *childTree : modelTree->GetChildren())
    {
        GameObject *childGo =
            CreateGameObjectFromModelNodeTree(modelScene, childTree);
        childGo->SetParent(gameObject);
    }

    return gameObject;
}

GameObject *Model::CreateGameObjectFromModel() const
{
    GameObject *modelGo =
        CreateGameObjectFromModelNodeTree(m_modelScene, m_modelScene.modelTree);

    // If skinned mesh renderer, set bone references to gameObjects
    Array<SkinnedMeshRenderer *> smrs =
        modelGo->GetComponentsInDescendantsAndThis<SkinnedMeshRenderer>();
    for (SkinnedMeshRenderer *smr : smrs)
    {
        smr->RetrieveBonesInitialTransformationFromCurrentHierarchy();
    }
    return modelGo;
}

const String &Model::GetRootGameObjectName() const
{
    return m_modelScene.rootGameObjectName;
}

const Array<AH<Mesh>> &Model::GetMeshes() const
{
    return m_modelScene.meshes;
}

const Array<AH<Material>> &Model::GetMaterials() const
{
    return m_modelScene.materials;
}

const Array<AH<Animation>> &Model::GetAnimations() const
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
    ClearEmbeddedAssets();
    if (!ModelIO::ImportModel(modelFilepath, this, &m_modelScene))
    {
        Debug_Error("Can not load model " << modelFilepath << ". "
                                          << "Look for errors above.");
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
