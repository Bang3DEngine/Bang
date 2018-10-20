#include "Bang/SkinnedMeshRenderer.h"

#include <ext/alloc_traits.h>
#include <map>
#include <unordered_map>
#include <utility>

#include "Bang/Animator.h"
#include "Bang/Assert.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/FastDynamicCast.h"
#include "Bang/GameObject.h"
#include "Bang/IEventsName.h"
#include "Bang/IEventsObjectGatherer.h"
#include "Bang/Map.tcc"
#include "Bang/Matrix4.h"
#include "Bang/Matrix4.tcc"
#include "Bang/Mesh.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Model.h"
#include "Bang/ObjectGatherer.h"
#include "Bang/ObjectGatherer.tcc"
#include "Bang/Set.tcc"
#include "Bang/ShaderProgram.h"
#include "Bang/Transform.h"
#include "Bang/UMap.tcc"

namespace Bang
{
class ICloneable;
class Resource;
}

using namespace Bang;

SkinnedMeshRenderer::SkinnedMeshRenderer()
{
    CONSTRUCT_CLASS_ID(SkinnedMeshRenderer);

    m_gameObjectGatherer = new ObjectGatherer<GameObject, true>();
    m_gameObjectGatherer->RegisterListener(this);
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
    delete m_gameObjectGatherer;
}

Matrix4 SkinnedMeshRenderer::GetBoneTransformMatrixFor(
    GameObject *boneGameObject,
    const Matrix4 &transformInBoneSpace,
    UMap<GameObject *, Matrix4> *boneTransformInRootSpaceCache) const
{
    ASSERT(boneTransformInRootSpaceCache);
    {
        auto it = boneTransformInRootSpaceCache->Find(boneGameObject);
        if (it != boneTransformInRootSpaceCache->End())
        {
            return it->second;
        }
    }

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

    Matrix4 parentBoneTransformInRootSpace = GetBoneTransformMatrixFor(
        parentBoneGo,
        parentBoneGo->GetTransform()->GetLocalToParentMatrix(),
        boneTransformInRootSpaceCache);

    Matrix4 boneTransformInRootSpace =
        parentBoneTransformInRootSpace * parentBoneSpaceToRootSpace *
        transformInBoneSpace * rootSpaceToBoneSpace;

    boneTransformInRootSpaceCache->Add(boneGameObject,
                                       boneTransformInRootSpace);

    return boneTransformInRootSpace;
}

const Matrix4 &SkinnedMeshRenderer::GetInitialTransformMatrixFor(
    const String &boneName) const
{
    if (m_initialTransforms.ContainsKey(boneName))
    {
        return m_initialTransforms.Get(boneName);
    }
    return Matrix4::Identity;
}

const Map<String, Matrix4> &SkinnedMeshRenderer::GetInitialTransforms() const
{
    return m_initialTransforms;
}

const Set<String> &SkinnedMeshRenderer::GetBonesNames() const
{
    return m_bonesNames;
}

void SkinnedMeshRenderer::SetBoneUniforms(ShaderProgram *sp)
{
    sp->SetBool("B_HasBoneAnimations", true);
    sp->SetMatrix4Array("B_BoneAnimationMatrices",
                        m_bonesTransformsMatricesArrayUniform,
                        false);
}

void SkinnedMeshRenderer::UpdateBonesMatricesFromTransformMatrices()
{
    Map<String, Matrix4> bonesMatrices;
    UMap<GameObject *, Matrix4> bonesTransformsCache;
    for (const String &boneName : GetBonesNames())
    {
        if (GetActiveMesh())
        {
            if (GameObject *boneGameObject = GetBoneGameObject(boneName))
            {
                Matrix4 localToParent =
                    boneGameObject->GetTransform()->GetLocalToParentMatrix();
                Matrix4 boneTransformMatrix = GetBoneTransformMatrixFor(
                    boneGameObject, localToParent, &bonesTransformsCache);
                bonesMatrices.Add(boneName, boneTransformMatrix);
            }
        }
    }
    SetSkinnedMeshRendererCurrentBoneMatrices(bonesMatrices);
}

void SkinnedMeshRenderer::UpdateTransformMatricesFromInitialBonePosition()
{
    for (const String &boneName : GetBonesNames())
    {
        if (GameObject *go = GetBoneGameObject(boneName))
        {
            go->GetTransform()->FillFromMatrix(
                GetInitialTransformMatrixFor(boneName));
        }
    }
}

void SkinnedMeshRenderer::OnRender()
{
    MeshRenderer::OnRender();
}

void SkinnedMeshRenderer::Bind()
{
    MeshRenderer::Bind();
}

void SkinnedMeshRenderer::SetUniformsOnBind(ShaderProgram *sp)
{
    MeshRenderer::SetUniformsOnBind(sp);
    UpdateBonesMatricesFromTransformMatrices();
    SetBoneUniforms(sp);
}

void SkinnedMeshRenderer::SetRootBoneGameObjectName(
    const String &rootBoneGameObjectName)
{
    if (rootBoneGameObjectName != GetRootBoneGameObjectName())
    {
        p_rootBoneGameObject = nullptr;  // Reset cached root bone gameObject
        m_rootBoneGameObjectName = rootBoneGameObjectName;
        m_gameObjectGatherer->SetRoot(GetRootBoneGameObject());
        UpdateTransformMatricesFromInitialBonePosition();
        UpdateBonesMatricesFromTransformMatrices();
    }
}

Model *SkinnedMeshRenderer::GetActiveModel() const
{
    if (Mesh *mesh = GetActiveMesh())
    {
        if (Resource *parentRes = mesh->GetParentResource())
        {
            return DCAST<Model *>(parentRes);
        }
    }
    return nullptr;
}

GameObject *SkinnedMeshRenderer::GetRootBoneGameObject() const
{
    if (!p_rootBoneGameObject)
    {
        p_rootBoneGameObject = GetGameObject()
                                   ? GetGameObject()->FindInAncestorsAndThis(
                                         GetRootBoneGameObjectName(), true)
                                   : nullptr;
    }
    return p_rootBoneGameObject;
}

const String &SkinnedMeshRenderer::GetRootBoneGameObjectName() const
{
    return m_rootBoneGameObjectName;
}

GameObject *SkinnedMeshRenderer::GetBoneGameObject(const String &boneName) const
{
    GameObject *boneGo = nullptr;
    GameObject *rootBoneGo = GetRootBoneGameObject();
    auto it = m_boneNameToBoneGameObject.Find(boneName);
    if (it != m_boneNameToBoneGameObject.End())
    {
        boneGo = it->second;
    }
    else
    {
        boneGo = rootBoneGo ? rootBoneGo->FindInChildren(boneName) : nullptr;
        m_boneNameToBoneGameObject.Add(boneName, boneGo);
    }
    return boneGo;
}

Matrix4 SkinnedMeshRenderer::GetBoneSpaceToRootSpaceMatrix(
    const String &boneName) const
{
    return m_boneSpaceToRootSpaceMatrices.ContainsKey(boneName)
               ? m_boneSpaceToRootSpaceMatrices.Get(boneName)
               : Matrix4::Identity;
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
            auto it = mesh->GetBonesIds().Find(boneName);
            if (it != mesh->GetBonesIds().End())
            {
                uint boneIdx = it->second;
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
        if (GetRootBoneGameObject())
        {
            GetRootBoneGameObject()
                ->EventEmitter<IEventsName>::RegisterListener(this);

            m_bonesNames.Clear();
            m_initialTransforms.Clear();

            // Fill boneName to gameObject
            for (const auto &it : allBones)
            {
                const String &boneName = it.first;
                m_bonesNames.Add(boneName);
                if (GameObject *boneGo =
                        GetRootBoneGameObject()->FindInChildrenAndThis(boneName,
                                                                       true))
                {
                    m_initialTransforms.Add(
                        boneName,
                        boneGo->GetTransform()->GetLocalToParentMatrix());
                    boneGo->EventEmitter<IEventsName>::RegisterListener(this);
                }
            }

            // Calculate boneSpace to rootSpace matrices
            m_boneSpaceToRootSpaceMatrices.Clear();
            for (const auto &it : allBones)
            {
                const String &boneName = it.first;
                const Mesh::Bone &bone = it.second;
                Matrix4 boneSpaceToRootSpace =
                    bone.rootNodeSpaceToBoneBindSpace.Inversed();
                m_boneSpaceToRootSpaceMatrices.Add(boneName,
                                                   boneSpaceToRootSpace);
            }
        }
    }
}

void SkinnedMeshRenderer::OnObjectGathered(GameObject *go)
{
    p_rootBoneGameObject = nullptr;
    m_boneNameToBoneGameObject.Clear();
    RetrieveBonesBindPoseFromCurrentHierarchy();
}

void SkinnedMeshRenderer::OnObjectUnGathered(GameObject *previousGameObject,
                                             GameObject *go)
{
    p_rootBoneGameObject = nullptr;
    m_boneNameToBoneGameObject.Clear();
    RetrieveBonesBindPoseFromCurrentHierarchy();
}

void SkinnedMeshRenderer::OnNameChanged(GameObject *,
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
        m_boneSpaceToRootSpaceMatrices.Add(
            newName, m_boneSpaceToRootSpaceMatrices.Get(oldName));
        m_boneSpaceToRootSpaceMatrices.Remove(oldName);
    }

    if (m_initialTransforms.ContainsKey(oldName))
    {
        m_initialTransforms.Add(newName, m_initialTransforms.Get(oldName));
        m_initialTransforms.Remove(oldName);
    }
}

void SkinnedMeshRenderer::CloneInto(ICloneable *cloneable) const
{
    MeshRenderer::CloneInto(cloneable);

    SkinnedMeshRenderer *smrClone = SCAST<SkinnedMeshRenderer *>(cloneable);
    smrClone->m_bonesNames = m_bonesNames;
    smrClone->m_boneSpaceToRootSpaceMatrices = m_boneSpaceToRootSpaceMatrices;
    smrClone->m_initialTransforms = m_initialTransforms;
}

void SkinnedMeshRenderer::Reflect()
{
    MeshRenderer::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(SkinnedMeshRenderer,
                                   "Root Bone Name",
                                   SetRootBoneGameObjectName,
                                   GetRootBoneGameObjectName,
                                   BANG_REFLECT_HINT_HIDDEN());
}

Matrix4 SkinnedMeshRenderer::GetModelMatrixUniform() const
{
    return GetRootBoneGameObject()
               ? GetRootBoneGameObject()
                     ->GetTransform()
                     ->GetLocalToWorldMatrix()
               : MeshRenderer::GetModelMatrixUniform();
}
