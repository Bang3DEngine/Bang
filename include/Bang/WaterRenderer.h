#ifndef WATERRENDERER_H
#define WATERRENDERER_H

#include "Bang/Array.h"
#include "Bang/Vector3.h"
#include "Bang/Renderer.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class Mesh;

class WaterRenderer : public Renderer
{
    COMPONENT(WaterRenderer)

public:
	WaterRenderer();
	virtual ~WaterRenderer();

    // Renderer
    virtual void OnRender() override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    RH<Mesh> p_planeMesh;
};

NAMESPACE_BANG_END

#endif // WATERRENDERER_H

