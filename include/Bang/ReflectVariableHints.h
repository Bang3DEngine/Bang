#ifndef REFLECTVARIABLEHINTS_H
#define REFLECTVARIABLEHINTS_H

#include "Bang/Bang.h"
#include "Bang/ClassDB.h"
#include "Bang/Math.h"
#include "Bang/String.h"
#include "Bang/Vector4.h"

namespace Bang
{
class ReflectVariableHints
{
public:
    static const String KeyObjectPtrClass;
    static const String KeyMinValue;
    static const String KeyMaxValue;
    static const String KeyStepValue;
    static const String KeyExtension;
    static const String KeyIsButton;
    static const String KeyIsBlocked;
    static const String KeyIsZoomablePreview;
    static const String KeyIsSlider;
    static const String KeyIsShown;
    static const String KeyIsEnum;
    static const String KeyIsEnumFlags;

    ReflectVariableHints(const String &hintsString = "");
    ~ReflectVariableHints();

    void Update(const String &hintsString);

    void SetObjectPtrClass(const String &objectPtrClass);
    void AddExtensions(const Array<String> &extensions);
    void SetExtensions(const Array<String> &extensions);
    void SetMinValue(const Vector4 &minValue);
    void SetMaxValue(const Vector4 &maxValue);
    void SetIsZoomablePreview(bool zoomablePreview);
    void SetStepValue(float stepValue);
    void SetIsButton(bool isButton);
    void SetIsSlider(bool isSlider);
    void SetIsShown(bool isShown);
    void SetIsEnum(bool isEnum);
    void SetIsBlocked(bool isBlocked);
    void SetIsEnumFlags(bool isEnumFlags);

    String GetObjectPtrClass() const;
    ClassIdType GetObjectPtrClassIdBegin() const;
    ClassIdType GetObjectPtrClassIdEnd() const;
    const Array<String> &GetExtensions() const;
    const Vector4 &GetMinValue() const;
    const Vector4 &GetMaxValue() const;
    float GetStepValue() const;
    bool GetIsZoomablePreview() const;
    bool GetIsButton() const;
    bool GetIsSlider() const;
    bool GetIsShown() const;
    bool GetIsEnum() const;
    bool GetIsBlocked() const;
    bool GetIsEnumFlags() const;

    String GetHintsString() const;

    bool operator==(const ReflectVariableHints &rhs) const;
    bool operator!=(const ReflectVariableHints &rhs) const;

private:
    String m_objectPtrClass = "";
    bool m_isButton = false;
    bool m_isZoomablePreview = false;
    bool m_isBlocked = false;
    float m_stepValue = 1.0f;
    Vector4 m_minValue = Vector4::NInfinity();
    Vector4 m_maxValue = Vector4::Infinity();
    Array<String> m_extensions;
    bool m_isSlider = false;
    bool m_isShown = true;
    bool m_isEnum = false;
    bool m_isEnumFlags = false;
};
}

#endif  // REFLECTVARIABLEHINTS_H
