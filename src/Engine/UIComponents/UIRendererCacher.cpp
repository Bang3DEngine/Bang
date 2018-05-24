#include "Bang/UIRendererCacher.h"

#include "Bang/GL.h"
#include "Bang/Rect.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GameObject.h"
#include "Bang/UIRenderer.h"
#include "Bang/Framebuffer.h"
#include "Bang/RectTransform.h"
#include "Bang/ShaderProgram.h"
#include "Bang/MaterialFactory.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

UIRendererCacher::UIRendererCacher()
{
    p_cacheFramebuffer = new Framebuffer(1, 1);
    p_cacheFramebuffer->CreateAttachmentTex2D(GL::Attachment::COLOR0,
                                              GL::ColorFormat::RGBA8);
    p_cacheFramebuffer->CreateAttachmentTex2D(GL::Attachment::DEPTH,
                                              GL::ColorFormat::DEPTH16);
}

UIRendererCacher::~UIRendererCacher()
{
    delete p_cacheFramebuffer;
}

void UIRendererCacher::OnStart()
{
    // Prepare image renderer
    Texture2D *tex = p_cacheFramebuffer->GetAttachmentTex2D(GL::Attachment::COLOR0);
    if (p_cachedImageRenderer)
    {
        tex->SetWrapMode(GL::WrapMode::REPEAT);
        p_cachedImageRenderer->SetImageTexture(tex);
        p_cachedImageRenderer->SetTint(Color::White);
        p_cachedImageRenderer->GetImageTexture()->SetAlphaCutoff(1.0f);
        p_cachedImageRenderer->SetMode(UIImageRenderer::Mode::TEXTURE);
        p_cachedImageRenderer->GetImageTexture()->SetFilterMode(
                                                GL::FilterMode::NEAREST);
        SetCachingEnabled(true);
    }
}

void UIRendererCacher::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);

    if (renderPass == RenderPass::CANVAS)
    {
        if (IsCachingEnabled() && m_needNewImageToSnapshot)
        {
            // Save previous state
            GLId prevBoundDrawFramebuffer = GL::GetBoundId(GL::BindTarget::DRAW_FRAMEBUFFER);
            GLId prevBoundReadFramebuffer = GL::GetBoundId(GL::BindTarget::READ_FRAMEBUFFER);
            GL::BlendFactor prevBlendSrcFactorColor   = GL::GetBlendSrcFactorColor();
            GL::BlendFactor prevBlendDstFactorColor   = GL::GetBlendDstFactorColor();
            GL::BlendFactor prevBlendSrcFactorAlpha   = GL::GetBlendSrcFactorAlpha();
            GL::BlendFactor prevBlendDstFactorAlpha   = GL::GetBlendDstFactorAlpha();
            Array<GL::Attachment> prevDrawAttachments = GL::GetDrawBuffers();
            GL::Attachment prevReadAttachment         = GL::GetReadBuffer();
            bool wasBlendEnabled                      = GL::IsEnabled(GL::Enablable::BLEND);

            p_cacheFramebuffer->Bind();

            GBuffer *gbuffer = GEngine::GetActiveRenderingCamera()->GetGBuffer();
            AARect rtRectNDC(GetGameObject()->GetRectTransform()->GetViewportAARectNDC());
            p_cacheFramebuffer->Resize(gbuffer->GetWidth(), gbuffer->GetHeight());

            GL::ReadBuffer( GBuffer::AttColor );
            GL::DrawBuffers( {GL::Attachment::COLOR0} );
            GL::ClearColorBuffer(Color::Zero);
            GL::ClearDepthBuffer(1.0f);
            GL::ClearStencilBuffer(0);

            Vector2 rtSize = rtRectNDC.GetSize();
            Vector2 rtOri = rtRectNDC.GetMinXMinY() * 0.5f + 0.5f;
            Vector2 rtOriInvY = Vector2(rtOri.x, rtOri.y);
            p_cachedImageRenderer->GetActiveMaterial()->SetAlbedoUvOffset( rtOriInvY );
            p_cachedImageRenderer->GetActiveMaterial()->SetAlbedoUvMultiply( rtSize * 0.5f );

            SetContainerVisible(true);
            p_cachedImageRenderer->SetVisible(false);
            GL::BlendFuncSeparate(GL::BlendFactor::SRC_ALPHA,
                                  GL::BlendFactor::ONE_MINUS_SRC_ALPHA,
                                  GL::BlendFactor::ONE,
                                  GL::BlendFactor::ONE_MINUS_SRC_ALPHA);
            GetContainer()->Render(RenderPass::CANVAS);
            p_cachedImageRenderer->SetVisible(true);
            SetContainerVisible(false);

            // Restore gl state
            GL::Bind(GL::BindTarget::DRAW_FRAMEBUFFER, prevBoundDrawFramebuffer);
            GL::Bind(GL::BindTarget::READ_FRAMEBUFFER, prevBoundReadFramebuffer);
            GL::DrawBuffers(prevDrawAttachments);
            GL::ReadBuffer(prevReadAttachment);
            GL::BlendFuncSeparate(prevBlendSrcFactorColor,
                                  prevBlendDstFactorColor,
                                  prevBlendSrcFactorAlpha,
                                  prevBlendDstFactorAlpha);
            GL::SetEnabled(GL::Enablable::BLEND, wasBlendEnabled, false);

            m_needNewImageToSnapshot = false;
        }
        else if (!IsCachingEnabled())
        {
            SetContainerVisible(true);
            p_cachedImageRenderer->SetVisible(false);
        }
    }
}

void UIRendererCacher::OnAfterChildrenRender(RenderPass renderPass)
{
    Component::OnAfterChildrenRender(renderPass);
}

void UIRendererCacher::SetCachingEnabled(bool enabled)
{
    if (enabled != IsCachingEnabled())
    {
        m_cachingEnabled = enabled;
        p_cachedImageRenderer->SetEnabled( IsCachingEnabled() );
    }
}

bool UIRendererCacher::IsCachingEnabled() const
{
    return m_cachingEnabled;
}

GameObject *UIRendererCacher::GetContainer() const
{
    return p_uiRenderersContainer;
}

void UIRendererCacher::OnChanged()
{
    m_needNewImageToSnapshot = true;
}

void UIRendererCacher::OnChildAdded(GameObject*, GameObject*)
{
    List<GameObject*> children = GetContainer()->GetChildrenRecursively();
    for (GameObject *child : children)
    {
        List<Renderer*> renderers = child->GetComponents<Renderer>();
        for (Renderer *rend : renderers)
        {
            rend->EventEmitter<IRendererChangedListener>::RegisterListener(this);
        }

        if (child->GetTransform())
        {
            child->GetTransform()->EventEmitter<ITransformListener>::RegisterListener(this);
        }

        child->EventEmitter<IObjectListener>::RegisterListener(this);
        child->EventEmitter<IChildrenListener>::RegisterListener(this);
        child->EventEmitter<IGameObjectVisibilityChangedListener>::RegisterListener(this);
    }

    OnChanged();
}

void UIRendererCacher::OnChildRemoved(GameObject *removedChild, GameObject*)
{
    List<GameObject*> children = removedChild->GetChildrenRecursively();
    children.PushBack(removedChild);
    for (GameObject *child : children)
    {
        List<Renderer*> renderers = child->GetComponents<Renderer>();
        for (Renderer *rend : renderers)
        {
            rend->EventEmitter<IRendererChangedListener>::UnRegisterListener(this);
        }
        child->EventEmitter<IChildrenListener>::UnRegisterListener(this);
        child->EventEmitter<IGameObjectVisibilityChangedListener>::UnRegisterListener(this);
    }

    OnChanged();
}

void UIRendererCacher::OnTransformChanged()
{
    OnChanged();
}

void UIRendererCacher::OnRendererChanged(Renderer*)
{
    OnChanged();
}

void UIRendererCacher::OnStarted()
{
    Component::OnStarted();
    OnChanged();
}

void UIRendererCacher::OnEnabled()
{
    Component::OnEnabled();
    OnChanged();
}

void UIRendererCacher::OnDisabled()
{
    Component::OnDisabled();
    OnChanged();
}

void UIRendererCacher::SetContainerVisible(bool visible)
{
    SetReceiveEvents(false);
    GetContainer()->SetVisible(visible);
    SetReceiveEvents(true);
}

UIRendererCacher* UIRendererCacher::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIRendererCacher *rendererCacher = go->AddComponent<UIRendererCacher>();
    UIImageRenderer *cacherImageRend = go->AddComponent<UIImageRenderer>();

    GameObject *container = GameObjectFactory::CreateUIGameObject();

    rendererCacher->p_cachedImageRenderer = cacherImageRend;
    rendererCacher->p_uiRenderersContainer = container;

    container->SetParent(go);
    container->EventEmitter<IChildrenListener>::RegisterListener(rendererCacher);
    rendererCacher->OnChildAdded(container, go);

    return rendererCacher;
}

void UIRendererCacher::OnVisibilityChanged(GameObject*)
{
    OnRendererChanged(nullptr);
}
