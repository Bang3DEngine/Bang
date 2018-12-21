#include "Bang/UISlider.h"

#include "Bang/Assert.h"
#include "Bang/ClassDB.h"
#include "Bang/Cursor.h"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/IEventsValueChanged.h"
#include "Bang/Input.h"
#include "Bang/Math.h"
#include "Bang/RectTransform.h"
#include "Bang/TextureFactory.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIInputNumber.h"
#include "Bang/UILayoutElement.h"

using namespace Bang;

UISlider::UISlider()
{
    SET_INSTANCE_CLASS_ID(UISlider)
}

UISlider::~UISlider()
{
}

void UISlider::OnUpdate()
{
    Component::OnUpdate();

    if (GetSliderFocusable()->IsBeingPressed())
    {
        GetHandleRenderer()->SetTint(GetPressedColor());
        SetValuePercent(GetMouseRelativePercent());
    }
    else
    {
        GetHandleRenderer()->SetTint(GetSliderFocusable()->IsMouseOver()
                                         ? GetOverColor()
                                         : GetIdleColor());
    }
}

void UISlider::OnValueChanged(EventEmitter<IEventsValueChanged> *object)
{
    ASSERT(object == GetInputNumber());
    EventListener<IEventsValueChanged>::SetReceiveEvents(false);
    SetValue(GetInputNumber()->GetValue());
    UpdateSliderHandlerFromInputNumberValue();
    EventEmitter<IEventsValueChanged>::PropagateToListeners(
        &IEventsValueChanged::OnValueChanged, this);
    EventListener<IEventsValueChanged>::SetReceiveEvents(true);
}

float UISlider::GetMouseRelativePercent() const
{
    GameObject *guide = GetGuideRenderer()->GetGameObject();
    float mouseLocalX =
        guide->GetRectTransform()
            ->FromViewportPointToLocalPointNDC(Input::GetMousePosition())
            .x;
    mouseLocalX = Math::Clamp(mouseLocalX, -1.0f, 1.0f);

    float mousePercent = mouseLocalX * 0.5f + 0.5f;
    return mousePercent;
}

void UISlider::UpdateSliderHandlerFromInputNumberValue()
{
    GetHandleRectTransform()->SetAnchors(
        Vector2(GetValuePercent() * 2.0f - 1.0f, 0));
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
    SetValue(minVal + (maxVal - minVal) * percent);
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
    const float percent =
        (range != 0.0f) ? ((GetValue() - minValue) / range) : 0.0f;
    ASSERT(percent >= -0.001f && percent <= 1.001f);
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

UIFocusable *UISlider::GetSliderFocusable() const
{
    return p_sliderFocusable;
}

bool UISlider::HasFocus() const
{
    return GetSliderFocusable()->IsBeingPressed() ||
           GetInputNumber()->HasFocus();
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
    UIFocusable *sliderFocusable = sliderContainer->AddComponent<UIFocusable>();
    sliderFocusable->SetCursorType(Cursor::Type::HAND);

    UILayoutElement *sliderLE =
        sliderContainer->AddComponent<UILayoutElement>();
    sliderLE->SetFlexibleSize(Vector2::One());

    UIImageRenderer *guideRenderer =
        GameObjectFactory::CreateUIImage(Color::Black());
    RectTransform *guideRT = guideRenderer->GetGameObject()->GetRectTransform();
    guideRT->SetAnchorMin(Vector2(-1.0f, 0.0f));
    guideRT->SetAnchorMax(Vector2(1.0f, 0.0f));
    guideRT->SetMarginTop(-1);

    UIImageRenderer *handleRenderer =
        GameObjectFactory::CreateUIImage(slider->m_idleColor);
    handleRenderer->SetImageTexture(TextureFactory::GetCircleIcon());
    RectTransform *handleRT =
        handleRenderer->GetGameObject()->GetRectTransform();
    handleRT->SetAnchors(Vector2::Zero());
    handleRT->SetMargins(-6);

    guideRenderer->GetGameObject()->SetParent(sliderContainer);
    handleRenderer->GetGameObject()->SetParent(sliderContainer);

    UIInputNumber *inputNumber = GameObjectFactory::CreateUIInputNumber();
    UILayoutElement *inputNumberLE =
        inputNumber->GetGameObject()->AddComponent<UILayoutElement>();
    inputNumberLE->SetMinWidth(20);
    inputNumberLE->SetFlexibleWidth(0.2f);
    inputNumber->EventEmitter<IEventsValueChanged>::RegisterListener(slider);

    slider->p_guideRenderer = guideRenderer;
    slider->p_handleRenderer = handleRenderer;
    slider->p_sliderFocusable = sliderFocusable;
    slider->p_inputNumber = inputNumber;

    slider->GetInputNumber()->SetMinMaxValues(0.0f, 100.0f);
    slider->UpdateSliderHandlerFromInputNumberValue();

    sliderContainer->SetParent(go);
    inputNumber->GetGameObject()->SetParent(go);

    return slider;
}
