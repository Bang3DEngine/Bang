#include "Bang/UIDragDroppable.h"

#include "Bang/GL.h"
#include "Bang/Input.h"
#include "Bang/Scene.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/Material.h"
#include "Bang/UICanvas.h"
#include "Bang/GameObject.h"
#include "Bang/Framebuffer.h"
#include "Bang/UIFocusable.h"
#include "Bang/SceneManager.h"
#include "Bang/RectTransform.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutIgnorer.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

const Time UIDragDroppable::DragInitTime = Time::Seconds(0.25);

UIDragDroppable::UIDragDroppable()
{
    CONSTRUCT_CLASS_ID(UIDragDroppable);
    m_pressTime.SetInfinity();
}

UIDragDroppable::~UIDragDroppable()
{
}

void UIDragDroppable::OnUpdate()
{
    Component::OnUpdate();

    if ( GetFocusable() )
    {
        const Array<InputEvent> &events = Input::GetEnqueuedEvents();
        for (const InputEvent &inputEvent : events)
        {
            if (GetFocusable()->IsMouseOver())
            {
                if (inputEvent.type == InputEvent::Type::MOUSE_DOWN &&
                    inputEvent.mouseButton == MouseButton::LEFT)
                {
                    m_pressTime = inputEvent.timestamp;

                    RectTransform *thisRT = GetGameObject()->GetRectTransform();
                    const AARecti thisRect( thisRT->GetViewportAARect() );
                    m_dragGrabOffset = (inputEvent.mousePosWindow -
                                        thisRect.GetMin());
                }
            }

            if (inputEvent.type == InputEvent::Type::MOUSE_UP &&
                inputEvent.mouseButton == MouseButton::LEFT)
            {
                m_pressTime.SetInfinity();
            }

        }

        if (!IsBeingDragged() && GetFocusable()->IsBeingPressed())
        {
            Time passedPressedTime = Time::GetPassedTimeSince(m_pressTime);
            if (passedPressedTime >= UIDragDroppable::DragInitTime)
            {
                OnDragStarted();
            }
        }
    }
}

void UIDragDroppable::SetFocusable(UIFocusable *focusable)
{
    if (focusable != GetFocusable())
    {
        if (GetFocusable())
        {
            GetFocusable()->EventEmitter<IEventsFocus>::UnRegisterListener(this);
        }

        p_focusable = focusable;

        if (GetFocusable())
        {
            GetFocusable()->EventEmitter<IEventsFocus>::RegisterListener(this);
        }
    }
}

void UIDragDroppable::SetShowDragDropGameObject(bool showDragDropObject)
{
    m_showDragDropGameObject = showDragDropObject;
    if (!GetShowDragDropGameObject() && m_dragDropGameObject)
    {
        GameObject::Destroy(m_dragDropGameObject);
    }
}

bool UIDragDroppable::IsBeingDragged() const
{
    return m_beingDragged;
}

UIFocusable* UIDragDroppable::GetFocusable() const
{
    return p_focusable;
}

bool UIDragDroppable::GetShowDragDropGameObject() const
{
    return m_showDragDropGameObject;
}

void UIDragDroppable::OnDragStarted()
{
    if (!IsBeingDragged())
    {
        m_beingDragged = true;

        UICanvas *canvas = UICanvas::GetActive(this);
        if (canvas)
        {
            canvas->NotifyDragStarted(this);
            EventEmitter<IEventsDragDrop>::PropagateToListeners(
                                &IEventsDragDrop::OnDragStarted, this);
        }

        if (GetShowDragDropGameObject())
        {
            if (!m_dragDropGameObject)
            {
                m_dragDropGameObject = GameObjectFactory::CreateUIGameObject();
                m_dragDropGameObject->AddComponent<UILayoutIgnorer>();
                m_dragDropGameObject->Start();

                p_dragDropImageRenderer = m_dragDropGameObject->
                                          AddComponent<UIImageRenderer>();
                p_dragDropImageRenderer->SetTint(Color::White.WithAlpha(0.5f));
                p_dragDropImageRenderer->SetMode(UIImageRenderer::Mode::TEXTURE);

                if (canvas)
                {
                    m_dragDropGameObject->SetParent(canvas->GetGameObject());
                }
            }
            MoveDragDropGameObjectTo( Input::GetMousePosition() );
        }
    }
}

void UIDragDroppable::OnDragUpdate()
{
    if (m_dragDropGameObject)
    {
        MoveDragDropGameObjectTo( Input::GetMousePosition() );
        EventEmitter<IEventsDragDrop>::PropagateToListeners(
                            &IEventsDragDrop::OnDragUpdate, this);
    }
}

void UIDragDroppable::OnDropped()
{
    if (IsBeingDragged())
    {
        m_beingDragged = false;
        m_pressTime.SetInfinity();

        EventEmitter<IEventsDragDrop>::PropagateToListeners(
                            &IEventsDragDrop::OnDrop, this);

        if (m_dragDropGameObject)
        {
            GameObject::Destroy(m_dragDropGameObject);
            m_dragDropGameObject = nullptr;
            p_dragDropImageRenderer = nullptr;
        }
    }
}

UIEventResult UIDragDroppable::OnUIEvent(UIFocusable *focusable,
                                         const UIEvent &event)
{
    ASSERT(GetFocusable() && focusable == GetFocusable());

    // if (event.type == UIEvent::Type::MOUSE_CLICK_DOWN)
    // {
    //     OnDragStarted();
    //     return UIEventResult::INTERCEPT;
    // }
    return UIEventResult::IGNORE;
}

void UIDragDroppable::MoveDragDropGameObjectTo(const Vector2i &pos)
{
    if (m_dragDropGameObject)
    {
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
}

void UIDragDroppable::OnDestroy()
{
    Object::OnDestroy();
    if (IsBeingDragged())
    {
        OnDropped();
    }
}
