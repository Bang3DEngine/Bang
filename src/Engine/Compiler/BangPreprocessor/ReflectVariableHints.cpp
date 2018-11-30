#include "Bang/ReflectVariableHints.h"

#include "Bang/StreamOperators.h"

using namespace Bang;

const String ReflectVariableHints::KeyMinValue = "MinValue";
const String ReflectVariableHints::KeyMaxValue = "MaxValue";
const String ReflectVariableHints::KeyStepValue = "StepValue";
const String ReflectVariableHints::KeyIsSlider = "IsSlider";
const String ReflectVariableHints::KeyExtension = "Extension";
const String ReflectVariableHints::KeyZoomablePreview = "ZoomablePreview";
const String ReflectVariableHints::KeyIsHidden = "IsHidden";
const String ReflectVariableHints::KeyIsEnum = "IsEnum";

ReflectVariableHints::ReflectVariableHints(const String &hintsString)
{
    Update(hintsString);
}

ReflectVariableHints::~ReflectVariableHints()
{
}

void ReflectVariableHints::Update(const String &hintsString)
{
    Array<String> hints = hintsString.Split<Array>(';');
    for (const String &hint : hints)
    {
        auto IsTrue = [](const String &valueStr) {
            return (valueStr == "1" || valueStr == "true");
        };

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
            else if (keyStr == ReflectVariableHints::KeyStepValue)
            {
                iss >> m_stepValue;
            }
            else if (keyStr == ReflectVariableHints::KeyIsSlider)
            {
                m_isSlider = IsTrue(valueStr);
            }
            else if (keyStr == ReflectVariableHints::KeyIsEnum)
            {
                m_isEnum = IsTrue(valueStr);
            }
            else if (keyStr == ReflectVariableHints::KeyExtension)
            {
                String valueStrMod = valueStr;
                if (valueStrMod.BeginsWith("["))
                {
                    valueStrMod = valueStrMod.SubString(1, valueStr.Size() - 2);
                }

                Array<String> extensions = valueStrMod.Split<Array>(',', true);
                m_extensions.PushBack(extensions);
            }
            else if (keyStr == ReflectVariableHints::KeyZoomablePreview)
            {
                m_zoomablePreview = IsTrue(valueStr);
            }
            else if (keyStr == ReflectVariableHints::KeyIsHidden)
            {
                m_isHidden = IsTrue(valueStr);
            }
        }
    }
}

bool ReflectVariableHints::GetZoomablePreview() const
{
    return m_zoomablePreview;
}

const Array<String> &ReflectVariableHints::GetExtensions() const
{
    return m_extensions;
}

const Vector4 &ReflectVariableHints::GetMinValue() const
{
    return m_minValue;
}

const Vector4 &ReflectVariableHints::GetMaxValue() const
{
    return m_maxValue;
}

float ReflectVariableHints::GetStepValue() const
{
    return m_stepValue;
}

bool ReflectVariableHints::GetIsSlider() const
{
    return m_isSlider;
}

bool ReflectVariableHints::GetIsHidden() const
{
    return m_isHidden;
}

bool ReflectVariableHints::GetIsEnum() const
{
    return m_isEnum;
}
