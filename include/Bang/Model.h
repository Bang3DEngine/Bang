#ifndef MODEL_H
#define MODEL_H

#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/Map.h"
#include "Bang/Mesh.h"
#include "Bang/ModelIO.h"
#include "Bang/String.h"

namespace Bang
{
template <class>
class Array;
template <class>
class AssetHandle;
class Animation;
class GameObject;
class Material;
class MetaNode;
class Path;

class Model : public Asset
{
    ASSET(Model)

public:
    Model();
    virtual ~Model() override;

    GameObject *CreateGameObjectFromModel() const;

    const String &GetRootGameObjectName() const;
    const Array<AH<Mesh>> &GetMeshes() const;
    const Array<AH<Material>> &GetMaterials() const;
    const Array<AH<Animation>> &GetAnimations() const;
    const Array<String> &GetMeshesNames() const;
    const Array<String> &GetMaterialsNames() const;
    const Array<String> &GetAnimationsNames() const;
    const Map<String, Mesh::Bone> &GetAllBones() const;

    // Asset
    void Import(const Path &modelFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    ModelIOScene m_modelScene;
};
}

#endif  // MODEL_H
