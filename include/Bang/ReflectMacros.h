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

#define BANG_REFLECT_VAR_ENUM(Name, Setter, Getter, EnumType)       \
    ReflectVar<uint32_t>(                                           \
        Name,                                                       \
        [this](uint32_t x) { Setter(SCAST<EnumType>(x)); },         \
        [this]() -> uint32_t { return SCAST<uint32_t>(Getter()); }, \
        BANG_REFLECT_HINT_ENUM(true));

#define BANG_REFLECT_VAR_ENUM_FLAGS(Name, Setter, Getter)         \
    ReflectVar<FlagsPrimitiveType>(                               \
        Name,                                                     \
        [this](FlagsPrimitiveType x) {                            \
            Setter(SCAST<FlagsPrimitiveType>(x));                 \
        },                                                        \
        [this]() { return SCAST<FlagsPrimitiveType>(Getter()); }, \
        BANG_REFLECT_HINT_ENUM_FLAGS(true));

#define BANG_REFLECT_VAR_ASSET(Name, Setter, Getter, AssetClass, Hints)   \
    ReflectVar<GUID>(                                                     \
        Name,                                                             \
        [this](GUID v) { Setter(Assets::Load<AssetClass>(v).Get()); },    \
        [this]() -> GUID {                                                \
            return Getter() ? Getter()->GetGUID() : GUID::Empty();        \
        },                                                                \
        BANG_REFLECT_HINT_EXTENSIONS(                                     \
            Extensions::GetExtension(AssetClass::GetClassNameStatic())) + \
            Hints);

#define BANG_REFLECT_BUTTON_HINTED(Class, Name, ActionFunction, Hints)         \
    ReflectVar<bool>(                                                          \
        Name,                                                                  \
        [=](bool) { ActionFunction(); },                                       \
        []() { return true; },                                                 \
        BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsButton, true) + \
            Hints);
#define BANG_REFLECT_BUTTON(Class, Name, ActionFunction) \
    BANG_REFLECT_BUTTON_HINTED(Class, Name, ActionFunction, "")

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

#define BANG_REFLECT_HINT_BLOCKED(blocked) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsBlocked, blocked)

#define BANG_REFLECT_HINT_SLIDER(minValue, maxValue)     \
    BANG_REFLECT_HINT_MINMAX_VALUE(minValue, maxValue) + \
        BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsSlider, true)

#define BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(trueOrFalse)                     \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsZoomablePreview, \
                                trueOrFalse)

#define BANG_REFLECT_HINT_ENUM(trueOrFalse) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsEnum, trueOrFalse)
#define BANG_REFLECT_HINT_ENUM_FLAGS(trueOrFalse)                     \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsEnumFlags, \
                                trueOrFalse)

#endif  // REFLECTMACROS_H
