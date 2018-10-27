#include "Bang/AnimatorLayerMask.h"

#include "Bang/Animator.h"
#include "Bang/GameObject.h"

using namespace Bang;

AnimatorLayerMask::AnimatorLayerMask()
{
}

AnimatorLayerMask::~AnimatorLayerMask()
{
}

void AnimatorLayerMask::AddBone(const String &boneName)
{
    m_boneNames.PushBack(boneName);
}

void AnimatorLayerMask::RemoveBone(uint i)
{
    if (i < m_boneNames.Size())
    {
        m_boneNames.RemoveByIndex(i);
    }
}

void AnimatorLayerMask::RemoveBone(const String &boneName)
{
    RemoveBone(GetBoneNames().IndexOf(boneName));
}

void AnimatorLayerMask::ClearBones()
{
    m_boneNames.Clear();
}

const Array<String> &AnimatorLayerMask::GetBoneMaskNames() const
{
    return m_boneNames;
}

Set<String> AnimatorLayerMask::GetBoneMaskNamesSet(Animator *animator) const
{
    Set<String> boneMaskSet;
    if (animator)
    {
        if (GameObject *animatorGo = animator->GetGameObject())
        {
            for (const String &maskBoneName : GetBoneMaskNames())
            {
                GameObject *maskRootBoneGo =
                    animatorGo->GetParent()->FindInChildren(maskBoneName, true);
                if (maskRootBoneGo)
                {
                    Array<GameObject *> descendants =
                        maskRootBoneGo->GetChildrenRecursively();
                    boneMaskSet.Add(maskRootBoneGo->GetName());
                    for (GameObject *descendant : descendants)
                    {
                        boneMaskSet.Add(descendant->GetName());
                    }

                    // Todo, this should stop in the root node!!!
                    // GameObject *upGo = maskRootBoneGo;
                    // while ((upGo = upGo->GetParent()))
                    // {
                    //     boneMaskSet.Add(upGo->GetName());
                    // }
                }
            }
        }
    }
    return boneMaskSet;
}

const Array<String> &AnimatorLayerMask::GetBoneNames() const
{
    return m_boneNames;
}

void AnimatorLayerMask::Import(const Path &resourceFilepath)
{
    BANG_UNUSED(resourceFilepath);
}

void AnimatorLayerMask::ImportMeta(const MetaNode &metaNode)
{
    Resource::ImportMeta(metaNode);

    m_boneNames = metaNode.GetArray<String>("BoneNames");
}

void AnimatorLayerMask::ExportMeta(MetaNode *metaNode) const
{
    Resource::ExportMeta(metaNode);

    metaNode->SetArray<String>("BoneNames", m_boneNames);
}
