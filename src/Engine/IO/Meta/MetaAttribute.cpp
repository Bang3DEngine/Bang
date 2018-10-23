#include "Bang/MetaAttribute.h"

using namespace Bang;

MetaAttribute::MetaAttribute()
{
}

MetaAttribute::MetaAttribute(const String &name, const String &value)
{
    Set(name, value);
}

void MetaAttribute::Set(const String &name, const String &value)
{
    SetName(name);
    SetValue(value);
}

const String &MetaAttribute::GetName() const
{
    return m_name;
}

const String &MetaAttribute::GetStringValue() const
{
    return m_value;
}

String MetaAttribute::ToString() const
{
    String str = "";
    str += GetName();
    str += "=\"";
    str += GetStringValue();
    str += "\"";
    return str;
}

MetaAttribute MetaAttribute::FromString(const String &string)
{
    MetaAttribute attribute;

    String str = string.Trim();

    const String TokenSpace = " \t\n";
    int nameBegin = str.IndexOfOneNotOf(TokenSpace, 0);
    if (nameBegin == -1)
    {
        return attribute;
    }

    int nameEnd = str.IndexOfOneOf(TokenSpace + ":" + "=", nameBegin + 1);
    if (nameEnd == -1)
    {
        return attribute;
    }

    int valueBegin = str.IndexOfOneOf("\"", nameEnd + 1) + 1;
    if (valueBegin == -1)
    {
        return attribute;
    }

    int valueEnd = str.IndexOfOneOf("\"", valueBegin + 1);
    if (valueEnd == -1)
    {
        return attribute;
    }

    String name = str.SubString(nameBegin, nameEnd - 1);
    String value = str.SubString(valueBegin, valueEnd - 1);
    attribute.SetName(name);
    attribute.SetValue(value);

    return attribute;
}

bool MetaAttribute::operator==(const MetaAttribute &rhs) const
{
    return GetName() == rhs.GetName() && m_value == rhs.m_value;
}

bool MetaAttribute::operator!=(const MetaAttribute &rhs) const
{
    return !(*this == rhs);
}

void MetaAttribute::SetValue(const String &value)
{
    m_value = value;
}

void MetaAttribute::SetName(const String &name)
{
    m_name = name;
}
