#ifndef LINERENDERER_H
#define LINERENDERER_H

#include "Bang/AABox.h"
#include "Bang/Array.h"
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

class LineRenderer : public Renderer
{
    COMPONENT(LineRenderer)

public:
    void SetPoint(int i, const Vector3 &point);
    void SetPoints(const Array<Vector3> &points);

    const Array<Vector3> &GetPoints() const;
    virtual AABox GetAABBox() const override;

    // Renderer
    void Bind() override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    LineRenderer();
    virtual ~LineRenderer() override;

    // Renderer
    virtual void OnRender() override;

private:
    AH<Mesh> p_mesh;
    Array<Vector3> m_points;
};
}

#endif  // LINERENDERER_H
