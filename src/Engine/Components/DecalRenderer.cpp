#include "Bang/DecalRenderer.h"

#include "Bang/GameObject.h"
#include "Bang/Transform.h"

using namespace Bang;

DecalRenderer::DecalRenderer()
{
    CONSTRUCT_CLASS_ID(DecalRenderer);
}

DecalRenderer::~DecalRenderer()
{
}

void DecalRenderer::OnRender()
{
    Renderer::OnRender();
}

Vector3 DecalRenderer::GetBoxSize() const
{
    return GetGameObject()->GetTransform()->GetScale();
}
