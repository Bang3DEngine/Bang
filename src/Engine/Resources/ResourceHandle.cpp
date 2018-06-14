#include "Bang/ResourceHandle.h"

#include "Bang/Resource.h"
#include "Bang/Resources.h"

NAMESPACE_BANG_BEGIN

void OnResourceSet(String typeId, Resource *resource)
{
    Resources::RegisterResourceUsage(typeId, resource);
}

void OnResourceUnSet(String typeId, Resource *resource)
{
    Resources::UnRegisterResourceUsage(typeId, resource);
}

NAMESPACE_BANG_END
