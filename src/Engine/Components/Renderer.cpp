#include "Bang/Renderer.h"

#include <functional>

#include "Bang/GL.h"
#include "Bang/AABox.h"
#include "Bang/AARect.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/MetaNode.h"
#include "Bang/Transform.h"
#include "Bang/Resources.h"
#include "Bang/GLUniforms.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/TextureFactory.h"
#include "Bang/MaterialFactory.h"

USING_NAMESPACE_BANG

Renderer::Renderer()
{
    CONSTRUCT_CLASS_ID(Renderer);
    SetMaterial(MaterialFactory::GetDefault().Get());
}

Renderer::~Renderer()
{
}

Material *Renderer::GetActiveMaterial() const
{
    if (p_material)
    {
        return GetMaterial();
    }
    return GetSharedMaterial();
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
    GL::SetViewProjMode( GetViewProjMode() );
    GLUniforms::SetModelMatrix( GetModelMatrixUniform() );
    GL::SetDepthMask( GetDepthMask() );

    if (Material *mat = GetActiveMaterial())
    {
        mat->Bind();
        if (ShaderProgram *sp = mat->GetShaderProgram())
        {
            sp->SetBool(GLUniforms::UniformName_ReceivesShadows, GetReceivesShadows());
        }
    }
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


void Renderer::SetMaterial(Material *mat)
{
    if (GetSharedMaterial() != mat)
    {
        if (GetSharedMaterial())
        {
            GetSharedMaterial()->EventEmitter<IEventsResource>::
                                 UnRegisterListener(this);
        }

        if (p_material.Get())
        {
            p_material.Get()->EventEmitter<IEventsResource>::
                              UnRegisterListener(this);
            p_material.Set(nullptr);
        }

        p_sharedMaterial.Set(mat);

        if (GetSharedMaterial())
        {
            GetSharedMaterial()->EventEmitter<IEventsResource>::
                                 RegisterListener(this);
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

bool Renderer::IsVisible() const { return m_visible; }

bool Renderer::GetDepthMask() const
{
    return m_depthMask;
}
Material* Renderer::GetSharedMaterial() const
{
    return p_sharedMaterial.Get();
}

void Renderer::OnResourceChanged(Resource*)
{
    PropagateRendererChanged();
}

AABox Renderer::GetAABBox() const
{
    return AABox::Empty;
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
Material* Renderer::GetMaterial() const
{
    if (!p_material)
    {
        if (GetSharedMaterial())
        {
            p_material = Resources::Clone<Material>(GetSharedMaterial());
            p_material.Get()->EventEmitter<IEventsResource>::
                              RegisterListener(const_cast<Renderer*>(this));
        }
    }
    return p_material.Get();
}

AARect Renderer::GetBoundingRect(Camera *camera) const
{
    return AARect::NDCRect;
    return camera ? camera->GetViewportBoundingAARectNDC(GetAABBox()) :
                    AARect::Zero;
}

void Renderer::PropagateRendererChanged()
{
    EventEmitter<IEventsRendererChanged>::PropagateToListeners(
                &IEventsRendererChanged::OnRendererChanged, this);
}

Matrix4 Renderer::GetModelMatrixUniform() const
{
    return GetGameObject()->GetTransform() ?
                    GetGameObject()->GetTransform()->GetLocalToWorldMatrix() :
                    Matrix4::Identity;
}

void Renderer::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Renderer *r = Cast<Renderer*>(clone);
    r->SetMaterial(GetSharedMaterial());
    r->SetCastsShadows(GetCastsShadows());
    r->SetReceivesShadows(GetReceivesShadows());
    r->SetRenderPrimitive(GetRenderPrimitive());
    r->SetUseReflectionProbes( GetUseReflectionProbes() );
}

void Renderer::ImportMeta(const MetaNode &meta)
{
    Component::ImportMeta(meta);

    if (meta.Contains("DepthMask"))
    {
        SetDepthMask( meta.Get<bool>("DepthMask") );
    }

    if (meta.Contains("Visible"))
    {
        SetVisible( meta.Get<bool>("Visible") );
    }

    if (meta.Contains("Material"))
    {
        SetMaterial(Resources::Load<Material>(meta.Get<GUID>("Material")).Get());
    }

    if (meta.Contains("UseReflectionProbes"))
    {
        SetUseReflectionProbes(meta.Get<bool>("UseReflectionProbes"));
    }

    if (meta.Contains("CastsShadows"))
    {
        SetCastsShadows(meta.Get<bool>("CastsShadows"));
    }

    if (meta.Contains("ReceivesShadows"))
    {
        SetReceivesShadows(meta.Get<bool>("ReceivesShadows"));
    }
}

void Renderer::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("Visible", IsVisible());
    metaNode->Set("DepthMask", GetDepthMask());

    Material* sMat = GetSharedMaterial();
    metaNode->Set("Material", sMat ? sMat->GetGUID() : GUID::Empty());
    metaNode->Set("CastsShadows", GetCastsShadows());
    metaNode->Set("ReceivesShadows", GetReceivesShadows());
    metaNode->Set("UseReflectionProbes", GetUseReflectionProbes());
}
