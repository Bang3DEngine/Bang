#ifndef UILIST_H
#define UILIST_H

#include <functional>

#include "Bang/UMap.h"
#include "Bang/Array.h"
#include "Bang/Component.h"
#include "Bang/IEventsFocus.h"
#include "Bang/IEventsUIList.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIDirLayout;
FORWARD class UIScrollArea;
FORWARD class UIScrollPanel;
FORWARD class UIImageRenderer;

using GOItem = GameObject;

class UIList : public Component,
               public EventListener<IEventsFocus>,
               public EventListener<IEventsDestroy>,
               public EventEmitter<IEventsUIList>
{
    COMPONENT(UIList)

public:
    enum Action { SELECTION_IN, SELECTION_OUT, MOUSE_OVER, MOUSE_OUT,
                  PRESSED, DOUBLE_CLICKED_LEFT, CLICKED_LEFT, CLICKED_RIGHT };

    // Component
    void OnUpdate() override;

    void AddItem(GOItem *newItem);
    void AddItem(GOItem *newItem, int index);
    void MoveItem(GOItem *item, int index);
    void RemoveItem(GOItem *item);
    void ClearSelection();
    void Clear();

    void SetIdleColor(const Color &idleColor);
    void SetOverColor(const Color &overColor);
    void SetSelectedColor(const Color &selectedColor);
    void SetUseSelectedColor(bool useSelectColor);

    const Array<GOItem*>& GetItems() const;
    GOItem *GetItem(int i) const;

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
    const Color& GetIdleColor() const;
    const Color& GetOverColor() const;
    const Color& GetSelectedColor() const;

    bool SomeChildHasFocus() const;
    int GetSelectedIndex() const;
    GOItem* GetSelectedItem() const;

    void SetWideSelectionMode(bool wideSelectionMode);

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    using SelectionCallback = std::function<void(GOItem *item, Action action)>;
    void SetSelectionCallback(SelectionCallback selectionCallback);

protected:
    UIList();
    virtual ~UIList();

    void AddItem_(GOItem *newItem, int index, bool moving);
    void RemoveItem_(GOItem *item, bool moving);

private:
    Array<GOItem*> p_items;
    UMap<GOItem*, UIImageRenderer*> p_itemsBackground;
    bool m_someChildHasFocus = false;

    int m_selectionIndex = -1;
    GOItem *p_itemUnderMouse = nullptr;
    SelectionCallback m_selectionCallback;

    GameObject *p_container = nullptr;
    UIScrollPanel *p_scrollPanel = nullptr;

    bool m_useSelectColor = true;
    Color m_idleColor = Color::Zero;
    Color m_overColor = Color::VeryLightBlue;
    Color m_selectedColor = Color::LightBlue;

    bool m_wideSelectionMode = true;

    void HandleShortcuts();

    // IEventsFocus
    virtual void OnFocusTaken(EventEmitter<IEventsFocus> *focusable) override;
    virtual void OnFocusLost(EventEmitter<IEventsFocus> *focusable) override;

    static UIList* CreateInto(GameObject *go, bool withScrollPanel);
    void CallSelectionCallback(GameObject *item, Action action);

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UILIST_H
