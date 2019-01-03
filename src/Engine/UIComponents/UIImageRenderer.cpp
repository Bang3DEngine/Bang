#include "Bang/UIImageRenderer.h"

#include <istream>

#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/ClassDB.h"
#include "Bang/GL.h"
#include "Bang/GUID.h"
#include "Bang/ICloneable.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Path.h"
#include "Bang/ShaderProgram.h"
#include "Bang/Texture2D.h"

namespace Bang
{
class Camera;
}

using namespace Bang;

UIImageRenderer::UIImageRenderer()
{
    SET_INSTANCE_CLASS_ID(UIImageRenderer)

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
        GetActiveMaterial()->GetShaderProgram()->SetInt(
            "B_ImageMode", SCAST<int>(GetMode()), false);
        GetActiveMaterial()->GetShaderProgram()->SetVector2(
            "B_Slice9BorderStrokePx",
            Vector2(GetSlice9BorderStrokePx()),
            false);
    }

    if (GetTint().a > 0.0f)
    {
        GL::Render(p_quadMesh.Get()->GetVAO(),
                   GetRenderPrimitive(),
                   p_quadMesh.Get()->GetNumVerticesIds());
    }
}

void UIImageRenderer::SetImageTexture(const Path &imagePath)
{
    if (imagePath.IsFile())
    {
        AH<Texture2D> tex = Assets::Load<Texture2D>(imagePath);
        SetImageTexture(tex.Get());
    }
    else
    {
        SetImageTexture(nullptr);
    }
}

void UIImageRenderer::SetImageTexture(Texture2D *imageTexture)
{
    if (imageTexture != GetImageTexture())
    {
        p_imageTexture.Set(imageTexture);
        GetMaterial()->SetAlbedoTexture(GetImageTexture());
    }
}

void UIImageRenderer::SetTint(const Color &tint)
{
    if (tint != GetTint())
    {
        m_tint = tint;
        GetMaterial()->SetAlbedoColor(GetTint());
    }
}

void UIImageRenderer::SetMode(UIImageRenderer::Mode mode)
{
    if (mode != GetMode())
    {
        m_mode = mode;
        switch (GetMode())
        {
            case Mode::TEXTURE: p_quadMesh = MeshFactory::GetUIPlane(); break;

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

void UIImageRenderer::SetSlice9BorderStrokePx(
    const Vector2i &slice9borderStrokePx)
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

const Vector2i &UIImageRenderer::GetSlice9BorderStrokePx() const
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

void UIImageRenderer::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    UIRenderer::CloneInto(clone, cloneGUID);
    UIImageRenderer *img = SCAST<UIImageRenderer *>(clone);
    img->SetImageTexture(GetImageTexture());
    img->SetTint(GetTint());
}

void UIImageRenderer::ImportMeta(const MetaNode &metaNode)
{
    UIRenderer::ImportMeta(metaNode);

    if (metaNode.Contains("Image"))
    {
        AH<Texture2D> tex =
            Assets::Load<Texture2D>(metaNode.Get<GUID>("Image"));
        SetImageTexture(tex.Get());
    }

    if (metaNode.Contains("Tint"))
    {
        SetTint(metaNode.Get<Color>("Tint"));
    }

    if (metaNode.Contains("Mode"))
    {
        SetMode(SCAST<UIImageRenderer::Mode>(metaNode.Get<int>("Mode")));
    }

    if (metaNode.Contains("Slice9BorderStrokePx"))
    {
        SetSlice9BorderStrokePx(metaNode.Get<Vector2i>("Slice9BorderStrokePx"));
    }
}

void UIImageRenderer::ExportMeta(MetaNode *metaNode) const
{
    UIRenderer::ExportMeta(metaNode);

    Texture2D *imgTex = GetImageTexture();
    metaNode->Set("Image", imgTex ? imgTex->GetGUID() : GUID::Empty());
    metaNode->Set("Tint", GetTint());
    metaNode->Set("Mode", SCAST<int>(GetMode()));
    metaNode->Set("Slice9BorderStrokePx", GetSlice9BorderStrokePx());
}
