#include "Bang/UIRendererCacher.h"

#include "Bang/Array.h"
#include "Bang/Camera.h"
#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/Framebuffer.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsComponent.h"
#include "Bang/IEventsGameObjectVisibilityChanged.h"
#include "Bang/IEventsRendererChanged.h"
#include "Bang/IEventsTransform.h"
#include "Bang/Material.h"
#include "Bang/RectTransform.h"
#include "Bang/Renderer.h"
#include "Bang/Texture2D.h"
#include "Bang/Transform.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/Vector2.h"

namespace Bang
{
class IEventsObject;
class Object;
}  // namespace Bang

using namespace Bang;

UIRendererCacher::UIRendererCacher()
{
    SET_INSTANCE_CLASS_ID(UIRendererCacher)

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
    Component::OnStart();

    // Prepare image renderer
    Texture2D *tex =
        p_cacheFramebuffer->GetAttachmentTex2D(GL::Attachment::COLOR0);
    if (p_cachedImageRenderer)
    {
        tex->SetWrapMode(GL::WrapMode::REPEAT);
        p_cachedImageRenderer->SetImageTexture(tex);
        p_cachedImageRenderer->SetTint(Color::White());
        p_cachedImageRenderer->GetImageTexture()->SetAlphaCutoff(1.0f);
        p_cachedImageRenderer->SetMode(UIImageRenderer::Mode::TEXTURE);
        p_cachedImageRenderer->GetImageTexture()->SetFilterMode(
            GL::FilterMode::NEAREST);
    }
}

void UIRendererCacher::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);

    if (renderPass == RenderPass::CANVAS)
    {
        if (IsCachingEnabled() && m_needNewImageToSnapshot)
        {
            GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
            GL::Push(GL::Pushable::BLEND_STATES);

            p_cacheFramebuffer->Bind();

            GBuffer *gbuffer =
                GEngine::GetActiveRenderingCamera()->GetGBuffer();
            AARect rtRectNDC(
                GetGameObject()->GetRectTransform()->GetViewportAARectNDC());
            p_cacheFramebuffer->Resize(gbuffer->GetWidth(),
                                       gbuffer->GetHeight());

            GL::ReadBuffer(GBuffer::AttColor);
            GL::DrawBuffers({GL::Attachment::COLOR0});
            GL::ClearColorStencilDepthBuffers();

            Vector2 rtSize = rtRectNDC.GetSize();
            Vector2 rtOri = rtRectNDC.GetMinXMinY() * 0.5f + 0.5f;
            Vector2 rtOriInvY = Vector2(rtOri.x, rtOri.y);
            p_cachedImageRenderer->GetActiveMaterial()->SetAlbedoUvOffset(
                rtOriInvY);
            p_cachedImageRenderer->GetActiveMaterial()->SetAlbedoUvMultiply(
                rtSize * 0.5f);

            SetContainerVisible(true);
            p_cachedImageRenderer->SetVisible(false);
            GL::BlendFuncSeparate(GL::BlendFactor::SRC_ALPHA,
                                  GL::BlendFactor::ONE_MINUS_SRC_ALPHA,
                                  GL::BlendFactor::ONE,
                                  GL::BlendFactor::ONE_MINUS_SRC_ALPHA);
            GetContainer()->Render(renderPass);

            GL::Pop(GL::Pushable::BLEND_STATES);
            GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

            m_needNewImageToSnapshot = false;
            p_cachedImageRenderer->SetVisible(true);
            SetContainerVisible(false);
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
        p_cachedImageRenderer->SetEnabled(IsCachingEnabled());
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

void UIRendererCacher::OnComponentAdded(Component *addedComponent, int)
{
    if (Renderer *rend = DCAST<Renderer *>(addedComponent))
    {
        rend->EventEmitter<IEventsRendererChanged>::RegisterListener(this);
    }
}

void UIRendererCacher::OnComponentRemoved(Component *removedComponent,
                                          GameObject *)
{
    if (Renderer *rend = DCAST<Renderer *>(removedComponent))
    {
        rend->EventEmitter<IEventsRendererChanged>::UnRegisterListener(this);
    }
}

void UIRendererCacher::OnChildAdded(GameObject *, GameObject *)
{
    Array<GameObject *> children = GetContainer()->GetChildrenRecursively();
    for (GameObject *child : children)
    {
        Array<Renderer *> renderers = child->GetComponents<Renderer>();
        for (Renderer *rend : renderers)
        {
            rend->EventEmitter<IEventsRendererChanged>::RegisterListener(this);
        }

        if (child->GetTransform())
        {
            child->GetTransform()
                ->EventEmitter<IEventsTransform>::RegisterListener(this);
        }

        child->EventEmitter<IEventsObject>::RegisterListener(this);
        child->EventEmitter<IEventsChildren>::RegisterListener(this);
        child->EventEmitter<IEventsComponent>::RegisterListener(this);
        child->EventEmitter<
            IEventsGameObjectVisibilityChanged>::RegisterListener(this);
    }

    OnChanged();
}

void UIRendererCacher::OnChildRemoved(GameObject *removedChild, GameObject *)
{
    Array<GameObject *> children = removedChild->GetChildrenRecursively();
    children.PushBack(removedChild);
    for (GameObject *child : children)
    {
        if (child)
        {
            Array<Renderer *> renderers = child->GetComponents<Renderer>();
            for (Renderer *rend : renderers)
            {
                rend->EventEmitter<IEventsRendererChanged>::UnRegisterListener(
                    this);
            }
            child->EventEmitter<IEventsChildren>::UnRegisterListener(this);
            child->EventEmitter<IEventsComponent>::UnRegisterListener(this);
            child->EventEmitter<
                IEventsGameObjectVisibilityChanged>::UnRegisterListener(this);
        }
    }

    OnChanged();
}

void UIRendererCacher::OnTransformChanged()
{
    OnChanged();
}

void UIRendererCacher::OnRendererChanged(Renderer *)
{
    OnChanged();
}

void UIRendererCacher::OnStarted(Object *object)
{
    Component::OnStarted(object);
    OnChanged();
}

void UIRendererCacher::OnEnabled(Object *object)
{
    Component::OnEnabled(object);
    OnChanged();
}

void UIRendererCacher::OnDisabled(Object *object)
{
    Component::OnDisabled(object);
    OnChanged();
}

void UIRendererCacher::SetContainerVisible(bool visible)
{
    EventListener<IEventsGameObjectVisibilityChanged>::SetReceiveEvents(false);
    GetContainer()->SetVisible(visible);
    EventListener<IEventsGameObjectVisibilityChanged>::SetReceiveEvents(true);
}

UIRendererCacher *UIRendererCacher::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIRendererCacher *rendererCacher = go->AddComponent<UIRendererCacher>();
    UIImageRenderer *cacherImageRend = go->AddComponent<UIImageRenderer>();

    GameObject *container = GameObjectFactory::CreateUIGameObject();

    rendererCacher->p_cachedImageRenderer = cacherImageRend;
    rendererCacher->p_uiRenderersContainer = container;

    container->SetParent(go);
    // container->EventEmitter<IEventsChildren>::RegisterListener(rendererCacher);
    // rendererCacher->OnChildAdded(container, go);

    return rendererCacher;
}

void UIRendererCacher::OnVisibilityChanged(GameObject *)
{
    OnRendererChanged(nullptr);
}
