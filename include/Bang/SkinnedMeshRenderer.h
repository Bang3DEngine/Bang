#ifndef SKINNEDMESHRENDERER_H
#define SKINNEDMESHRENDERER_H

#include <functional>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/DPtr.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsObjectGatherer.h"
#include "Bang/Map.h"
#include "Bang/MeshRenderer.h"
#include "Bang/MetaNode.h"
#include "Bang/ObjectGatherer.h"
#include "Bang/Set.h"
#include "Bang/String.h"
#include "Bang/Transformation.h"
#include "Bang/UMap.h"

namespace Bang
{
template <class>
class IEventsObjectGatherer;
class GameObject;
class ICloneable;
class Model;
class ShaderProgram;
template <class ObjectType, bool RECURSIVE>
class ObjectGatherer;

class SkinnedMeshRenderer
    : public MeshRenderer,
      public EventListener<IEventsObjectGatherer<GameObject>>
{
    COMPONENT(SkinnedMeshRenderer)

public:
    SkinnedMeshRenderer();
    virtual ~SkinnedMeshRenderer() override;

    // MeshRenderer
    void OnRender() override;
    virtual void Bind() override;
    virtual void SetUniformsOnBind(ShaderProgram *sp) override;
    Matrix4 GetModelMatrixUniform() const override;

    void SetRootBoneGameObjectName(const String &rootBoneGameObjectName);

    Model *GetActiveModel() const;
    GameObject *GetRootBoneGameObject() const;
    const String &GetRootBoneGameObjectName() const;
    GameObject *GetBoneGameObject(const String &boneName) const;
    Transformation GetBoneSpaceToRootSpaceTransformation(
        const String &boneName) const;
    Transformation GetBoneTransformationFor(
        GameObject *boneGameObject,
        const Transformation &transform,
        UMap<GameObject *, Transformation> *boneTransformInRootSpaceCache)
        const;
    const Transformation &GetInitialTransformationFor(
        const String &boneName) const;
    const Map<String, Transformation> &GetInitialTransformations() const;

    void SetBoneUniforms(ShaderProgram *sp);
    void UpdateBonesMatricesFromTransformMatrices();
    void UpdateTransformMatricesFromInitialBonePosition();
    void SetSkinnedMeshRendererCurrentBoneMatrices(
        const Map<String, Matrix4> &boneMatrices);
    void SetSkinnedMeshRendererCurrentBoneMatrices(
        const Array<Matrix4> &boneMatrices);

    void RetrieveBonesInitialTransformationFromCurrentHierarchy();

    // ObjectGatherer
    virtual void OnObjectGathered(GameObject *go) override;
    virtual void OnObjectUnGathered(GameObject *previousGameObject,
                                    GameObject *go) override;

    // ICloneable
    void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    void Reflect() override;

private:
    Map<String, Transformation> m_initialTransformations;
    Map<String, Transformation> m_boneSpaceToRootSpaceTransformations;
    Array<Matrix4> m_bonesTransformsMatricesArrayUniform;

    ObjectGatherer<GameObject, true> *m_gameObjectGatherer = nullptr;

    String m_rootBoneGameObjectName = "";
    mutable DPtr<GameObject> p_rootBoneGameObject = nullptr;
    mutable Map<String, GameObject *> m_boneNameToBoneGameObject;
};
}

#endif  // SKINNEDMESHRENDERER_H
