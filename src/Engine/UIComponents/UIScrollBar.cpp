#include "Bang/UIScrollBar.h"

#include "Bang/Assert.h"
#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/Cursor.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/Input.h"
#include "Bang/Math.h"
#include "Bang/MouseButton.h"
#include "Bang/Rect.h"
#include "Bang/RectTransform.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIScrollArea.h"
#include "Bang/Vector2.h"

using namespace Bang;

UIScrollBar::UIScrollBar()
{
    SET_INSTANCE_CLASS_ID(UIScrollBar)
}

UIScrollBar::~UIScrollBar()
{
}

void UIScrollBar::OnUpdate()
{
    Component::OnUpdate();

    if (IsBeingGrabbed())
    {
        p_barImg->SetTint(Color::White().WithValue(0.8f));

        Vector2 mouseCoordsPx(Input::GetMousePosition());
        AARect scrollRectPx = GetScrollingRect();
        AARect barRectPx(GetBar()->GetRectTransform()->GetViewportRect());
        if (!m_wasGrabbed)
        {
            m_grabOffsetPx = (mouseCoordsPx - barRectPx.GetMin());
        }

        Vector2 offsettedMouseCoordsPxRel = mouseCoordsPx - m_grabOffsetPx;
        Vector2 emptySpacePx = scrollRectPx.GetSize() - barRectPx.GetSize();
        emptySpacePx = Vector2::Max(emptySpacePx, Vector2::One());

        Vector2 mousePercent =
            ((emptySpacePx != Vector2::Zero())
                 ? Vector2(offsettedMouseCoordsPxRel - scrollRectPx.GetMin()) /
                       emptySpacePx
                 : Vector2::Zero());
        mousePercent.y = 1.0f - mousePercent.y;
        float scrollPercent = mousePercent.GetAxis(GetScrollAxis());
        scrollPercent = Math::Clamp(scrollPercent, 0.0f, 1.0f);

        SetScrollingPercent(scrollPercent);
    }
    m_wasGrabbed = IsBeingGrabbed();

    if (Input::GetMouseButtonUp(MouseButton::LEFT))
    {
        if (GetFocusable()->IsMouseOver())
        {
            OnMouseEnter();
        }
        else
        {
            OnMouseExit();
        }
    }
}

void UIScrollBar::SetSide(Side side)
{
    if (side != GetSide())
    {
        m_side = side;
        UpdateLengthThicknessMargins();
    }
}

void UIScrollBar::SetScrolling(int _scrollingPx)
{
    int scrollingPx = Math::Clamp(_scrollingPx, 0, GetScrollingSpacePx());
    m_scrollingPx = scrollingPx;

    Vector2i scrolling = (GetScrollAxis() == Axis::VERTICAL)
                             ? Vector2i(0, -scrollingPx)
                             : Vector2i(scrollingPx, 0);
    GetScrollArea()->SetScrolling(scrolling);
}

void UIScrollBar::SetScrollingPercent(float _percent)
{
    float percent = Math::Clamp(_percent, 0.0f, 1.0f);
    int scrollingPx = Math::Round<int>(percent * GetScrollingSpacePx());
    SetScrolling(scrollingPx);
}

void UIScrollBar::SetLength(int lengthPx)
{
    if (lengthPx != GetLength())
    {
        m_length = lengthPx;
        UpdateLengthThicknessMargins();
    }
}

void UIScrollBar::SetLengthPercent(float lengthPercent)
{
    Vector2i length(
        Vector2::Round(Vector2(GetScrollingRect().GetSize()) * lengthPercent));
    SetLength(GetScrollAxis() == Axis::VERTICAL ? length.y : length.x);
}

void UIScrollBar::SetThickness(int thickPx)
{
    if (thickPx != GetThickness())
    {
        m_thickness = thickPx;
        UpdateLengthThicknessMargins();
    }
}

Side UIScrollBar::GetSide() const
{
    return m_side;
}
int UIScrollBar::GetScrolling() const
{
    return m_scrollingPx;
}
float UIScrollBar::GetScrollingPercent() const
{
    int scrollSpacePx = GetScrollingSpacePx();
    return scrollSpacePx > 0 ? SCAST<float>(GetScrolling()) / scrollSpacePx : 0;
}

int UIScrollBar::GetLength() const
{
    return m_length;
}
int UIScrollBar::GetThickness() const
{
    return m_thickness;
}
Axis UIScrollBar::GetScrollAxis() const
{
    switch (GetSide())
    {
        case Side::LEFT:
        case Side::RIGHT: return Axis::VERTICAL;
        case Side::TOP:
        case Side::BOT: return Axis::HORIZONTAL;
    }
    ASSERT(false);
    return Axis::HORIZONTAL;
}

bool UIScrollBar::IsBeingGrabbed() const
{
    return GetFocusable()->IsBeingPressed();
}

void UIScrollBar::UpdateLengthThicknessMargins()
{
    RectTransform *rt = GetGameObject()->GetRectTransform();
    RectTransform *barRT = GetBar()->GetRectTransform();
    if (GetScrollAxis() == Axis::HORIZONTAL)
    {
        bool bot = (GetSide() == Side::BOT);
        rt->SetAnchorX(Vector2(-1, 1));
        rt->SetAnchorY(Vector2(bot ? -1 : 1));
        rt->SetMargins(0);
        rt->SetMarginTop(bot ? -GetThickness() : 0);
        rt->SetMarginBot(bot ? 0 : -GetThickness());

        barRT->SetAnchorX(Vector2(-1));
        barRT->SetAnchorY(Vector2(-1, 1));
        barRT->SetMargins(0, 0, -GetLength(), 0);
    }
    else
    {
        bool left = (GetSide() == Side::LEFT);
        rt->SetAnchorX(Vector2(left ? -1 : 1));
        rt->SetAnchorY(Vector2(-1, 1));
        rt->SetMargins(0);
        rt->SetMarginRight(left ? -GetThickness() : 0);
        rt->SetMarginLeft(left ? 0 : -GetThickness());

        barRT->SetAnchorX(Vector2(-1, 1));
        barRT->SetAnchorY(Vector2(1));
        barRT->SetMargins(0, 0, 0, -GetLength());
    }
    SetScrolling(GetScrolling());
}

UIScrollBar *UIScrollBar::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    go->SetName("ScrollBar");
    UIScrollBar *scrollBar = go->AddComponent<UIScrollBar>();

    UIScrollArea *scrollArea = GameObjectFactory::CreateUIScrollAreaInto(go);
    scrollArea->GetBackground()->SetTint(Color::White().WithValue(0.4f));

    GameObject *bar = GameObjectFactory::CreateUIGameObjectNamed("Bar");
    UIImageRenderer *barImg = bar->AddComponent<UIImageRenderer>();
    // barImg->SetImageTexture(
    // TextureFactory::Get9SliceRoundRectTexture().Get() );
    // barImg->SetMode(UIImageRenderer::Mode::SLICE_9);

    UIFocusable *barFocusable = bar->AddComponent<UIFocusable>();
    barFocusable->SetCursorType(Cursor::Type::HAND);
    barFocusable->EventEmitter<IEventsFocus>::RegisterListener(scrollBar);

    // GameObjectFactory::AddInnerShadow(scrollArea->GetGameObject());
    GameObjectFactory::AddInnerBorder(scrollArea->GetGameObject());
    // GameObjectFactory::AddOuterShadow(bar, Vector2i(1, 5));
    GameObjectFactory::AddInnerBorder(bar);

    UIFocusable *scrollAreaFocusable =
        scrollArea->GetGameObject()->AddComponent<UIFocusable>();
    scrollAreaFocusable->SetCursorType(Cursor::Type::HAND);

    scrollBar->p_bar = bar;
    scrollBar->p_barImg = barImg;
    scrollBar->p_barFocusable = barFocusable;
    scrollBar->p_scrollArea = scrollArea;
    scrollBar->p_scrollAreaFocusable = scrollAreaFocusable;
    scrollBar->SetSide(Side::LEFT);
    scrollBar->SetLength(50);
    scrollBar->SetThickness(10);
    scrollBar->OnMouseExit();  // Set bar color

    scrollArea->SetContainedGameObject(bar);

    return scrollBar;
}

void UIScrollBar::OnMouseEnter()
{
    if (!IsBeingGrabbed())
    {
        p_barImg->SetTint(Color::White().WithValue(1.0f));
    }
}

void UIScrollBar::OnMouseExit()
{
    if (!IsBeingGrabbed())
    {
        p_barImg->SetTint(Color::White().WithValue(0.9f));
    }
}

int UIScrollBar::GetScrollingSpacePx() const
{
    if (!IsEnabledRecursively())
    {
        return 0;
    }

    int scrollingSpace = GetScrollingRect().GetSize().GetAxis(GetScrollAxis());
    scrollingSpace -= GetLength();
    return Math::Max(scrollingSpace, 0);
}

AARect UIScrollBar::GetScrollingRect() const
{
    GameObject *cont = GetScrollArea()->GetGameObject();
    RectTransform *rt = cont->GetRectTransform();
    return AARect(rt->GetViewportRect());
}

UIScrollArea *UIScrollBar::GetScrollArea() const
{
    return p_scrollArea;
}
GameObject *UIScrollBar::GetBar() const
{
    return p_bar;
}

UIEventResult UIScrollBar::OnUIEvent(UIFocusable *, const UIEvent &event)
{
    if (event.type == UIEvent::Type::MOUSE_ENTER)
    {
        OnMouseEnter();
        return UIEventResult::INTERCEPT;
    }
    else if (event.type == UIEvent::Type::MOUSE_EXIT)
    {
        OnMouseExit();
        return UIEventResult::INTERCEPT;
    }

    return UIEventResult::IGNORE;
}

UIFocusable *UIScrollBar::GetFocusable() const
{
    return p_barFocusable;
}
