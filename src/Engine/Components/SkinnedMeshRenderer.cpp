#include "Bang/SkinnedMeshRenderer.h"

#include <map>
#include <unordered_map>
#include <utility>

#include "Bang/Animator.h"
#include "Bang/Assert.h"
#include "Bang/ClassDB.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
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
class Asset;
}  // namespace Bang

using namespace Bang;

SkinnedMeshRenderer::SkinnedMeshRenderer()
{
    SET_INSTANCE_CLASS_ID(SkinnedMeshRenderer);

    m_gameObjectGatherer = new ObjectGatherer<GameObject, true>();
    m_gameObjectGatherer->RegisterListener(this);
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
    delete m_gameObjectGatherer;
}

Transformation SkinnedMeshRenderer::GetBoneTransformationFor(
    GameObject *boneGameObject,
    const Transformation &transformInBoneSpace,
    UMap<GameObject *, Transformation> *boneTransformInRootSpaceCache) const
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
        return Transformation::Identity();
    }

    const String &boneName = boneGameObject->GetName();
    if (!GetBoneGameObject(boneName))
    {
        if (Transform *tr = boneGameObject->GetTransform())
        {
            return Transformation(tr->GetLocalToParentMatrix());
        }
        return Transformation::Identity();
    }

    GameObject *parentBoneGo = boneGameObject->GetParent();
    String parentBoneGoName = parentBoneGo ? parentBoneGo->GetName() : nullptr;

    Transformation boneSpaceToRootSpace = Transformation::Identity();
    if (GetBoneGameObject(boneName))
    {
        boneSpaceToRootSpace = GetBoneSpaceToRootSpaceTransformation(boneName);
    }
    Transformation rootSpaceToBoneSpace = boneSpaceToRootSpace.Inversed();

    Transformation parentBoneSpaceToRootSpace = Transformation::Identity();
    if (GetBoneGameObject(parentBoneGoName))
    {
        parentBoneSpaceToRootSpace =
            GetBoneSpaceToRootSpaceTransformation(parentBoneGoName);
    }

    Transformation parentBoneTransformInRootSpace = GetBoneTransformationFor(
        parentBoneGo,
        parentBoneGo->GetTransform()->GetLocalTransformation(),
        boneTransformInRootSpaceCache);

    Transformation boneTransformInRootSpace =
        parentBoneTransformInRootSpace * parentBoneSpaceToRootSpace *
        transformInBoneSpace * rootSpaceToBoneSpace;

    boneTransformInRootSpaceCache->Add(boneGameObject,
                                       boneTransformInRootSpace);

    return boneTransformInRootSpace;
}

const Transformation &SkinnedMeshRenderer::GetInitialTransformationFor(
    const String &boneName) const
{
    if (m_initialTransformations.ContainsKey(boneName))
    {
        return m_initialTransformations.Get(boneName);
    }
    return Transformation::Identity();
}

const Map<String, Transformation>
    &SkinnedMeshRenderer::GetInitialTransformations() const
{
    return m_initialTransformations;
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
    if (GameObject *rootBoneGo = GetRootBoneGameObject())
    {
        Map<String, Matrix4> bonesMatrices;
        UMap<GameObject *, Transformation> bonesTransformsCache;
        Array<GameObject *> childrenRecursive =
            rootBoneGo->GetChildrenRecursively();
        for (GameObject *child : childrenRecursive)
        {
            const String &boneName = child->GetName();
            GameObject *boneGameObject = child;
            const Transformation &localToParent =
                boneGameObject->GetTransform()->GetLocalTransformation();
            Transformation boneTransformMatrix = GetBoneTransformationFor(
                boneGameObject, localToParent, &bonesTransformsCache);

            bonesMatrices.Add(boneName, boneTransformMatrix.GetMatrix());
        }
        SetSkinnedMeshRendererCurrentBoneMatrices(bonesMatrices);
    }
}

void SkinnedMeshRenderer::UpdateTransformMatricesFromInitialBonePosition()
{
    if (GameObject *rootBoneGo = GetRootBoneGameObject())
    {
        Array<GameObject *> childrenRecursive =
            rootBoneGo->GetChildrenRecursively();
        for (GameObject *child : childrenRecursive)
        {
            const String &boneName = child->GetName();
            GameObject *boneGameObject = child;
            boneGameObject->GetTransform()->FillFromTransformation(
                GetInitialTransformationFor(boneName));
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
        if (Asset *parentRes = mesh->GetParentAsset())
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

Transformation SkinnedMeshRenderer::GetBoneSpaceToRootSpaceTransformation(
    const String &boneName) const
{
    return m_boneSpaceToRootSpaceTransformations.ContainsKey(boneName)
               ? m_boneSpaceToRootSpaceTransformations.Get(boneName)
               : Transformation::Identity();
}

void SkinnedMeshRenderer::SetSkinnedMeshRendererCurrentBoneMatrices(
    const Map<String, Matrix4> &boneMatrices)
{
    if (Mesh *mesh = GetActiveMesh())
    {
        Array<Matrix4> boneMatricesArray(Animator::MaxNumBones,
                                         Matrix4::Identity());
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

void SkinnedMeshRenderer::
    RetrieveBonesInitialTransformationFromCurrentHierarchy()
{
    if (Model *model = GetActiveModel())
    {
        const Map<String, Mesh::Bone> &allBones = model->GetAllBones();

        // Retrieve root gameObject
        SetRootBoneGameObjectName(model->GetRootGameObjectName());
        if (GetRootBoneGameObject())
        {
            m_initialTransformations.Clear();

            // Fill boneName to gameObject
            for (const auto &it : allBones)
            {
                const String &boneName = it.first;
                if (GameObject *boneGo =
                        GetRootBoneGameObject()->FindInChildrenAndThis(boneName,
                                                                       true))
                {
                    m_initialTransformations.Add(
                        boneName,
                        boneGo->GetTransform()->GetLocalTransformation());
                }
            }

            // Calculate boneSpace to rootSpace matrices
            m_boneSpaceToRootSpaceTransformations.Clear();
            for (const auto &it : allBones)
            {
                const String &boneName = it.first;
                const Mesh::Bone &bone = it.second;
                m_boneSpaceToRootSpaceTransformations.Add(
                    boneName,
                    bone.rootSpaceToBoneBindSpaceTransformation.Inversed());
            }
        }
    }
}

void SkinnedMeshRenderer::OnObjectGathered(GameObject *)
{
    p_rootBoneGameObject = nullptr;
    m_boneNameToBoneGameObject.Clear();
    RetrieveBonesInitialTransformationFromCurrentHierarchy();
}

void SkinnedMeshRenderer::OnObjectUnGathered(GameObject *, GameObject *)
{
    p_rootBoneGameObject = nullptr;
    m_boneNameToBoneGameObject.Clear();
    RetrieveBonesInitialTransformationFromCurrentHierarchy();
}

void SkinnedMeshRenderer::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    MeshRenderer::CloneInto(clone, cloneGUID);

    SkinnedMeshRenderer *smrClone = SCAST<SkinnedMeshRenderer *>(clone);
    smrClone->m_boneSpaceToRootSpaceTransformations =
        m_boneSpaceToRootSpaceTransformations;
    smrClone->m_initialTransformations = m_initialTransformations;
}

void SkinnedMeshRenderer::Reflect()
{
    MeshRenderer::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(SkinnedMeshRenderer,
                                   "Root Bone Name",
                                   SetRootBoneGameObjectName,
                                   GetRootBoneGameObjectName,
                                   BANG_REFLECT_HINT_SHOWN(false));
}

Matrix4 SkinnedMeshRenderer::GetModelMatrixUniform() const
{
    return GetRootBoneGameObject()
               ? GetRootBoneGameObject()
                     ->GetTransform()
                     ->GetLocalToWorldMatrix()
               : MeshRenderer::GetModelMatrixUniform();
}
