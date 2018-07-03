#include "Bang/SkinnedMeshRenderer.h"

#include "Bang/MaterialFactory.h"

USING_NAMESPACE_BANG

SkinnedMeshRenderer::SkinnedMeshRenderer()
{
    SetMaterial( MaterialFactory::GetDefaultAnimated().Get() );
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

