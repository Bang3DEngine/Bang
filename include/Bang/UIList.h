#ifndef UILIST_H
#define UILIST_H

#include <functional>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/IEvents.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsFocus.h"
#include "Bang/IEventsUIList.h"
#include "Bang/String.h"
#include "Bang/UITheme.h"
#include "Bang/UMap.h"

namespace Bang
{
class UIDirLayout;
class UIFocusable;
class UIImageRenderer;
class UIScrollPanel;

using GOItem = GameObject;

class UIList : public Component,
               public EventListener<IEventsFocus>,
               public EventListener<IEventsDestroy>,
               public EventEmitter<IEventsUIList>
{
    COMPONENT(UIList)

public:
    enum class Action
    {
        SELECTION_IN,
        SELECTION_OUT,
        MOUSE_OVER,
        MOUSE_OUT,
        PRESSED,
        DOUBLE_CLICKED_LEFT,
        MOUSE_LEFT_DOWN,
        MOUSE_LEFT_UP,
        MOUSE_RIGHT_DOWN
    };

    void AddItem(GOItem *newItem);
    void AddItem(GOItem *newItem, int index);
    void MoveItem(GOItem *item, int index);
    void RemoveItem(GOItem *item);
    void ClearSelection();
    void Clear();

    void SetIdleColor(const Color &idleColor);
    void SetOverColor(const Color &overColor);
    void SetSelectedColor(const Color &selectedColor);

    const Array<GOItem *> &GetItems() const;
    GOItem *GetItem(int i) const;

    void SetNotifySelectionOnFullClick(bool notifySelectionOnFullClick);
    void ScrollToBegin();
    void ScrollTo(int i);
    void ScrollTo(GOItem *item);
    void ScrollToEnd();
    void SetSelection(int i);
    void SetSelection(GOItem *item);

    int GetNumItems() const;
    UIDirLayout *GetDirLayout() const;
    GameObject *GetContainer() const;
    UIScrollPanel *GetScrollPanel() const;
    const Color &GetIdleColor() const;
    const Color &GetOverColor() const;
    const Color &GetSelectedColor() const;

    bool SomeChildHasFocus() const;
    int GetSelectedIndex() const;
    GOItem *GetSelectedItem() const;
    UIFocusable *GetFocusable() const;

    void SetWideSelectionMode(bool wideSelectionMode);

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    using SelectionCallback = std::function<void(GOItem *item, Action action)>;
    void SetSelectionCallback(
        std::function<void(GOItem *item, Action action)> selectionCallback);
    void ClearSelectionCallback();

protected:
    UIList();
    virtual ~UIList() override;

    void AddItem_(GOItem *newItem, int index, bool moving);
    void RemoveItem_(GOItem *item, bool moving);

    UIEventResult OnMouseMove(bool forceColorsUpdate = false,
                              bool callCallbacks = true);
    UIImageRenderer *GetItemBg(GOItem *item) const;

private:
    Array<GOItem *> p_items;
    UIDirLayout *p_dirLayout = nullptr;
    UIFocusable *p_focusable = nullptr;
    UMap<GOItem *, UIImageRenderer *> p_itemsBackground;

    int m_selectionIndex = -1;
    GOItem *p_itemUnderMouse = nullptr;
    SelectionCallback m_selectionCallback;

    GameObject *p_container = nullptr;
    UIScrollPanel *p_scrollPanel = nullptr;

    Color m_idleColor = Color::Zero();
    Color m_overColor = UITheme::GetOverColor();
    Color m_selectedColor = UITheme::GetSelectedColor();

    bool m_wideSelectionMode = true;
    bool m_notifySelectionOnFullClick = false;

    void SetItemUnderMouse(GOItem *itemUnderMouse, bool callCallbacks);

    // IEventsFocus
    UIEventResult OnUIEvent(UIFocusable *focusable,
                            const UIEvent &event) override;
    UIEventResult UIEventCallback(UIFocusable *focusable, const UIEvent &event);

    static UIList *CreateInto(GameObject *go, bool withScrollPanel);
    void CallSelectionCallback(GameObject *item, Action action);

    friend class GameObjectFactory;
};
}  // namespace Bang

#endif  // UILIST_H
