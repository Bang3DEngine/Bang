#ifndef UIIMAGERENDERER_H
#define UIIMAGERENDERER_H

#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"
#include "Bang/UIRenderer.h"

namespace Bang
{
class Camera;
class ICloneable;
class Mesh;
class Path;
class Texture2D;

class UIImageRenderer : public UIRenderer
{
    COMPONENT(UIImageRenderer)

public:
    enum class Mode
    {
        TEXTURE = 0,
        TEXTURE_INV_UVY = 1,
        SLICE_9 = 2,
        SLICE_9_INV_UVY = 3
    };

    UIImageRenderer();
    virtual ~UIImageRenderer() override;

    // UIRenderer
    virtual void OnRender() override;

    void SetImageTexture(const Path &imagePath);
    void SetImageTexture(Texture2D *imageTexture);
    void SetTint(const Color &tint);
    void SetMode(UIImageRenderer::Mode mode);
    void SetSlice9BorderStrokePx(const Vector2i &slice9borderStrokePx);

    Mode GetMode() const;
    const Color &GetTint() const;
    Texture2D *GetImageTexture() const;
    const Vector2i &GetSlice9BorderStrokePx() const;

    // IEventsTransform
    virtual void OnTransformChanged() override;

    // Renderer
    virtual AARect GetBoundingRect(Camera *camera = nullptr) const override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    AH<Mesh> p_quadMesh;
    Color m_tint = Color::White();
    AH<Texture2D> p_imageTexture;
    Mode m_mode = Undef<Mode>();
    Vector2i m_slice9BorderStrokePx = Vector2i(8);
};
}

#endif  // UIIMAGERENDERER_H
