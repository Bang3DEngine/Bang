#include "Bang/ReflectVariableHints.h"

#include "Bang/ClassDB.h"
#include "Bang/ReflectMacros.h"
#include "Bang/StreamOperators.h"

using namespace Bang;

const String ReflectVariableHints::KeyObjectPtrClass = "ObjectPtrClass";
const String ReflectVariableHints::KeyMinValue = "MinValue";
const String ReflectVariableHints::KeyMaxValue = "MaxValue";
const String ReflectVariableHints::KeyStepValue = "StepValue";
const String ReflectVariableHints::KeyIsSlider = "IsSlider";
const String ReflectVariableHints::KeyIsButton = "IsButton";
const String ReflectVariableHints::KeyIsBlocked = "IsBlocked";
const String ReflectVariableHints::KeyExtension = "Extension";
const String ReflectVariableHints::KeyIsZoomablePreview = "IsZoomablePreview";
const String ReflectVariableHints::KeyIsShown = "IsShown";
const String ReflectVariableHints::KeyIsEnum = "IsEnum";
const String ReflectVariableHints::KeyIsEnumFlags = "IsEnumFlags";

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
                if (valueStr.BeginsWith("Vector") || valueStr.BeginsWith("("))
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
                    SetMinValue(valueVec);
                }
                else
                {
                    SetMaxValue(valueVec);
                }
            }
            else if (keyStr == ReflectVariableHints::KeyIsButton)
            {
                SetIsButton(true);
            }
            else if (keyStr == ReflectVariableHints::KeyObjectPtrClass)
            {
                String objectPtrClass;
                iss >> objectPtrClass;
                SetObjectPtrClass(objectPtrClass);
            }
            else if (keyStr == ReflectVariableHints::KeyStepValue)
            {
                float stepValue;
                iss >> stepValue;
                SetStepValue(stepValue);
            }
            else if (keyStr == ReflectVariableHints::KeyIsSlider)
            {
                SetIsSlider(IsTrue(valueStr));
            }
            else if (keyStr == ReflectVariableHints::KeyIsEnum)
            {
                SetIsEnum(IsTrue(valueStr));
            }
            else if (keyStr == ReflectVariableHints::KeyIsEnumFlags)
            {
                SetIsEnumFlags(IsTrue(valueStr));
            }
            else if (keyStr == ReflectVariableHints::KeyExtension)
            {
                String valueStrMod = valueStr;
                if (valueStrMod.BeginsWith("["))
                {
                    valueStrMod = valueStrMod.SubString(1, valueStr.Size() - 2);
                }

                Array<String> extensions = valueStrMod.Split<Array>(',', true);
                SetExtensions(extensions);
            }
            else if (keyStr == ReflectVariableHints::KeyIsZoomablePreview)
            {
                SetIsZoomablePreview(IsTrue(valueStr));
            }
            else if (keyStr == ReflectVariableHints::KeyIsShown)
            {
                SetIsShown(IsTrue(valueStr));
            }
            else if (keyStr == ReflectVariableHints::KeyIsBlocked)
            {
                SetIsBlocked(IsTrue(valueStr));
            }
        }
    }
}

void ReflectVariableHints::SetObjectPtrClass(const String &objectPtrClass)
{
    m_objectPtrClass = objectPtrClass;
}

void ReflectVariableHints::SetIsZoomablePreview(bool zoomablePreview)
{
    m_isZoomablePreview = zoomablePreview;
}

void ReflectVariableHints::AddExtensions(const Array<String> &extensions)
{
    m_extensions.PushBack(extensions);
}

void ReflectVariableHints::SetExtensions(const Array<String> &extensions)
{
    m_extensions = extensions;
}

void ReflectVariableHints::SetMinValue(const Vector4 &minValue)
{
    m_minValue = minValue;
}

void ReflectVariableHints::SetMaxValue(const Vector4 &maxValue)
{
    m_maxValue = maxValue;
}

void ReflectVariableHints::SetStepValue(float stepValue)
{
    m_stepValue = stepValue;
}

void ReflectVariableHints::SetIsButton(bool isButton)
{
    m_isButton = isButton;
}

void ReflectVariableHints::SetIsSlider(bool isSlider)
{
    m_isSlider = isSlider;
}

void ReflectVariableHints::SetIsShown(bool isShown)
{
    m_isShown = isShown;
}

void ReflectVariableHints::SetIsEnum(bool isEnum)
{
    m_isEnum = isEnum;
}

void ReflectVariableHints::SetIsBlocked(bool isBlocked)
{
    m_isBlocked = isBlocked;
}

void ReflectVariableHints::SetIsEnumFlags(bool isEnumFlags)
{
    m_isEnumFlags = isEnumFlags;
}

String ReflectVariableHints::GetObjectPtrClass() const
{
    return m_objectPtrClass;
}

ClassIdType ReflectVariableHints::GetObjectPtrClassIdBegin() const
{
    return ClassDB::GetClassIdBegin(GetObjectPtrClass());
}

ClassIdType ReflectVariableHints::GetObjectPtrClassIdEnd() const
{
    return ClassDB::GetClassIdEnd(GetObjectPtrClass());
}

bool ReflectVariableHints::GetIsZoomablePreview() const
{
    return m_isZoomablePreview;
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

bool ReflectVariableHints::GetIsButton() const
{
    return m_isButton;
}

bool ReflectVariableHints::GetIsSlider() const
{
    return m_isSlider;
}

bool ReflectVariableHints::GetIsShown() const
{
    return m_isShown;
}

bool ReflectVariableHints::GetIsEnum() const
{
    return m_isEnum;
}

bool ReflectVariableHints::GetIsBlocked() const
{
    return m_isBlocked;
}

bool ReflectVariableHints::GetIsEnumFlags() const
{
    return m_isEnumFlags;
}

String ReflectVariableHints::GetHintsString() const
{
    String hintsString = "";

#define ADD_TO_HINTS_STRING(KEY_NAME)           \
    hintsString += BANG_REFLECT_HINT_KEY_VALUE( \
        ReflectVariableHints::Key##KEY_NAME, Get##KEY_NAME());

    ADD_TO_HINTS_STRING(ObjectPtrClass);
    ADD_TO_HINTS_STRING(MinValue);
    ADD_TO_HINTS_STRING(MaxValue);
    ADD_TO_HINTS_STRING(StepValue);
    ADD_TO_HINTS_STRING(IsSlider);
    ADD_TO_HINTS_STRING(IsButton);
    ADD_TO_HINTS_STRING(IsBlocked);
    for (const String &extension : GetExtensions())
    {
        hintsString += BANG_REFLECT_HINT_KEY_VALUE(
            ReflectVariableHints::KeyExtension, extension);
    }
    ADD_TO_HINTS_STRING(IsZoomablePreview);
    ADD_TO_HINTS_STRING(IsShown);
    ADD_TO_HINTS_STRING(IsEnum);
    ADD_TO_HINTS_STRING(IsEnumFlags);

#undef ADD_TO_HINTS_STRING

    return hintsString;
}

bool ReflectVariableHints::operator==(const ReflectVariableHints &rhs) const
{
    return GetHintsString() == rhs.GetHintsString();
}

bool ReflectVariableHints::operator!=(const ReflectVariableHints &rhs) const
{
    return !(*this == rhs);
}
