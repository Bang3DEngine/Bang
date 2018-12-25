#ifndef WATERRENDERER_H
#define WATERRENDERER_H

#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Renderer.h"
#include "Bang/String.h"

namespace Bang
{
class ICloneable;
class Mesh;
class MetaNode;

class WaterRenderer : public Renderer
{
    COMPONENT(WaterRenderer)

public:
    WaterRenderer();
    virtual ~WaterRenderer() override;

    // Renderer
    virtual void OnRender() override;

    // Serializable
    void Reflect() override;

private:
    AH<Mesh> p_planeMesh;
};
}

#endif  // WATERRENDERER_H
