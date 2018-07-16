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

#include "Bang/Input.h"
Matrix4 GetBoneTransformMatrixFor(
                const String &boneName,
                GameObject *boneGameObject,
                GameObject *rootBoneGameObject,
                const Map<String, Matrix4> &boneSpaceToParentBoneSpaceMatrices,
                const Map<String, Matrix4> &boneSpaceToRootSpaceMatrices,
                const Map<String, Mesh::Bone> &bonesPool,
                bool recursiveCall = false)
{
    if (!boneSpaceToParentBoneSpaceMatrices.ContainsKey(boneName))
    {
        return Matrix4::Identity;
    }

    Matrix4 localToParent;
    if (Transform *tr = boneGameObject->GetTransform())
    {
        localToParent = tr->GetLocalToParentMatrix();
    }

    Matrix4 boneSpaceToRootSpace = Matrix4::Identity;
    if (boneSpaceToRootSpaceMatrices.ContainsKey(boneName))
    {
        boneSpaceToRootSpace = boneSpaceToRootSpaceMatrices.Get(boneName);
    }
    Matrix4 rootSpaceToBoneSpace = boneSpaceToRootSpace.Inversed();

    GameObject *parentBoneGo = boneGameObject->GetParent();
    String parentBoneGoName = parentBoneGo ? parentBoneGo->GetName() : nullptr;
    Matrix4 parentBoneSpaceToRootSpace = Matrix4::Identity;
    if (boneSpaceToRootSpaceMatrices.ContainsKey(parentBoneGoName))
    {
        parentBoneSpaceToRootSpace = boneSpaceToRootSpaceMatrices.Get(parentBoneGoName);
    }
    Matrix4 rootSpaceToParentBoneSpace = parentBoneSpaceToRootSpace.Inversed();


    Matrix4 parentBoneTransformInRootSpace = GetBoneTransformMatrixFor(
                                              parentBoneGoName,
                                              parentBoneGo,
                                              rootBoneGameObject,
                                              boneSpaceToParentBoneSpaceMatrices,
                                              boneSpaceToRootSpaceMatrices,
                                              bonesPool,
                                              true);
    Matrix4 parentBoneToRootNode = Matrix4::Identity;
    if (GameObject *parentBoneGameObject = boneGameObject->GetParent())
    {
        if (bonesPool.ContainsKey(parentBoneGameObject->GetName()))
        {
            const Mesh::Bone &parentBone = bonesPool.Get(parentBoneGameObject->
                                                         GetName());
            parentBoneToRootNode = parentBone.rootNodeSpaceToBoneBindSpace.Inversed();
        }
    }

    if (Input::GetKeyDown(Key::R))
    {
        boneGameObject->GetTransform()->SetLocalPosition(Vector3::Zero);
        boneGameObject->GetTransform()->SetLocalRotation(Quaternion::Identity);
        boneGameObject->GetTransform()->SetLocalScale(Vector3::One);
    }

    ASSERT(boneSpaceToParentBoneSpaceMatrices.ContainsKey(boneName));
    Matrix4 boneSpaceToParentBoneSpace = boneSpaceToParentBoneSpaceMatrices.Get(boneName);
    Matrix4 boneTransformInBoneSpace =
                      boneGameObject->GetTransform()->GetLocalToParentMatrix();

    Matrix4 boneTransformInRootSpace = parentBoneTransformInRootSpace *
                                       // boneSpaceToRootSpace *
                                       // parentBoneTransformInRootSpace *
                                   // parentBoneSpaceToRootSpace *
                                   boneSpaceToRootSpace *
                                       // boneSpaceToParentBoneSpace *
                                       // localToParent *
                                     // boneTransformInBoneSpace.Inversed() *
                                     // boneSpaceToParentBoneSpace.Inversed() *
                                     boneTransformInBoneSpace *
                                       //
                                   rootSpaceToBoneSpace
                                   // rootSpaceToParentBoneSpace
                                       // rootSpaceToBoneSpace
                                       ;

    return boneTransformInRootSpace;
}

void SkinnedMeshRenderer::OnRender()
{
    Array<Matrix4> boneMatrices;
    boneMatrices.Resize(Animator::MaxNumBones);
    for (int i = 0; i < boneMatrices.Size(); ++i)
    {
        boneMatrices[i] = Matrix4::Identity;
    }

    if (GameObject *rootBoneGo = GetRootBoneGameObject())
    {
        for (const auto &it : GetBoneNameToGameObject())
        {
            if (GetActiveMesh())
            {
                const String &boneName = it.first;
                GameObject *boneGameObject = it.second;
                uint boneIdx = GetActiveMesh()->GetBoneIndex(boneName);
                if (boneIdx < boneMatrices.Size())
                {
                    const auto &bonesPool = GetActiveMesh()->GetBonesPool();
                    Matrix4 localToRootMatrix = GetBoneTransformMatrixFor(
                                boneName,
                                boneGameObject,
                                rootBoneGo,
                                m_boneSpaceToParentSpaceMatrices,
                                m_boneSpaceToRootSpaceMatrices,
                                bonesPool);
                    boneMatrices[boneIdx] = localToRootMatrix;
                }
            }
        }
    }

    if (GetActiveMesh() && Input::GetKeyDown(Key::X))
    {
        Debug_Peek(GetActiveMesh()->GetBonesIndices());
    }

    // Map<String, Matrix4> boneNameToTransformation;
    // boneNameToTransformation.Add();

    if (Material *mat = GetActiveMaterial())
    {
        if (ShaderProgram *sp = mat->GetShaderProgram())
        {
            sp->SetMatrix4Array("B_BoneAnimationMatrices", boneMatrices, false);
        }
    }

    MeshRenderer::OnRender();
    /*
    */
}

void SkinnedMeshRenderer::SetRootBoneGameObject(GameObject *rootBoneGameObject)
{
    p_rootBoneGameObject = rootBoneGameObject;
}

void SkinnedMeshRenderer::SetBoneGameObject(const String &boneName,
                                            GameObject *gameObject)
{
    m_boneNameToGameObject.Add(boneName, gameObject);
}

GameObject *SkinnedMeshRenderer::GetRootBoneGameObject() const
{
    return p_rootBoneGameObject;
}

Matrix4 SkinnedMeshRenderer::GetBoneToRootBoneMatrix(const String &boneName) const
{
    Matrix4 boneToRootBoneMatrix = Matrix4::Identity;
    if (GameObject *boneGo = GetBoneGameObject(boneName))
    {
        if (GameObject *rootBoneGo = GetRootBoneGameObject())
        {
            boneToRootBoneMatrix =
                   rootBoneGo->GetTransform()->GetLocalToWorldMatrixInv() *
                   boneGo->GetTransform()->GetLocalToWorldMatrix();
        }
    }
    return boneToRootBoneMatrix;
}

GameObject *SkinnedMeshRenderer::GetBoneGameObject(const String &boneName) const
{
    if (m_boneNameToGameObject.ContainsKey(boneName))
    {
        return m_boneNameToGameObject.Get(boneName);
    }
    return nullptr;
}

const Map<String, GameObject *> &SkinnedMeshRenderer::GetBoneNameToGameObject() const
{
    return m_boneNameToGameObject;
}

void SkinnedMeshRenderer::RetrieveBonesBindPoseFromCurrentHierarchy()
{
    if (Mesh *mesh = GetActiveMesh())
    {
        // Retrieve root gameObject
        p_rootBoneGameObject = nullptr;
        if (Resource *parentRes = mesh->GetParentResource())
        {
            if (Model *model = DCAST<Model*>(parentRes))
            {
                GameObject *rootBoneGameObject =
                        GetGameObject()->
                        FindInAncestorsAndThis(model->GetRootGameObjectName());
                SetRootBoneGameObject(rootBoneGameObject);
            }
        }

        // Fill boneName to gameObject
        ASSERT(GetRootBoneGameObject());
        m_boneNameToGameObject.Clear();
        for (const auto &it : mesh->GetBonesPool())
        {
            const String &boneName = it.first;
            GameObject *boneGo = GetRootBoneGameObject()->
                                 FindInChildren(boneName, true);
            if (boneGo)
            {
                m_boneNameToGameObject.Add(boneName, boneGo);
            }
        }

        // Calculate boneSpace to rootSpace matrices
        m_boneSpaceToRootSpaceMatrices.Clear();
        for (const auto &it : mesh->GetBonesPool())
        {
            const String &boneName = it.first;
            const Mesh::Bone &bone = it.second;
            Matrix4 boneSpaceToRootSpace = bone.rootNodeSpaceToBoneBindSpace.Inversed();
            m_boneSpaceToRootSpaceMatrices.Add(boneName, boneSpaceToRootSpace);
        }

        // Calculate boneSpace to parentBoneSpace matrices
        m_boneSpaceToParentSpaceMatrices.Clear();
        for (const auto &it : m_boneSpaceToRootSpaceMatrices)
        {
            const String &boneName = it.first;
            const Matrix4 &boneSpaceToRootSpaceMatrix = it.second;

            Matrix4 boneSpaceToParentSpaceMatrix = Matrix4::Identity;
            if (m_boneNameToGameObject.ContainsKey(boneName))
            {
                GameObject *boneGo = m_boneNameToGameObject.Get(boneName);
                GameObject *parentGo = boneGo->GetParent();
                if (parentGo)
                {
                    const String &parentGoName = parentGo->GetName();
                    if (m_boneSpaceToRootSpaceMatrices.ContainsKey(parentGoName))
                    {
                        const Matrix4 &parentBoneSpaceToRootSpaceMatrix =
                            m_boneSpaceToRootSpaceMatrices.Get(parentGoName);
                        boneSpaceToParentSpaceMatrix =
                                boneSpaceToRootSpaceMatrix *
                                parentBoneSpaceToRootSpaceMatrix.Inversed();
                    }
                }
            }
            m_boneSpaceToParentSpaceMatrices.Add(boneName,
                                                 boneSpaceToParentSpaceMatrix);
        }

    }

}

void SkinnedMeshRenderer::ImportXML(const XMLNode &xmlInfo)
{
    MeshRenderer::ImportXML(xmlInfo);

    for (const auto &it : xmlInfo.GetAttributes())
    {
        const String attrName = it.first;
        const XMLAttribute &xmlAttr = it.second;
        if (attrName.BeginsWith(SkinnedMeshRenderer::XMLBoneGameObjectPrefix))
        {
            String boneName = attrName.SubString(
                        SkinnedMeshRenderer::XMLBoneGameObjectPrefix.Size());
            GUID gameObjectGUID = xmlAttr.Get<GUID>();
            GameObject *gameObject = GetGameObject()->GetScene()->
                                     FindInChildren(gameObjectGUID, true);
            SetBoneGameObject(boneName, gameObject);
        }
    }

    if (xmlInfo.Contains("RootBoneGameObject"))
    {
        GUID rootBoneGoGUID = xmlInfo.Get<GUID>("RootBoneGameObject");
        SetRootBoneGameObject( GetGameObject()->GetScene()->
                               FindInChildren(rootBoneGoGUID) );
    }
}

void SkinnedMeshRenderer::ExportXML(XMLNode *xmlInfo) const
{
    MeshRenderer::ExportXML(xmlInfo);

    for (const auto &it : GetBoneNameToGameObject())
    {
        String boneName = it.first;
        GameObject *boneGameObject = it.second;
        String attrName = SkinnedMeshRenderer::XMLBoneGameObjectPrefix + boneName;
        xmlInfo->Set<GUID>(attrName, boneGameObject->GetGUID());
    }

    xmlInfo->Set("RootBoneGameObject", GetRootBoneGameObject());
}

