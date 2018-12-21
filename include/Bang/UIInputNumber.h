#ifndef UIINPUTNUMBER_H
#define UIINPUTNUMBER_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsFocus.h"
#include "Bang/IEventsValueChanged.h"
#include "Bang/Math.h"
#include "Bang/String.h"

namespace Bang
{
class GameObject;
class IEventsValueChanged;
class UIFocusable;
class UIInputText;

class UIInputNumber : public Component,
                      public EventEmitter<IEventsValueChanged>,
                      public EventListener<IEventsValueChanged>,
                      public EventListener<IEventsFocus>
{
    COMPONENT(UIInputNumber)

public:
    UIInputNumber();
    virtual ~UIInputNumber() override;

    void SetValue(float v);
    void SetMinValue(float min);
    void SetMaxValue(float max);
    void SetBlocked(bool blocked);
    void SetMinMaxValues(float min, float max);
    void SetDecimalPlaces(uint decimalPlaces);
    void SetStep(float step);

    float GetValue() const;
    uint GetDecimalPlaces() const;
    UIInputText *GetInputText() const;
    float GetMinValue() const;
    float GetMaxValue() const;
    float GetStep() const;
    const Vector2 &GetMinMaxValues() const;

    bool HasFocus() const;

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

private:
    float m_value = 0.0f;
    float m_step = 1.0f;
    uint m_decimalPlaces = 3;
    UIInputText *p_inputText = nullptr;
    Vector2 m_minMaxValues =
        Vector2(Math::NegativeInfinity<float>(), Math::Infinity<float>());

    void UpdateValueFromText();
    void UpdateTextFromValue();
    void ChangeTextColorBasedOnMinMax();

    // IEventsValueChanged
    void OnValueChanged(EventEmitter<IEventsValueChanged> *) override;

    static UIInputNumber *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};
}

#endif  // UIINPUTNUMBER_H
