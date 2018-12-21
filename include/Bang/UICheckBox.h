#ifndef UICHECKBOX_H
#define UICHECKBOX_H

#include <vector>

#include "Bang/Array.tcc"
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
#include "Bang/String.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"

namespace Bang
{
class GameObject;
class IEventsValueChanged;

class UICheckBox : public Component,
                   public EventEmitter<IEventsValueChanged>,
                   public EventEmitter<IEventsFocus>,
                   public EventListener<IEventsFocus>
{
    COMPONENT(UICheckBox)

public:
    void OnUpdate() override;

    void SetSize(int size);
    void SetChecked(bool checked);

    bool IsChecked() const;
    int GetSize() const;

    UIImageRenderer *GetTickImage() const;
    UIImageRenderer *GetBackgroundImage() const;
    UILayoutElement *GetLayoutElement() const;
    UIFocusable *GetFocusable() const;

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

private:
    int m_size = -1;
    bool m_isChecked = true;
    DPtr<UIFocusable> p_focusable;
    DPtr<UIImageRenderer> p_border;
    DPtr<UIImageRenderer> p_tickImage;
    DPtr<UIImageRenderer> p_checkBgImage;
    DPtr<UILayoutElement> p_layoutElement;

    UICheckBox();
    virtual ~UICheckBox() override;

    static UICheckBox *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};
}

#endif  // UICHECKBOX_H
