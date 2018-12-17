#ifndef REFLECTMACROS_H
#define REFLECTMACROS_H

#define BANG_REFLECT_VAR_HINTED(Name, Setter, Getter, Hints) \
    ReflectVar(Name, Setter, Getter, Hints);
#define BANG_REFLECT_VAR(Name, Setter, Getter) \
    BANG_REFLECT_VAR_HINTED(Name, Setter, Getter, "");

#define BANG_REFLECT_VAR_MEMBER_HINTED(Class, Name, Setter, Getter, Hints) \
    ReflectVarMember(Name, &Class::Setter, &Class::Getter, this, Hints);
#define BANG_REFLECT_VAR_MEMBER(Class, Name, Setter, Getter) \
    BANG_REFLECT_VAR_MEMBER_HINTED(Class, Name, Setter, Getter, "")

#define BANG_REFLECT_VAR_MEMBER_ENUM(Class, Name, Setter, Getter) \
    ReflectVarMember(Name,                                        \
                     &Class::Setter,                              \
                     &Class::Getter,                              \
                     this,                                        \
                     BANG_REFLECT_HINT_ENUM(true));
#define BANG_REFLECT_VAR_MEMBER_ENUM_FLAGS(Class, Name, Setter, Getter) \
    ReflectVar<FlagsPrimitiveType>(                                     \
        Name,                                                           \
        [this](FlagsPrimitiveType x) {                                  \
            Setter(SCAST<FlagsPrimitiveType>(x));                       \
        },                                                              \
        [this]() { return SCAST<FlagsPrimitiveType>(Getter()); },       \
        this,                                                           \
        BANG_REFLECT_HINT_ENUM_FLAGS(true));

#define BANG_REFLECT_VAR_MEMBER_RESOURCE(                                    \
    Class, Name, Setter, Getter, ResourceClass, Hints)                       \
    ReflectVar<GUID>(                                                        \
        Name,                                                                \
        [this](GUID v) { Setter(Resources::Load<ResourceClass>(v).Get()); }, \
        [this]() -> GUID {                                                   \
            return Getter() ? Getter()->GetGUID() : GUID::Empty();           \
        },                                                                   \
        Hints);

#define BANG_REFLECT_BUTTON(Class, Name, ActionFunction) \
    ReflectVar<bool>(                                    \
        Name,                                            \
        [=](bool) { ActionFunction(); },                 \
        []() { return true; },                           \
        BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsButton, true));

#define BANG_REFLECT_HINT_ENUM_FIELD_VALUE(   \
    enumName, enumFieldName, enumFieldValue)  \
    GetReflectStructPtr()->AddEnumFieldValue( \
        enumName, enumFieldName, enumFieldValue)

#define BANG_REFLECT_HINT_KEY_VALUE(key, value) \
    key + ":" + String::ToString(value) + ";"

#define BANG_REFLECT_HINT_OBJECT_PTR_CLASS(objectPtrClass)               \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyObjectPtrClass, \
                                objectPtrClass)

#define BANG_REFLECT_HINT_MIN_VALUE(minValue) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyMinValue, minValue)

#define BANG_REFLECT_HINT_MAX_VALUE(maxValue) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyMaxValue, maxValue)

#define BANG_REFLECT_HINT_SHOWN(trueOrFalse) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsShown, trueOrFalse)

#define BANG_REFLECT_HINT_MINMAX_VALUE(minValue, maxValue) \
    BANG_REFLECT_HINT_MIN_VALUE(minValue) +                \
        BANG_REFLECT_HINT_MAX_VALUE(maxValue)

#define BANG_REFLECT_HINT_STEP_VALUE(stepValue) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyStepValue, stepValue)

#define BANG_REFLECT_HINT_EXTENSIONS(extensions) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyExtension, extensions)

#define BANG_REFLECT_HINT_SLIDER(minValue, maxValue)     \
    BANG_REFLECT_HINT_MINMAX_VALUE(minValue, maxValue) + \
        BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsSlider, true)

#define BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(trueOrFalse)                   \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyZoomablePreview, \
                                trueOrFalse)

#define BANG_REFLECT_HINT_ENUM(trueOrFalse) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsEnum, trueOrFalse)
#define BANG_REFLECT_HINT_ENUM_FLAGS(trueOrFalse)                     \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsEnumFlags, \
                                trueOrFalse)

#endif  // REFLECTMACROS_H
