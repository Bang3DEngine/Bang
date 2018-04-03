#include "Bang/UISlider.h"

#include "Bang/Input.h"
#include "Bang/Paths.h"
#include "Bang/Cursor.h"
#include "Bang/Texture2D.h"
#include "Bang/IconManager.h"
#include "Bang/GameObject.h"
#include "Bang/RectTransform.h"
#include "Bang/UIInputNumber.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/UIHorizontalLayout.h"

USING_NAMESPACE_BANG


UISlider::UISlider()
{
}

UISlider::~UISlider()
{
}

void UISlider::OnUpdate()
{
    Component::OnUpdate();

    if (GetHandleFocusable()->IsBeingPressed())
    {
        GetHandleRenderer()->SetTint( GetPressedColor() );

        GameObject *guide = GetGuideRenderer()->GetGameObject();
        float mouseLocalX = guide->GetRectTransform()->
                                    FromViewportPointToLocalPointNDC(
                                            Input::GetMousePosition() ).x;
        mouseLocalX = Math::Clamp(mouseLocalX, -1.0f, 1.0f);

        float mousePercent = mouseLocalX * 0.5f + 0.5f;
        SetValuePercent(mousePercent);
    }
    else
    {
        GetHandleRenderer()->SetTint( GetHandleFocusable()->IsMouseOver() ?
                                       GetOverColor() : GetIdleColor());
    }
}

void UISlider::OnValueChanged(Object *object)
{
    ASSERT(object == GetInputNumber());
    IValueChangedListener::SetReceiveEvents(false);
    SetValue( GetInputNumber()->GetValue() );
    UpdateSliderHandlerFromInputNumberValue();
    EventEmitter<IValueChangedListener>::PropagateToListeners(
                &IValueChangedListener::OnValueChanged, this);
    IValueChangedListener::SetReceiveEvents(true);
}

void UISlider::UpdateSliderHandlerFromInputNumberValue()
{
    GetHandleRectTransform()->SetAnchors(
                Vector2(GetValuePercent() * 2.0f - 1.0f, 0) );
}

RectTransform *UISlider::GetHandleRectTransform() const
{
    return GetHandleRenderer()->GetGameObject()->GetRectTransform();
}

void UISlider::SetValue(float value)
{
    GetInputNumber()->SetValue(value);
}

void UISlider::SetMinMaxValues(float minValue, float maxValue)
{
    GetInputNumber()->SetMinMaxValues(minValue, maxValue);
}

void UISlider::SetValuePercent(float percent)
{
    const float minVal = GetInputNumber()->GetMinValue();
    const float maxVal = GetInputNumber()->GetMaxValue();
    SetValue( minVal + (maxVal - minVal) * percent);
    UpdateSliderHandlerFromInputNumberValue();
}

float UISlider::GetValue() const
{
    return GetInputNumber()->GetValue();
}

float UISlider::GetValuePercent() const
{
    const float minValue = GetInputNumber()->GetMinValue();
    const float maxValue = GetInputNumber()->GetMaxValue();
    const float range = (maxValue - minValue);
    const float percent = (range != 0.0f) ?
                            ((GetValue() - minValue) / range) : 0.0f;
    ASSERT(percent >= 0.0f && percent <= 1.0f);
    return percent;
}

UIInputNumber *UISlider::GetInputNumber() const
{
    return p_inputNumber;
}

UIImageRenderer *UISlider::GetGuideRenderer() const
{
    return p_guideRenderer;
}

UIImageRenderer *UISlider::GetHandleRenderer() const
{
    return p_handleRenderer;
}

UIFocusable *UISlider::GetHandleFocusable() const
{
    return p_handleFocusable;
}

bool UISlider::HasFocus() const
{
    return GetHandleFocusable()->IsBeingPressed() || GetInputNumber()->HasFocus();
}

const Color &UISlider::GetIdleColor() const
{
    return m_idleColor;
}

const Color &UISlider::GetOverColor() const
{
    return m_overColor;
}

const Color &UISlider::GetPressedColor() const
{
    return m_pressedColor;
}

UISlider *UISlider::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UISlider *slider = go->AddComponent<UISlider>();

    UIHorizontalLayout *hl = go->AddComponent<UIHorizontalLayout>();
    hl->SetSpacing(10);

    UILayoutElement *hlLE = go->AddComponent<UILayoutElement>();
    hlLE->SetFlexibleWidth(1.0f);

    GameObject *sliderContainer = GameObjectFactory::CreateUIGameObject();

    UILayoutElement *sliderLE = sliderContainer->AddComponent<UILayoutElement>();
    sliderLE->SetFlexibleSize( Vector2::One );

    UIImageRenderer *guideRenderer = GameObjectFactory::CreateUIImage(Color::Black);
    RectTransform *guideRT = guideRenderer->GetGameObject()->GetRectTransform();
    guideRT->SetAnchorMin( Vector2(-1.0f, 0.0f) );
    guideRT->SetAnchorMax( Vector2( 1.0f, 0.0f) );
    guideRT->SetMarginTop(-1);

    UIImageRenderer *handleRenderer = GameObjectFactory::CreateUIImage(slider->m_idleColor);
    handleRenderer->SetImageTexture( IconManager::GetCircleIcon().Get() );
    RectTransform *handleRT = handleRenderer->GetGameObject()->GetRectTransform();
    handleRT->SetAnchors(Vector2::Zero);
    handleRT->SetMargins(-6);

    UIFocusable *handleFocusable = handleRenderer->GetGameObject()
                                   ->AddComponent<UIFocusable>();
    handleFocusable->EventEmitter<IFocusListener>::RegisterListener(slider);
    handleFocusable->SetCursorType( Cursor::Type::Hand );

    guideRenderer->GetGameObject()->SetParent(sliderContainer);
    handleRenderer->GetGameObject()->SetParent(sliderContainer);

    UIInputNumber *inputNumber = GameObjectFactory::CreateUIInputNumber();
    UILayoutElement *inputNumberLE = inputNumber->GetGameObject()
                                     ->AddComponent<UILayoutElement>();
    inputNumberLE->SetMinWidth(20);
    inputNumberLE->SetFlexibleWidth( 0.2f );
    inputNumber->EventEmitter<IValueChangedListener>::RegisterListener(slider);

    slider->p_guideRenderer = guideRenderer;
    slider->p_handleRenderer = handleRenderer;
    slider->p_handleFocusable = handleFocusable;
    slider->p_inputNumber = inputNumber;

    slider->GetInputNumber()->SetMinMaxValues(0.0f, 100.0f);
    slider->UpdateSliderHandlerFromInputNumberValue();

    sliderContainer->SetParent(go);
    inputNumber->GetGameObject()->SetParent(go);

    return slider;
}

