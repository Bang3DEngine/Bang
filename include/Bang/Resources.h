#ifndef RESOURCES_H
#define RESOURCES_H

#include <sstream>

#include "Bang/UMap.h"
#include "Bang/USet.h"
#include "Bang/Asset.h"
#include "Bang/Paths.h"
#include "Bang/TypeMap.h"
#include "Bang/Resource.h"
#include "Bang/Serializable.h"
#include "Bang/XMLNodeReader.h"
#include "Bang/ResourceHandle.h"
#include "Bang/ImportFilesManager.h"

NAMESPACE_BANG_BEGIN

FORWARD class Asset;
FORWARD class MeshFactory;
FORWARD class MaterialFactory;
FORWARD class ShaderProgramFactory;

class Resources
{
private:
    struct ResourceEntry : public IToString
    {
        Resource *resource = nullptr;
        uint usageCount = 0; // Number of RH's using this resource entry
        String ToString() const override
        {
            return "RE(" + String(resource) + ", " + String(usageCount) + ")";
        }
    };

public:
    Resources();
    virtual ~Resources();

    static void Add(Resource *res);

    template<class ResourceClass = Resource, class ...Args>
    static RH<ResourceClass> Create(const Args&... args);
    template<class ResourceClass = Resource, class ...Args>
    static RH<ResourceClass> Create(const GUID &guid, const Args&... args);
    template<class ResourceClass = Resource, class ...Args>
    static RH<ResourceClass> CreateEmbeddedResource(
                                        Resource *parentResource,
                                        const GUID::GUIDType embeddedFileGUID,
                                        const Args&... args);
    static void CreateResourceXMLAndImportFile(const Resource *resource,
                                               const Path &exportFilepath);

    template <class ResourceClass = Resource>
    static RH<ResourceClass> Load(const Path &filepath);
    template <class ResourceClass = Resource>
    static RH<ResourceClass> Load(const GUID &guid);

    static RH<Resource> LoadFromExtension(const Path &filepath);

    static void Import(Resource *res);

    template<class ResourceClass = Resource>
    static RH<ResourceClass> Clone(const ResourceClass *src);

    static void SetPermanent(Resource *resource, bool permanent);
    static bool IsPermanent(Resource *resource);
    static void SetPermanent(const Path &resourcePath, bool permanent);
    static bool IsPermanent(const Path &resourcePath);

    static void RegisterResourceUsage(Resource *resource);
    static void UnRegisterResourceUsage(Resource *resource);

    static void Remove(const GUID &guid);

    static bool IsEmbeddedResource(const GUID &guid);
    static bool IsEmbeddedResource(const Path &path);

    template <class ResourceClass = Resource>
    static bool Contains(const GUID &guid);

    template<class ResourceClass = Resource>
    static ResourceClass* GetCached(const GUID &guid);
    template<class ResourceClass = Resource>
    static ResourceClass* GetCached(const Path &path);

    static Path GetResourcePath(const Resource *resource);

    template <class ResourceClass = Resource>
    static Array<ResourceClass*> GetAll();
    static Array<Resource*> GetAllResources();

    MeshFactory *GetMeshFactory() const;
    MaterialFactory *GetMaterialFactory() const;
    ShaderProgramFactory *GetShaderProgramFactory() const;

    virtual void Init();
    virtual Array<Path> GetLookUpPaths() const;

    void Destroy();

    static Resources* GetInstance();

private:
    USet<Path> m_permanentResourcesPaths;
    USet<Resource*> m_permanentResources;
    UMap<GUID, ResourceEntry> m_resourcesCache;

    MeshFactory *m_meshFactory = nullptr;
    MaterialFactory *m_materialFactory = nullptr;
    ShaderProgramFactory *m_shaderProgramFactory = nullptr;

    virtual MeshFactory* CreateMeshFactory() const;

    template <class ResourceClass, class ...Args>
    static ResourceClass *Create_(const Args&... args);
    template <class ResourceClass, class ...Args>
    static ResourceClass *Create_(const GUID &guid, const Args&... args);

    RH<Resource> Load_(std::function<Resource*()> creator,
                       const Path &path);
    RH<Resource> Load_(std::function<Resource*()> creator,
                       const GUID &guid);

    Resource* GetCached_(const GUID &guid) const;
    Resource* GetCached_(const Path &path) const;
    bool Contains_(Resource *resource) const;

    friend class Window;
    friend class GUIDManager;
    friend class IResourceHandle;
};

NAMESPACE_BANG_END

#include "Resources.tcc"

#endif // RESOURCES_H
