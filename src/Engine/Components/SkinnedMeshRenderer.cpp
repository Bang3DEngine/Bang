#include "Bang/SkinnedMeshRenderer.h"

#include "Bang/Mesh.h"
#include "Bang/Model.h"
#include "Bang/Scene.h"
#include "Bang/Material.h"
#include "Bang/Animator.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/MaterialFactory.h"

USING_NAMESPACE_BANG

const String SkinnedMeshRenderer::XMLBoneGameObjectPrefix =
                                        "SkinnedMeshRendererBoneGameObject_";

SkinnedMeshRenderer::SkinnedMeshRenderer()
{
    SetMaterial( MaterialFactory::GetDefaultAnimated().Get() );
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

Matrix4 SkinnedMeshRenderer::GetBoneTransformMatrixFor(
                                    GameObject *boneGameObject,
                                    const Matrix4 &transformInBoneSpace) const
{
    if (!boneGameObject || boneGameObject == GetRootBoneGameObject())
    {
        return Matrix4::Identity;
    }

    const String &boneName = boneGameObject->GetName();
    if (!GetBoneGameObject(boneName))
    {
        if (Transform *tr = boneGameObject->GetTransform())
        {
            return tr->GetLocalToParentMatrix();
        }
        return Matrix4::Identity;
    }

    GameObject *parentBoneGo = boneGameObject->GetParent();
    String parentBoneGoName = parentBoneGo ? parentBoneGo->GetName() : nullptr;

    Matrix4 boneSpaceToRootSpace = Matrix4::Identity;
    if (GetBoneGameObject(boneName))
    {
        boneSpaceToRootSpace = GetBoneSpaceToRootSpaceMatrix(boneName);
    }
    Matrix4 rootSpaceToBoneSpace = boneSpaceToRootSpace.Inversed();

    Matrix4 parentBoneSpaceToRootSpace = Matrix4::Identity;
    if (GetBoneGameObject(parentBoneGoName))
    {
        parentBoneSpaceToRootSpace =
                               GetBoneSpaceToRootSpaceMatrix(parentBoneGoName);
    }

    Matrix4 parentBoneTransformInRootSpace =
                    GetBoneTransformMatrixFor(parentBoneGo,
                                              parentBoneGo->GetTransform()->
                                              GetLocalToParentMatrix());

    Matrix4 boneTransformInRootSpace = parentBoneTransformInRootSpace *
                                       parentBoneSpaceToRootSpace *
                                       transformInBoneSpace *
                                       rootSpaceToBoneSpace;
    return boneTransformInRootSpace;
}

Matrix4 SkinnedMeshRenderer::GetInitialTransformMatrixFor(const String &boneName) const
{
    if (m_initialTransforms.ContainsKey(boneName))
    {
        return m_initialTransforms.Get(boneName);
    }
    return Matrix4::Identity;
}

const Set<String> &SkinnedMeshRenderer::GetBonesNames() const
{
    return m_bonesNames;
}

void SkinnedMeshRenderer::UpdateBonesMatricesFromTransformMatrices()
{
    Map<String, Matrix4> bonesMatrices;
    for (const String &boneName : GetBonesNames())
    {
        if (GetActiveMesh())
        {
            if (GameObject *boneGameObject = GetBoneGameObject(boneName))
            {
                Matrix4 localToParent = boneGameObject->GetTransform()->
                                        GetLocalToParentMatrix();
                Matrix4 boneTransformMatrix = GetBoneTransformMatrixFor(
                                                              boneGameObject,
                                                              localToParent);
                bonesMatrices.Add(boneName, boneTransformMatrix);
            }
        }
    }
    SetSkinnedMeshRendererCurrentBoneMatrices(bonesMatrices);
}

void SkinnedMeshRenderer::OnRender()
{
    bool updateBonesMatricesFromTransformMatrices = true;
    Array<Animator*> animators = GetGameObject()->GetComponents<Animator>();
    for (Animator *animator : animators)
    {
        if (animator->IsPlaying())
        {
            m_needsToUpdateToDefaultMatrices = true;
            updateBonesMatricesFromTransformMatrices = false;
        }
    }

    if (updateBonesMatricesFromTransformMatrices)
    {
        if (m_needsToUpdateToDefaultMatrices)
        {
            for (const String &boneName : m_bonesNames)
            {
                if (GameObject *go = GetBoneGameObject(boneName))
                {
                    go->GetTransform()->FillFromMatrix(
                                    GetInitialTransformMatrixFor(boneName) );
                }
            }
            m_needsToUpdateToDefaultMatrices = false;
        }

        UpdateBonesMatricesFromTransformMatrices();
    }

    // Bind matrices
    if (Material *mat = GetActiveMaterial())
    {
        if (ShaderProgram *sp = mat->GetShaderProgram())
        {
            sp->Bind();
            sp->SetMatrix4Array("B_BoneAnimationMatrices",
                                m_bonesTransformsMatricesArrayUniform,
                                false);
        }
    }

    MeshRenderer::OnRender();
}

void SkinnedMeshRenderer::SetRootBoneGameObjectName(
                                          const String &rootBoneGameObjectName)
{
    m_rootBoneGameObjectName = rootBoneGameObjectName;
}

Model *SkinnedMeshRenderer::GetActiveModel() const
{
    if (Mesh *mesh = GetActiveMesh())
    {
        if (Resource *parentRes = mesh->GetParentResource())
        {
            return DCAST<Model*>(parentRes);
        }
    }
    return nullptr;
}

GameObject *SkinnedMeshRenderer::GetRootBoneGameObject() const
{
    return GetGameObject()->FindInAncestorsAndThis( GetRootBoneGameObjectName(),
                                                    true );
}

const String &SkinnedMeshRenderer::GetRootBoneGameObjectName() const
{
    return m_rootBoneGameObjectName;
}

GameObject *SkinnedMeshRenderer::GetBoneGameObject(const String &boneName) const
{
    GameObject *rootBoneGo = GetRootBoneGameObject();
    if (rootBoneGo)
    {
        return rootBoneGo->FindInChildren(boneName);
    }
    return nullptr;
}

Matrix4 SkinnedMeshRenderer::GetBoneSpaceToRootSpaceMatrix(const String &boneName) const
{
    return m_boneSpaceToRootSpaceMatrices.ContainsKey(boneName) ?
                m_boneSpaceToRootSpaceMatrices.Get(boneName) :
                Matrix4::Identity;
}

void SkinnedMeshRenderer::SetSkinnedMeshRendererCurrentBoneMatrices(
                                       const Map<String, Matrix4> &boneMatrices)
{
    if (Mesh *mesh = GetActiveMesh())
    {
        Array<Matrix4> boneMatricesArray(Animator::MaxNumBones,
                                         Matrix4::Identity);
        for (const auto &pair : boneMatrices)
        {
            const String &boneName = pair.first;
            const Matrix4 &boneMatrix = pair.second;
            if (mesh->GetBonesIndices().ContainsKey(boneName))
            {
                uint boneIdx = mesh->GetBonesIndices().Get(boneName);
                if (boneIdx < boneMatricesArray.Size())
                {
                    boneMatricesArray[boneIdx] = boneMatrix;
                }
            }
        }
        SetSkinnedMeshRendererCurrentBoneMatrices(boneMatricesArray);
    }
}

void SkinnedMeshRenderer::SetSkinnedMeshRendererCurrentBoneMatrices(
                            const Array<Matrix4> &boneMatrices)
{
    m_bonesTransformsMatricesArrayUniform = boneMatrices;
}

void SkinnedMeshRenderer::RetrieveBonesBindPoseFromCurrentHierarchy()
{
    if (Model *model = GetActiveModel())
    {
        const Map<String, Mesh::Bone> &allBones = model->GetAllBones();

        // Retrieve root gameObject
        SetRootBoneGameObjectName(model->GetRootGameObjectName());
        ASSERT(GetRootBoneGameObject());
        GetRootBoneGameObject()->EventEmitter<IEventsName>::RegisterListener(this);

        // Fill boneName to gameObject
        for (const auto &it : allBones)
        {
            const String &boneName = it.first;
            m_bonesNames.Add(boneName);
            if (GameObject *boneGo = GetRootBoneGameObject()->
                                     FindInChildrenAndThis(boneName, true))
            {
                m_initialTransforms.Add(boneName, boneGo->GetTransform()->
                                                  GetLocalToParentMatrix());
                boneGo->EventEmitter<IEventsName>::RegisterListener(this);
            }
        }

        // Calculate boneSpace to rootSpace matrices
        m_boneSpaceToRootSpaceMatrices.Clear();
        for (const auto &it : allBones)
        {
            const String &boneName = it.first;
            const Mesh::Bone &bone = it.second;
            Matrix4 boneSpaceToRootSpace = bone.rootNodeSpaceToBoneBindSpace.Inversed();
            m_boneSpaceToRootSpaceMatrices.Add(boneName, boneSpaceToRootSpace);
        }
    }
}

void SkinnedMeshRenderer::OnNameChanged(GameObject*,
                                        const String &oldName,
                                        const String &newName)
{
    if (oldName == m_rootBoneGameObjectName)
    {
        m_rootBoneGameObjectName = newName;
    }

    if (m_bonesNames.Contains(oldName))
    {
        m_bonesNames.Remove(oldName);
        m_bonesNames.Add(newName);
    }

    if (m_boneSpaceToRootSpaceMatrices.ContainsKey(oldName))
    {
        m_boneSpaceToRootSpaceMatrices.Add(newName,
                        m_boneSpaceToRootSpaceMatrices.Get(oldName));
        m_boneSpaceToRootSpaceMatrices.Remove(oldName);
    }

    if (m_initialTransforms.ContainsKey(oldName))
    {
        m_initialTransforms.Add(newName, m_initialTransforms.Get(oldName));
        m_initialTransforms.Remove(oldName);
    }
}

void SkinnedMeshRenderer::CloneInto(ICloneable *clone) const
{
    MeshRenderer::CloneInto(clone);

    SkinnedMeshRenderer *smrClone = SCAST<SkinnedMeshRenderer*>(clone);
    smrClone->m_bonesNames = m_bonesNames;
    smrClone->m_rootBoneGameObjectName = m_rootBoneGameObjectName;
    smrClone->m_boneSpaceToRootSpaceMatrices = m_boneSpaceToRootSpaceMatrices;
    smrClone->m_needsToUpdateToDefaultMatrices = m_needsToUpdateToDefaultMatrices;
    smrClone->m_initialTransforms = m_initialTransforms;
}

void SkinnedMeshRenderer::ImportXML(const XMLNode &xmlInfo)
{
    MeshRenderer::ImportXML(xmlInfo);

    if (xmlInfo.Contains("RootBoneGameObject"))
    {
        String rootBoneName = xmlInfo.Get<String>("RootBoneGameObjectName");
        SetRootBoneGameObjectName(rootBoneName);
    }
}

void SkinnedMeshRenderer::ExportXML(XMLNode *xmlInfo) const
{
    MeshRenderer::ExportXML(xmlInfo);

    xmlInfo->Set("RootBoneGameObjectName", GetRootBoneGameObjectName());
}

Matrix4 SkinnedMeshRenderer::GetModelMatrixUniform() const
{
    return GetRootBoneGameObject() ?
             GetRootBoneGameObject()->GetTransform()->GetLocalToWorldMatrix() :
             MeshRenderer::GetModelMatrixUniform();
}

