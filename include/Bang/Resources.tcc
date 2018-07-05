#ifndef RESOURCES_TCC
#define RESOURCES_TCC

#include "Bang/Resources.h"

#include "Bang/Debug.h"

NAMESPACE_BANG_BEGIN

template <class ResourceClass>
RH<ResourceClass> Resources::Load(const Path &filepath)
{
    return RH<ResourceClass>( SCAST<ResourceClass*>(
        Resources::GetInstance()->Load_(
           []() -> Resource*
           {
               return SCAST<Resource*>( Resources::Create_<ResourceClass>() );
           },
           GetTypeId<ResourceClass>(),
           filepath).Get() ) );
}

template <class ResourceClass>
RH<ResourceClass> Resources::Load(const GUID &guid)
{
    return RH<ResourceClass>( SCAST<ResourceClass*>(
        Resources::GetInstance()->Load_(
           []() -> Resource*
           {
               return SCAST<Resource*>( Resources::Create_<ResourceClass>() );
           },
           GetTypeId<ResourceClass>(),
           guid).Get() ) );
}

template<class ResourceClass, class ...Args>
RH<ResourceClass> Resources::Create(const Args&... args)
{
    return RH<ResourceClass>( Resources::Create_<ResourceClass, Args...>(
                                  args...) );
}
template<class ResourceClass, class ...Args>
RH<ResourceClass> Resources::Create(const GUID &guid, const Args&... args)
{
    return RH<ResourceClass>( Resources::Create_<ResourceClass, Args...>(
                                  guid, args...) );
}

template <class ResourceClass, class ...Args>
ResourceClass *Resources::Create_(const Args&... args)
{
    return Create_<ResourceClass, Args...>(GUIDManager::GetNewGUID(), args...);
}

template <class ResourceClass, class ...Args>
ResourceClass *Resources::Create_(const GUID &guid, const Args&... args)
{
    ResourceClass *res = new ResourceClass(args...);
    res->SetGUID(guid);
    return res;
}

template<class ResourceClass, class ...Args>
RH<ResourceClass> Resources::CreateEmbeddedResource(
                                        Resource *parentResource,
                                        const GUID::GUIDType embeddedFileGUID,
                                        const Args&... args)
{
    GUID newResourceEmbeddedFileGUID;
    GUIDManager::CreateEmbeddedFileGUID(parentResource->GetGUID(),
                                        embeddedFileGUID,
                                        &newResourceEmbeddedFileGUID);
    RH<ResourceClass> embeddedRes =
           Resources::Create<ResourceClass, Args...>(newResourceEmbeddedFileGUID,
                                                     args...);
    embeddedRes.Get()->SetParentResource(parentResource);
    return embeddedRes;
}

template <class ResourceClass>
Array<ResourceClass*> Resources::GetAll()
{
    Array<ResourceClass*> result;
    Array<Resource*> resources = Resources::GetAllResources();
    for (Resource *res : resources)
    {
        ResourceClass *rc = Cast<ResourceClass*>(res);
        if (rc) { result.PushBack(rc); }
    }
    return result;
}

template<class ResourceClass>
bool Resources::Contains(const GUID &guid)
{
    return Contains_(GetTypeId<ResourceClass>(), guid);
}

template<class ResourceClass>
ResourceClass* Resources::GetCached(const GUID &guid)
{
    Resource *res = Resources::GetInstance()->
                               GetCached_(GetTypeId<ResourceClass>(), guid);
    return SCAST<ResourceClass*>(res);
}
template<class ResourceClass>
ResourceClass* Resources::GetCached(const Path &path)
{
    Resource *res = Resources::GetInstance()->
                               GetCached_(GetTypeId<ResourceClass>(), path);
    return SCAST<ResourceClass*>(res);
}

template<class ResourceClass>
RH<ResourceClass> Resources::Clone(const ResourceClass *src)
{
    RH<ResourceClass> rh;
    if (src)
    {
        rh = Resources::Create<ResourceClass>();
        src->CloneInto( rh.Get() );
    }
    return rh;
}

NAMESPACE_BANG_END

#endif // RESOURCES_TCC
