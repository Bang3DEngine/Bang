#ifndef UISLIDER_H
#define UISLIDER_H

#include "Bang/Component.h"
#include "Bang/UIFocusable.h"
#include "Bang/IEventEmitter.h"
#include "Bang/IValueChangedListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class RectTransform;
FORWARD class UIInputNumber;
FORWARD class UIImageRenderer;

class UISlider : public Component,
                 public IValueChangedListener,
                 public EventEmitter<IValueChangedListener>
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

    const Color& GetIdleColor() const;
    const Color& GetOverColor() const;
    const Color& GetPressedColor() const;

private:
    Color m_idleColor    = Color::White;
    Color m_overColor    = Color::VeryLightBlue;
    Color m_pressedColor = Color::Blue;

    UIImageRenderer *p_guideRenderer  = nullptr;
    UIImageRenderer *p_handleRenderer = nullptr;
    UIFocusable *p_sliderFocusable    = nullptr;
    UIInputNumber *p_inputNumber      = nullptr;

	UISlider();
    virtual ~UISlider();

    // Component
    void OnUpdate() override;

    // IValueChangedListener
    void OnValueChanged(Object *object) override;

    float GetMouseRelativePercent() const;
    void UpdateSliderHandlerFromInputNumberValue();
    RectTransform *GetHandleRectTransform() const;

    static UISlider* CreateInto(GameObject *go);

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UISLIDER_H

