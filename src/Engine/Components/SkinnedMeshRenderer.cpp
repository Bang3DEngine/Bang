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

Matrix4 GetBoneTransformMatrixFor(
                const SkinnedMeshRenderer *smr,
                const String &boneName,
                GameObject *boneGameObject,
                GameObject *rootBoneGameObject,
                const Map<String, Matrix4> &boneSpaceToRootSpaceMatrices,
                const Map<String, Mesh::Bone> &bonesPool)
{
    if (!boneGameObject || boneGameObject == rootBoneGameObject)
    {
        return Matrix4::Identity;
    }

    if (!boneSpaceToRootSpaceMatrices.ContainsKey(boneName))
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
    if (boneSpaceToRootSpaceMatrices.ContainsKey(boneName))
    {
        boneSpaceToRootSpace = boneSpaceToRootSpaceMatrices.Get(boneName);
    }
    Matrix4 rootSpaceToBoneSpace = boneSpaceToRootSpace.Inversed();

    Matrix4 parentBoneSpaceToRootSpace = Matrix4::Identity;
    if (boneSpaceToRootSpaceMatrices.ContainsKey(parentBoneGoName))
    {
        parentBoneSpaceToRootSpace = boneSpaceToRootSpaceMatrices.Get(parentBoneGoName);
    }

    Matrix4 localToParentTransform = boneGameObject->GetTransform()->GetLocalToParentMatrix();

    Matrix4 parentBoneTransformInRootSpace = GetBoneTransformMatrixFor(
                                              smr,
                                              parentBoneGoName,
                                              parentBoneGo,
                                              rootBoneGameObject,
                                              boneSpaceToRootSpaceMatrices,
                                              bonesPool);

    Matrix4 boneTransformInRootSpace = parentBoneTransformInRootSpace *
                                       parentBoneSpaceToRootSpace *
                                       localToParentTransform *
                                       rootSpaceToBoneSpace;
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
                                                  this,
                                                  boneName,
                                                  boneGameObject,
                                                  rootBoneGo,
                                                  m_boneSpaceToRootSpaceMatrices,
                                                  bonesPool);
                    boneMatrices[boneIdx] = localToRootMatrix;
                }
            }
        }
    }

    if (Material *mat = GetActiveMaterial())
    {
        if (ShaderProgram *sp = mat->GetShaderProgram())
        {
            sp->SetMatrix4Array("B_BoneAnimationMatrices", boneMatrices, false);
        }
    }

    MeshRenderer::OnRender();
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
    if (Model *model = GetActiveModel())
    {
        const Map<String, Mesh::Bone> &allBones = model->GetAllBones();

        // Retrieve root gameObject
        p_rootBoneGameObject = nullptr;
        List<GameObject*> children = GetGameObject()->GetChildrenRecursively();
        for (GameObject *child : children)
        {
            GameObject *parent = child->GetParent();
            if (  allBones.ContainsKey(child->GetName()) &&
                 !allBones.ContainsKey(parent->GetName()) )
            {
                SetRootBoneGameObject(child);
                break;
            }
        }
        if (!GetRootBoneGameObject()) // Pick root if not found previously
        {
            GameObject *rootBoneGameObject = GetGameObject()->
                        FindInAncestorsAndThis(model->GetRootGameObjectName());
            SetRootBoneGameObject(rootBoneGameObject);
        }

        // Fill boneName to gameObject
        ASSERT(GetRootBoneGameObject());
        m_boneNameToGameObject.Clear();
        for (const auto &it : allBones)
        {
            const String &boneName = it.first;
            if (GameObject *boneGo = GetRootBoneGameObject()->
                                     FindInChildren(boneName, true))
            {
                m_boneNameToGameObject.Add(boneName, boneGo);
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

Matrix4 SkinnedMeshRenderer::GetModelMatrixUniform() const
{
    return GetRootBoneGameObject() ?
             GetRootBoneGameObject()->GetTransform()->GetLocalToWorldMatrix() :
             MeshRenderer::GetModelMatrixUniform();
}

