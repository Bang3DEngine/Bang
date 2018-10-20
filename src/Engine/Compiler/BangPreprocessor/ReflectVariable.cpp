#include "Bang/ReflectVariable.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "Bang/Array.tcc"
#include "Bang/BangPreprocessor.h"
#include "Bang/StreamOperators.h"

using namespace Bang;

using BP = BangPreprocessor;

void ReflectVariable::FromString(String::Iterator propBegin,
                                 String::Iterator propEnd,
                                 ReflectVariable *outReflectedVar,
                                 bool *success)
{
    *success = false;

    String::Iterator propListBegin, propListEnd;
    BP::GetNextScope(
        propBegin, propEnd, &propListBegin, &propListEnd, '(', ')');

    // Process property list
    String propertyListStr(propListBegin + 1, propListEnd - 1);
    Array<String> propertyList = propertyListStr.Split<Array>(',', true);
    if (propertyList.Size() == 0)
    {
        std::cerr << "BP Error: BANG_VARIABLE has 0 properties,"
                     " but must have at least a name"
                  << std::endl;
        return;
    }
    outReflectedVar->m_name = propertyList[0];

    // Skip modifiers
    String nextWord = "";
    String::Iterator wordEnd = propListEnd;
    do
    {
        String::Iterator wordBegin;
        BP::FindNextWord(wordEnd, propEnd, &wordBegin, &wordEnd);
        nextWord = String(wordBegin, wordEnd);
    } while (BP::Modifiers.Contains(nextWord));

    String variableTypeStr = nextWord;
    if (!Variant::ExistsType(variableTypeStr))
    {
        std::cerr << "BP Error: Expected a variable type,"
                     "but got '"
                  << variableTypeStr << "'" << std::endl;
        return;
    }

    outReflectedVar->GetVariant().SetType(
        Variant::GetTypeFromString(variableTypeStr));

    String::Iterator nameBegin, nameEnd;
    BP::FindNextWord(wordEnd, propEnd, &nameBegin, &nameEnd);
    if (nameBegin == propEnd || nameEnd == propEnd)
    {
        std::cerr << "BP Error: Expected a variable name" << std::endl;
        return;
    }
    outReflectedVar->m_codeName = String(nameBegin, nameEnd);

    String::Iterator assignBegin = std::find(nameEnd, propEnd, '=');
    if (assignBegin != propEnd)
    {
        String initValueStr(assignBegin + 1, propEnd - 1);
        initValueStr = initValueStr.Trim({' ', '"'});
        outReflectedVar->SetInitValueString(initValueStr);
    }

    *success = true;
}

void ReflectVariable::SetName(const String &name)
{
    m_name = name;
}

void ReflectVariable::SetCodeName(const String &varCodeName)
{
    m_codeName = varCodeName;
}

void ReflectVariable::SetInitValue(const Variant &initValueVariant)
{
    GetVariant() = initValueVariant;

    std::ostringstream oss;
    oss << initValueVariant;
    m_initValueString = oss.str();
}

void ReflectVariable::SetInitValueString(const String &initValueStr)
{
    m_initValueString = initValueStr;

    std::istringstream iss(GetInitValueString());
    iss >> GetVariant();
}

void ReflectVariable::SetHints(const ReflectVariableHints &hints)
{
    m_hints = hints;
}

void ReflectVariable::SetSetter(std::function<void(const Variant &)> setter)
{
    m_setter = setter;
}

void ReflectVariable::SetGetter(std::function<Variant()> getter)
{
    m_getter = getter;
}

Variant &ReflectVariable::GetVariant()
{
    return m_variant;
}

Variant ReflectVariable::GetCurrentValue() const
{
    if (auto getter = GetGetter())
    {
        return getter();
    }
    return GetInitValue();
}

const String &ReflectVariable::GetName() const
{
    return m_name;
}

const Variant &ReflectVariable::GetVariant() const
{
    return m_variant;
}

const String &ReflectVariable::GetCodeName() const
{
    return m_codeName;
}

const String &ReflectVariable::GetInitValueString() const
{
    return m_initValueString;
}

const ReflectVariableHints &ReflectVariable::GetHints() const
{
    return m_hints;
}

const ReflectVariable::SetterFunc &ReflectVariable::GetSetter() const
{
    return m_setter;
}

const ReflectVariable::GetterFunc &ReflectVariable::GetGetter() const
{
    return m_getter;
}

const Variant &ReflectVariable::GetInitValue() const
{
    return GetVariant();
}

bool ReflectVariable::operator==(const ReflectVariable &rhs) const
{
    return GetName() == rhs.GetName() && GetCodeName() == rhs.GetCodeName() &&
           GetVariant() == rhs.GetVariant() &&
           GetInitValueString() == rhs.GetInitValueString();
}

bool ReflectVariable::operator!=(const ReflectVariable &rhs) const
{
    return !(*this == rhs);
}
