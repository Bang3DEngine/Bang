#ifndef ANIMATORLAYERMASK_H
#define ANIMATORLAYERMASK_H

#include "Bang/Asset.h"
#include "Bang/Bang.h"
#include "Bang/Set.h"
#include "Bang/String.h"

namespace Bang
{
class Animator;
class AnimatorLayerMask : public Asset
{
    ASSET(AnimatorLayerMask)

public:
    struct BoneEntry
    {
        String boneName = "";
        bool addAscendants = true;
        bool addDescendants = true;
        bool operator==(const BoneEntry &rhs) const
        {
            return boneName == rhs.boneName &&
                   addAscendants == rhs.addAscendants &&
                   addDescendants == rhs.addDescendants;
        }
        bool operator!=(const BoneEntry &rhs) const
        {
            return !((*this) == rhs);
        }
    };

    AnimatorLayerMask();
    virtual ~AnimatorLayerMask() override;

    void AddBoneEntry(const AnimatorLayerMask::BoneEntry &boneName);
    void RemoveBoneEntry(uint i);
    void RemoveBoneEntry(const String &boneName);
    void ClearBoneEntries();

    const Array<AnimatorLayerMask::BoneEntry> &GetBoneEntries() const;
    Set<String> GetBoneMaskNamesSet(Animator *animator) const;

    // Asset
    virtual void Import(const Path &assetFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Array<BoneEntry> m_boneEntries;
};
}

#endif  // ANIMATORLAYERMASK_H
