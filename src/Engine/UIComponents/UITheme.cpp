#include "Bang/UITheme.h"

using namespace Bang;

Color UITheme::GetOverColor()
{
    return UITheme::GetBaseColor().WithSaturation(0.5f);
}

Color UITheme::GetSelectedColor()
{
    return UITheme::GetBaseColor().WithSaturation(0.85f);
}

Color UITheme::GetFocusedBorderColor()
{
    return UITheme::GetBaseColor();
}

Color UITheme::GetNotFocusedBorderColor()
{
    return Color::Black();
}

float UITheme::GetFocusedBorderStroke()
{
    return 2.0f;
}

float UITheme::GetNotFocusedBorderStroke()
{
    return 1.0f;
}

Color UITheme::GetArrowsColor()
{
    return Color::Black();
}

Color UITheme::GetInputsBackgroundColor()
{
    return Color::White();
}

Color UITheme::GetTickColor()
{
    return Color::Black();  // UITheme::GetBaseColor();
}

Color UITheme::GetCheckBoxBackgroundColor()
{
    return UITheme::GetInputsBackgroundColor();
}

Color UITheme::GetInputTextBackgroundColor()
{
    return UITheme::GetInputsBackgroundColor();
}

Color UITheme::GetInputTextBlockedBackgroundColor()
{
    return Color::Zero();
}

Color UITheme::GetSelectionTextColor()
{
    return UITheme::GetBaseColor().WithSaturation(0.5f);
}

Color UITheme::GetBaseColor()
{
    return Color::Orange();
}
