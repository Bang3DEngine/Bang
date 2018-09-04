#ifndef SKINNEDMESHRENDERER_H
#define SKINNEDMESHRENDERER_H

#include "Bang/Set.h"
#include "Bang/Map.h"
#include "Bang/IEventsName.h"
#include "Bang/MeshRenderer.h"

NAMESPACE_BANG_BEGIN

FORWARD class Model;

class SkinnedMeshRenderer : public MeshRenderer,
                            public EventListener<IEventsName>
{
    COMPONENT(SkinnedMeshRenderer);

public:
	SkinnedMeshRenderer();
	virtual ~SkinnedMeshRenderer();

    // MeshRenderer
    void OnRender() override;
    Matrix4 GetModelMatrixUniform() const override;

    void SetRootBoneGameObjectName(const String &rootBoneGameObjectName);

    Model *GetActiveModel() const;
    GameObject *GetRootBoneGameObject() const;
    const String& GetRootBoneGameObjectName() const;
    GameObject *GetBoneGameObject(const String &boneName) const;
    Matrix4 GetBoneSpaceToRootSpaceMatrix(const String &boneName) const;
    Matrix4 GetBoneTransformMatrixFor(GameObject *boneGameObject,
                                      const Matrix4 &transform) const;
    Matrix4 GetInitialTransformMatrixFor(const String &boneName) const;
    const Set<String> &GetBonesNames() const;

    void UpdateBonesMatricesFromTransformMatrices();
    void SetSkinnedMeshRendererCurrentBoneMatrices(
                                    const Map<String, Matrix4> &boneMatrices);
    void SetSkinnedMeshRendererCurrentBoneMatrices(
                                    const Array<Matrix4> &boneMatrices);

    void RetrieveBonesBindPoseFromCurrentHierarchy();

    // IEventsName
    virtual void OnNameChanged(GameObject *go,
                               const String &oldName,
                               const String &newName) override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Set<String> m_bonesNames;
    String m_rootBoneGameObjectName = "";
    Map<String, Matrix4> m_boneSpaceToRootSpaceMatrices;
    Map<String, Matrix4> m_initialTransforms;
    bool m_needsToUpdateToDefaultMatrices = true;

    Array<Matrix4> m_bonesTransformsMatricesArrayUniform;

};

NAMESPACE_BANG_END

#endif // SKINNEDMESHRENDERER_H

