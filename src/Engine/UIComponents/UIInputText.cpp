#include "Bang/UIInputText.h"

#include "Bang/Alignment.h"
#include "Bang/Array.h"
#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/DPtr.tcc"
#include "Bang/Font.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/IEventsValueChanged.h"
#include "Bang/Input.h"
#include "Bang/Key.h"
#include "Bang/Math.h"
#include "Bang/RectTransform.h"
#include "Bang/SystemClipboard.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILabel.h"
#include "Bang/UILayoutManager.h"
#include "Bang/UIRectMask.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UITextCursor.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UITheme.h"
#include "Bang/Vector2.h"

using namespace Bang;

const Vector2i UIInputText::LookAheadOffsetPx = Vector2i(5);
const int UIInputText::MarginX = 5;
const int UIInputText::MarginY = 2;

UIInputText::UIInputText()
{
    SET_INSTANCE_CLASS_ID(UIInputText)
}

UIInputText::~UIInputText()
{
}

void UIInputText::OnUpdate()
{
    Component::OnUpdate();

    if (GetLabel())
    {
        bool hasFocus = GetLabel()->GetFocusable()->HasFocus();
        if (hasFocus)
        {
            HandleTyping();
            UpdateTextScrolling();
            UpdateCursorRenderer();
        }

        p_cursor->SetEnabled(hasFocus);
    }
}

void UIInputText::UpdateCursorRenderer()
{
    // Cursor "I" position update
    if (GetLabel() && GetText())
    {
        float cursorX = GetLabel()->GetCursorXViewportNDC(GetCursorIndex());
        float fontHeight =
            GetText()->GetFont()->GetFontHeight(GetText()->GetTextSize());
        float fontHeightNDC =
            GL::FromViewportAmountToViewportAmountNDC(Vector2(0, fontHeight)).y;

        constexpr float MarginsNDC = 0.1f;
        AARect limitsRect(GetLabelRT()->GetViewportAARectNDC());
        Vector2 minPoint =
            Vector2(cursorX, limitsRect.GetCenter().y - fontHeightNDC / 2.0f);
        Vector2 maxPoint =
            Vector2(cursorX, limitsRect.GetCenter().y + fontHeightNDC / 2.0f);

        const RectTransform *cParentRT =
            p_cursor->GetGameObject()->GetParent()->GetRectTransform();
        Vector2 minPointLocalNDC =
            cParentRT->FromViewportPointNDCToLocalPointNDC(minPoint);
        Vector2 maxPointLocalNDC =
            cParentRT->FromViewportPointNDCToLocalPointNDC(maxPoint);
        minPointLocalNDC.y = Math::Clamp(
            minPointLocalNDC.y, -1.0f + MarginsNDC, 1.0f - MarginsNDC);
        maxPointLocalNDC.y = Math::Clamp(
            maxPointLocalNDC.y, -1.0f + MarginsNDC, 1.0f - MarginsNDC);
        minPointLocalNDC = Vector2::Min(minPointLocalNDC, maxPointLocalNDC);
        maxPointLocalNDC = Vector2::Max(minPointLocalNDC, maxPointLocalNDC);

        RectTransform *cRT = p_cursor->GetGameObject()->GetRectTransform();
        cRT->SetAnchors(minPointLocalNDC, maxPointLocalNDC);
    }
}

void UIInputText::UpdateTextScrolling()
{
    if (p_scrollArea && GetText())
    {
        Vector2i prevScrollPx = p_scrollArea->GetScrolling();
        p_scrollArea->SetScrolling(Vector2i::Zero());
        GetText()->RegenerateCharQuadsVAO();

        Vector2 scrollNDC = Vector2::Zero();
        AARect labelLimits(GetLabelRT()->GetViewportAARectNDC());
        AARect contentRectNDC = GetText()->GetContentViewportNDCRect();
        if (contentRectNDC.GetWidth() > labelLimits.GetWidth() &&
            GetCursorIndex() > 0)
        {
            p_scrollArea->SetScrolling(prevScrollPx);
            GetText()->RegenerateCharQuadsVAO();
            contentRectNDC = GetText()->GetContentViewportNDCRect();
            float cursorX = GetLabel()->GetCursorXViewportNDC(GetCursorIndex());
            float lookAheadNDC = GL::FromViewportAmountToViewportAmountNDC(
                                     Vector2(LookAheadOffsetPx))
                                     .x;
            if (cursorX < labelLimits.GetMin().x)
            {
                scrollNDC.x = labelLimits.GetMin().x - cursorX + lookAheadNDC;
            }
            else if (cursorX > labelLimits.GetMax().x)
            {
                scrollNDC.x = labelLimits.GetMax().x - cursorX - lookAheadNDC;
            }
            else
            {
                if (contentRectNDC.GetMin().x < labelLimits.GetMin().x &&
                    contentRectNDC.GetMax().x < labelLimits.GetMax().x)
                {
                    scrollNDC.x = labelLimits.GetMax().x -
                                  contentRectNDC.GetMax().x - lookAheadNDC;
                }
            }

            Vector2i scrollPx(
                GL::FromViewportAmountNDCToViewportAmount(scrollNDC));
            p_scrollArea->SetScrolling(prevScrollPx + scrollPx);
        }
    }
}

void UIInputText::HandleTyping()
{
    if (IsBlocked())
    {
        return;
    }

    String inputText = Input::PollInputText();
    inputText = FilterAllowedInputText(inputText);

    // Key typing handling
    if (!inputText.IsEmpty())
    {
        ReplaceSelectedText(inputText);
        SetCursorIndex(GetCursorIndex() + inputText.Size());
        GetLabel()->ResetSelection();
    }
}

String UIInputText::FilterAllowedInputText(const String &inputText)
{
    if (m_allowedCharacters.IsEmpty())
    {
        return inputText;
    }

    String allowedText = "";
    for (char c : inputText)
    {
        if (m_allowedCharacters.Contains(String(c)))
        {
            allowedText += c;
        }
    }
    return allowedText;
}

bool UIInputText::IsSelecting() const
{
    if (GetLabel())
    {
        return IsShiftPressed() || GetLabel()->IsSelectingWithMouse();
    }
    return false;
}

int UIInputText::GetCursorIndex() const
{
    return GetLabel() ? GetLabel()->GetCursorIndex() : -1;
}
int UIInputText::GetSelectionIndex() const
{
    return GetLabel() ? GetLabel()->GetSelectionIndex() : -1;
}
UILabel *UIInputText::GetLabel() const
{
    return p_label;
}

RectTransform *UIInputText::GetLabelRT() const
{
    return GetLabel() ? GetLabel()->GetGameObject()->GetRectTransform()
                      : nullptr;
}

RectTransform *UIInputText::GetTextRT() const
{
    return GetText() ? GetText()->GetGameObject()->GetRectTransform() : nullptr;
}

RectTransform *UIInputText::GetRT() const
{
    return GetGameObject()->GetRectTransform();
}

void UIInputText::SetCursorIndex(int index)
{
    if (GetLabel()->GetCursorIndex() != index)
    {
        GetLabel()->SetCursorIndex(index);
        GetCursor()->ResetTickTime();
        UpdateCursorRenderer();
    }
}

void UIInputText::SetSelection(int selectionBeginIndex, int selectionEndIndex)
{
    GetLabel()->SetSelection(selectionBeginIndex, selectionEndIndex);
    UpdateCursorRenderer();
}

String UIInputText::GetSelectedText() const
{
    return GetLabel()->GetSelectedText();
}

void UIInputText::ReplaceSelectedText(const String &replaceStr)
{
    String content = GetText()->GetContent();
    int minIndex = GetLabel()->GetSelectionBeginIndex();
    int maxIndex = GetLabel()->GetSelectionEndIndex();

    if (minIndex >= 0 && minIndex <= SCAST<int>(content.Size()) &&
        maxIndex >= 0 && maxIndex <= SCAST<int>(content.Size()))
    {
        content.Remove(minIndex, maxIndex);
        content.Insert(minIndex, replaceStr);
        GetText()->SetContent(content);
    }

    SetCursorIndex(minIndex);
    GetLabel()->ResetSelection();

    EventEmitter<IEventsValueChanged>::PropagateToListeners(
        &IEventsValueChanged::OnValueChanged, this);
}

void UIInputText::SetBlocked(bool blocked)
{
    if (blocked != IsBlocked())
    {
        m_isBlocked = blocked;
        GetFocusable()->SetConsiderForTabbing(!IsBlocked());

        if (IsBlocked())
        {
            GetBackground()->SetTint(
                UITheme::GetInputTextBlockedBackgroundColor());
        }
        else
        {
            GetBackground()->SetTint(UITheme::GetInputTextBackgroundColor());
        }
    }
}

void UIInputText::SetAllowedCharacters(const String &allowedCharacters)
{
    m_allowedCharacters = allowedCharacters;
}

UITextCursor *UIInputText::GetCursor() const
{
    return p_cursor;
}

UITextRenderer *UIInputText::GetText() const
{
    return (p_label ? p_label->GetText() : nullptr);
}

UIFocusable *UIInputText::GetFocusable() const
{
    return GetLabel()->GetFocusable();
}

UIImageRenderer *UIInputText::GetBackground() const
{
    return p_background;
}

bool UIInputText::IsShiftPressed() const
{
    return Input::GetKey(Key::LSHIFT) || Input::GetKey(Key::RSHIFT);
}

UIInputText *UIInputText::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIInputText *inputText = go->AddComponent<UIInputText>();

    UIImageRenderer *bg = go->AddComponent<UIImageRenderer>();
    // bg->SetImageTexture( TextureFactory::Get9SliceRoundRectTexture().Get() );
    // bg->SetMode(UIImageRenderer::Mode::SLICE_9);
    bg->SetTint(UITheme::GetInputTextBackgroundColor());
    inputText->p_background = bg;

    UIScrollArea *scrollArea = GameObjectFactory::CreateUIScrollAreaInto(go);
    scrollArea->GetGameObject()->GetRectTransform()->SetAnchors(
        Vector2::Zero());
    scrollArea->GetMask()->SetMasking(true);
    scrollArea->GetBackground()->SetTint(Color::Zero());
    inputText->p_scrollArea = scrollArea;

    inputText->p_border = GameObjectFactory::AddInnerBorder(go);
    // GameObjectFactory::AddInnerShadow(go, Vector2i(3));

    GameObject *cursorGo = GameObjectFactory::CreateUIGameObject();
    UITextCursor *cursor = cursorGo->AddComponent<UITextCursor>();
    inputText->p_cursor = cursor;
    cursor->SetEnabled(false);

    UILabel *label = GameObjectFactory::CreateUILabel();
    label->SetSelectable(true);
    label->GetMask()->SetMasking(false);
    label->GetGameObject()->GetRectTransform()->SetMargins(
        MarginX, MarginY, MarginX, MarginY);
    label->GetFocusable()->EventEmitter<IEventsFocus>::RegisterListener(
        inputText);
    label->GetFocusable()->SetConsiderForTabbing(true);
    inputText->p_label = label;

    label->GetGameObject()->SetParent(scrollArea->GetContainer());
    cursorGo->SetParent(label->GetGameObject());

    inputText->SetCursorIndex(inputText->GetText()->GetContent().Size());
    inputText->GetText()->SetHorizontalAlign(HorizontalAlignment::LEFT);
    inputText->GetText()->SetVerticalAlign(VerticalAlignment::CENTER);
    inputText->GetText()->SetWrapping(false);
    inputText->GetLabel()->ResetSelection();
    inputText->UpdateCursorRenderer();

    return inputText;
}

bool UIInputText::IsWordBoundary(char prevChar, char nextChar) const
{
    const bool prevCharIsUpperCase = String::IsUpperCase(prevChar);
    const bool nextCharIsUpperCase = String::IsUpperCase(nextChar);
    const bool prevCharIsLetter = String::IsLetter(prevChar);
    const bool nextCharIsLetter = String::IsLetter(nextChar);

    if (!prevCharIsUpperCase && nextCharIsUpperCase)
    {
        return true;
    }

    if (prevCharIsLetter != nextCharIsLetter)
    {
        return true;
    }

    return false;
}

int UIInputText::GetCtrlStopIndex(int cursorIndex, bool forward) const
{
    const String &content = GetText()->GetContent();
    const int fwdInc = (forward ? 1 : -1);

    if (cursorIndex == 0 && !forward)
    {
        return 0;
    }

    if (cursorIndex == content.Size() && forward)
    {
        return content.Size();
    }

    int i = cursorIndex;
    if (forward)
    {
        i += fwdInc;
    }
    while (i >= 0 && i < content.Size())
    {
        if ((i + fwdInc) < 0 || (i + fwdInc) == content.Size())
        {
            i += fwdInc;
            break;
        }

        const int minIndex = Math::Min(i + fwdInc, i);
        const int maxIndex = Math::Max(i + fwdInc, i);
        const char prevChar = content[minIndex];
        const char nextChar = content[maxIndex];
        if (IsWordBoundary(prevChar, nextChar))
        {
            i += fwdInc;
            break;
        }

        i += fwdInc;
    }

    return i;
}

UIEventResult UIInputText::OnUIEvent(UIFocusable *focusable,
                                     const UIEvent &event)
{
    switch (event.type)
    {
        case UIEvent::Type::FOCUS_TAKEN:
            if (!IsBlocked())
            {
                GameObjectFactory::MakeBorderFocused(p_border);
                Input::PollInputText();
            }
            GetLabel()->SelectAll();
            return UIEventResult::INTERCEPT;
            break;

        case UIEvent::Type::FOCUS_LOST:
            GameObjectFactory::MakeBorderNotFocused(p_border);
            UpdateCursorRenderer();
            return UIEventResult::INTERCEPT;
            break;

        case UIEvent::Type::KEY_DOWN:
        {
            switch (event.key.key)
            {
                case Key::V:
                    if (!IsBlocked() &&
                        event.key.modifiers.IsOn(KeyModifier::LCTRL))
                    {
                        String clipboardText = SystemClipboard::Get();
                        clipboardText = FilterAllowedInputText(clipboardText);
                        ReplaceSelectedText(clipboardText);
                        SetCursorIndex(GetCursorIndex() + clipboardText.Size());
                        GetLabel()->ResetSelection();
                        return UIEventResult::INTERCEPT;
                    }
                    break;

                case Key::X:
                    if (!IsBlocked() &&
                        event.key.modifiers.IsOn(KeyModifier::LCTRL))
                    {
                        String selectedText = GetSelectedText();
                        if (selectedText.Size() > 0)
                        {
                            SystemClipboard::Set(selectedText);
                            ReplaceSelectedText("");
                            return UIEventResult::INTERCEPT;
                        }
                    }
                    break;

                case Key::LEFT:
                case Key::RIGHT:
                {
                    int indexAdvance = (event.key.key == Key::LEFT ? -1 : 1);

                    if (event.key.modifiers.IsOn(KeyModifier::LCTRL))
                    {
                        bool fwd = (indexAdvance > 0);
                        int startIdx = GetCursorIndex() + (fwd ? 0 : -1);
                        indexAdvance =
                            GetCtrlStopIndex(startIdx, fwd) - GetCursorIndex();
                        indexAdvance += (fwd ? 0 : 1);
                    }

                    int newIndex;
                    if (GetSelectedText().Size() >= 1 && !IsSelecting())
                    {
                        const int leadingIndex =
                            indexAdvance > 0 ? Math::Max(GetCursorIndex(),
                                                         GetSelectionIndex())
                                             : Math::Min(GetCursorIndex(),
                                                         GetSelectionIndex());
                        newIndex = leadingIndex;
                    }
                    else
                    {
                        newIndex = GetCursorIndex() + indexAdvance;
                    }
                    newIndex = Math::Clamp(
                        newIndex, 0, GetText()->GetContent().Size());
                    SetCursorIndex(newIndex);

                    if (!IsSelecting())  // Selection resetting handling
                    {
                        GetLabel()->ResetSelection();
                    }
                    return UIEventResult::INTERCEPT;
                }
                break;

                case Key::DELETE:
                case Key::BACKSPACE:
                {
                    if (!IsBlocked() && !GetText()->GetContent().IsEmpty())
                    {
                        int offsetCursor = 0;
                        int offsetSelection = 1;
                        bool removeText = false;
                        bool selecting = GetSelectedText().Size() > 0;
                        switch (event.key.key)
                        {
                            case Key::DELETE:
                                if (selecting)
                                {
                                    offsetSelection += -1;
                                }
                                removeText = true;
                                break;

                            case Key::BACKSPACE:
                                offsetCursor += (selecting ? 0 : -1);
                                offsetSelection += -1;
                                removeText = true;
                                break;

                            default: break;
                        }

                        if (removeText)
                        {
                            if (GetCursorIndex() > GetSelectionIndex())
                            {
                                // Swap indices
                                int oldSelectionIndex = GetSelectionIndex();
                                GetLabel()->SetSelectionIndex(GetCursorIndex());
                                SetCursorIndex(oldSelectionIndex);
                            }

                            SetCursorIndex(GetCursorIndex() + offsetCursor);
                            GetLabel()->SetSelectionIndex(GetSelectionIndex() +
                                                          offsetSelection);
                            ReplaceSelectedText("");
                            GetLabel()->ResetSelection();
                            return UIEventResult::INTERCEPT;
                        }
                    }
                }
                break;

                case Key::END:
                case Key::HOME:
                {
                    int index = (event.key.key == Key::HOME
                                     ? 0
                                     : GetText()->GetContent().Size());
                    SetCursorIndex(index);
                    if (!IsSelecting())
                    {
                        GetLabel()->ResetSelection();
                    }
                    return UIEventResult::INTERCEPT;
                }
                break;

                default: break;
            }
        }
        break;

        default: break;
    }

    Array<UIEventResult> eventResults =
        EventEmitter<IEventsFocus>::PropagateToListenersAndGatherResult<
            UIEventResult>(&IEventsFocus::OnUIEvent, focusable, event);
    if (eventResults.Contains(UIEventResult::INTERCEPT))
    {
        return UIEventResult::INTERCEPT;
    }

    return UIEventResult::IGNORE;
}

void UIInputText::CalculateLayout(Axis axis)
{
    GameObject *textGo = GetText()->GetGameObject();

    Vector2i minSize = UILayoutManager::GetMinSize(textGo);
    Vector2i prefSize = UILayoutManager::GetPreferredSize(textGo);
    Vector2 flexSize = Vector2(1, 1);
    minSize += Vector2i(MarginX, MarginY) * 2;
    prefSize += Vector2i(MarginX, MarginY) * 2;
    minSize.x = prefSize.x = -1;

    SetCalculatedLayout(axis,
                        minSize.GetAxis(axis),
                        prefSize.GetAxis(axis),
                        flexSize.GetAxis(axis));
}

bool UIInputText::IsBlocked() const
{
    return m_isBlocked;
}
