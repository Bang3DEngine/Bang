#ifndef MODEL_H
#define MODEL_H

#include "Bang/Asset.h"
#include "Bang/ModelIO.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD struct ModelIONode;

class Model : public Asset
{
    ASSET(Model)

public:
	Model();
	virtual ~Model();

    GameObject *CreateGameObjectFromModel() const;

    const String& GetRootGameObjectName() const;
    const Array< RH<Mesh> >& GetMeshes() const;
    const Array< RH<Material> >& GetMaterials() const;
    const Array< RH<Animation> >& GetAnimations() const;
    const Array<String>& GetMeshesNames() const;
    const Array<String>& GetMaterialsNames() const;
    const Array<String>& GetAnimationsNames() const;
    const Map<String, Mesh::Bone>& GetAllBones() const;

    // Resource
    void Import(const Path &modelFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    ModelIOScene m_modelScene;
};

NAMESPACE_BANG_END

#endif // MODEL_H

