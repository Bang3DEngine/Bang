#include "Bang/ResourceHandle.h"

#include "Bang/Resources.h"

NAMESPACE_BANG_BEGIN

void OnResourceSet(Resource *resource)
{
    Resources::RegisterResourceUsage(resource);
}

void OnResourceUnSet(Resource *resource)
{
    Resources::UnRegisterResourceUsage(resource);
}

Bang::ResourceHandleBase::operator bool() const
{
    return (Get() != nullptr);
}

bool ResourceHandleBase::operator==(const ResourceHandleBase &rhs) const
{
    return Get() == rhs.Get();
}

ResourceHandleBase::~ResourceHandleBase()
{
    Set(nullptr);
}

bool ResourceHandleBase::operator==(const Resource *res) const
{
    return Get() == res;
}

bool ResourceHandleBase::operator!=(const ResourceHandleBase &rhs) const
{
    return !(*this == rhs);
}

bool ResourceHandleBase::operator<(const ResourceHandleBase &rhs) const
{
    return Get() < rhs.Get();
}

Resource *ResourceHandleBase::Get() const
{
    return p_resource;
}

void ResourceHandleBase::Set(Resource *resource)
{
    if (Get() != resource)
    {
        if (Get())
        {
            // Must be done in two steps, so that we avoid unset loops
            Resource *prevResource = p_resource;
            p_resource = nullptr;
            OnResourceUnSet(prevResource);
        }

        p_resource = resource;
        if (Get())
        {
            OnResourceSet(Get());
        }
    }
}

NAMESPACE_BANG_END
