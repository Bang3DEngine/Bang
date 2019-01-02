#include "Bang/Renderer.h"

#include "Bang/AABox.h"
#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Camera.h"
#include "Bang/ClassDB.h"
#include "Bang/Extensions.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/GLUniforms.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/ICloneable.h"
#include "Bang/IEventsAsset.h"
#include "Bang/IEventsRendererChanged.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/ShaderProgram.h"
#include "Bang/Transform.h"

namespace Bang
{
class Asset;
}

using namespace Bang;

Renderer::Renderer()
{
    SET_INSTANCE_CLASS_ID(Renderer);
    SetMaterial(MaterialFactory::GetDefault().Get());
}

Renderer::~Renderer()
{
}

Material *Renderer::GetActiveMaterial() const
{
    if (GetCopiedMaterial())
    {
        return GetCopiedMaterial();
    }
    return GetSharedMaterial();
}

Material *Renderer::GetCopiedMaterial() const
{
    return p_copiedMaterial.Get();
}

void Renderer::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);

    GEngine *ge = GEngine::GetInstance();
    ASSERT(ge);

    if (ge->CanRenderNow(this, renderPass))
    {
        ge->Render(this);
    }
}

void Renderer::OnRender()
{
    // Empty
}

void Renderer::Bind()
{
    GL::SetViewProjMode(GetViewProjMode());
    GLUniforms::SetModelMatrix(GetModelMatrixUniform());
    GL::SetDepthMask(GetDepthMask());

    if (Material *mat = GetActiveMaterial())
    {
        if (ShaderProgram *sp = mat->GetShaderProgram())
        {
            if (sp->IsLinked())
            {
                mat->Bind();
                SetUniformsOnBind(sp);
            }
        }
    }
}

void Renderer::SetUniformsOnBind(ShaderProgram *sp)
{
    sp->SetBool(GLUniforms::UniformName_ReceivesShadows, GetReceivesShadows());
}

void Renderer::UnBind()
{
    // if (GetActiveMaterial()) { GetActiveMaterial()->UnBind(); }
}

void Renderer::SetVisible(bool visible)
{
    if (visible != IsVisible())
    {
        m_visible = visible;
        PropagateRendererChanged();
    }
}

void Renderer::SetMaterial(Material *sharedMaterial, Material *materialCopy)
{
    if (GetSharedMaterial() != sharedMaterial)
    {
        if (GetSharedMaterial())
        {
            GetSharedMaterial()->EventEmitter<IEventsAsset>::UnRegisterListener(
                this);
        }
        if (GetCopiedMaterial())
        {
            GetCopiedMaterial()->EventEmitter<IEventsAsset>::UnRegisterListener(
                this);
        }

        p_sharedMaterial.Set(sharedMaterial);
        p_copiedMaterial.Set(materialCopy);

        if (GetSharedMaterial())
        {
            GetSharedMaterial()->EventEmitter<IEventsAsset>::RegisterListener(
                this);
        }
        if (GetCopiedMaterial())
        {
            GetCopiedMaterial()->EventEmitter<IEventsAsset>::RegisterListener(
                this);
        }

        PropagateRendererChanged();
    }
}

void Renderer::SetDepthMask(bool depthMask)
{
    if (depthMask != GetDepthMask())
    {
        m_depthMask = depthMask;
        PropagateRendererChanged();
    }
}

void Renderer::SetViewProjMode(GL::ViewProjMode viewProjMode)
{
    if (viewProjMode != GetViewProjMode())
    {
        m_viewProjMode = viewProjMode;
        PropagateRendererChanged();
    }
}
void Renderer::SetRenderPrimitive(GL::Primitive renderPrimitive)
{
    if (renderPrimitive != GetRenderPrimitive())
    {
        m_renderPrimitive = renderPrimitive;
        PropagateRendererChanged();
    }
}

void Renderer::SetUseReflectionProbes(bool useReflectionProbes)
{
    if (useReflectionProbes != GetUseReflectionProbes())
    {
        m_useReflectionProbes = useReflectionProbes;
        PropagateRendererChanged();
    }
}
void Renderer::SetCastsShadows(bool castsShadows)
{
    if (castsShadows != GetCastsShadows())
    {
        m_castsShadows = castsShadows;
        PropagateRendererChanged();
    }
}
void Renderer::SetReceivesShadows(bool receivesShadows)
{
    if (receivesShadows != GetReceivesShadows())
    {
        m_receivesShadows = receivesShadows;
        PropagateRendererChanged();
    }
}

bool Renderer::IsVisible() const
{
    return m_visible;
}

bool Renderer::GetDepthMask() const
{
    return m_depthMask;
}
Material *Renderer::GetSharedMaterial() const
{
    return p_sharedMaterial.Get();
}

void Renderer::OnAssetChanged(Asset *)
{
    PropagateRendererChanged();
}

AABox Renderer::GetAABBox() const
{
    return AABox::Empty();
}

bool Renderer::GetCastsShadows() const
{
    return m_castsShadows;
}

bool Renderer::GetReceivesShadows() const
{
    return m_receivesShadows;
}

GL::ViewProjMode Renderer::GetViewProjMode() const
{
    return m_viewProjMode;
}

GL::Primitive Renderer::GetRenderPrimitive() const
{
    return m_renderPrimitive;
}

bool Renderer::GetUseReflectionProbes() const
{
    return m_useReflectionProbes;
}
Material *Renderer::GetMaterial() const
{
    if (!GetCopiedMaterial())
    {
        if (GetSharedMaterial())
        {
            p_copiedMaterial = Assets::Clone<Material>(GetSharedMaterial());
            GetCopiedMaterial()->EventEmitter<IEventsAsset>::RegisterListener(
                const_cast<Renderer *>(this));
        }
    }
    return GetCopiedMaterial();
}

AARect Renderer::GetBoundingRect(Camera *camera) const
{
    return AARect::NDCRect();
    return camera ? camera->GetViewportBoundingAARectNDC(GetAABBox())
                  : AARect::Zero();
}

void Renderer::PropagateRendererChanged()
{
    EventEmitter<IEventsRendererChanged>::PropagateToListeners(
        &IEventsRendererChanged::OnRendererChanged, this);
}

Matrix4 Renderer::GetModelMatrixUniform() const
{
    return GetGameObject()->GetTransform()
               ? GetGameObject()->GetTransform()->GetLocalToWorldMatrix()
               : Matrix4::Identity();
}

void Renderer::Reflect()
{
    BANG_REFLECT_VAR_MEMBER(Renderer, "Visible", SetVisible, IsVisible);
    BANG_REFLECT_VAR_MEMBER(Renderer, "Depth Mask", SetDepthMask, GetDepthMask);
    BANG_REFLECT_VAR_ASSET(
        "Material",
        SetMaterial,
        GetSharedMaterial,
        Material,
        BANG_REFLECT_HINT_EXTENSIONS(Extensions::GetMaterialExtension()) +
            BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(true));
    BANG_REFLECT_VAR_MEMBER(Renderer,
                            "Use Refl Probes",
                            SetUseReflectionProbes,
                            GetUseReflectionProbes);
    BANG_REFLECT_VAR_MEMBER(
        Renderer, "Casts Shadows", SetCastsShadows, GetCastsShadows);
    BANG_REFLECT_VAR_MEMBER(
        Renderer, "Receives Shadows", SetReceivesShadows, GetReceivesShadows);
}
