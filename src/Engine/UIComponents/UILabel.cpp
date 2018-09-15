#include "Bang/UILabel.h"

#include "Bang/Font.h"
#include "Bang/Input.h"
#include "Bang/Cursor.h"
#include "Bang/UICanvas.h"
#include "Bang/GameObject.h"
#include "Bang/UIRectMask.h"
#include "Bang/UIFocusable.h"
#include "Bang/RectTransform.h"
#include "Bang/UITextRenderer.h"
#include "Bang/SystemClipboard.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/UIHorizontalLayout.h"

USING_NAMESPACE_BANG

UILabel::UILabel()
{
    ResetSelection();
    SetSelectable(false);
    SetSelectAllOnFocus(true);
}

UILabel::~UILabel()
{
}

void UILabel::OnUpdate()
{
    Component::OnUpdate();

    if (GetFocusable() && GetFocusable()->HasFocus())
    {
        if (m_selectingWithMouse)
        {
            HandleMouseSelection();
        }
    }
}

void UILabel::SetCursorIndex(int index)
{
    if (index != GetCursorIndex())
    {
        m_cursorIndex = index;
        UpdateSelectionQuadRenderer();
    }
}
void UILabel::SetSelectionIndex(int index)
{
    if (index != GetSelectionIndex())
    {
        m_selectionIndex = index;
        UpdateSelectionQuadRenderer();
    }
}

void UILabel::SetSelectable(bool selectable)
{
    m_selectable = selectable;
    if (GetFocusable())
    {
        GetFocusable()->SetCursorType(selectable ? Cursor::Type::IBEAM :
                                                   Cursor::Type::ARROW);
    }

    if (p_selectionQuad)
    {
        p_selectionQuad->SetEnabled(selectable);
    }
}
void UILabel::SetSelection(int cursorIndex, int selectionIndex)
{
    SetCursorIndex(cursorIndex);
    SetSelectionIndex(selectionIndex);
}

String UILabel::GetSelectedText() const
{
    if (GetText())
    {
        if (GetCursorIndex() == GetSelectionIndex() ||
            GetSelectionBeginIndex() >= GetSelectionEndIndex())
        {
            return "";
        }
        return GetText()->GetContent().SubString(GetSelectionBeginIndex(),
                                                 GetSelectionEndIndex()-1);
    }
    return "";
}
void UILabel::ResetSelection()
{
    SetSelectionIndex( GetCursorIndex() );
}
void UILabel::SelectAll()
{
    SetSelection(GetText()->GetContent().Size(), 0);

}
void UILabel::SetSelectAllOnFocus(bool selectAllOnFocus)
{
    m_selectAllOnFocusTaken = selectAllOnFocus;
}


bool UILabel::IsSelectable() const
{
    return m_selectable;
}

int UILabel::GetSelectionBeginIndex() const
{
    return Math::Min(GetCursorIndex(), GetSelectionIndex());
}

int UILabel::GetSelectionEndIndex() const
{
    return Math::Max(GetCursorIndex(), GetSelectionIndex());
}

float UILabel::GetCursorXViewportNDC(int cursorIndex) const
{
    if (GetText())
    {
        return GetTextParentRT()->FromLocalPointNDCToViewportPointNDC(
                                Vector2(GetCursorXLocalNDC(cursorIndex), 0) ).x;
    }
    return 0.0f;
}

float UILabel::GetCursorXLocalNDC(int cursorIndex) const
{
    if (!GetText())
    {
        return 0.0f;
    }

    float localTextX = 0.0f;
    const int textLength = GetText()->GetContent().Size();
    if (cursorIndex > 0 && cursorIndex < textLength) // Between two chars
    {
        AARect currentCharRect = GetText()->GetCharRectLocalNDC(cursorIndex - 1);
        AARect nextCharRect = GetText()->GetCharRectLocalNDC(cursorIndex);
        if (GetText()->GetContent()[cursorIndex-1] != ' ')
        {
            localTextX = (currentCharRect.GetMax().x +
                          nextCharRect.GetMin().x) / 2.0f;
        }
        else
        {
            localTextX = nextCharRect.GetMin().x;
        }
    }
    else if (!GetText()->GetCharRectsLocalNDC().IsEmpty()) // Begin or end
    {
        localTextX = (cursorIndex == 0 ?
                      GetText()->GetCharRectsLocalNDC().Front().GetMin().x :
                      GetText()->GetCharRectsLocalNDC().Back().GetMax().x);
    }
    else // Is empty
    {
        HorizontalAlignment hAlign = GetText()->GetHorizontalAlignment();
        if (hAlign == HorizontalAlignment::LEFT)
        {
            return -1;
        }
        if (hAlign == HorizontalAlignment::CENTER)
        {
            return 0;
        }
        if (hAlign == HorizontalAlignment::RIGHT)
        {
            return 1;
        }
    }

    return Vector2(localTextX, 0).x;
}

bool UILabel::GetSelectAllOnFocus() const
{
    return m_selectAllOnFocusTaken;
}

int UILabel::GetClosestCharIndexTo(const Vector2 &coordsLocalNDC)
{
    int closestCharIndex = 0;
    float minDist = Math::Infinity<float>();
    const Array<AARect>& charRectsNDC = GetText()->GetCharRectsLocalNDC();
    for (uint i = 0; i < charRectsNDC.Size(); ++i)
    {
        const AARect &cr = charRectsNDC[i];
        float distToMinX = Math::Abs(coordsLocalNDC.x - cr.GetMin().x);
        if (distToMinX < minDist)
        {
            minDist = distToMinX;
            closestCharIndex = i;
        }

        float distToMaxX = Math::Abs(coordsLocalNDC.x - cr.GetMax().x);
        if (distToMaxX < minDist)
        {
            minDist = distToMaxX;
            closestCharIndex = i + 1;
        }
    }
    return closestCharIndex;
}

int UILabel::GetCursorIndex() const
{
    return m_cursorIndex;
}

int UILabel::GetSelectionIndex() const
{
    return m_selectionIndex;
}

bool UILabel::IsSelectingWithMouse() const
{
    return m_selectingWithMouse;
}

UIRectMask *UILabel::GetMask() const
{
    return p_mask;
}

UITextRenderer *UILabel::GetText() const
{
    return p_text;
}

UIFocusable *UILabel::GetFocusable() const
{
    return p_focusable;
}

UIEventResult UILabel::OnUIEvent(UIFocusable*, const UIEvent &event)
{
    switch (event.type)
    {
        case UIEvent::Type::KEY_DOWN:
            if (event.key.modifiers.IsOn(KeyModifier::LCTRL))
            {
                if (event.key.key == Key::C)
                {
                    String selectedText = GetSelectedText();
                    SystemClipboard::Set( selectedText );
                    return UIEventResult::INTERCEPT;
                }
            }
        break;

        case UIEvent::Type::FOCUS_TAKEN:
        case UIEvent::Type::MOUSE_CLICK_DOUBLE:
            if (GetFocusable() && GetFocusable()->IsFocusEnabled())
            {
                if (GetSelectAllOnFocus() && IsSelectable())
                {
                    SelectAll();
                }
                else
                {
                    ResetSelection();
                }
                UpdateSelectionQuadRenderer();

                return UIEventResult::INTERCEPT;
            }
        break;

        case UIEvent::Type::FOCUS_LOST:
        {
            ResetSelection();
            m_selectingWithMouse = false;
            UpdateSelectionQuadRenderer();

            return UIEventResult::INTERCEPT;
        }
        break;

        case UIEvent::Type::MOUSE_CLICK_DOWN:
        {
            if (GetFocusable() && GetFocusable()->HasFocus() &&
                GetFocusable()->IsFocusEnabled())
            {
                if (IsSelectable())
                {
                    HandleMouseSelection();
                    m_selectingWithMouse = true;
                }
                else
                {
                    ResetSelection();
                }
                UpdateSelectionQuadRenderer();

                return UIEventResult::INTERCEPT;
            }
        }
        break;

        case UIEvent::Type::MOUSE_CLICK_UP:
        {
            m_selectingWithMouse = false;
            UpdateSelectionQuadRenderer();

            return UIEventResult::INTERCEPT;
        }
        break;

        default:
        break;
    }

    return UIEventResult::IGNORE;
}

void UILabel::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (object == GetText())
    {
        p_text = nullptr;
    }
    else if (object == p_selectionQuad)
    {
        p_selectionQuad = nullptr;
    }
    else if (object == GetFocusable())
    {
        p_focusable = nullptr;
    }
    else if (object == GetMask())
    {
        p_mask = nullptr;
    }
}

RectTransform *UILabel::GetTextParentRT() const
{
    if (!GetText())
    {
        return nullptr;
    }
    return GetText()->GetGameObject()->GetParent()->GetRectTransform();
}
bool UILabel::IsShiftPressed() const
{
    return Input::GetKey(Key::LSHIFT) || Input::GetKey(Key::RSHIFT);
}

void UILabel::HandleMouseSelection()
{
    // Find the closest visible char bounds to the mouse position
    if (Input::GetMouseButton(MouseButton::LEFT))
    {
        Vector2 mouseCoordsLocalNDC = Input::GetMousePositionNDC();
        mouseCoordsLocalNDC = GetTextParentRT()->FromViewportPointNDCToLocalPointNDC(
                                            Vector2(mouseCoordsLocalNDC) );
        int closestCharIndex = GetClosestCharIndexTo(mouseCoordsLocalNDC);
        SetCursorIndex(closestCharIndex);

        // Move the selection index accordingly
        if (!IsShiftPressed() && Input::GetMouseButtonDown(MouseButton::LEFT))
        {
            ResetSelection();
        }
        if (!IsSelectingWithMouse())
        {
            ResetSelection();
        }
    }

    if (Input::GetMouseButtonDoubleClick(MouseButton::LEFT))
    {
        SelectAll();
    }
}

void UILabel::UpdateSelectionQuadRenderer()
{
    if (GetText() && p_selectionQuad)
    {
        GetText()->RegenerateCharQuadsVAO();
        float cursorX     = GetCursorXViewportNDC( GetCursorIndex() );
        float selectionX  = GetCursorXViewportNDC( GetSelectionIndex() );

        AARect r ( GetGameObject()->GetRectTransform()->GetViewportAARectNDC() );
        Vector2 p1(Math::Min(cursorX, selectionX), r.GetMin().y );
        Vector2 p2(Math::Max(cursorX, selectionX), r.GetMax().y );

        RectTransform *textParentRT = GetTextParentRT();
        p1 = textParentRT->FromViewportPointNDCToLocalPointNDC(p1);
        p2 = textParentRT->FromViewportPointNDCToLocalPointNDC(p2);

        RectTransform *quadRT = p_selectionQuad->GetRectTransform();
        quadRT->SetAnchors( Vector2::Min(p1, p2), Vector2::Max(p1, p2) );
    }
}

UILabel *UILabel::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    GameObjectFactory::CreateUIRendererCacherInto(go);

    UILabel *label = go->AddComponent<UILabel>();

    UIHorizontalLayout *hl = go->AddComponent<UIHorizontalLayout>();
    hl->SetChildrenVerticalAlignment(VerticalAlignment::CENTER);
    hl->SetChildrenVerticalStretch(Stretch::FULL);
    hl->SetChildrenHorizontalStretch(Stretch::FULL);

    UILayoutElement *le = go->AddComponent<UILayoutElement>();
    le->SetFlexibleSize( Vector2::One );

    UIRectMask *mask = go->AddComponent<UIRectMask>();
    label->p_mask = mask;
    label->p_mask->SetMasking(false);

    GameObject *textGO = GameObjectFactory::CreateUIGameObject();
    UITextRenderer *text = textGO->AddComponent<UITextRenderer>();
    text->SetVerticalAlign(VerticalAlignment::CENTER);
    text->SetTextSize(12);
    text->SetWrapping(false);

    GameObject *selectionQuadGo = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *selectionQuad = selectionQuadGo->AddComponent<UIImageRenderer>();
    selectionQuad->SetTint(Color::LightBlue);

    UIFocusable *focusable = go->AddComponent<UIFocusable>();
    focusable->EventEmitter<IEventsFocus>::RegisterListener(label);

    label->p_text = text;
    label->p_focusable = focusable;
    label->p_selectionQuad = selectionQuadGo;

    label->p_text->EventEmitter<IEventsDestroy>::RegisterListener(label);
    label->p_mask->EventEmitter<IEventsDestroy>::RegisterListener(label);
    label->p_focusable->EventEmitter<IEventsDestroy>::RegisterListener(label);
    label->p_selectionQuad->EventEmitter<IEventsDestroy>::RegisterListener(label);

    selectionQuadGo->SetParent(go);
    textGO->SetParent(go);

    label->ResetSelection();
    label->UpdateSelectionQuadRenderer();

    return label;
}

