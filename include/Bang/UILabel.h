#ifndef UILABEL_H
#define UILABEL_H

#include "Bang/DPtr.h"
#include "Bang/Component.h"
#include "Bang/IEventsFocus.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIRectMask;
FORWARD class UIFocusable;
FORWARD class RectTransform;
FORWARD class UITextRenderer;

class UILabel : public Component,
                public EventListener<IEventsFocus>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UILabel)

public:
	virtual ~UILabel();

    void OnUpdate() override;

    void SetCursorIndex(int index);
    void SetSelectionIndex(int index);
    void SetSelectable(bool selectable);
    void SetSelection(int cursorIndex, int selectionIndex);

    void SelectAll();
    void ResetSelection();
    void SetSelectAllOnFocus(bool selectAllOnFocus);

    bool IsSelectable() const;
    int GetCursorIndex() const;
    int GetSelectionIndex() const;
    int GetSelectionBeginIndex() const;
    int GetSelectionEndIndex() const;

    float GetCursorXViewportNDC(int cursorIndex) const;
    float GetCursorXLocalNDC(int cursorIndex) const;

    bool IsSelectingWithMouse() const;

    UIRectMask *GetMask() const;
    String GetSelectedText() const;
    UITextRenderer *GetText() const;
    UIFocusable *GetFocusable() const;
    bool GetSelectAllOnFocus() const;

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

private:
    int m_cursorIndex = 0;
    int m_selectionIndex = 0;
    bool m_selectingWithMouse = false;
    bool m_selectable = Undef<bool>();
    bool m_selectAllOnFocusTaken = false;

    DPtr<UIRectMask> p_mask;
    DPtr<UITextRenderer> p_text;
    DPtr<UIFocusable> p_focusable;
    DPtr<GameObject> p_selectionQuad;

    UILabel();

    int GetClosestCharIndexTo(const Vector2 &coordsLocalNDC);
    void HandleMouseSelection();
    void UpdateSelectionQuadRenderer();

    bool IsShiftPressed() const;
    RectTransform *GetTextParentRT() const;

    static UILabel *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UILABEL_H_H

