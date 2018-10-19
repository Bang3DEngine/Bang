#ifndef REFLECTVARIABLEHINTS_H
#define REFLECTVARIABLEHINTS_H

#include "Bang/Bang.h"
#include "Bang/Math.h"
#include "Bang/String.h"
#include "Bang/Vector4.h"

#define BANG_REFLECT_HINT_KEY_VALUE(key, value) key + ":" + "" #value + ";"

#define BANG_REFLECT_HINT_MIN_VALUE(minValue) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyMinValue, minValue)

#define BANG_REFLECT_HINT_MAX_VALUE(maxValue) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyMaxValue, maxValue)

#define BANG_REFLECT_HINT_MINMAX_VALUE(minValue, maxValue) \
    BANG_REFLECT_HINT_MIN_VALUE(minValue) +                \
        BANG_REFLECT_HINT_MAX_VALUE(maxValue)

#define BANG_REFLECT_HINT_SLIDER(minValue, maxValue)     \
    BANG_REFLECT_HINT_MINMAX_VALUE(minValue, maxValue) + \
        BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsSlider, true)

#define BANG_REFLECT_HINT_ENUM() \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsEnum, true)

namespace Bang
{
class ReflectVariableHints
{
public:
    static const String KeyMinValue;
    static const String KeyMaxValue;
    static const String KeyIsSlider;
    static const String KeyIsEnum;

    ReflectVariableHints(const String &hintsString = "");
    ~ReflectVariableHints();

    const Vector4 &GetMinValue() const;
    const Vector4 &GetMaxValue() const;
    bool GetIsSlider() const;
    bool GetIsEnum() const;

private:
    String m_hintsString = "";
    Vector4 m_minValue = Vector4::NInfinity;
    Vector4 m_maxValue = Vector4::Infinity;
    bool m_isSlider = false;
    bool m_isEnum = false;
};
}

#endif  // REFLECTVARIABLEHINTS_H
