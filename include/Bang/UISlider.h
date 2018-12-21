#ifndef UISLIDER_H
#define UISLIDER_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsValueChanged.h"
#include "Bang/String.h"
#include "Bang/UITheme.h"

namespace Bang
{
class GameObject;
class IEventsValueChanged;
class RectTransform;
class UIFocusable;
class UIImageRenderer;
class UIInputNumber;

class UISlider : public Component,
                 public EventListener<IEventsValueChanged>,
                 public EventEmitter<IEventsValueChanged>
{
    COMPONENT(UISlider)

public:
    void SetValue(float value);
    void SetMinMaxValues(float minValue, float maxValue);
    void SetValuePercent(float percent);

    float GetValue() const;
    float GetValuePercent() const;

    UIInputNumber *GetInputNumber() const;
    UIImageRenderer *GetGuideRenderer() const;
    UIImageRenderer *GetHandleRenderer() const;
    UIFocusable *GetSliderFocusable() const;
    bool HasFocus() const;

    const Color &GetIdleColor() const;
    const Color &GetOverColor() const;
    const Color &GetPressedColor() const;

private:
    Color m_idleColor = Color::White();
    Color m_overColor = UITheme::GetOverColor();
    Color m_pressedColor = UITheme::GetSelectedColor();

    UIImageRenderer *p_guideRenderer = nullptr;
    UIImageRenderer *p_handleRenderer = nullptr;
    UIFocusable *p_sliderFocusable = nullptr;
    UIInputNumber *p_inputNumber = nullptr;

    UISlider();
    virtual ~UISlider() override;

    // Component
    void OnUpdate() override;

    // IEventsValueChanged
    void OnValueChanged(EventEmitter<IEventsValueChanged> *object) override;

    float GetMouseRelativePercent() const;
    void UpdateSliderHandlerFromInputNumberValue();
    RectTransform *GetHandleRectTransform() const;

    static UISlider *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};
}

#endif  // UISLIDER_H
