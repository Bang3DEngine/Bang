#ifndef UICOMBOBOX_H
#define UICOMBOBOX_H

#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/IEventsFocus.h"
#include "Bang/String.h"
#include "Bang/Time.h"
#include "Bang/UIList.h"

namespace Bang
{
class GameObject;
class IEventsValueChanged;
class UIFocusable;
class UIImageRenderer;
class UITextRenderer;

class UIComboBox : public Component,
                   public EventEmitter<IEventsValueChanged>,
                   public EventEmitter<IEventsFocus>,
                   public EventListener<IEventsFocus>
{
    COMPONENT(UIComboBox)

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
    const Array<int> &GetValues() const;
    Array<int> GetSelectedValues() const;
    int GetSelectedValuesForFlag() const;
    const Array<String> &GetLabels() const;
    const Array<int> &GetSelectedIndices() const;

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

protected:
    UIComboBox();
    virtual ~UIComboBox() override;

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
    Array<UIImageRenderer *> m_checkImgs;
    bool m_listRecentlyToggled = false;
    Time m_timeWithListShown;

    UIList *p_list = nullptr;
    UIFocusable *p_focusable = nullptr;
    UIImageRenderer *p_border = nullptr;
    UITextRenderer *p_selectedItemText = nullptr;

    friend class GameObjectFactory;
};
}

#endif  // UICOMBOBOX_H
