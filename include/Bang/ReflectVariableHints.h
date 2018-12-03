#ifndef REFLECTVARIABLEHINTS_H
#define REFLECTVARIABLEHINTS_H

#include "Bang/Bang.h"
#include "Bang/Math.h"
#include "Bang/String.h"
#include "Bang/Vector4.h"

namespace Bang
{
class ReflectVariableHints
{
public:
    static const String KeyMinValue;
    static const String KeyMaxValue;
    static const String KeyStepValue;
    static const String KeyExtension;
    static const String KeyButton;
    static const String KeyZoomablePreview;
    static const String KeyIsSlider;
    static const String KeyIsShown;
    static const String KeyIsEnum;

    ReflectVariableHints(const String &hintsString = "");
    ~ReflectVariableHints();

    void Update(const String &hintsString);

    bool GetZoomablePreview() const;
    const Array<String> &GetExtensions() const;
    const Vector4 &GetMinValue() const;
    const Vector4 &GetMaxValue() const;
    float GetStepValue() const;
    bool GetIsButton() const;
    bool GetIsSlider() const;
    bool GetIsShown() const;
    bool GetIsEnum() const;

    bool operator==(const ReflectVariableHints &rhs) const;
    bool operator!=(const ReflectVariableHints &rhs) const;

private:
    bool m_isButton = false;
    bool m_zoomablePreview = false;
    float m_stepValue = 1.0f;
    Vector4 m_minValue = Vector4::NInfinity();
    Vector4 m_maxValue = Vector4::Infinity();
    Array<String> m_extensions;
    bool m_isSlider = false;
    bool m_isShown = true;
    bool m_isEnum = false;
};
}

#endif  // REFLECTVARIABLEHINTS_H
