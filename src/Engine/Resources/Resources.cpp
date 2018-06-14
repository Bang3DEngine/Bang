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
#include "Bang/MaterialFactory.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

Resources::Resources()
{
    m_meshFactory = CreateMeshFactory();
    m_materialFactory = new MaterialFactory();
    m_shaderProgramFactory = new ShaderProgramFactory();
}

Resources::~Resources()
{
    ASSERT_MSG(m_meshFactory == nullptr,          "Call Destroy()");
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
    for (auto& itMap : rs->m_resourcesCache)
    {
        for (const auto& it : itMap.second) { result.PushBack(it.second.resource); }
    }
    return result;
}

void Resources::PrintAll()
{
    Resources *rs = Resources::GetInstance();
    for (const auto &typePair : rs->m_resourcesCache)
    {
        Debug_Log(typePair.first);
        for (const auto &guidResPair : typePair.second)
        {
            const GUID &guid     = guidResPair.first;
            const Resource *res  = guidResPair.second.resource;
            const int usageCount = guidResPair.second.usageCount;
            Debug_Log("    - " << Resources::GetResourcePath(res)
                               << ", usages: (" << usageCount << ")"
                               << ", GUID: (" << guid << ")");
        }
    }
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
                              const String &resourceClassTypeId,
                              const Path &filepath)
{
    if (!Resources::IsEmbeddedResource(filepath) && !filepath.IsFile())
    {
        Debug_Warn("Filepath '" << filepath.GetAbsolute() << "' not found");
    }

    Resource *res = GetCached_(resourceClassTypeId, filepath);
    if (!res)
    {
        res = creator();
        Path importFilepath = ImportFilesManager::GetImportFilepath(filepath);
        res->ImportXMLFromFile(importFilepath); // Get resource GUID
        Resources::Import(res); // Actually import all
    }
    return RH<Resource>(res);
}

RH<Resource> Resources::Load_(std::function<Resource*()> creator,
                              const String &resourceClassTypeId,
                              const GUID &guid)
{
    if (guid.IsEmpty()) { return RH<Resource>(nullptr); }

    RH<Resource> resRH( GetCached_(resourceClassTypeId, guid) );
    if (!resRH)
    {
        if (!Resources::IsEmbeddedResource(guid))
        {
            Path resPath = ImportFilesManager::GetImportFilepath(guid);
            if (resPath.IsFile())
            {
                resRH.Set( Load_(creator, resourceClassTypeId, resPath).Get() );
            }
        }
        else
        {
            GUID parentGUID = guid.WithoutEmbeddedFileGUID();
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

Array<Resource*> Resources::GetAllCached(const Path &path)
{
    GUID guid = ImportFilesManager::GetGUIDFromFilepath(path);
    return GetAllCached(guid);
}

Array<Resource*> Resources::GetAllCached(const GUID &guid)
{
    Resources *rs = Resources::GetInstance();

    Array<Resource*> foundResources;
    for (const auto &map : rs->m_resourcesCache)
    {
        if ( map.second.ContainsKey(guid) )
        {
            foundResources.PushBack(map.second.Get(guid).resource);
        }
    }
    return foundResources;
}

Resource* Resources::GetCached_(const TypeId &resourceClassTypeId,
                                const GUID &guid) const
{
    if (m_resourcesCache.ContainsKey(resourceClassTypeId) &&
        m_resourcesCache.Get(resourceClassTypeId).ContainsKey(guid))
    {
        return m_resourcesCache.Get(resourceClassTypeId).Get(guid).resource;
    }
    else
    {
        if (Resources::IsEmbeddedResource(guid))
        {
            GUID parentGUID = guid.WithoutEmbeddedFileGUID();
            if (Resource *parentRes = GetCached_(resourceClassTypeId, parentGUID))
            {
                return parentRes->GetEmbeddedResource(guid.GetEmbeddedFileGUID());
            }
        }
    }

    return nullptr;
}

Resource* Resources::GetCached_(const TypeId &resourceClassTypeId,
                                const Path &path) const
{
    GUID guid = ImportFilesManager::GetGUIDFromFilepath(path);
    return GetCached_(resourceClassTypeId, guid);
}

bool Resources::Contains_(Resource *resource) const
{
    if (!resource)
    {
        return false;
    }
    return GetCached_(GetTypeId(resource), resource->GetGUID());
}


void Resources::Add(const TypeId &resourceClassTypeId, Resource *res)
{
    const GUID &guid = res->GetGUID();
    ASSERT(res != nullptr);
    ASSERT(!guid.IsEmpty());
    ASSERT(!resourceClassTypeId.IsEmpty());

    Resources *rs = Resources::GetInstance(); ASSERT(rs);
    ASSERT(!rs->GetCached_(resourceClassTypeId, guid));

    if (!rs->m_resourcesCache.ContainsKey(resourceClassTypeId))
    {
        rs->m_resourcesCache.Add(resourceClassTypeId);
    }

    ResourceEntry resourceEntry;
    resourceEntry.resource = res;
    resourceEntry.usageCount = 0;
    ASSERT(!rs->m_resourcesCache.Get(resourceClassTypeId).ContainsKey(guid));
    rs->m_resourcesCache.Get(resourceClassTypeId).Add(guid, resourceEntry);
}

bool Resources::IsEmbeddedResource(const GUID &guid)
{
    return (guid.GetEmbeddedFileGUID() != GUID::EmptyGUID);
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

void Resources::Remove(const TypeId &resTypeId, const GUID &guid)
{
    Resources *rs = Resources::GetInstance(); ASSERT(rs);
    ASSERT(rs->m_resourcesCache.ContainsKey(resTypeId));

    auto &map = rs->m_resourcesCache.Get(resTypeId);
    ASSERT(map.ContainsKey(guid));

    auto it = map.Find(guid);
    ResourceEntry resEntry = it->second;
    map.Remove(it);

    ASSERT(resEntry.resource != nullptr);
    ASSERT(resEntry.usageCount == 0);

    bool totallyUnused = true;
    for (const auto &pair : rs->m_resourcesCache)
    {
        if (pair.second.ContainsKey(resEntry.resource->GetGUID()))
        {
            totallyUnused = false;
            break;
        }
    }

    if (totallyUnused)
    {
        Destroy(resEntry.resource);
    }
}

Array<Path> Resources::GetLookUpPaths() const
{
    return {Paths::GetProjectAssetsDir(), Paths::GetEngineAssetsDir()};
}

void Resources::RegisterResourceUsage(const TypeId &resourceClassTypeId,
                                      Resource *resource)
{
    Resources *rs = Resources::GetInstance();
    const GUID &guid = resource->GetGUID();
    ASSERT(!guid.IsEmpty());
    ASSERT(!resourceClassTypeId.IsEmpty());

    if (!rs->GetCached_(resourceClassTypeId, guid))
    {
        Resources::Add(resourceClassTypeId, resource);
    }
    ++rs->m_resourcesCache.Get(resourceClassTypeId).Get(guid).usageCount;
}

void Resources::UnRegisterResourceUsage(const TypeId &resourceClassTypeId,
                                        Resource *resource)
{
    Resources *rs = Resources::GetInstance();
    const GUID &guid = resource->GetGUID();
    ASSERT(!guid.IsEmpty());
    ASSERT(!resourceClassTypeId.IsEmpty());

    if (rs)
    {
        ASSERT(rs->GetCached_(resourceClassTypeId, guid));
        uint *resourcesUsage = &(rs->m_resourcesCache.Get(resourceClassTypeId).
                                 Get(guid).usageCount);
        ASSERT(*resourcesUsage >= 1);
        --(*resourcesUsage);

        if (*resourcesUsage == 0)
        {
            const Path resourcePath = Resources::GetResourcePath(resource);
            if (!Resources::IsPermanent(resource) &&
                !Resources::IsPermanent(resourcePath) )
            {
                Resources::Remove(resourceClassTypeId, guid);
            }
        }
    }
}

void Resources::Destroy(Resource *resource)
{
    if (!resource) { return; }

    if (EventEmitter<IEventsDestroy> *destroyable =
                            DCAST< EventEmitter<IEventsDestroy>* >(resource))
    {
        destroyable->EventEmitter<IEventsDestroy>::PropagateToListeners(
                                    &IEventsDestroy::OnDestroyed, destroyable);
    }
    delete resource;
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

MaterialFactory *Resources::GetMaterialFactory() const
{
    return m_materialFactory;
}

ShaderProgramFactory *Resources::GetShaderProgramFactory() const
{
    return m_shaderProgramFactory;
}

void Resources::Init()
{
}

void Resources::Destroy()
{
    delete m_meshFactory;          m_meshFactory          = nullptr;
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
