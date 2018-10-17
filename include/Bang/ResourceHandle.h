#ifndef RESOURCEHANDLE_H
#define RESOURCEHANDLE_H

#include "Bang/BangDefines.h"

NAMESPACE_BANG_BEGIN

FORWARD class Resource;

void OnResourceSet(Resource *resource);
void OnResourceUnSet(Resource *resource);

class ResourceHandleBase
{
protected:
    ResourceHandleBase() = default;
    virtual ~ResourceHandleBase();

    bool operator==(const Resource *res) const;
    bool operator==(const ResourceHandleBase &rhs) const;
    bool operator!=(const ResourceHandleBase &rhs) const;
    bool operator< (const ResourceHandleBase &rhs) const;
    operator bool() const;

    Resource* Get() const;

    void Set(Resource* resource);

private:
    Resource *p_resource = nullptr;
};

template <class ResourceClass>
class ResourceHandle : public ResourceHandleBase
{
public:
    ResourceHandle() = default;

    explicit ResourceHandle(ResourceClass *res) : ResourceHandleBase()
    {
        Set(res);
    }
    ResourceHandle(ResourceHandle<ResourceClass> &&rhs)
    {
       *this = rhs;
    }
    ResourceHandle(const ResourceHandle<ResourceClass> &rhs)
    {
        *this = rhs;
    }

    void Set(ResourceClass* resource)
    {
        ResourceHandleBase::Set( SCAST<Resource*>(resource) );
    }

    ResourceClass *Get() const
    {
        return SCAST<ResourceClass*>(ResourceHandleBase::Get());
    }

    bool operator==(const ResourceClass *res) const
    {
        return ResourceHandleBase::operator=(res);
    }
    bool operator==(const ResourceHandleBase &rhs) const
    {
        return ResourceHandleBase::operator==(rhs);
    }
    bool operator!=(const ResourceHandleBase &rhs) const
    {
        return ResourceHandleBase::operator!=(rhs);
    }
    bool operator< (const ResourceHandleBase &rhs) const
    {
        return ResourceHandleBase::operator<(rhs);
    }
    operator bool() const
    {
        return ResourceHandleBase::operator bool();
    }

    ResourceHandle<ResourceClass>& operator=(const ResourceHandle<ResourceClass> &rhs)
    {
        if (&rhs != this)
        {
            Set(rhs.Get());
        }
        return *this;
    }

    ResourceHandle& operator=(ResourceHandle<ResourceClass> &&rhs)
    {
        if (&rhs != this)
        {
            Set(rhs.Get());
            rhs.Set(nullptr);
        }
        return *this;
    }

private:

    friend class Resources;
};

template<class T>
using RH = ResourceHandle<T>;

NAMESPACE_BANG_END

#endif // RESOURCEHANDLE_H

