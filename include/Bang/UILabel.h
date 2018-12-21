#ifndef UILABEL_H
#define UILABEL_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/DPtr.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/IEvents.h"
#include "Bang/IEventsFocus.h"
#include "Bang/String.h"
#include "Bang/UIFocusable.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UIRectMask.h"
#include "Bang/UITextRenderer.h"

namespace Bang
{
class RectTransform;

class UILabel : public Component, public EventListener<IEventsFocus>
{
    COMPONENT(UILabel)

public:
    virtual ~UILabel() override;

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
    DPtr<UILayoutElement> p_layoutElement;

    UILabel();

    int GetClosestCharIndexTo(const Vector2 &coordsLocalNDC);
    void HandleMouseSelection();
    void UpdateSelectionQuadRenderer();

    bool IsShiftPressed() const;
    RectTransform *GetTextParentRT() const;

    static UILabel *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};
}

#endif  // UILABEL_H_H
