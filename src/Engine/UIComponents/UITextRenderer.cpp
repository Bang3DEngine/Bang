#include "Bang/UITextRenderer.h"

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/Font.h"
#include "Bang/GL.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/ICloneable.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Math.h"
#include "Bang/Mesh.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Paths.h"
#include "Bang/Rect.h"
#include "Bang/RectTransform.h"
#include "Bang/TextFormatter.h"
#include "Bang/Vector2.h"

namespace Bang
{
class Camera;
class Texture2D;
}  // namespace Bang

using namespace Bang;

UITextRenderer::UITextRenderer() : UIRenderer()
{
    SET_INSTANCE_CLASS_ID(UITextRenderer)

    p_mesh = Assets::Create<Mesh>();
    SetMaterial(MaterialFactory::GetUIText().Get());

    AH<Font> font = Assets::Load<Font>(
        Paths::GetEngineAssetsDir().Append("Fonts").Append("Ubuntu.ttf"));
    SetFont(font.Get());
    SetContent("");
    SetTextSize(20);
    SetTextColor(Color::Black());

    SetRenderPrimitive(GL::Primitive::TRIANGLES);
    OnChanged();
}

UITextRenderer::~UITextRenderer()
{
}

void UITextRenderer::CalculateLayout(Axis axis)
{
    if (!GetFont())
    {
        SetCalculatedLayout(axis, 0, 0);
        return;
    }

    Vector2i minSize = Vector2i::Zero();
    Vector2i prefSize = Vector2i::Zero();
    if (axis == Axis::HORIZONTAL)
    {
        prefSize = TextFormatter::GetMinimumHeightTextSize(
            GetContent(), GetFont(), GetTextSize(), GetSpacingMultiplier());
    }
    else  // Vertical
    {
        uint numLines;
        RectTransform *rt = GetGameObject()->GetRectTransform();
        Array<TextFormatter::CharRect> charRects =
            TextFormatter::GetFormattedTextPositions(
                GetContent(),
                GetFont(),
                GetTextSize(),
                AARecti(rt->GetViewportRect()),
                GetSpacingMultiplier(),
                GetHorizontalAlignment(),
                GetVerticalAlignment(),
                IsWrapping(),
                &numLines);
        AARect rect =
            charRects.Size() > 0 ? charRects.Front().rectPx : AARect::Zero();
        for (const TextFormatter::CharRect &cr : charRects)
        {
            rect = AARect::Union(rect, cr.rectPx);
        }

        prefSize = Vector2i(rect.GetSize());
        prefSize.y = Math::Max<int>(
            prefSize.y,
            m_numberOfLines *
                SCAST<int>(GetFont()->GetFontHeight(GetTextSize())));
    }

    SetCalculatedLayout(axis, minSize.GetAxis(axis), prefSize.GetAxis(axis));
}

void UITextRenderer::RegenerateCharQuadsVAO() const
{
    if (!IInvalidatable<UITextRenderer>::IsInvalid())
    {
        return;
    }
    IInvalidatable<UITextRenderer>::Validate();

    if (!GetFont())
    {
        p_mesh.Get()->SetPositionsPool({});
        p_mesh.Get()->SetUvsPool({});
        p_mesh.Get()->UpdateVAOs(false);
        return;
    }

    // Get the quad positions of the rects of each char
    if (!GetGameObject())
    {
        return;
    }
    RectTransform *rt = GetGameObject()->GetRectTransform();
    if (!rt)
    {
        return;
    }

    Array<TextFormatter::CharRect> textCharRects =
        TextFormatter::GetFormattedTextPositions(GetContent(),
                                                 GetFont(),
                                                 GetTextSize(),
                                                 AARecti(rt->GetViewportRect()),
                                                 GetSpacingMultiplier(),
                                                 GetHorizontalAlignment(),
                                                 GetVerticalAlignment(),
                                                 IsWrapping(),
                                                 &m_numberOfLines);

    // Generate quad positions and uvs for the mesh, and load them
    Array<Vector2> textQuadUvs;
    Array<Vector2> textQuadPos2D;
    Array<Vector3> textQuadPos3D;

    m_charRectsLocalNDC.Clear();
    for (const TextFormatter::CharRect &cr : textCharRects)
    {
        if (!GetFont()->HasCharacter(cr.character))
        {
            continue;
        }

        Vector2 minPxPerf =
            GL::FromPixelsPointToPixelPerfect(cr.rectPx.GetMin());
        Vector2 maxPxPerf = cr.rectPx.GetMin() + cr.rectPx.GetSize();
        Vector2f minViewportNDC(
            GL::FromViewportPointToViewportPointNDC(minPxPerf));
        Vector2f maxViewportNDC(
            GL::FromViewportPointToViewportPointNDC(maxPxPerf));

        GetFont()->GetFontAtlas(GetTextSize());  // Load atlas
        Vector2 minUv = GetFont()->GetCharMinUv(GetTextSize(), cr.character);
        Vector2 maxUv = GetFont()->GetCharMaxUv(GetTextSize(), cr.character);
        // std::swap(minUv.y, maxUv.y);

        AARect charRectViewportNDC(minViewportNDC, maxViewportNDC);
        AARect charRectLocalNDC(
            rt->FromViewportAARectNDCToLocalAARectNDC(charRectViewportNDC));

        textQuadUvs.PushBack(Vector2(minUv.x, maxUv.y));
        textQuadPos2D.PushBack(charRectLocalNDC.GetMinXMinY());
        textQuadPos3D.PushBack(Vector3(charRectLocalNDC.GetMinXMinY(), 0));
        textQuadUvs.PushBack(Vector2(maxUv.x, maxUv.y));
        textQuadPos2D.PushBack(charRectLocalNDC.GetMaxXMinY());
        textQuadPos3D.PushBack(Vector3(charRectLocalNDC.GetMaxXMinY(), 0));
        textQuadUvs.PushBack(Vector2(maxUv.x, minUv.y));
        textQuadPos2D.PushBack(charRectLocalNDC.GetMaxXMaxY());
        textQuadPos3D.PushBack(Vector3(charRectLocalNDC.GetMaxXMaxY(), 0));

        textQuadUvs.PushBack(Vector2(minUv.x, maxUv.y));
        textQuadPos2D.PushBack(charRectLocalNDC.GetMinXMinY());
        textQuadPos3D.PushBack(Vector3(charRectLocalNDC.GetMinXMinY(), 0));
        textQuadUvs.PushBack(Vector2(maxUv.x, minUv.y));
        textQuadPos2D.PushBack(charRectLocalNDC.GetMaxXMaxY());
        textQuadPos3D.PushBack(Vector3(charRectLocalNDC.GetMaxXMaxY(), 0));
        textQuadUvs.PushBack(Vector2(minUv.x, minUv.y));
        textQuadPos2D.PushBack(charRectLocalNDC.GetMinXMaxY());
        textQuadPos3D.PushBack(Vector3(charRectLocalNDC.GetMinXMaxY(), 0));

        AARect charRectLocalNDCRaw(
            rt->FromViewportPointToLocalPointNDC(minPxPerf),
            rt->FromViewportPointToLocalPointNDC(maxPxPerf));
        m_charRectsLocalNDC.PushBack(charRectLocalNDCRaw);
    }

    m_textRectNDC = AARect::GetBoundingRectFromPositions(textQuadPos2D.Begin(),
                                                         textQuadPos2D.End());
    p_mesh.Get()->SetPositionsPool(textQuadPos3D);
    p_mesh.Get()->SetUvsPool(textQuadUvs);
    p_mesh.Get()->UpdateVAOs(false);
}

void UITextRenderer::Bind()
{
    UIRenderer::Bind();

    if (GetFont())
    {
        const int textSize = Math::Max(GetTextSize(), 1);
        Texture2D *fontAtlas = GetFont()->GetFontAtlas(textSize);
        GetMaterial()->SetAlbedoTexture(fontAtlas);
    }
}

void UITextRenderer::OnRender()
{
    UIRenderer::OnRender();
    RegenerateCharQuadsVAO();

    int vertCount = p_mesh.Get()->GetNumVerticesIds();
    if (vertCount >= 3)
    {
        GL::Render(p_mesh.Get()->GetVAO(), GetRenderPrimitive(), vertCount);
    }
}

void UITextRenderer::UnBind()
{
    UIRenderer::UnBind();
}

void UITextRenderer::SetHorizontalAlign(HorizontalAlignment horizontalAlignment)
{
    if (GetHorizontalAlignment() != horizontalAlignment)
    {
        m_horizontalAlignment = horizontalAlignment;
        OnChanged();
    }
}

void UITextRenderer::SetVerticalAlign(VerticalAlignment verticalAlignment)
{
    if (GetVerticalAlignment() != verticalAlignment)
    {
        m_verticalAlignment = verticalAlignment;
        OnChanged();
    }
}

void UITextRenderer::SetFont(Font *font)
{
    if (GetFont() != font)
    {
        p_font.Set(font);
        OnChanged();
    }
}

void UITextRenderer::SetKerning(bool kerning)
{
    if (IsKerning() != kerning)
    {
        m_kerning = kerning;
        OnChanged();
    }
}

void UITextRenderer::SetWrapping(bool wrapping)
{
    if (IsWrapping() != wrapping)
    {
        m_wrapping = wrapping;
        OnChanged();
    }
}

void UITextRenderer::SetContent(const String &content)
{
    if (GetContent() != content)
    {
        m_content = content;
        OnChanged();
    }
}

void UITextRenderer::SetTextSize(int size)
{
    if (GetTextSize() != size)
    {
        m_textSize = Math::Max(size, 1);
        OnChanged();
    }
}

void UITextRenderer::SetSpacingMultiplier(const Vector2 &spacingMultiplier)
{
    if (GetSpacingMultiplier() != spacingMultiplier)
    {
        m_spacingMultiplier = spacingMultiplier;
        OnChanged();
    }
}

void UITextRenderer::SetTextColor(const Color &textColor)
{
    if (textColor != GetTextColor())
    {
        GetMaterial()->SetAlbedoColor(textColor);
        OnChanged();
    }
}

Font *UITextRenderer::GetFont() const
{
    return p_font.Get();
}
bool UITextRenderer::IsKerning() const
{
    return m_kerning;
}
bool UITextRenderer::IsWrapping() const
{
    return m_wrapping;
}

const String &UITextRenderer::GetContent() const
{
    return m_content;
}
int UITextRenderer::GetTextSize() const
{
    return m_textSize;
}

const Vector2 &UITextRenderer::GetSpacingMultiplier() const
{
    return m_spacingMultiplier;
}
const Array<AARect> &UITextRenderer::GetCharRectsLocalNDC() const
{
    return m_charRectsLocalNDC;
}
const AARect &UITextRenderer::GetCharRectLocalNDC(uint charIndex) const
{
    return GetCharRectsLocalNDC()[charIndex];
}

AARect UITextRenderer::GetCharRectViewportNDC(uint charIndex) const
{
    return AARect(GetGameObject()
                      ->GetRectTransform()
                      ->FromLocalAARectNDCToViewportAARectNDC(
                          GetCharRectsLocalNDC()[charIndex]));
}
AARect UITextRenderer::GetContentViewportNDCRect() const
{
    return AARect(GetGameObject()
                      ->GetRectTransform()
                      ->FromLocalAARectNDCToViewportAARectNDC(m_textRectNDC));
}

VerticalAlignment UITextRenderer::GetVerticalAlignment() const
{
    return m_verticalAlignment;
}
HorizontalAlignment UITextRenderer::GetHorizontalAlignment() const
{
    return m_horizontalAlignment;
}

AARect UITextRenderer::GetBoundingRect(Camera *camera) const
{
    return GetContentViewportNDCRect();
}

const Color &UITextRenderer::GetTextColor() const
{
    return GetActiveMaterial()->GetAlbedoColor();
}

void UITextRenderer::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    UIRenderer::CloneInto(clone, cloneGUID);

    UITextRenderer *text = SCAST<UITextRenderer *>(clone);
    text->SetFont(GetFont());
    text->SetContent(GetContent());
    text->SetTextSize(GetTextSize());
    text->SetTextColor(GetTextColor());
    text->SetSpacingMultiplier(GetSpacingMultiplier());
    text->SetWrapping(IsWrapping());
    text->SetHorizontalAlign(GetHorizontalAlignment());
    text->SetVerticalAlign(GetVerticalAlignment());
}

void UITextRenderer::ImportMeta(const MetaNode &meta)
{
    UIRenderer::ImportMeta(meta);

    if (meta.Contains("Font"))
    {
        SetFont(Assets::Load<Font>(meta.Get<GUID>("Font")).Get());
    }

    if (meta.Contains("Content"))
    {
        SetContent(meta.Get<String>("Content"));
    }

    if (meta.Contains("TextSize"))
    {
        SetTextSize(meta.Get<float>("TextSize"));
    }

    if (meta.Contains("SpacingMultiplier"))
    {
        SetSpacingMultiplier(meta.Get<Vector2>("SpacingMultiplier"));
    }

    if (meta.Contains("Kerning"))
    {
        SetKerning(meta.Get<bool>("Kerning"));
    }

    if (meta.Contains("TextColor"))
    {
        SetTextColor(meta.Get<Color>("TextColor"));
    }

    if (meta.Contains("Wrapping"))
    {
        SetWrapping(meta.Get<bool>("Wrapping"));
    }

    if (meta.Contains("VerticalAlign"))
    {
        SetVerticalAlign(meta.Get<VerticalAlignment>("VerticalAlign"));
    }

    if (meta.Contains("HorizontalAlign"))
    {
        SetHorizontalAlign(meta.Get<HorizontalAlignment>("HorizontalAlign"));
    }
}

void UITextRenderer::ExportMeta(MetaNode *metaNode) const
{
    UIRenderer::ExportMeta(metaNode);

    metaNode->Set("Font", GetFont() ? GetFont()->GetGUID() : GUID::Empty());
    metaNode->Set("Content", GetContent());
    metaNode->Set("TextSize", GetTextSize());
    metaNode->Set("SpacingMultiplier", GetSpacingMultiplier());
    metaNode->Set("TextColor", GetTextColor());
    metaNode->Set("Kerning", IsKerning());
    metaNode->Set("Wrapping", IsWrapping());
    metaNode->Set("VerticalAlign", GetVerticalAlignment());
    metaNode->Set("HorizontalAlign", GetHorizontalAlignment());
}

void UITextRenderer::OnChanged()
{
    IInvalidatable<UITextRenderer>::Invalidate();
    IInvalidatable<ILayoutElement>::Invalidate();
    UIRenderer::PropagateRendererChanged();
}

void UITextRenderer::OnTransformChanged()
{
    UIRenderer::OnTransformChanged();
    OnChanged();
}
