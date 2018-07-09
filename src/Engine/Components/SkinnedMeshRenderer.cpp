#include "Bang/SkinnedMeshRenderer.h"

#include "Bang/Mesh.h"
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
Matrix4 GetBoneTransformMatrixFor(const String &boneName,
                                  GameObject *boneGameObject,
                                  GameObject *rootBoneGameObject,
                                  const Map<String, Mesh::Bone> &bonesPool)
{
    if (!boneGameObject->IsChildOf(rootBoneGameObject, true))
    {
        return Matrix4::Identity;
    }

    Matrix4 localToParent;
    if (Transform *tr = boneGameObject->GetTransform())
    {
        localToParent = tr->GetLocalToParentMatrix();
    }
    Matrix4 boneOffsetMatrix = bonesPool.Get(boneName).transform;
    Matrix4 parentToRootBone = GetBoneTransformMatrixFor(boneName,
                                                         boneGameObject->GetParent(),
                                                         rootBoneGameObject,
                                                         bonesPool);

    // localToParent = Matrix4::Identity;
    // boneOffsetMatrix = Matrix4::Identity;
    // parentToRootBone = Matrix4::Identity;
    if (Input::GetKeyDown(Key::R))
    {
        boneGameObject->GetTransform()->SetLocalPosition(Vector3::Zero);
        boneGameObject->GetTransform()->SetLocalRotation(Quaternion::Identity);
        boneGameObject->GetTransform()->SetLocalScale(Vector3::One);
    }
    if (Input::GetKeyDown(Key::F))
    {
        boneGameObject->GetTransform()->SetLocalPosition(Transform::GetPositionFromMatrix4(
                                                        boneOffsetMatrix));
        boneGameObject->GetTransform()->SetLocalRotation(Transform::GetRotationFromMatrix4(
                                                        boneOffsetMatrix));
        boneGameObject->GetTransform()->SetLocalScale(Transform::GetScaleFromMatrix4(
                                                        boneOffsetMatrix));

        // if (boneName == "thigh.R")
        // {
        //     Debug_Peek(boneOffsetMatrix);
        // }
    }
    /*
    */

    return parentToRootBone * localToParent; // * boneOffsetMatrix.Inversed();
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
                                                    bonesPool);
                    boneMatrices[boneIdx] = localToRootMatrix;
                    if (boneName == "thigh.R" && Input::GetKeyDown(Key::I))
                    {
                        Debug_Peek(boneMatrices[boneIdx]);
                    }
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
