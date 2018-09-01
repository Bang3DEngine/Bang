#include "Bang/UIImageRenderer.h"

#include "Bang/GL.h"
#include "Bang/Mesh.h"
#include "Bang/Image.h"
#include "Bang/Material.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/GameObject.h"
#include "Bang/MeshFactory.h"
#include "Bang/RectTransform.h"
#include "Bang/ShaderProgram.h"
#include "Bang/MaterialFactory.h"
#include "Bang/UILayoutManager.h"

USING_NAMESPACE_BANG

UIImageRenderer::UIImageRenderer()
{
    SetMaterial(MaterialFactory::GetUIImage().Get());
    SetMode(Mode::TEXTURE_INV_UVY);
}

UIImageRenderer::~UIImageRenderer()
{
}

void UIImageRenderer::OnRender()
{
    UIRenderer::OnRender();

    if (GetActiveMaterial() && GetActiveMaterial()->GetShaderProgram())
    {
        GetActiveMaterial()->GetShaderProgram()->SetInt("B_ImageMode",
                                                        SCAST<int>( GetMode() ),
                                                        false);
        GetActiveMaterial()->GetShaderProgram()->SetVector2(
                                              "B_Slice9BorderStrokePx",
                                              Vector2(GetSlice9BorderStrokePx()),
                                              false);
    }

    if (GetTint().a > 0.0f)
    {
        GL::Render(p_quadMesh.Get()->GetVAO(), GetRenderPrimitive(),
                   p_quadMesh.Get()->GetNumVertices());
    }
}

void UIImageRenderer::SetImageTexture(const Path &imagePath)
{
    if (imagePath.IsFile())
    {
        RH<Texture2D> tex = Resources::Load<Texture2D>(imagePath);
        SetImageTexture(tex.Get());
    }
    else { SetImageTexture(nullptr); }
}

void UIImageRenderer::SetImageTexture(Texture2D* imageTexture)
{
    if (imageTexture != GetImageTexture())
    {
        p_imageTexture.Set(imageTexture);
        GetMaterial()->SetAlbedoTexture( GetImageTexture() );
    }
}

void UIImageRenderer::SetTint(const Color &tint)
{
    if (tint != GetTint())
    {
        m_tint = tint;
        GetMaterial()->SetAlbedoColor( GetTint() );
    }
}

void UIImageRenderer::SetMode(UIImageRenderer::Mode mode)
{
    if (mode != GetMode())
    {
        m_mode = mode;
        switch (GetMode())
        {
            case Mode::TEXTURE:
            p_quadMesh = MeshFactory::GetUIPlane();
            break;

            case Mode::TEXTURE_INV_UVY:
            p_quadMesh = MeshFactory::GetUIPlaneInvUVY();
            break;

            case Mode::SLICE_9:
            p_quadMesh = MeshFactory::GetUIPlane3x3();
            break;

            case Mode::SLICE_9_INV_UVY:
            p_quadMesh = MeshFactory::GetUIPlane3x3InvUVY();
            break;
        }
    }
}

void UIImageRenderer::SetSlice9BorderStrokePx(const Vector2i& slice9borderStrokePx)
{
    m_slice9BorderStrokePx = slice9borderStrokePx;
}

UIImageRenderer::Mode UIImageRenderer::GetMode() const
{
    return m_mode;
}

const Color &UIImageRenderer::GetTint() const
{
    return m_tint;
}

Texture2D *UIImageRenderer::GetImageTexture() const
{
    return p_imageTexture.Get();
}

const Vector2i& UIImageRenderer::GetSlice9BorderStrokePx() const
{
    return m_slice9BorderStrokePx;
}

void UIImageRenderer::OnTransformChanged()
{
    UIRenderer::OnTransformChanged();
}

AARect UIImageRenderer::GetBoundingRect(Camera *camera) const
{
    AARect boundingRect = UIRenderer::GetBoundingRect(camera);
    return AARect(boundingRect.GetCenter(), boundingRect.GetCenter());
}

void UIImageRenderer::CloneInto(ICloneable *clone) const
{
    UIRenderer::CloneInto(clone);
    UIImageRenderer *img = Cast<UIImageRenderer*>(clone);
    img->SetImageTexture( GetImageTexture() );
    img->SetTint( GetTint() );
}

void UIImageRenderer::ImportXML(const XMLNode &xmlInfo)
{
    UIRenderer::ImportXML(xmlInfo);

    if (xmlInfo.Contains("Image"))
    {
        RH<Texture2D> tex = Resources::Load<Texture2D>(
                                            xmlInfo.Get<GUID>("Image"));
        SetImageTexture(tex.Get());
    }

    if (xmlInfo.Contains("Tint"))
    {
        SetTint( xmlInfo.Get<Color>("Tint") );
    }

    if (xmlInfo.Contains("Mode"))
    {
        SetMode( SCAST<UIImageRenderer::Mode>( xmlInfo.Get<int>("Mode") ) );
    }

    if (xmlInfo.Contains("Slice9BorderStrokePx"))
    {
        SetSlice9BorderStrokePx( xmlInfo.Get<Vector2i>("Slice9BorderStrokePx") );
    }
}

void UIImageRenderer::ExportXML(XMLNode *xmlInfo) const
{
    UIRenderer::ExportXML(xmlInfo);

    Texture2D *imgTex = GetImageTexture();
    xmlInfo->Set("Image", imgTex ? imgTex->GetGUID() : GUID::Empty());
    xmlInfo->Set("Tint", GetTint());
    xmlInfo->Set("Mode", SCAST<int>(GetMode()));
    xmlInfo->Set("Slice9BorderStrokePx", GetSlice9BorderStrokePx());
}

