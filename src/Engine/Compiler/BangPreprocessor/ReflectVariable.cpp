#include "Bang/ReflectVariable.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "Bang/Array.tcc"
#include "Bang/BangPreprocessor.h"

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
        std::cerr << "BP Error: BANG_REFLECT_VARIABLE has 0 properties,"
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
        String initValue(assignBegin + 1, propEnd - 1);
        initValue = initValue.Trim({' ', '"'});
        outReflectedVar->m_initValue = initValue;
    }

    *success = true;
}

String ReflectVariable::GetInitializationCode(
    const String &rvarInitVarName) const
{
    String src = R"VERBATIM(
        RVAR_VARIABLE_NAME.SetName("RVAR_NAME");
        RVAR_VARIABLE_NAME.GetVariant().SetType(Variant::Type::VARIABLE_TYPE);
        RVAR_VARIABLE_NAME.SetCodeName("VARIABLE_CODE_NAME");
        RVAR_VARIABLE_NAME.SetInitValue("VARIABLE_INIT_VALUE");
    )VERBATIM";
    src.ReplaceInSitu("RVAR_VARIABLE_NAME", rvarInitVarName);
    src.ReplaceInSitu("RVAR_NAME", GetName());
    src.ReplaceInSitu(
        "VARIABLE_TYPE",
        Variant::GetTypeToString(GetVariant().GetType()).ToUpper());
    src.ReplaceInSitu("VARIABLE_CODE_NAME", GetCodeName());
    src.ReplaceInSitu("VARIABLE_INIT_VALUE", GetInitValue());
    return src;
}

void ReflectVariable::SetName(const String &name)
{
    m_name = name;
}

void ReflectVariable::SetCodeName(const String &varCodeName)
{
    m_codeName = varCodeName;
}

void ReflectVariable::SetInitValue(const String &initValue)
{
    m_initValue = initValue;
}

Variant &ReflectVariable::GetVariant()
{
    return m_variant;
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

const String &ReflectVariable::GetInitValue() const
{
    return m_initValue;
}

bool ReflectVariable::operator==(const ReflectVariable &rhs) const
{
    return GetName() == rhs.GetName() && GetCodeName() == rhs.GetCodeName() &&
           GetVariant() == rhs.GetVariant() &&
           GetInitValue() == rhs.GetInitValue();
}

bool ReflectVariable::operator!=(const ReflectVariable &rhs) const
{
    return !(*this == rhs);
}
