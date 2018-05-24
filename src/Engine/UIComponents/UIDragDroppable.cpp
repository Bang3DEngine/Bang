#include "Bang/UIDragDroppable.h"

#include "Bang/GL.h"
#include "Bang/Input.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/Material.h"
#include "Bang/UICanvas.h"
#include "Bang/GameObject.h"
#include "Bang/IFocusable.h"
#include "Bang/Framebuffer.h"
#include "Bang/RectTransform.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutIgnorer.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

const float UIDragDroppable::DragInitTime = 0.3f;

UIDragDroppable::UIDragDroppable()
{
}

UIDragDroppable::~UIDragDroppable()
{
}

void UIDragDroppable::OnUpdate()
{
    Component::OnUpdate();

    // Drag start detection here

    if (GetFocusable()) { m_beingPressed = GetFocusable()->IsBeingPressed(); }
    else
    {
        if (Input::GetMouseButtonDown(MouseButton::LEFT))
        {
            RectTransform *thisRT = GetGameObject()->GetRectTransform();
            const AARecti thisRect( thisRT->GetViewportAARect() );
            m_dragGrabOffset = (Input::GetMousePosition() - thisRect.GetMin());
            m_beingPressed = GetGameObject()->GetRectTransform()->IsMouseOver();
        }
    }

    if (Input::GetMouseButtonUp(MouseButton::LEFT))
    {
        m_beingPressed = false;
    }

    if (m_beingPressed)
    {
        if (m_secondsSinceMouseIsDown < UIDragDroppable::DragInitTime)
        {
            m_secondsSinceMouseIsDown += Time::GetDeltaTime();
            if (m_secondsSinceMouseIsDown >= UIDragDroppable::DragInitTime)
            {
                OnDragStarted();
            }
        }
    }
    else
    {
        m_secondsSinceMouseIsDown = 0.0f;
    }
}

void UIDragDroppable::SetFocusable(IFocusable *focusable)
{
    if (focusable != GetFocusable())
    {
        if (GetFocusable())
        {
            GetFocusable()->EventEmitter<IFocusListener>::UnRegisterListener(this);
        }

        p_focusable = focusable;

        if (GetFocusable())
        {
            GetFocusable()->EventEmitter<IFocusListener>::RegisterListener(this);
        }
    }
}

void UIDragDroppable::SetShowDragDropGameObject(bool showDragDropObject)
{
    m_showDragDropGameObject = showDragDropObject;

    if (!IsShowDragDropGameObject() && m_dragDropGameObject)
    {
        GameObject::Destroy(m_dragDropGameObject);
    }
}

bool UIDragDroppable::IsBeingDragged() const
{
    return m_beingDragged;
}

IFocusable *UIDragDroppable::GetFocusable() const
{
    return p_focusable;
}

bool UIDragDroppable::IsShowDragDropGameObject() const
{
    return m_showDragDropGameObject;
}

void UIDragDroppable::OnDragStarted()
{
    m_beingDragged = true;

    UICanvas *canvas = UICanvas::GetActive(this);
    if (!canvas) { return; }

    canvas->NotifyDragStarted(this);

    /*
    if (!m_dragDropFB)
    {
        m_dragDropFB = new Framebuffer();
        m_dragDropFB->CreateAttachmentTex2D(GL::Attachment::Color0,
                                            GL::ColorFormat::RGBA_UByte8);
        m_dragDropFB->CreateAttachmentTex2D(GL::Attachment::Depth,
                                            GL::ColorFormat::Depth16);

        m_dragDropTexture.Set(m_dragDropFB->GetAttachmentTex2D(GL::Attachment::Color0));
        m_dragDropTexture.Get()->SetWrapMode(GL::WrapMode::Repeat);
        m_dragDropTexture.Get()->SetAlphaCutoff(1.0f);
        m_dragDropTexture.Get()->SetFilterMode(GL::FilterMode::Nearest);

        Vector2 fbSize = thisRT->GetViewportAARect().GetSize();
        m_dragDropFB->Resize(fbSize.x, fbSize.y);
    }*/

    if (IsShowDragDropGameObject())
    {
        m_dragDropGameObject = GameObjectFactory::CreateUIGameObject();
        m_dragDropGameObject->AddComponent<UILayoutIgnorer>();
        m_dragDropGameObject->Start();

        MoveDragDropGameObjectTo( Input::GetMousePosition() );

        p_dragDropImageRenderer = m_dragDropGameObject->AddComponent<UIImageRenderer>();
        // p_dragDropImageRenderer->SetImageTexture(m_dragDropTexture.Get());
        p_dragDropImageRenderer->SetTint(Color::White.WithAlpha(0.5f));
        p_dragDropImageRenderer->SetMode(UIImageRenderer::Mode::TEXTURE);

        m_dragDropGameObject->SetParent(canvas->GetGameObject());
    }

    /*
    // Save previous state
    GLId prevBoundDrawFramebuffer = GL::GetBoundId(GL::BindTarget::DrawFramebuffer);
    GLId prevBoundReadFramebuffer = GL::GetBoundId(GL::BindTarget::ReadFramebuffer);
    GL::BlendFactor prevBlendSrcFactorColor   = GL::GetBlendSrcFactorColor();
    GL::BlendFactor prevBlendDstFactorColor   = GL::GetBlendDstFactorColor();
    GL::BlendFactor prevBlendSrcFactorAlpha   = GL::GetBlendSrcFactorAlpha();
    GL::BlendFactor prevBlendDstFactorAlpha   = GL::GetBlendDstFactorAlpha();
    Array<GL::Attachment> prevDrawAttachments = GL::GetDrawBuffers();
    GL::Attachment prevReadAttachment         = GL::GetReadBuffer();
    bool wasBlendEnabled                      = GL::IsEnabled(GL::Enablable::Blend);

    m_dragDropFB->Bind();

    Camera *cam = GEngine::GetActiveRenderingCamera();
    GBuffer *gbuffer = GEngine::GetActiveRenderingCamera()->GetGBuffer();
    AARect rtRectNDC(GetGameObject()->GetRectTransform()->GetViewportAARectNDC());
    m_dragDropFB->Resize(gbuffer->GetWidth(), gbuffer->GetHeight());

    GL::ReadBuffer( GBuffer::AttColor );
    GL::DrawBuffers( {GL::Attachment::Color0} );
    GL::ClearColorBuffer(Color::Zero);
    GL::ClearDepthBuffer(1.0f);
    GL::ClearStencilBuffer(0);

    Vector2 rtSize = rtRectNDC.GetSize();
    Vector2 rtOri = rtRectNDC.GetMinXMinY() * 0.5f + 0.5f;
    Vector2 rtOriInvY = Vector2(rtOri.x, rtOri.y);
    p_dragDropImageRenderer->GetActiveMaterial()->SetAlbedoUvOffset( rtOriInvY );
    p_dragDropImageRenderer->GetActiveMaterial()->SetAlbedoUvMultiply( rtSize * 0.5f );

    GL::BlendFuncSeparate(GL::BlendFactor::SrcAlpha,
                          GL::BlendFactor::OneMinusSrcAlpha,
                          GL::BlendFactor::One,
                          GL::BlendFactor::OneMinusSrcAlpha);
    GetGameObject()->Render(RenderPass::Canvas);

    // Restore gl state
    GL::Bind(GL::BindTarget::DrawFramebuffer, prevBoundDrawFramebuffer);
    GL::Bind(GL::BindTarget::ReadFramebuffer, prevBoundReadFramebuffer);
    GL::DrawBuffers(prevDrawAttachments);
    GL::ReadBuffer(prevReadAttachment);
    GL::BlendFuncSeparate(prevBlendSrcFactorColor,
                          prevBlendDstFactorColor,
                          prevBlendSrcFactorAlpha,
                          prevBlendDstFactorAlpha);
    GL::SetEnabled(GL::Enablable::Blend, wasBlendEnabled, false);
    */
}

void UIDragDroppable::OnDragUpdate()
{
    MoveDragDropGameObjectTo( Input::GetMousePosition() );
}

void UIDragDroppable::OnDropped()
{
    m_beingDragged = false;

    if (m_dragDropGameObject)
    {
        GameObject::Destroy(m_dragDropGameObject);
        m_dragDropGameObject = nullptr;
        p_dragDropImageRenderer = nullptr;
    }
}

void UIDragDroppable::OnClicked(IFocusable *focusable, ClickType clickType)
{
    ASSERT(GetFocusable() && focusable == GetFocusable());
    IFocusListener::OnClicked(focusable, clickType);

    if (clickType == ClickType::DOWN)
    {
        OnDragStarted();
    }
}

void UIDragDroppable::MoveDragDropGameObjectTo(const Vector2i &pos)
{
    if (!m_dragDropGameObject) { return; }

    RectTransform *thisRT = GetGameObject()->GetRectTransform();
    AARecti thisRect( thisRT->GetViewportAARect() );

    Vector2i offsetedPos = pos - m_dragGrabOffset;
    RectTransform *ddGoRT = m_dragDropGameObject->GetRectTransform();
    ddGoRT->SetPivotPosition(Vector2::Zero);
    ddGoRT->SetMarginLeftBot( offsetedPos );
    ddGoRT->SetMarginRightTop( -ddGoRT->GetMarginLeftBot() +
                               -thisRect.GetSize());
    ddGoRT->SetAnchorMin( -Vector2::One );
    ddGoRT->SetAnchorMax( -Vector2::One );
    ddGoRT->SetPosition( Vector3::Zero ); // thisRT->GetPosition() );
    ddGoRT->SetRotation( thisRT->GetRotation() );
    ddGoRT->SetScale( thisRT->GetScale() );
    ddGoRT->TranslateLocal( Vector3(0, 0, -0.3f) );
}

