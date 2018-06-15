#ifndef RESOURCEHANDLE_H
#define RESOURCEHANDLE_H

#include "Bang/TypeMap.h"

NAMESPACE_BANG_BEGIN

FORWARD class Resource;

void OnResourceSet(String typeId, Resource *resource);
void OnResourceUnSet(String typeId, Resource *resource);

template <class ResourceClass>
class ResourceHandle
{
public:
    ResourceHandle()
    {
    }
    explicit ResourceHandle(ResourceClass *res)
    {
        Set(res);
    }
    ResourceHandle(const ResourceHandle &rhs)
    {
        *this = rhs;
    }

    ResourceHandle& operator=(const ResourceHandle &rhs)
    {
        if (&rhs != this)
        {
            Set(rhs.Get());
        }
        return *this;
    }
    ResourceHandle(ResourceHandle &&rhs)
    {
       *this = rhs;
    }
    ResourceHandle& operator=(ResourceHandle &&rhs)
    {
        if (&rhs != this)
        {
            p_resource = rhs.Get();
            m_typeId = rhs.m_typeId;

            rhs.p_resource = nullptr;
            rhs.m_typeId = "";
        }
        return *this;
    }

    ~ResourceHandle()
    {
        Set(nullptr);
    }

    bool operator==(const ResourceHandle &rhs) const
    {
        return Get() == rhs.Get();
    }
    bool operator!=(const ResourceHandle &rhs) const
    {
        return !(*this == rhs);
    }
    bool operator< (const ResourceHandle &rhs) const
    {
        return Get() < rhs.Get();
    }
    operator bool() const
    {
        return (Get() != nullptr);
    }

    ResourceClass* Get() const
    {
        return p_resource;
    }

    void Set(ResourceClass* resource)
    {
        if (Get() != resource)
        {
            if (Get())
            {
                // Must be done in two steps, so that we avoid unset loops
                Resource *prevResource = p_resource;
                TypeId prevTypeId = m_typeId;

                p_resource = nullptr;
                m_typeId = "";

                OnResourceUnSet(prevTypeId, prevResource);
            }

            p_resource = resource;
            if (Get())
            {
                m_typeId = GetTypeId( Get() );
                OnResourceSet(m_typeId, Get());
            }
        }
    }

private:
    TypeId m_typeId = "";
    ResourceClass* p_resource = nullptr;

    friend class Resources;
};

template<class T>
using RH = ResourceHandle<T>;

NAMESPACE_BANG_END

#endif // RESOURCEHANDLE_H

