#ifndef LINERENDERER_H
#define LINERENDERER_H

#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Renderer.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"
#include "Bang/Vector3.h"

NAMESPACE_BANG_BEGIN

FORWARD class ICloneable;
FORWARD class Mesh;
FORWARD class MetaNode;

class LineRenderer : public Renderer
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(LineRenderer)

public:
    void SetPoint(int i, const Vector3& point);
    void SetPoints(const Array<Vector3>& points);

    const Array<Vector3>& GetPoints() const;
    virtual AABox GetAABBox() const override;

    // Renderer
    void Bind() override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    LineRenderer();
    virtual ~LineRenderer() override;

    // Renderer
    virtual void OnRender() override;

private:
    RH<Mesh> p_mesh;
    Array<Vector3> m_points;
};

NAMESPACE_BANG_END

#endif // LINERENDERER_H
