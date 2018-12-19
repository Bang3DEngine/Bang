#ifndef DECALRENDERER_H
#define DECALRENDERER_H

#include "Bang/Bang.h"
#include "Bang/Renderer.h"

namespace Bang
{
class DecalRenderer : public Renderer
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(DecalRenderer);

public:
    DecalRenderer();
    virtual ~DecalRenderer() override;

    // Renderer
    void OnRender() override;

    Vector3 GetBoxSize() const;

private:
};
}

#endif  // DECALRENDERER_H
