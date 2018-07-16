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
            // SkinnedMeshRenderer *smr = SCAST<SkinnedMeshRenderer*>(mr);
            // smr->SetBoneGameObject();

            /*
            if (RH<Shader> vAnimShader = Resources::Load<Shader>(
                  ShaderProgramFactory::GetDefaultAnimatedVertexShaderPath()))
            {
                RH<Material> materialAnimRH;
                materialAnimRH.Set( SCAST<Material*>(material->Clone()) );
                materialAnimRH.Get()->GetShaderProgram()->SetVertexShader(
                                                            vAnimShader.Get());
                materialAnimRH.Get()->GetShaderProgram()->Link();
                mr->SetMaterial(materialAnimRH.Get());
            }
            else
            {
                mr->SetMaterial(material);
            }
            */
        }

        mr->SetMesh(mesh);
        mr->SetMaterial(material);
    }

    if (addAnimator && modelScene.animations.Size() >= 1)
    {
        Animator *animator = gameObject->AddComponent<Animator>();
        animator->SetAnimation( modelScene.animations[0].Get() );
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

void SetGameObjectFromModelBoneTransforms(GameObject *gameObject,
                                          GameObject *rootBone,
                                          const ModelIOScene &modelScene)
{
    /*
    // Is it a bone gameObject ?
    const Mesh::Bone *gameObjectBone = nullptr;
    for (const RH<Mesh> &meshRH : modelScene.meshes)
    {
        if (meshRH.Get()->GetBonesPool().ContainsKey(gameObject->GetName()))
        {
            gameObjectBone = &(meshRH.Get()->GetBonesPool().
                               Get(gameObject->GetName()));
            break;
        }
    }

    // Set transform
    Matrix4 gameObjectLocalToParent = Matrix4::Identity;
    if (gameObjectBone)
    {
        // gameObject->GetTransform()->SetLocalPosition(Vector3::Zero);
        // gameObject->GetTransform()->SetLocalRotation(Quaternion::Identity);
        // gameObject->GetTransform()->SetLocalScale(Vector3::One);
        Matrix4 parentToWorldMatrix = gameObject->GetParent() ?
            gameObject->GetParent()->GetTransform()->GetLocalToWorldMatrix() :
            Matrix4::Identity;
        Matrix4 worldToParentMatrix = parentToWorldMatrix.Inversed();
        Matrix4 localToWorldMatrix = gameObject->GetTransform()->GetLocalToWorldMatrix();
        Matrix4 worldToLocalMatrix = localToWorldMatrix.Inversed();

        // gameObjectLocalToParent =
                  // worldToParentMatrix *
                  // rootBone->GetTransform()->GetLocalToWorldMatrix() *
                  // gameObjectBone->boneInRootBoneSpaceToLocalSpace.Inversed();
    }
    // gameObject->GetTransform()->FillFromMatrix(gameObjectLocalToParent);

    // Set children transforms
    for (GameObject *child : gameObject->GetChildren())
    {
        SetGameObjectFromModelBoneTransforms(child, rootBone, modelScene);
    }
    */
}

GameObject *Model::CreateGameObjectFromModel() const
{
    GameObject *modelGo = CreateGameObjectFromModelNodeTree(m_modelScene,
                                                            m_modelScene.modelTree);

    Vector3 scale = Vector3::One;
    Vector3 bboxSize = modelGo->GetAABBoxWorld().GetSize();
    float minSize = Math::Min(Math::Min(bboxSize.x, bboxSize.y), bboxSize.z);
    if (minSize > 0.0f)
    {
        scale = Vector3(1.0f / minSize);
    }
    modelGo->GetTransform()->SetLocalScale(scale);

    // If skinned mesh renderer, set bone references to gameObjects
    List<SkinnedMeshRenderer*> smrs =
                    modelGo->GetComponentsInChildren<SkinnedMeshRenderer>(true);
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
