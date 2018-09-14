#ifndef UIINPUTNUMBER_H
#define UIINPUTNUMBER_H

#include "Bang/UIFocusable.h"
#include "Bang/EventEmitter.h"
#include "Bang/IEventsFocus.h"
#include "Bang/IEventsValueChanged.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIInputText;

class UIInputNumber : public Component,
                      public EventEmitter<IEventsValueChanged>,
                      public EventListener<IEventsValueChanged>,
                      public EventListener<IEventsFocus>
{
    COMPONENT(UIInputNumber)

public:
    UIInputNumber();
    virtual ~UIInputNumber();

    void OnStart() override;
    void OnUpdate() override;

    void SetValue(float v);
    void SetMinMaxValues(float min, float max);
    void SetDecimalPlaces(uint decimalPlaces);

    float GetValue() const;
    uint GetDecimalPlaces() const;
    UIInputText* GetInputText() const;
    float GetMinValue() const;
    float GetMaxValue() const;
    const Vector2 &GetMinMaxValues() const;

    bool HasFocus() const;

    // IEventsFocus
    virtual void OnUIEvent(IFocusable *focusable, const UIEventExt &event) override;

private:
    float m_value            = 0.0f;
    bool m_hasFocus          = false;
    uint m_decimalPlaces     = 3;
    UIInputText *p_inputText = nullptr;
    Vector2 m_minMaxValues   = Vector2(Math::NegativeInfinity<float>(),
                                       Math::Infinity<float>());

    void UpdateValueFromText();
    void UpdateTextFromValue();
    void ChangeTextColorBasedOnMinMax();

    // IEventsValueChanged
    void OnValueChanged(EventEmitter<IEventsValueChanged>*) override;

    static UIInputNumber *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UIINPUTNUMBER_H

