#include "Bang/UIRenderer.h"

#include "Bang/Rect.h"
#include "Bang/AARect.h"
#include "Bang/GameObject.h"
#include "Bang/RectTransform.h"

USING_NAMESPACE_BANG

UIRenderer::UIRenderer()
{
    SetViewProjMode(GL::ViewProjMode::CANVAS);
    SetCullByRectTransform(false);
}

UIRenderer::~UIRenderer()
{
}

void UIRenderer::OnRender()
{
    Renderer::OnRender();
}

void UIRenderer::OnRender(RenderPass renderPass)
{
    bool render = true;
    if (GetCullByRectTransform())
    {
        if (RectTransform *rt = GetGameObject()->GetRectTransform())
        {
            const AARect rectNDC = rt->GetViewportAARectNDC();
            render = (rectNDC != AARect::Zero &&
                      AARect::Intersection(AARect::NDCRect, rectNDC) != AARect::Zero);
        }
    }

    if (render) { Renderer::OnRender(renderPass); }
}

void UIRenderer::SetCullByRectTransform(bool cullByRectTransform)
{
    m_cullByRectTransform = cullByRectTransform;
}

bool UIRenderer::GetCullByRectTransform() const
{
    return m_cullByRectTransform;
}

void UIRenderer::OnEnabled(Object *object)
{
    Renderer::OnEnabled(object);
    PropagateRendererChanged();
}

void UIRenderer::OnDisabled(Object *object)
{
    Renderer::OnDisabled(object);
    PropagateRendererChanged();
}

void UIRenderer::OnTransformChanged()
{
    PropagateRendererChanged();
}

void UIRenderer::OnChildAdded(GameObject*, GameObject*)
{
    PropagateRendererChanged();
}

void UIRenderer::OnChildRemoved(GameObject*, GameObject*)
{
    PropagateRendererChanged();
}

void UIRenderer::OnParentChanged(GameObject*, GameObject*)
{
    PropagateRendererChanged();
}

AARect UIRenderer::GetBoundingRect(Camera *camera) const
{
    GameObject *go = GetGameObject();
    RectTransform *rt = go ? go->GetRectTransform() : nullptr;
    return rt ? (rt->GetViewportAARect()) : AARect::Zero;
}
