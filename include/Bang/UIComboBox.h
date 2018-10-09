#ifndef UICOMBOBOX_H
#define UICOMBOBOX_H

#include "Bang/Map.h"
#include "Bang/UIList.h"
#include "Bang/Component.h"
#include "Bang/EventEmitter.h"
#include "Bang/IEventsFocus.h"
#include "Bang/IEventsValueChanged.h"

NAMESPACE_BANG_BEGIN

FORWARD class UITextRenderer;

class UIComboBox : public Component,
                   public EventEmitter<IEventsValueChanged>,
                   public EventEmitter<IEventsFocus>,
                   public EventListener<IEventsFocus>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIComboBox)

public:
    void AddItem(const String &label, int value);
    void SetSelectionByIndex(int index, bool selected);
    void SetSelectionByValue(int value, bool selected);
    void SetSelectionByLabel(const String &label, bool selected);
    void SetSelectionByIndex(int index);
    void SetSelectionByValue(int value);
    void SetSelectionByLabel(const String &label);
    void SetSelectionForFlag(int flagValue);
    void SetMultiCheck(bool multicheck);
    void ClearSelectionByIndex(int index);
    void ClearSelectionByValue(int value);
    void ClearSelection();
    void ClearItems();
    void RemoveItem(const String &label);

    void ShowList();
    void HideList();
    int GetNumItems() const;
    bool IsListBeingShown() const;
    bool IsSelectedByIndex(int index) const;

    bool HasFocus() const;
    bool GetMultiCheck() const;
    int GetSelectedValue() const;
    int GetSelectedIndex() const;
    String GetSelectedLabel() const;
    Array<int> GetSelectedValues() const;
    int GetSelectedValuesForFlag() const;
    const Array<int>& GetSelectedIndices() const;

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;
protected:
    UIComboBox();
    virtual ~UIComboBox();

    void OnUpdate() override;

    void UpdateSelectedItemTopText();

    UIList *GetList() const;
    static UIComboBox *CreateInto(GameObject *go);
    static void CreateIntoWithoutAddingComponent(UIComboBox *comboBox,
                                                 GameObject *go);

    // UIList item handler
    void OnListSelectionCallback(GameObject *item, UIList::Action action);

private:
    bool m_multiCheck = false;
    Array<int> m_indexToValue;
    Array<String> m_indexToLabel;
    Array<int> m_selectedIndices;
    Array<UIImageRenderer*> m_checkImgs;
    bool m_listRecentlyToggled = false;
    Time m_timeWithListShown;

    UIList *p_list = nullptr;
    UIFocusable *p_focusable = nullptr;
    UIImageRenderer *p_border = nullptr;
    UITextRenderer *p_selectedItemText = nullptr;


    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UICOMBOBOX_H

