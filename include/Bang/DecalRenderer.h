#ifndef DECALRENDERER_H
#define DECALRENDERER_H

#include "Bang/Bang.h"
#include "Bang/Renderer.h"

namespace Bang
{
class Texture2D;
class DecalRenderer : public Renderer
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(DecalRenderer);

public:
    DecalRenderer();
    virtual ~DecalRenderer() override;

    // Renderer
    void OnRender() override;

    void SetDecalTexture(Texture2D *decalTexture);

    Vector3 GetBoxSize() const;
    Texture2D *GetDecalTexture() const;
    Matrix4 GetProjectionMatrix() const;

private:
    RH<Mesh> m_cubeMesh;
};
}

#endif  // DECALRENDERER_H
