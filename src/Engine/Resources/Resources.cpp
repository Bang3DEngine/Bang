#include "Bang/Resources.h"

#include "Bang/File.h"
#include "Bang/Asset.h"
#include "Bang/Debug.h"
#include "Bang/Paths.h"
#include "Bang/Model.h"
#include "Bang/Object.h"
#include "Bang/Random.h"
#include "Bang/Extensions.h"
#include "Bang/Application.h"
#include "Bang/MeshFactory.h"
#include "Bang/TextureFactory.h"
#include "Bang/MaterialFactory.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

Resources::Resources()
{
}

void Resources::Init()
{
    m_meshFactory = CreateMeshFactory();
    m_textureFactory = CreateTextureFactory();
    m_materialFactory = new MaterialFactory();
    m_shaderProgramFactory = new ShaderProgramFactory();
}

Resources::~Resources()
{
    ASSERT_MSG(m_meshFactory == nullptr,          "Call Destroy()");
    ASSERT_MSG(m_textureFactory == nullptr,       "Call Destroy()");
    ASSERT_MSG(m_materialFactory == nullptr,      "Call Destroy()");
    ASSERT_MSG(m_shaderProgramFactory == nullptr, "Call Destroy()");
}

RH<Resource> Resources::LoadFromExtension(const Path &path)
{
    RH<Resource> resRH;
    if (path.HasExtension(Extensions::GetModelExtensions()))
    {
        resRH.Set( Resources::Load<Model>(path).Get() );
    }
    return resRH;
}

void Resources::Import(Resource *res)
{
    res->Import_( res->GetResourceFilepath() );
}

Array<Resource*> Resources::GetAllResources()
{
    Array<Resource*> result;
    Resources *rs = Resources::GetInstance();
    for (const auto& it : rs->m_resourcesCache)
    {
        result.PushBack(it.second.resource);
    }
    return result;
}

void Resources::CreateResourceXMLAndImportFile(const Resource *resource,
                                               const Path &exportFilepath)
{
    File::Write(exportFilepath, "");
    Path importFilePath = ImportFilesManager::GetImportFilepath(exportFilepath);
    resource->ExportXMLToFile(importFilePath);
    ImportFilesManager::RegisterImportFilepath(importFilePath); // Once created
}

RH<Resource> Resources::Load_(std::function<Resource*()> creator,
                              const Path &filepath)
{
    if (!Resources::IsEmbeddedResource(filepath) && !filepath.IsFile())
    {
        Debug_Warn("Filepath '" << filepath.GetAbsolute() << "' not found");
        return RH<Resource>(nullptr);
    }

    RH<Resource> resRH;
    Resource *res = GetCached_(filepath);
    resRH.Set(res); // Register as soon as possible
    if (!res)
    {
        res = creator();

        Path importFilepath = ImportFilesManager::GetImportFilepath(filepath);
        res->ImportXMLFromFile(importFilepath); // Get resource GUID
        resRH.Set(res); // Register as soon as possible

        Resources::Import(res); // Actually import all
    }

    return resRH;
}

RH<Resource> Resources::Load_(std::function<Resource*()> creator, const GUID &guid)
{
    if (guid.IsEmpty()) { return RH<Resource>(nullptr); }

    RH<Resource> resRH( GetCached_(guid) );
    if (!resRH)
    {
        if (!Resources::IsEmbeddedResource(guid))
        {
            Path resPath = ImportFilesManager::GetImportFilepath(guid);
            if (resPath.IsFile())
            {
                resRH.Set( Load_(creator, resPath).Get() );
            }
        }
        else
        {
            GUID parentGUID = guid.WithoutEmbeddedResourceGUID();
            Path parentPath = ImportFilesManager::GetFilepath(parentGUID);
            if (parentPath.IsFile())
            {
                if (RH<Resource> parentResRH =
                                    Resources::LoadFromExtension(parentPath))
                {
                    resRH.Set( parentResRH.Get()->GetEmbeddedResource(guid) );
                }
            }
        }
    }
    return resRH;
}

Resource* Resources::GetCached_(const GUID &guid) const
{
    if (m_resourcesCache.ContainsKey(guid))
    {
        return m_resourcesCache.Get(guid).resource;
    }
    else
    {
        if (Resources::IsEmbeddedResource(guid))
        {
            GUID parentGUID = guid.WithoutEmbeddedResourceGUID();
            if (Resource *parentRes = GetCached_(parentGUID))
            {
                return parentRes->GetEmbeddedResource(guid.GetEmbeddedResourceGUID());
            }
        }
    }

    return nullptr;
}

Resource* Resources::GetCached_(const Path &path) const
{
    GUID guid = ImportFilesManager::GetGUID(path);
    return GetCached_(guid);
}

bool Resources::Contains_(Resource *resource) const
{
    return resource && GetCached_(resource->GetGUID());
}


void Resources::Add(Resource *res)
{
    const GUID &guid = res->GetGUID();
    ASSERT(res != nullptr);
    ASSERT(!guid.IsEmpty());

    Resources *rs = Resources::GetInstance(); ASSERT(rs);
    ASSERT(!rs->GetCached_(guid));

    ResourceEntry resourceEntry;
    resourceEntry.resource = res;
    resourceEntry.usageCount = 0;
    rs->m_resourcesCache.Add(guid, resourceEntry);
}

bool Resources::IsEmbeddedResource(const GUID &guid)
{
    return (guid.GetEmbeddedResourceGUID() != GUID::EmptyGUID);
}

bool Resources::IsEmbeddedResource(const Path &resourcePath)
{
    return resourcePath.GetDirectory().IsFile();
}

void Resources::SetPermanent(Resource *resource, bool permanent)
{
    if (resource)
    {
        Resources *rs = Resources::GetInstance(); ASSERT(rs);
        if (permanent) { rs->m_permanentResources.Add(resource); }
        else { rs->m_permanentResources.Remove(resource); }
    }
}

bool Resources::IsPermanent(Resource *resource)
{
    if (!resource) { return false; }

    Resources *rs = Resources::GetInstance(); ASSERT(rs);
    return rs->m_permanentResources.Contains(resource);
}

void Resources::SetPermanent(const Path &resourcePath, bool permanent)
{
    if (!resourcePath.IsEmpty())
    {
        Resources *rs = Resources::GetInstance(); ASSERT(rs);
        if (permanent) { rs->m_permanentResourcesPaths.Add(resourcePath); }
        else { rs->m_permanentResourcesPaths.Remove(resourcePath); }
    }
}

bool Resources::IsPermanent(const Path &resourcePath)
{
    if (resourcePath.IsEmpty()) { return false; }

    Resources *rs = Resources::GetInstance(); ASSERT(rs);
    return rs->m_permanentResourcesPaths.Contains(resourcePath);
}

void Resources::Remove(const GUID &guid)
{
    Resources *rss = Resources::GetInstance(); ASSERT(rss);

    auto it = rss->m_resourcesCache.Find(guid);
    ASSERT(it != rss->m_resourcesCache.End());

    const ResourceEntry &resEntry = it->second;
    ASSERT(resEntry.resource != nullptr);
    ASSERT(resEntry.usageCount == 0);

    if (Resource *resource = resEntry.resource)
    {
        if (EventEmitter<IEventsDestroy> *destroyable =
                             DCAST< EventEmitter<IEventsDestroy>* >(resource))
        {
            destroyable->EventEmitter<IEventsDestroy>::PropagateToListeners(
                                    &IEventsDestroy::OnDestroyed, destroyable);
        }

        delete resource;
    }

    rss->m_resourcesCache.Remove(it);
}

Array<Path> Resources::GetLookUpPaths() const
{
    return {Paths::GetProjectAssetsDir(), Paths::GetEngineAssetsDir()};
}

void Resources::RegisterResourceUsage(Resource *resource)
{
    Resources *rs = Resources::GetInstance();
    const GUID &guid = resource->GetGUID();
    ASSERT(!guid.IsEmpty());

    if (!rs->GetCached_(guid))
    {
        Resources::Add(resource);
    }
    ++rs->m_resourcesCache.Get(guid).usageCount;
}

void Resources::UnRegisterResourceUsage(Resource *res)
{
    Resources *rs = Resources::GetInstance();
    const GUID &guid = res->GetGUID();
    ASSERT(!guid.IsEmpty());

    if (rs)
    {
        ASSERT(rs->GetCached_(guid));
        uint *resourcesUsage = &(rs->m_resourcesCache.Get(guid).usageCount);
        ASSERT(*resourcesUsage >= 1);
        --(*resourcesUsage);

        if (*resourcesUsage == 0)
        {
            const Path resPath = Resources::GetResourcePath(res);
            if (!Resources::IsPermanent(res) && !Resources::IsPermanent(resPath) )
            {
                Resources::Remove(guid);
            }
        }
    }
}

Path Resources::GetResourcePath(const Resource *resource)
{
    if (!resource) { return Path::Empty; }
    return ImportFilesManager::GetFilepath(resource->GetGUID());
}

MeshFactory *Resources::GetMeshFactory() const
{
    return m_meshFactory;
}

TextureFactory *Resources::GetTextureFactory() const
{
    return m_textureFactory;
}

MaterialFactory *Resources::GetMaterialFactory() const
{
    return m_materialFactory;
}

ShaderProgramFactory *Resources::GetShaderProgramFactory() const
{
    return m_shaderProgramFactory;
}

void Resources::Destroy()
{
    delete m_meshFactory;          m_meshFactory          = nullptr;
    delete m_textureFactory;       m_textureFactory       = nullptr;
    delete m_materialFactory;      m_materialFactory      = nullptr;
    delete m_shaderProgramFactory; m_shaderProgramFactory = nullptr;
}

Resources *Resources::GetInstance()
{
    Application *app = Application::GetInstance();
    return  app ? app->GetResources() : nullptr;
}

MeshFactory *Resources::CreateMeshFactory() const
{
    return new MeshFactory();
}

TextureFactory *Resources::CreateTextureFactory() const
{
    return new TextureFactory();
}
