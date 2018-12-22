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

void AnimatorLayerMask::AddBoneEntry(
    const AnimatorLayerMask::BoneEntry &boneEntry)
{
    m_boneEntries.PushBack(boneEntry);
}

void AnimatorLayerMask::RemoveBoneEntry(uint i)
{
    if (i < m_boneEntries.Size())
    {
        m_boneEntries.RemoveByIndex(i);
    }
}

void AnimatorLayerMask::RemoveBoneEntry(const String &boneName)
{
    for (uint i = 0; i < GetBoneEntries().Size(); ++i)
    {
        if (GetBoneEntries()[i].boneName == boneName)
        {
            RemoveBoneEntry(i);
            break;
        }
    }
}

void AnimatorLayerMask::ClearBoneEntries()
{
    m_boneEntries.Clear();
}

const Array<AnimatorLayerMask::BoneEntry> &AnimatorLayerMask::GetBoneEntries()
    const
{
    return m_boneEntries;
}

Set<String> AnimatorLayerMask::GetBoneMaskNamesSet(Animator *animator) const
{
    Set<String> boneMaskSet;
    if (animator)
    {
        if (GameObject *animatorGo = animator->GetGameObject())
        {
            for (const BoneEntry &maskBoneEntry : GetBoneEntries())
            {
                const String &boneName = maskBoneEntry.boneName;
                if (GameObject *boneGo =
                        animatorGo->GetParent()->FindInChildren(boneName, true))
                {
                    boneMaskSet.Add(boneGo->GetName());

                    if (maskBoneEntry.addDescendants)
                    {
                        Array<GameObject *> descendants =
                            boneGo->GetChildrenRecursively();
                        for (GameObject *descendant : descendants)
                        {
                            boneMaskSet.Add(descendant->GetName());
                        }
                    }

                    if (maskBoneEntry.addAscendants)
                    {
                        GameObject *ascendantGo = boneGo->GetParent();
                        while (ascendantGo)
                        {
                            boneMaskSet.Add(ascendantGo->GetName());

                            bool stop = (ascendantGo->FindInChildren(
                                animatorGo->GetName(), false));
                            if (stop)
                            {
                                break;
                            }
                            ascendantGo = ascendantGo->GetParent();
                        }
                    }
                }
            }
        }
    }
    return boneMaskSet;
}

void AnimatorLayerMask::Import(const Path &assetFilepath)
{
    BANG_UNUSED(assetFilepath);
}

void AnimatorLayerMask::ImportMeta(const MetaNode &metaNode)
{
    Asset::ImportMeta(metaNode);

    m_boneEntries.Clear();
    Array<String> boneNames = metaNode.GetArray<String>("BoneNames");
    Array<bool> boneAddDescendants = metaNode.GetArray<bool>("AddDescendants");
    Array<bool> boneAddAscendants = metaNode.GetArray<bool>("AddAscendants");
    for (uint i = 0; i < boneNames.Size(); ++i)
    {
        AnimatorLayerMask::BoneEntry boneEntry;
        boneEntry.boneName = (i < boneNames.Size() ? boneNames[i] : "");
        boneEntry.addDescendants =
            (i < boneAddDescendants.Size() ? boneAddDescendants[i] : true);
        boneEntry.addAscendants =
            (i < boneAddAscendants.Size() ? boneAddAscendants[i] : true);
        m_boneEntries.PushBack(boneEntry);
    }
}

void AnimatorLayerMask::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);

    Array<String> boneNames;
    Array<bool> boneAddDescendants;
    Array<bool> boneAddAscendants;
    for (const AnimatorLayerMask::BoneEntry &boneEntry : GetBoneEntries())
    {
        boneNames.PushBack(boneEntry.boneName);
        boneAddDescendants.PushBack(boneEntry.addDescendants);
        boneAddAscendants.PushBack(boneEntry.addAscendants);
    }
    metaNode->SetArray<String>("BoneNames", boneNames);
    metaNode->SetArray<bool>("AddDescendants", boneAddDescendants);
    metaNode->SetArray<bool>("AddAscendants", boneAddAscendants);
}
