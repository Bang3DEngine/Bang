#include "Bang/ReflectVariable.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "Bang/Array.tcc"
#include "Bang/BangPreprocessor.h"
#include "Bang/Object.h"
#include "Bang/ReflectMacros.h"
#include "Bang/StreamOperators.h"

using namespace Bang;

using BP = BangPreprocessor;

void ReflectVariable::FromString(String::Iterator varBegin,
                                 String::Iterator varEnd,
                                 ReflectVariable *outReflectedVar,
                                 bool *success)
{
    *success = false;

    String::Iterator varListBegin, varListEnd;
    BP::GetNextScope(varBegin, varEnd, &varListBegin, &varListEnd, '(', ')');

    // Process property list
    String varListStr(varListBegin + 1, varListEnd - 1);
    Array<String> propertyList = varListStr.Split<Array>(',', true);
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
    String::Iterator wordEnd = varListEnd;
    do
    {
        String::Iterator wordBegin;
        BP::FindNextWord(wordEnd, varEnd, &wordBegin, &wordEnd);
        nextWord = String(wordBegin, wordEnd);
    } while (BP::Modifiers.Contains(nextWord));

    String variableTypeStr = nextWord;

    String::Iterator nameBegin, nameEnd;
    BP::FindNextWord(wordEnd, varEnd, &nameBegin, &nameEnd);
    if (nameBegin == varEnd || nameEnd == varEnd)
    {
        std::cerr << "BP Error: Expected a variable name" << std::endl;
        return;
    }

    String varCodeName = String(nameBegin, nameEnd);
    if (varCodeName.BeginsWith("*"))
    {
        const String ptrClassName = variableTypeStr;

        variableTypeStr += "*";
        varCodeName = varCodeName.SubString(1);

        outReflectedVar->GetHintsPtr()->Update(
            BANG_REFLECT_HINT_OBJECT_PTR_CLASS(ptrClassName));
    }

    if (!Variant::ExistsType(variableTypeStr))
    {
        std::cerr << "BP Error: Expected a variable type, "
                     "but got '"
                  << variableTypeStr << "'" << std::endl;
        return;
    }

    outReflectedVar->SetTypeString(variableTypeStr);
    outReflectedVar->GetVariant().SetType(
        Variant::GetTypeFromString(variableTypeStr));
    outReflectedVar->m_codeName = varCodeName;

    String::Iterator assignBegin = std::find(nameEnd, varEnd, '=');
    if (assignBegin != varEnd)
    {
        String initValueStr(assignBegin + 1, varEnd - 1);
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

void ReflectVariable::SetTypeString(const String &typeString)
{
    m_typeString = typeString;
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

ReflectVariableHints *ReflectVariable::GetHintsPtr()
{
    return &m_hints;
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

ReflectVariable::SetterFunc ReflectVariable::GetSetter() const
{
    return m_setter;
}

ReflectVariable::GetterFunc ReflectVariable::GetGetter() const
{
    return m_getter;
}

const String &ReflectVariable::GetTypeString() const
{
    return m_typeString;
}

const String &ReflectVariable::GetInitValueString() const
{
    return m_initValueString;
}

const ReflectVariableHints &ReflectVariable::GetHints() const
{
    return m_hints;
}

bool ReflectVariable::EqualsWithoutValue(const ReflectVariable &rhs) const
{
    return GetName() == rhs.GetName() && GetCodeName() == rhs.GetCodeName() &&
           GetHints() == rhs.GetHints() &&
           GetInitValueString() == rhs.GetInitValueString();
}

const Variant &ReflectVariable::GetInitValue() const
{
    return GetVariant();
}

bool ReflectVariable::operator==(const ReflectVariable &rhs) const
{
    return EqualsWithoutValue(rhs) && GetVariant() == rhs.GetVariant();
}

bool ReflectVariable::operator!=(const ReflectVariable &rhs) const
{
    return !(*this == rhs);
}
