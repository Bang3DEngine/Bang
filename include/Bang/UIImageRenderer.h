#ifndef UIIMAGERENDERER_H
#define UIIMAGERENDERER_H

#include "Bang/Texture2D.h"
#include "Bang/Alignment.h"
#include "Bang/UIRenderer.h"
#include "Bang/AspectRatioMode.h"
#include "Bang/ITransformListener.h"

NAMESPACE_BANG_BEGIN

class UIImageRenderer : public UIRenderer
{
    COMPONENT(UIImageRenderer)

public:
    enum class Mode
    {
        TEXTURE = 0,
        SLICE_9 = 1
    };

    UIImageRenderer();
    virtual ~UIImageRenderer();

    // UIRenderer
    virtual void OnRender() override;

    void SetImageTexture(const Path &imagePath);
    void SetImageTexture(Texture2D* imageTexture);
    void SetTint(const Color& tint);
    void SetMode(Mode mode);
    void SetSlice9BorderStrokePx(const Vector2i& slice9borderStrokePx);

    Mode GetMode() const;
    const Color& GetTint() const;
    Texture2D *GetImageTexture() const;
    const Vector2i& GetSlice9BorderStrokePx() const;

    // ITransformListener
    virtual void OnTransformChanged() override;

    // Renderer
    virtual AARect GetBoundingRect(Camera *camera = nullptr) const override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    RH<Mesh> p_quadMesh;
    Color m_tint = Color::White;
    RH<Texture2D> p_imageTexture;
    Mode m_mode = Undef<Mode>();
    Vector2i m_slice9BorderStrokePx = Vector2i(8);
};

NAMESPACE_BANG_END

#endif // UIIMAGERENDERER_H
