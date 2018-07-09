#ifndef SKINNEDMESHRENDERER_H
#define SKINNEDMESHRENDERER_H

#include "Bang/Map.h"
#include "Bang/MeshRenderer.h"

NAMESPACE_BANG_BEGIN

class SkinnedMeshRenderer : public MeshRenderer
{
    COMPONENT(SkinnedMeshRenderer);

public:
	SkinnedMeshRenderer();
	virtual ~SkinnedMeshRenderer();

    // MeshRenderer
    void OnRender() override;

    void SetRootBoneGameObject(GameObject* rootBoneGameObject);
    void SetBoneGameObject(const String &boneName, GameObject *gameObject);

    GameObject* GetRootBoneGameObject() const;
    GameObject *GetBoneGameObject(const String &boneName) const;
    const Map<String, GameObject*>& GetBoneNameToGameObject() const;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    static const String XMLBoneGameObjectPrefix;

    GameObject *p_rootBoneGameObject = nullptr;
    Map<String, GameObject*> m_boneNameToGameObject;
    bool m_hadAnimatorBefore = true;
};

NAMESPACE_BANG_END

#endif // SKINNEDMESHRENDERER_H

