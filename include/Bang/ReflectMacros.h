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
    ReflectVarMemberEnum(                                         \
        Name, &Class::Setter, &Class::Getter, this, BANG_REFLECT_HINT_ENUM());

#define BANG_REFLECT_VAR_MEMBER_RESOURCE(                                    \
    Class, Name, Setter, Getter, ResourceClass, Hints)                       \
    ReflectVar<GUID>(                                                        \
        Name,                                                                \
        [this](GUID v) { Setter(Resources::Load<ResourceClass>(v).Get()); }, \
        [this]() -> GUID {                                                   \
            return Getter() ? Getter()->GetGUID() : GUID::Empty();           \
        },                                                                   \
        Hints);

#define BANG_REFLECT_HINT_ENUM_FIELD(enumName, enumFieldName) \
    GetReflectStructPtr()->AddEnumField(enumName, enumFieldName)

#define BANG_REFLECT_HINT_ENUM_FIELD_VALUE(   \
    enumName, enumFieldName, enumFieldValue)  \
    GetReflectStructPtr()->AddEnumFieldValue( \
        enumName, enumFieldName, enumFieldValue)

#define BANG_REFLECT_HINT_KEY_VALUE(key, value) \
    key + ":" + String::ToString(value) + ";"

#define BANG_REFLECT_HINT_MIN_VALUE(minValue) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyMinValue, minValue)

#define BANG_REFLECT_HINT_MAX_VALUE(maxValue) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyMaxValue, maxValue)

#define BANG_REFLECT_HINT_MINMAX_VALUE(minValue, maxValue) \
    BANG_REFLECT_HINT_MIN_VALUE(minValue) +                \
        BANG_REFLECT_HINT_MAX_VALUE(maxValue)

#define BANG_REFLECT_HINT_EXTENSION(extension) \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyExtension, extension)

#define BANG_REFLECT_HINT_SLIDER(minValue, maxValue)     \
    BANG_REFLECT_HINT_MINMAX_VALUE(minValue, maxValue) + \
        BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsSlider, true)

#define BANG_REFLECT_HINT_ZOOMABLE_PREVIEW() \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyZoomablePreview, true)

#define BANG_REFLECT_HINT_ENUM() \
    BANG_REFLECT_HINT_KEY_VALUE(ReflectVariableHints::KeyIsEnum, true)

#endif  // REFLECTMACROS_H
