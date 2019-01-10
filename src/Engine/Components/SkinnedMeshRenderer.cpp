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
class Serializable;
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

void SkinnedMeshRenderer::OnUpdate()
{
    Component::OnUpdate();
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
    GameObject *boneGo = GetBoneGameObject(boneName);
    if (!boneGo)
    {
        if (Transform *tr = boneGameObject->GetTransform())
        {
            return Transformation(tr->GetLocalToParentMatrix());
        }
        return Transformation::Identity();
    }

    GameObject *parentBoneGo = boneGameObject->GetParent();
    String parentBoneGoName = parentBoneGo ? parentBoneGo->GetName() : nullptr;

    Transformation rootSpaceToBoneSpace;
    if (boneGo)
    {
        rootSpaceToBoneSpace = GetRootSpaceToBoneSpaceTransformation(boneName);
    }

    Transformation parentBoneSpaceToRootSpace;
    if (GetBoneGameObject(parentBoneGoName))
    {
        parentBoneSpaceToRootSpace =
            GetRootSpaceToBoneSpaceTransformation(parentBoneGoName).Inversed();
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
    UMap<GameObject *, Transformation> bonesTransformsCache;
    Array<GameObject *> allBones = GetAllBoneGameObjects();
    for (GameObject *boneGo : allBones)
    {
        const String &boneName = boneGo->GetName();
        GameObject *boneGameObject = boneGo;
        const Transformation &localToParent =
            boneGameObject->GetTransform()->GetLocalTransformation();
        Transformation boneTransformMatrix = GetBoneTransformationFor(
            boneGameObject, localToParent, &bonesTransformsCache);

        bonesMatrices.Add(boneName, boneTransformMatrix.GetMatrix());
    }
    SetSkinnedMeshRendererCurrentBoneMatrices(bonesMatrices);
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

Array<GameObject *> SkinnedMeshRenderer::GetAllBoneGameObjects() const
{
    Array<GameObject *> allBones;
    if (GetRootBoneGameObject())
    {
        Array<GameObject *> allRootGoChildren =
            GetRootBoneGameObject()->GetChildrenRecursively();
        for (GameObject *rootBoneChild : allRootGoChildren)
        {
            if (m_boneNames.Contains(rootBoneChild->GetName()))
            {
                allBones.PushBack(rootBoneChild);
            }
        }
    }
    return allBones;
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

Transformation SkinnedMeshRenderer::GetRootSpaceToBoneSpaceTransformation(
    const String &boneName) const
{
    if (Model *model = GetActiveModel())
    {
        auto it = model->GetAllBones().Find(boneName);
        if (it != model->GetAllBones().End())
        {
            return it->second.rootSpaceToBoneBindSpaceTransformation;
        }
    }
    return Transformation::Identity();
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

void SkinnedMeshRenderer::ResetBoneTransformation(GameObject *boneGo)
{
    if (Model *model = GetActiveModel())
    {
        Transformation bindSpaceToRootSpaceTransformation;
        auto itThis = model->GetAllBones().Find(boneGo->GetName());
        if (itThis != model->GetAllBones().End())
        {
            const Mesh::Bone &bone = itThis->second;
            const Transformation &rootSpaceToBoneBindSpaceTransformation =
                bone.rootSpaceToBoneBindSpaceTransformation;
            bindSpaceToRootSpaceTransformation =
                rootSpaceToBoneBindSpaceTransformation.Inversed();
        }

        Transformation parentRootSpaceToBoneBindSpaceTransformation;
        auto itParent =
            model->GetAllBones().Find(boneGo->GetParent()->GetName());
        if (itParent != model->GetAllBones().End())
        {
            const Mesh::Bone &parentBone = itParent->second;
            parentRootSpaceToBoneBindSpaceTransformation =
                parentBone.rootSpaceToBoneBindSpaceTransformation;
        }

        Transformation localSpaceToParentSpace =
            (parentRootSpaceToBoneBindSpaceTransformation *
             bindSpaceToRootSpaceTransformation);

        boneGo->GetTransform()->FillFromTransformation(localSpaceToParentSpace);
    }
}

void SkinnedMeshRenderer::ResetBoneTransformations()
{
    Array<GameObject *> allBoneGos = GetAllBoneGameObjects();
    for (GameObject *boneGo : allBoneGos)
    {
        ResetBoneTransformation(boneGo);
    }
}

void SkinnedMeshRenderer::OnObjectGathered(GameObject *)
{
}

void SkinnedMeshRenderer::OnObjectUnGathered(GameObject *, GameObject *go)
{
    if (go == GetRootBoneGameObject())
    {
        p_rootBoneGameObject = nullptr;
    }
    m_boneNameToBoneGameObject.Remove(go->GetName());
}

void SkinnedMeshRenderer::Reflect()
{
    MeshRenderer::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(SkinnedMeshRenderer,
                                   "Root Bone Name",
                                   SetRootBoneGameObjectName,
                                   GetRootBoneGameObjectName,
                                   BANG_REFLECT_HINT_SHOWN(true));

    BANG_REFLECT_BUTTON(SkinnedMeshRenderer,
                        "Reset bone transformations",
                        [this]() { ResetBoneTransformations(); });
}

void SkinnedMeshRenderer::OnMeshLoaded(Mesh *)
{
    m_boneNames.Clear();
    if (Model *model = GetActiveModel())
    {
        Array<String> boneNames = model->GetAllBones().GetKeys();
        for (const String &boneName : boneNames)
        {
            m_boneNames.Add(boneName);
        }
    }
    UpdateBonesMatricesFromTransformMatrices();
}

Matrix4 SkinnedMeshRenderer::GetModelMatrixUniform() const
{
    return GetRootBoneGameObject()
               ? GetRootBoneGameObject()
                     ->GetTransform()
                     ->GetLocalToWorldMatrix()
               : MeshRenderer::GetModelMatrixUniform();
}
