#ifndef ANIMATORLAYERMASK_H
#define ANIMATORLAYERMASK_H

#include "Bang/Bang.h"
#include "Bang/Resource.h"
#include "Bang/Set.h"
#include "Bang/String.h"

namespace Bang
{
class Animator;
class AnimatorLayerMask : public Resource
{
    RESOURCE(AnimatorLayerMask)

public:
    AnimatorLayerMask();
    virtual ~AnimatorLayerMask();

    void AddBone(const String &boneName);
    void RemoveBone(uint i);
    void RemoveBone(const String &boneName);
    void ClearBones();

    const Array<String> &GetBoneMaskNames() const;
    Set<String> GetBoneMaskNamesSet(Animator *animator) const;

    const Array<String> &GetBoneNames() const;

    // Resource
    virtual void Import(const Path &resourceFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Array<String> m_boneNames;
};
}

#endif  // ANIMATORLAYERMASK_H
