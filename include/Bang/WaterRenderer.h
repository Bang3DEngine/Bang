#ifndef WATERRENDERER_H
#define WATERRENDERER_H

#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Renderer.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"

namespace Bang
{
class ICloneable;
class Mesh;
class MetaNode;

class WaterRenderer : public Renderer
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(WaterRenderer)

public:
    WaterRenderer();
    virtual ~WaterRenderer() override;

    // Renderer
    virtual void OnRender() override;

    // Serializable
    void Reflect() override;

private:
    RH<Mesh> p_planeMesh;
};
}

#endif  // WATERRENDERER_H
