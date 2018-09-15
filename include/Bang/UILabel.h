#ifndef UILABEL_H
#define UILABEL_H

#include "Bang/Component.h"
#include "Bang/IEventsFocus.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIRectMask;
FORWARD class UIFocusable;
FORWARD class RectTransform;
FORWARD class UITextRenderer;

class UILabel : public Component,
                public EventListener<IEventsFocus>,
                public EventListener<IEventsDestroy>
{
    COMPONENT(UILabel)

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

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

private:
    int m_cursorIndex = 0;
    int m_selectionIndex = 0;
    bool m_selectingWithMouse = false;
    bool m_selectable = Undef<bool>();
    bool m_selectAllOnFocusTaken = false;

    UIRectMask *p_mask = nullptr;
    UITextRenderer *p_text = nullptr;
    UIFocusable *p_focusable = nullptr;
    GameObject *p_selectionQuad = nullptr;

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

