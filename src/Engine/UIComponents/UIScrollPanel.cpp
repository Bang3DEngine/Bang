#include "Bang/UIScrollPanel.h"

#include "Bang/AARect.h"
#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/Key.h"
#include "Bang/Math.h"
#include "Bang/Rect.h"
#include "Bang/RectTransform.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UIScrollBar.h"
#include "Bang/Vector2.h"

using namespace Bang;

const float UIScrollPanel::WheelScrollSpeedPx = 20.0f;

UIScrollPanel::UIScrollPanel()
{
    SET_INSTANCE_CLASS_ID(UIScrollPanel)
}

UIScrollPanel::~UIScrollPanel()
{
}

void UIScrollPanel::UpdateScrollUI()
{
    Vector2 contentSize = Vector2::Max(GetContentSize(), Vector2::One());
    Vector2 containerSize = GetContainerSize();

    // Handle vertical/horizontal show mode
    HandleScrollShowMode(contentSize, containerSize);

    // Set containedGo anchors according to the current scrolling and scroll
    // area margins to leave space for the scroll bar
    HandleScrollAreaRectTransform();

    // Handle scroll percent and scroll bar

    // Set bar length
    Vector2 sizeProp = Vector2(containerSize) / Vector2(contentSize);
    sizeProp = Vector2::Clamp(sizeProp, Vector2(0.1f), Vector2::One());
    GetHorizontalScrollBar()->SetLengthPercent(sizeProp.x);
    GetVerticalScrollBar()->SetLengthPercent(sizeProp.y);

    Vector2 scrollMaxAmount = GetMaxScrollLength();

    Vector2 scrollingPercent = GetScrollingPercent();
    Vector2i scrolling(scrollingPercent * scrollMaxAmount);
    if (contentSize.x > containerSize.x || contentSize.y > containerSize.y)
    {
        // Apply scrollings
        Vector2i scrollEnabledMask(IsHorizontalScrollEnabledAndNoFit() ? 1 : 0,
                                   IsVerticalScrollEnabledAndNoFit() ? 1 : 0);
        scrollingPercent *= Vector2(scrollEnabledMask);

        scrolling = Vector2i(scrollingPercent * scrollMaxAmount);
        scrolling = Vector2i::Min(scrolling, Vector2i(contentSize));
    }

    SetScrolling(scrolling);
}

void UIScrollPanel::OnUpdate()
{
    Component::OnUpdate();
    UpdateScrollUI();
}

void UIScrollPanel::OnPostUpdate()
{
    Component::OnPostUpdate();

    UIScrollArea *sa = GetScrollArea();
    if (sa->GetContainedGameObject())
    {
        RectTransform *referenceRT = sa->GetGameObject()->GetRectTransform();
        RectTransform *toConvertRT =
            sa->GetContainedGameObject()->GetRectTransform();
        RectTransform *toConvertParentRT =
            toConvertRT->GetGameObject()->GetParent()->GetRectTransform();

        AARect refRect(referenceRT->GetViewportRect());

        Vector2 newAnchorMin =
            toConvertParentRT->FromViewportPointToLocalPointNDC(
                refRect.GetMin());
        Vector2 newAnchorMax =
            toConvertParentRT->FromViewportPointToLocalPointNDC(
                refRect.GetMax());

        constexpr float Epsilon = 0.0001f;
        if (GetForceHorizontalFit() &&
            (Math::Abs(newAnchorMin.x - toConvertRT->GetAnchorMin().x) >
                 Epsilon ||
             Math::Abs(newAnchorMax.x - toConvertRT->GetAnchorMax().x) >
                 Epsilon))
        {
            toConvertRT->SetAnchorMinX(newAnchorMin.x);
            toConvertRT->SetAnchorMaxX(newAnchorMax.x);
        }

        if (GetForceVerticalFit() &&
            (Math::Abs(newAnchorMin.y - toConvertRT->GetAnchorMin().y) >
                 Epsilon ||
             Math::Abs(newAnchorMax.y - toConvertRT->GetAnchorMax().y) >
                 Epsilon))
        {
            toConvertRT->SetAnchorMinY(newAnchorMin.y);
            toConvertRT->SetAnchorMaxY(newAnchorMax.y);
        }
    }
    UpdateScrollUI();
}

void UIScrollPanel::SetForceVerticalFit(bool forceVerticalFit)
{
    if (forceVerticalFit != m_forceVerticalFit)
    {
        m_forceVerticalFit = forceVerticalFit;
        UpdateScrollUI();
    }
}

void UIScrollPanel::SetForceHorizontalFit(bool forceHorizontalFit)
{
    if (forceHorizontalFit != m_forceHorizontalFit)
    {
        m_forceHorizontalFit = forceHorizontalFit;
        UpdateScrollUI();
    }
}

void UIScrollPanel::SetVerticalScrollBarSide(HorizontalSide side)
{
    if (GetVerticalScrollBarSide() != side)
    {
        GetVerticalScrollBar()->SetSide(
            side == HorizontalSide::LEFT ? Side::LEFT : Side::RIGHT);
        m_verticalScrollBarSide = side;
        UpdateScrollUI();
    }
}

void UIScrollPanel::SetHorizontalScrollBarSide(VerticalSide side)
{
    if (GetHorizontalScrollBarSide() != side)
    {
        GetHorizontalScrollBar()->SetSide(
            side == VerticalSide::TOP ? Side::TOP : Side::BOT);
        m_horizontalScrollBarSide = side;
        UpdateScrollUI();
    }
}

void UIScrollPanel::SetVerticalShowScrollMode(ShowScrollMode showScrollMode)
{
    if (GetVerticalShowScrollMode() != showScrollMode)
    {
        m_verticalShowScrollMode = showScrollMode;
        UpdateScrollUI();
    }
}

void UIScrollPanel::SetHorizontalShowScrollMode(ShowScrollMode showScrollMode)
{
    if (GetHorizontalShowScrollMode() != showScrollMode)
    {
        m_horizontalShowScrollMode = showScrollMode;
        UpdateScrollUI();
    }
}

void UIScrollPanel::SetVerticalScrollEnabled(bool enabled)
{
    if (m_verticalScrollEnabled != enabled)
    {
        m_verticalScrollEnabled = enabled;
        UpdateScrollUI();
    }
}

void UIScrollPanel::SetHorizontalScrollEnabled(bool enabled)
{
    if (m_horizontalScrollEnabled != enabled)
    {
        m_horizontalScrollEnabled = enabled;
        UpdateScrollUI();
    }
}

void UIScrollPanel::SetScrolling(const Vector2i &scrolling)
{
    const Vector2 maxScrollLength = GetMaxScrollLength();
    Vector2 scrollPerc;
    scrollPerc.x =
        (maxScrollLength.x > 0 ? scrolling.x / maxScrollLength.x : 0);
    scrollPerc.y =
        (maxScrollLength.y > 0 ? scrolling.y / maxScrollLength.y : 0);
    SetScrollingPercent(scrollPerc);
}

void UIScrollPanel::SetScrollingPercent(const Vector2 &scrollPerc)
{
    Vector2 scrollPercClamped =
        Vector2::Clamp(scrollPerc, Vector2::Zero(), Vector2::One());
    GetScrollArea()->SetScrolling(Vector2i(Vector2::Round(
        scrollPercClamped * Vector2(-1, 1) * GetMaxScrollLength())));
    GetHorizontalScrollBar()->SetScrollingPercent(scrollPercClamped.x);
    GetVerticalScrollBar()->SetScrollingPercent(scrollPercClamped.y);
}

Vector2i UIScrollPanel::GetScrolling() const
{
    return GetScrollArea()->GetScrolling();
}

Vector2 UIScrollPanel::GetScrollingPercent() const
{
    Vector2 scrollingPercent =
        Vector2(GetHorizontalScrollBar()->GetScrollingPercent(),
                GetVerticalScrollBar()->GetScrollingPercent());
    scrollingPercent =
        Vector2::Clamp(scrollingPercent, Vector2::Zero(), Vector2::One());
    return scrollingPercent;
}

bool UIScrollPanel::GetForceVerticalFit() const
{
    return m_forceVerticalFit;
}

bool UIScrollPanel::GetForceHorizontalFit() const
{
    return m_forceHorizontalFit;
}

HorizontalSide UIScrollPanel::GetVerticalScrollBarSide() const
{
    return m_verticalScrollBarSide;
}

VerticalSide UIScrollPanel::GetHorizontalScrollBarSide() const
{
    return m_horizontalScrollBarSide;
}

ShowScrollMode UIScrollPanel::GetVerticalShowScrollMode() const
{
    return m_verticalShowScrollMode;
}

ShowScrollMode UIScrollPanel::GetHorizontalShowScrollMode() const
{
    return m_horizontalShowScrollMode;
}

bool UIScrollPanel::IsVerticalScrollEnabled() const
{
    return m_verticalScrollEnabled;
}

bool UIScrollPanel::IsHorizontalScrollEnabled() const
{
    return m_horizontalScrollEnabled;
}

Vector2 UIScrollPanel::GetContentSize() const
{
    GameObject *containedGo = GetScrollArea()->GetContainedGameObject();
    return containedGo
               ? containedGo->GetRectTransform()->GetViewportRect().GetSize()
               : Vector2::Zero();
}

Vector2 UIScrollPanel::GetContainerSize() const
{
    return GetGameObject()->GetRectTransform()->GetViewportRect().GetSize();
}

Vector2 UIScrollPanel::GetMaxScrollLength() const
{
    return Vector2::Max(GetContentSize() - GetContainerSize(), Vector2::Zero());
}

UIEventResult UIScrollPanel::OnUIEvent(UIFocusable *focusable,
                                       const UIEvent &event)
{
    BANG_UNUSED(focusable);

    switch (event.type)
    {
        case UIEvent::Type::FOCUS_TAKEN:
            p_border->SetTint(Color::Orange());
            return UIEventResult::INTERCEPT;
            break;

        case UIEvent::Type::FOCUS_LOST:
            p_border->SetTint(Color::Black());
            return UIEventResult::INTERCEPT;
            break;

        case UIEvent::Type::KEY_DOWN:
        {
            switch (event.key.key)
            {
                case Key::UP:
                case Key::DOWN:
                {
                    int displacement = (event.key.key == Key::DOWN ? 1 : -1);
                    SetScrollingPercent(GetScrollingPercent() +
                                        0.1f * displacement);
                    return UIEventResult::INTERCEPT;
                }
                break;

                case Key::PAGEUP:
                case Key::PAGEDOWN:
                {
                    int inc = (event.key.key == Key::PAGEDOWN ? 1 : -1);
                    Vector2i scrollInc = (inc * Vector2i(GetContainerSize()));
                    SetScrolling(GetScrolling() + scrollInc);
                }
                break;

                case Key::HOME:
                    SetScrollingPercent(Vector2::Zero());
                    return UIEventResult::INTERCEPT;
                    break;

                case Key::END:
                    SetScrollingPercent(Vector2::One());
                    return UIEventResult::INTERCEPT;
                    break;

                default: break;
            }
        }
        break;

        case UIEvent::Type::WHEEL:
        {
            Vector2 contentSize =
                Vector2::Max(GetContentSize(), Vector2::One());
            Vector2 containerSize = GetContainerSize();

            Vector2 scrollMaxAmount = GetMaxScrollLength();

            Vector2 scrollingPercent =
                Vector2(GetHorizontalScrollBar()->GetScrollingPercent(),
                        GetVerticalScrollBar()->GetScrollingPercent());
            scrollingPercent = Vector2::Clamp(
                scrollingPercent, Vector2::Zero(), Vector2::One());
            Vector2i scrolling(scrollingPercent * scrollMaxAmount);
            if (contentSize.x > containerSize.x ||
                contentSize.y > containerSize.y)
            {
                // MouseWheel scrolling
                if (GetGameObject()->GetRectTransform()->IsMouseOver(false))
                {
                    Vector2i mouseWheelPx(event.wheel.amount *
                                          WheelScrollSpeedPx);
                    Vector2 mouseWheelPercent =
                        Vector2(mouseWheelPx) / contentSize;
                    scrollingPercent -= mouseWheelPercent;
                    scrollingPercent = Vector2::Clamp(
                        scrollingPercent, Vector2::Zero(), Vector2::One());
                }

                // Apply scrollings
                Vector2i scrollEnabledMask(
                    IsHorizontalScrollEnabledAndNoFit() ? 1 : 0,
                    IsVerticalScrollEnabledAndNoFit() ? 1 : 0);
                scrollingPercent *= Vector2(scrollEnabledMask);

                scrolling = Vector2i(scrollingPercent * scrollMaxAmount);
                scrolling = Vector2i::Min(scrolling, Vector2i(contentSize));
            }

            SetScrolling(scrolling);

            return UIEventResult::INTERCEPT;
        }
        break;

        default: break;
    }

    return UIEventResult::IGNORE;
}

UIScrollArea *UIScrollPanel::GetScrollArea() const
{
    return p_scrollArea;
}

UIScrollBar *UIScrollPanel::GetVerticalScrollBar() const
{
    return p_verticalScrollBar;
}

UIScrollBar *UIScrollPanel::GetHorizontalScrollBar() const
{
    return p_horizontalScrollBar;
}

void UIScrollPanel::HandleScrollAreaRectTransform()
{
    RectTransform *scrollAreaRT =
        GetScrollArea()->GetGameObject()->GetRectTransform();

    // Set margins to make room for the vertical scroll bar
    UIScrollBar *vScrollBar = GetVerticalScrollBar();
    GameObject *vScrollBarGo = vScrollBar->GetGameObject();
    int vScrollBarThickness =
        (vScrollBarGo->IsEnabledRecursively() ? vScrollBar->GetThickness() : 0);
    if (GetVerticalScrollBarSide() == HorizontalSide::RIGHT)
    {
        scrollAreaRT->SetMarginLeft(0);
        scrollAreaRT->SetMarginRight(vScrollBarThickness);
    }
    else
    {
        scrollAreaRT->SetMarginLeft(vScrollBarThickness);
        scrollAreaRT->SetMarginRight(0);
    }

    // Set margins to make room for the horizontal scroll bar
    UIScrollBar *hScrollBar = GetHorizontalScrollBar();
    GameObject *hScrollBarGo = hScrollBar->GetGameObject();
    RectTransform *vScrollBarRT = vScrollBarGo->GetRectTransform();
    int hScrollBarThickness =
        (hScrollBarGo->IsEnabledRecursively() ? hScrollBar->GetThickness() : 0);
    if (GetHorizontalScrollBarSide() == VerticalSide::BOT)
    {
        scrollAreaRT->SetMarginTop(0);
        vScrollBarRT->SetMarginTop(0);
        scrollAreaRT->SetMarginBot(hScrollBarThickness);
        vScrollBarRT->SetMarginBot(hScrollBarThickness);
    }
    else
    {
        scrollAreaRT->SetMarginTop(hScrollBarThickness);
        vScrollBarRT->SetMarginTop(hScrollBarThickness);
        scrollAreaRT->SetMarginBot(0);
        vScrollBarRT->SetMarginBot(0);
    }

    // Handle contained gameObject RectTransform anchors, depending on enabled
    // scrollings
    GameObject *containedGo = GetScrollArea()->GetContainedGameObject();
    if (containedGo)
    {
        RectTransform *containedGoRT = containedGo->GetRectTransform();
        if (IsVerticalScrollEnabledAndNoFit() &&
            IsHorizontalScrollEnabledAndNoFit())
        {
            containedGoRT->SetAnchors(Vector2(-1, 1));
        }
        else if (IsVerticalScrollEnabledAndNoFit() &&
                 !IsHorizontalScrollEnabledAndNoFit())
        {
            containedGoRT->SetAnchorX(Vector2(-1, 1));
            containedGoRT->SetAnchorY(Vector2(1));
        }
        else if (!IsVerticalScrollEnabledAndNoFit() &&
                 IsHorizontalScrollEnabledAndNoFit())
        {
            containedGoRT->SetAnchorX(Vector2(-1));
            containedGoRT->SetAnchorY(Vector2(-1, 1));
        }
        else
        {
            containedGoRT->SetAnchors(Vector2(-1, -1), Vector2(1, 1));
        }
    }
}

void UIScrollPanel::HandleScrollShowMode(const Vector2 &contentSize,
                                         const Vector2 &containerSize)
{
    bool showHorizontal = false;
    switch (GetHorizontalShowScrollMode())
    {
        case ShowScrollMode::NEVER: showHorizontal = false; break;
        case ShowScrollMode::WHEN_NEEDED:
            showHorizontal = (contentSize.x > containerSize.x);
            break;
        case ShowScrollMode::ALWAYS: showHorizontal = true; break;
    }
    showHorizontal = showHorizontal && IsHorizontalScrollEnabledAndNoFit();

    bool showVertical = false;
    switch (GetVerticalShowScrollMode())
    {
        case ShowScrollMode::NEVER: showVertical = false; break;
        case ShowScrollMode::WHEN_NEEDED:
            showVertical = (contentSize.y > containerSize.y);
            break;
        case ShowScrollMode::ALWAYS: showVertical = true; break;
    }
    showVertical = showVertical && IsVerticalScrollEnabledAndNoFit();

    GetHorizontalScrollBar()->GetGameObject()->SetEnabled(showHorizontal);
    GetVerticalScrollBar()->GetGameObject()->SetEnabled(showVertical);
}

bool UIScrollPanel::IsVerticalScrollEnabledAndNoFit() const
{
    return IsVerticalScrollEnabled() && !GetForceVerticalFit();
}

bool UIScrollPanel::IsHorizontalScrollEnabledAndNoFit() const
{
    return IsHorizontalScrollEnabled() && !GetForceHorizontalFit();
}

UIScrollPanel *UIScrollPanel::CreateInto(GameObject *go)
{
    GameObjectFactory::CreateUIGameObjectInto(go);

    UIScrollPanel *scrollPanel = go->AddComponent<UIScrollPanel>();

    UIScrollArea *scrollArea = GameObjectFactory::CreateUIScrollArea();
    UIScrollBar *verticalScrollBar = GameObjectFactory::CreateUIScrollBar();
    UIScrollBar *horizontalScrollBar = GameObjectFactory::CreateUIScrollBar();

    scrollArea->GetGameObject()->SetParent(go);
    verticalScrollBar->GetGameObject()->SetParent(go);
    horizontalScrollBar->GetGameObject()->SetParent(go);

    scrollPanel->p_scrollArea = scrollArea;
    scrollPanel->p_verticalScrollBar = verticalScrollBar;
    scrollPanel->p_horizontalScrollBar = horizontalScrollBar;

    GameObjectFactory::AddInnerShadow(
        scrollArea->GetGameObject(), Vector2i(10), 0.3f);

    GameObject *innerBorderGo = GameObjectFactory::CreateUIGameObject();
    scrollPanel->p_border = GameObjectFactory::AddInnerBorder(innerBorderGo);
    innerBorderGo->SetParent(go);

    UIFocusable *focusable = go->AddComponent<UIFocusable>();
    focusable->EventEmitter<IEventsFocus>::RegisterListener(scrollPanel);

    scrollPanel->SetVerticalShowScrollMode(ShowScrollMode::ALWAYS);
    scrollPanel->SetHorizontalShowScrollMode(ShowScrollMode::ALWAYS);
    scrollPanel->SetVerticalScrollBarSide(HorizontalSide::LEFT);
    scrollPanel->SetHorizontalScrollBarSide(VerticalSide::BOT);
    scrollPanel->UpdateScrollUI();  // To avoid first frame being wrong

    return scrollPanel;
}
