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

    void AddMesh(Mesh *mesh,
                 Material *material,
                 const String &meshName,
                 const String &materialName);

    RH<Mesh> GetMeshByName(const String &meshName);
    RH<Material> GetMaterialByName(const String &materialName);

    const Array< RH<Mesh> >& GetMeshes() const;
    const Array< RH<Material> >& GetMaterials() const;
    const Array< RH<Animation> >& GetAnimations() const;
    const Array<String>& GetMeshesNames() const;
    const Array<String>& GetMaterialsNames() const;
    const Array<String>& GetAnimationsNames() const;

    // Resource
    GUID::GUIDType GetNextEmbeddedFileGUID() const override;
    Resource* GetEmbeddedResource(const String &embeddedResourceName) const override;
    Resource* GetEmbeddedResource(GUID::GUIDType embeddedFileGUID) const override;
    String GetEmbeddedFileResourceName(GUID::GUIDType embeddedFileGUID) const override;
    void Import(const Path &modelFilepath) override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    ModelIOScene m_modelScene;

    std::pair<Resource*, String>
        GetEmbeddedFileResourceAndName(GUID::GUIDType embeddedFileGUID) const;

    static String GetNewName(const String &originalName,
                             const Array<String> &existingNames);
};

NAMESPACE_BANG_END

#endif // MODEL_H

