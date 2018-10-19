#include "Bang/ReflectVariableHints.h"

#include "Bang/StreamOperators.h"

using namespace Bang;

const String ReflectVariableHints::KeyMinValue = "MinValue";
const String ReflectVariableHints::KeyMaxValue = "MaxValue";
const String ReflectVariableHints::KeyIsSlider = "IsSlider";
const String ReflectVariableHints::KeyIsEnum = "IsEnum";

ReflectVariableHints::ReflectVariableHints(const String &hintsString)
{
    m_hintsString = hintsString;

    Array<String> hints = hintsString.Split<Array>(';');
    for (const String &hint : hints)
    {
        Array<String> keyValue = hint.Split<Array>(':');
        if (keyValue.Size() == 2)
        {
            const String &keyStr = keyValue.Front();
            const String &valueStr = keyValue.Back();
            std::istringstream iss(valueStr);

            bool isMinValueKey = (keyStr == ReflectVariableHints::KeyMinValue);
            bool isMaxValueKey = (keyStr == ReflectVariableHints::KeyMaxValue);
            if (isMinValueKey || isMaxValueKey)
            {
                Vector4 valueVec;
                if (valueStr.BeginsWith("Vector"))
                {
                    iss >> valueVec;
                }
                else
                {
                    float valueFloat;
                    iss >> valueFloat;
                    valueVec = Vector4(valueFloat);
                }

                if (isMinValueKey)
                {
                    m_minValue = valueVec;
                }
                else
                {
                    m_maxValue = valueVec;
                }
            }
            else if (keyStr == ReflectVariableHints::KeyIsSlider)
            {
                m_isSlider = true;
            }
            else if (keyStr == ReflectVariableHints::KeyIsEnum)
            {
                m_isEnum = true;
            }
        }
    }
}

ReflectVariableHints::~ReflectVariableHints()
{
}

const Vector4 &ReflectVariableHints::GetMinValue() const
{
    return m_minValue;
}

const Vector4 &ReflectVariableHints::GetMaxValue() const
{
    return m_maxValue;
}

bool ReflectVariableHints::GetIsSlider() const
{
    return m_isSlider;
}

bool ReflectVariableHints::GetIsEnum() const
{
    return m_isEnum;
}
