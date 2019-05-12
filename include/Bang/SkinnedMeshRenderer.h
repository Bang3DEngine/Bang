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
#include "BangMath/Transformation.h"
#include "Bang/UMap.h"

namespace Bang
{
template <class>
class IEventsObjectGatherer;
class GameObject;
class Serializable;
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
    void OnUpdate() override;
    void OnRender() override;
    virtual void Bind() override;
    virtual void SetUniformsOnBind(ShaderProgram *sp) override;
    Matrix4 GetModelMatrixUniform() const override;

    void SetRootBoneGameObjectName(const String &rootBoneGameObjectName);

    Model *GetActiveModel() const;
    Array<GameObject *> GetAllBoneGameObjects() const;
    GameObject *GetRootBoneGameObject() const;
    const String &GetRootBoneGameObjectName() const;
    GameObject *GetBoneGameObject(const String &boneName) const;
    Transformation GetRootSpaceToBoneSpaceTransformation(
        const String &boneName) const;
    Transformation GetBoneTransformationFor(
        GameObject *boneGameObject,
        const Transformation &transform,
        UMap<GameObject *, Transformation> *boneTransformInRootSpaceCache)
        const;

    void SetBoneUniforms(ShaderProgram *sp);
    void UpdateBonesMatricesFromTransformMatrices();
    void SetSkinnedMeshRendererCurrentBoneMatrices(
        const Map<String, Matrix4> &boneMatrices);
    void SetSkinnedMeshRendererCurrentBoneMatrices(
        const Array<Matrix4> &boneMatrices);

    void ResetBoneTransformation(GameObject *boneGo);
    void ResetBoneTransformations();

    // ObjectGatherer
    virtual void OnObjectGathered(GameObject *go) override;
    virtual void OnObjectUnGathered(GameObject *previousGameObject,
                                    GameObject *go) override;

    // Serializable
    void Reflect() override;

protected:
    void OnMeshLoaded(Mesh *mesh) override;

private:
    Array<Matrix4> m_bonesTransformsMatricesArrayUniform;

    ObjectGatherer<GameObject, true> *m_gameObjectGatherer = nullptr;

    String m_rootBoneGameObjectName = "";
    mutable USet<String> m_boneNames;
    mutable DPtr<GameObject> p_rootBoneGameObject = nullptr;
    mutable Map<String, GameObject *> m_boneNameToBoneGameObject;
};
}

#endif  // SKINNEDMESHRENDERER_H
