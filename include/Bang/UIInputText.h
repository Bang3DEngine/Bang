#ifndef UIINPUTTEXT_H
#define UIINPUTTEXT_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/DPtr.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/IEventsFocus.h"
#include "Bang/IEventsValueChanged.h"
#include "Bang/ILayoutElement.h"
#include "Bang/String.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILabel.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UITextCursor.h"

namespace Bang
{
class GameObject;
class IEventsValueChanged;
class RectTransform;
class UIFocusable;
class UITextRenderer;

class UIInputText : public Component,
                    public EventEmitter<IEventsValueChanged>,
                    public EventEmitter<IEventsFocus>,
                    public EventListener<IEventsFocus>,
                    public ILayoutElement
{
    COMPONENT(UIInputText)

public:
    void OnUpdate() override;

    void SetCursorIndex(int index);
    void SetSelection(int selectionBeginIndex, int selectionEndIndex);

    String GetSelectedText() const;
    void ReplaceSelectedText(const String &replaceStr);

    void SetBlocked(bool blocked);
    void SetAllowedCharacters(const String &allowedCharacters);

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

    // ILayoutElement
    virtual void CalculateLayout(Axis axis) override;

    bool IsBlocked() const;
    UILabel *GetLabel() const;
    UITextCursor *GetCursor() const;
    UITextRenderer *GetText() const;
    UIFocusable *GetFocusable() const;
    UIImageRenderer *GetBackground() const;

private:
    static const Vector2i LookAheadOffsetPx;
    static const int MarginX;
    static const int MarginY;

    bool m_isBlocked = false;
    String m_allowedCharacters = "";

    DPtr<UILabel> p_label;
    DPtr<UITextCursor> p_cursor;
    DPtr<UIImageRenderer> p_border;
    DPtr<UIScrollArea> p_scrollArea;
    DPtr<UIImageRenderer> p_background;

    UIInputText();
    virtual ~UIInputText() override;

    void HandleTyping();
    void HandleCursorIndices(bool existedSelection);
    String FilterAllowedInputText(const String &inputText);

    bool IsSelecting() const;
    int GetCursorIndex() const;
    int GetSelectionIndex() const;

    RectTransform *GetLabelRT() const;
    RectTransform *GetTextRT() const;
    RectTransform *GetRT() const;

    bool IsWordBoundary(char prevChar, char nextChar) const;
    int GetCtrlStopIndex(int cursorIndex, bool forward) const;

    void UpdateCursorRenderer();
    void UpdateTextScrolling();
    bool IsShiftPressed() const;

    static UIInputText *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};
}  // namespace Bang

#endif  // UIINPUTTEXT_H
