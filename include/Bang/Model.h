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
    const Array<String>& GetMeshesNames() const;
    const Array<String>& GetMaterialsNames() const;

    virtual GUID::GUIDType GetNextEmbeddedFileGUID() const override;
    virtual Resource* GetEmbeddedResource(
                            GUID::GUIDType embeddedFileGUID) const override;
    virtual String GetEmbeddedFileResourceName(
                            GUID::GUIDType embeddedFileGUID) const override;

    // Resource
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

