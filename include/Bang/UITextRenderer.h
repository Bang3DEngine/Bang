#ifndef UITEXTRENDERER_H
#define UITEXTRENDERER_H

#include "Bang/Alignment.h"
#include "Bang/Array.h"
#include "Bang/AssetHandle.h"
#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/IInvalidatable.h"
#include "Bang/ILayoutElement.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"
#include "Bang/UIRenderer.h"

namespace Bang
{
class Camera;
class Color;
class Font;
class ICloneable;
class Mesh;

class UITextRenderer : public UIRenderer,
                       public ILayoutElement,
                       public IInvalidatable<UITextRenderer>
{
    COMPONENT(UITextRenderer)

public:
    void RegenerateCharQuadsVAO() const;

    void SetFont(Font *font);
    void SetTextColor(const Color &textColor);
    void SetHorizontalAlign(HorizontalAlignment horizontalAlignment);
    void SetVerticalAlign(VerticalAlignment verticalAlignment);
    void SetKerning(bool kerning);
    void SetWrapping(bool wrapping);
    void SetContent(const String &content);
    void SetTextSize(int size);
    void SetSpacingMultiplier(const Vector2 &spacingMultiplier);

    Font *GetFont() const;
    bool IsKerning() const;
    const Color &GetTextColor() const;
    bool IsWrapping() const;
    VerticalAlignment GetVerticalAlignment() const;
    HorizontalAlignment GetHorizontalAlignment() const;
    const String &GetContent() const;
    int GetTextSize() const;
    const Vector2 &GetSpacingMultiplier() const;
    const Array<AARect> &GetCharRectsLocalNDC() const;
    const AARect &GetCharRectLocalNDC(uint charIndex) const;
    AARect GetCharRectViewportNDC(uint charIndex) const;
    AARect GetContentViewportNDCRect() const;
    virtual AARect GetBoundingRect(Camera *camera = nullptr) const override;

    // UIRenderer
    virtual void OnRender() override;
    virtual void Bind() override;
    virtual void UnBind() override;

    // ILayoutElement
    virtual void CalculateLayout(Axis axis) override;

    // IEventsTransform
    virtual void OnTransformChanged() override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    AH<Font> p_font;
    String m_content = "";
    int m_textSize = 64;
    Vector2 m_spacingMultiplier = Vector2::One();
    bool m_kerning = false;
    mutable AARect m_textRectNDC = AARect::Zero();

    bool m_wrapping = false;
    HorizontalAlignment m_horizontalAlignment = HorizontalAlignment::CENTER;
    VerticalAlignment m_verticalAlignment = VerticalAlignment::CENTER;

    AH<Mesh> p_mesh;
    mutable uint m_numberOfLines = 0;
    mutable Array<AARect> m_charRectsLocalNDC;

    UITextRenderer();
    virtual ~UITextRenderer() override;

    void OnChanged();
};
}

#endif  // UITEXTRENDERER_H
