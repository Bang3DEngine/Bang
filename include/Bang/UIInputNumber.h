#ifndef UIINPUTNUMBER_H
#define UIINPUTNUMBER_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsValueChanged.h"
#include "Bang/IEventsFocus.h"
#include "Bang/Math.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;
FORWARD class IEventsValueChanged;
FORWARD class UIFocusable;
FORWARD class UIInputText;

class UIInputNumber : public Component,
                      public EventEmitter<IEventsValueChanged>,
                      public EventListener<IEventsValueChanged>,
                      public EventListener<IEventsFocus>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIInputNumber)

public:
    UIInputNumber();
    virtual ~UIInputNumber() override;

    void SetValue(float v);
    void SetMinValue(float min);
    void SetMaxValue(float max);
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
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

private:
    float m_value            = 0.0f;
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

