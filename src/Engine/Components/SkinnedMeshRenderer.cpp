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
                const Map<String, Mesh::Bone> &bonesPool)
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

    // Matrix4 rootNodeToBoneSpace = bonesPool.Get(boneName).rootNodeSpaceToBoneSpace;
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
    // Matrix4 rootNodeToParentBoneSpace = bonesPool.ContainsKey(boneName) ?
    //           bonesPool.Get(boneName).rootNodeSpaceToBoneSpace : Matrix4::Identity;
    // Matrix4 parentBoneToRootNode = GetBoneTransformMatrixFor(boneName,
    //                                                          boneGameObject->GetParent(),
    //                                                          rootBoneGameObject,
    //                                                          bonesPool);


    Matrix4 parentBoneTransformInRootSpace = GetBoneTransformMatrixFor(
                                              parentBoneGoName,
                                              parentBoneGo,
                                              rootBoneGameObject,
                                              boneSpaceToParentBoneSpaceMatrices,
                                              boneSpaceToRootSpaceMatrices,
                                              bonesPool);
    Matrix4 parentBoneToRootNode = Matrix4::Identity;
    if (GameObject *parentBoneGameObject = boneGameObject->GetParent())
    {
        if (bonesPool.ContainsKey(parentBoneGameObject->GetName()))
        {
            const Mesh::Bone &parentBone = bonesPool.Get(parentBoneGameObject->
                                                         GetName());
            parentBoneToRootNode = parentBone.rootNodeSpaceToBoneSpace.Inversed();
        }
    }

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
        boneGameObject->GetTransform()->FillFromMatrix(rootSpaceToBoneSpace);
        // if (boneName == "thigh.R")
        // {
        //     Debug_Peek(boneOffsetMatrix);
        // }
    }
    /*
    */

    // return parentToRootBone *
    //        boneOffsetMatrix.Inversed() *
    //        localToParent *
    //        boneOffsetMatrix;
    // return parentToRootBone.Inversed() * localToParent;
    if (Input::GetKeyDown(Key::H) && boneName == "thigh.R")
    {
        Debug_Peek(boneGameObject->GetTransform()->GetLocalToWorldMatrix());
    }
    // Matrix4 rootBoneToWorldMatrix = rootBoneGameObject->GetTransform()->
    //                                 GetLocalToWorldMatrixInv();
    // Matrix4 worldToRootBoneMatrix = rootBoneToWorldMatrix.Inversed();
    // Matrix4 parentBoneSpaceToRootNode = rootNodeToParentBoneSpace.Inversed();
    // Matrix4 boneSpaceToRootNode   = rootNodeToBoneSpace.Inversed();
    // Matrix4 parentBoneSpaceToBoneSpace = rootNodeToBoneSpace * parentBoneSpaceToRootNode;

    ASSERT(boneSpaceToParentBoneSpaceMatrices.ContainsKey(boneName));
    Matrix4 boneSpaceToParentBoneSpace = boneSpaceToParentBoneSpaceMatrices.Get(boneName);
    Matrix4 boneTransformInParentBoneSpace =
                      boneGameObject->GetTransform()->GetLocalToParentMatrix();

    if (Input::GetKeyDown(Key::T) && boneName == "Armature")
    {
        Debug_Log(boneName << ": ");
        Debug_Peek(rootSpaceToBoneSpace);
        Debug_Peek(parentBoneToRootNode);
        Debug_Peek(boneTransformInParentBoneSpace);
        Debug_Peek(rootSpaceToBoneSpace *
                   parentBoneToRootNode *
                   boneTransformInParentBoneSpace);
        Debug_Log("\n");
    }
//     Matrix4 boneTransformInBoneSpace = rootNodeToBoneSpace *
//                                        parentBoneToRootNode *
//                                        boneTransformInParentBoneSpace;

    Matrix4 boneTransformInRootSpace = // parentBoneTransformInRootSpace *
                                       // boneSpaceToRootSpace *
                                       parentBoneSpaceToRootSpace *
                                       boneSpaceToParentBoneSpace.Inversed() *
                                       boneTransformInParentBoneSpace *
                                       rootSpaceToParentBoneSpace
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
        m_boneNameToGameObject.Clear();
        m_boneSpaceToParentSpaceMatrices.Clear();

        // Retrieve root gameObject
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

        for (const auto &it : mesh->GetBonesPool())
        {
            const String &boneName = it.first;
            const Mesh::Bone &bone = it.second;
            if (GameObject *boneGo = GetRootBoneGameObject()->FindInChildren(boneName,
                                                                             true))
            {
                // const String &boneGoParentName = boneGo->GetParent() ?
                //                     boneGo->GetParent ()->GetName() : "";
                // if (!sMesh->GetBonesPool().ContainsKey(boneGoParentName))
                // {
                //     rootBone = boneGo; // ->GetParent();
                //     smr->SetRootBoneGameObject(rootBone);
                // }
                SetBoneGameObject(boneName, boneGo);
                Matrix4 boneSpaceToParentSpaceMatrix =
                        boneGo->GetTransform()->GetLocalToParentMatrix();
                m_boneSpaceToParentSpaceMatrices.Add(boneName,
                                                     boneSpaceToParentSpaceMatrix);
            }
        }

        // Calculate boneSpace to rootSpace matrices
        m_boneSpaceToRootSpaceMatrices.Clear();
        for (const auto &it : mesh->GetBonesPool())
        {
            ASSERT(p_rootBoneGameObject);
            const String &boneName = it.first;
            const Mesh::Bone &bone = it.second;
            Matrix4 boneSpaceToRootSpace = Matrix4::Identity;
            if (m_boneNameToGameObject.ContainsKey(boneName))
            {
                /*
                GameObject *currentBoneGo = m_boneNameToGameObject.Get(boneName);
                while (currentBoneGo != p_rootBoneGameObject &&
                       m_boneNameToGameObject.ContainsKey(currentBoneGo->GetName()))
                {
                    Matrix4 currentBoneSpaceToParentSpaceMatrix =
                                    m_boneSpaceToParentSpaceMatrices.Get(
                                                currentBoneGo->GetName());
                    boneSpaceToRootSpace = currentBoneSpaceToParentSpaceMatrix *
                                           boneSpaceToRootSpace;
                    currentBoneGo = currentBoneGo->GetParent();
                }
                */

                boneSpaceToRootSpace = bone.rootNodeSpaceToBoneSpace.Inversed();
                m_boneSpaceToRootSpaceMatrices.Add(boneName, boneSpaceToRootSpace);
            }
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

