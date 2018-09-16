#ifndef UICHECKBOX_H
#define UICHECKBOX_H

#include "Bang/DPtr.h"
#include "Bang/Component.h"
#include "Bang/EventEmitter.h"
#include "Bang/IEventsFocus.h"
#include "Bang/IEventsValueChanged.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIFocusable;
FORWARD class UIImageRenderer;
FORWARD class UILayoutElement;

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
    virtual ~UICheckBox();

    static UICheckBox *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UICHECKBOX_H

