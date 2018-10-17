#include "Bang/Resources.h"

#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Bang/Application.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/CubeMapIBLGenerator.h"
#include "Bang/Debug.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.tcc"
#include "Bang/Extensions.h"
#include "Bang/File.h"
#include "Bang/GUID.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MaterialFactory.h"
#include "Bang/MeshFactory.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/Model.h"
#include "Bang/Paths.h"
#include "Bang/Resources.tcc"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/TextureFactory.h"
#include "Bang/UMap.tcc"

using namespace Bang;

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

void Resources::InitAfterGL()
{
    m_cubeMapIBLGenerator = new CubeMapIBLGenerator();
}

Resources::~Resources()
{
    Destroy();
}

RH<Resource> Resources::LoadFromExtension(const Path &path)
{
    RH<Resource> resRH;
    if(path.HasExtension(Extensions::GetModelExtensions()))
    {
        resRH.Set(Resources::Load<Model>(path).Get());
    }
    return resRH;
}

void Resources::Import(Resource *res)
{
    res->Import_(res->GetResourceFilepath());
}

Array<Resource *> Resources::GetAllResources()
{
    Array<Resource *> result;
    Resources *rs = Resources::GetInstance();
    for(const auto &it : rs->m_resourcesCache)
    {
        result.PushBack(it.second.resource);
    }
    return result;
}

void Resources::CreateResourceMetaAndImportFile(const Resource *resource,
                                                const Path &exportFilepath)
{
    File::Write(exportFilepath, "");
    Path importFilePath = MetaFilesManager::GetMetaFilepath(exportFilepath);
    resource->ExportMetaToFile(importFilePath);
    MetaFilesManager::RegisterMetaFilepath(importFilePath);  // Once created
}

RH<Resource> Resources::Load_(std::function<Resource *()> creator,
                              const Path &filepath)
{
    if(m_beingDestroyed)
    {
        return RH<Resource>(nullptr);
    }

    if(filepath.IsEmpty())
    {
        return RH<Resource>(nullptr);
    }

    if(!Resources::IsEmbeddedResource(filepath) && !filepath.IsFile())
    {
        Debug_Warn("Filepath '" << filepath.GetAbsolute() << "' not found");
        return RH<Resource>(nullptr);
    }

    RH<Resource> resRH;
    Resource *res = GetCached_(filepath);
    resRH.Set(res);  // Register as soon as possible
    if(!res)
    {
        res = creator();

        Path importFilepath = MetaFilesManager::GetMetaFilepath(filepath);
        res->ImportMetaFromFile(importFilepath);  // Get resource GUID
        resRH.Set(res);  // Register as soon as possible

        Resources::Import(res);  // Actually import all
    }

    return resRH;
}

RH<Resource> Resources::Load_(std::function<Resource *()> creator,
                              const GUID &guid)
{
    if(guid.IsEmpty())
    {
        return RH<Resource>(nullptr);
    }

    RH<Resource> resRH(GetCached_(guid));
    if(!resRH)
    {
        if(!Resources::IsEmbeddedResource(guid))
        {
            Path resPath = MetaFilesManager::GetMetaFilepath(guid);
            if(resPath.IsFile())
            {
                resRH.Set(Load_(creator, resPath).Get());
            }
        }
        else
        {
            GUID parentGUID = guid.WithoutEmbeddedResourceGUID();
            Path parentPath = MetaFilesManager::GetFilepath(parentGUID);
            if(parentPath.IsFile())
            {
                if(RH<Resource> parentResRH =
                       Resources::LoadFromExtension(parentPath))
                {
                    resRH.Set(parentResRH.Get()->GetEmbeddedResource(guid));
                }
            }
        }
    }
    return resRH;
}

Resource *Resources::GetCached_(const GUID &guid) const
{
    if(m_resourcesCache.ContainsKey(guid))
    {
        return m_resourcesCache.Get(guid).resource;
    }
    /*
    else
    {
        if (Resources::IsEmbeddedResource(guid))
        {
            GUID parentGUID = guid.WithoutEmbeddedResourceGUID();
            if (Resource *parentRes = GetCached_(parentGUID))
            {
                return
    parentRes->GetEmbeddedResource(guid.GetEmbeddedResourceGUID());
            }
        }
    }*/
    return nullptr;
}

Resource *Resources::GetCached_(const Path &path) const
{
    GUID guid = MetaFilesManager::GetGUID(path);
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

    Resources *rs = Resources::GetInstance();
    ASSERT(rs);
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

void Resources::Remove(const GUID &guid)
{
    Resources *rss = Resources::GetInstance();
    ASSERT(rss);

    auto it = rss->m_resourcesCache.Find(guid);
    ASSERT(it != rss->m_resourcesCache.End());

    const ResourceEntry &resEntry = it->second;
    ASSERT(resEntry.resource != nullptr);
    ASSERT(resEntry.usageCount == 0);

    Resource *resource = resEntry.resource;
    if(resource)
    {
        if(EventEmitter<IEventsDestroy> *destroyable =
               DCAST<EventEmitter<IEventsDestroy> *>(resource))
        {
            destroyable->EventEmitter<IEventsDestroy>::PropagateToListeners(
                &IEventsDestroy::OnDestroyed, destroyable);
        }
    }

    do
    {
        rss->m_resourcesCache.Remove(it);
        it = rss->m_resourcesCache.Find(guid);
    } while(it != rss->m_resourcesCache.End());

    if(resource)
    {
        delete resource;
    }
}

Array<Path> Resources::GetLookUpPaths() const
{
    return {Paths::GetProjectAssetsDir(), Paths::GetEngineAssetsDir()};
}

void Resources::RegisterResourceUsage(Resource *resource)
{
    const GUID &guid = resource->GetGUID();
    ASSERT(!guid.IsEmpty());

    Resources *rs = Resources::GetInstance();
    if(!rs->GetCached_(guid))
    {
        Resources::Add(resource);
    }
    ++rs->m_resourcesCache.Get(guid).usageCount;
}

void Resources::UnRegisterResourceUsage(Resource *res)
{
    if(Resources *rs = Resources::GetInstance())
    {
        const GUID &guid = res->GetGUID();
        ASSERT(!guid.IsEmpty());

        ASSERT(rs->GetCached_(guid));
        uint *resourcesUsage = &(rs->m_resourcesCache.Get(guid).usageCount);
        ASSERT(*resourcesUsage >= 1);
        --(*resourcesUsage);

        if(*resourcesUsage == 0)
        {
            Resources::Remove(guid);
        }
    }
}

Path Resources::GetResourcePath(const Resource *resource)
{
    Path resourcePath;
    if(resource)
    {
        resourcePath = MetaFilesManager::GetFilepath(resource->GetGUID());
    }
    return resourcePath;
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

CubeMapIBLGenerator *Resources::GetCubeMapIBLGenerator() const
{
    return m_cubeMapIBLGenerator;
}

ShaderProgramFactory *Resources::GetShaderProgramFactory() const
{
    return m_shaderProgramFactory;
}

void Resources::Destroy()
{
    m_beingDestroyed = true;

#define B_DESTROY_AND_NULL(p) \
    if(p)                     \
    {                         \
        delete p;             \
        p = nullptr;          \
    }
    B_DESTROY_AND_NULL(m_meshFactory);
    B_DESTROY_AND_NULL(m_textureFactory);
    B_DESTROY_AND_NULL(m_materialFactory);
    B_DESTROY_AND_NULL(m_cubeMapIBLGenerator);
    B_DESTROY_AND_NULL(m_shaderProgramFactory);
#undef B_DESTROY_AND_NULL
}

Resources *Resources::GetInstance()
{
    Application *app = Application::GetInstance();
    return app ? app->GetResources() : nullptr;
}

MeshFactory *Resources::CreateMeshFactory() const
{
    return new MeshFactory();
}

TextureFactory *Resources::CreateTextureFactory() const
{
    return new TextureFactory();
}
