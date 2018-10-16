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

NAMESPACE_BANG_END
