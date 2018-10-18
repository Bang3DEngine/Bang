#include "Bang/ReflectVariable.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "Bang/Array.tcc"
#include "Bang/BangPreprocessor.h"

using namespace Bang;

using BP = BangPreprocessor;

ReflectVariable::ReflectVariable()
{
}

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
    if (!ReflectVariable::ExistsType(variableTypeStr))
    {
        std::cerr << "BP Error: Expected a variable type,"
                     "but got '"
                  << variableTypeStr << "'" << std::endl;
        return;
    }

    outReflectedVar->SetType(
        ReflectVariable::GetTypeFromString(variableTypeStr));

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
            RVAR_VARIABLE_NAME.SetType(ReflectVariable::Type::VARIABLE_TYPE);
            RVAR_VARIABLE_NAME.SetCodeName("VARIABLE_CODE_NAME");
            RVAR_VARIABLE_NAME.SetInitValue("VARIABLE_INIT_VALUE");
    )VERBATIM";
    src.ReplaceInSitu("RVAR_VARIABLE_NAME", rvarInitVarName);
    src.ReplaceInSitu("RVAR_NAME", GetName());
    src.ReplaceInSitu("VARIABLE_TYPE",
                      ReflectVariable::GetTypeToString(GetType()).ToUpper());
    src.ReplaceInSitu("VARIABLE_CODE_NAME", GetCodeName());
    src.ReplaceInSitu("VARIABLE_INIT_VALUE", GetInitValue());
    return src;
}

void ReflectVariable::SetName(const String &name)
{
    m_name = name;
}

void ReflectVariable::SetType(ReflectVariable::Type type)
{
    m_variableType = type;
}

void ReflectVariable::SetCodeName(const String &varCodeName)
{
    m_codeName = varCodeName;
}

void ReflectVariable::SetInitValue(const String &initValue)
{
    m_initValue = initValue;
}

const String &ReflectVariable::GetName() const
{
    return m_name;
}

ReflectVariable::Type ReflectVariable::GetType() const
{
    return m_variableType;
}

const String &ReflectVariable::GetCodeName() const
{
    return m_codeName;
}

const String &ReflectVariable::GetInitValue() const
{
    return m_initValue;
}

String ReflectVariable::GetTypeToString(ReflectVariable::Type type)
{
    switch (type)
    {
        case ReflectVariable::Type::FLOAT: return "float";
        case ReflectVariable::Type::DOUBLE: return "double";
        case ReflectVariable::Type::INT: return "int";
        case ReflectVariable::Type::BOOL: return "bool";
        case ReflectVariable::Type::COLOR: return "Color";
        case ReflectVariable::Type::STRING: return "String";
        case ReflectVariable::Type::VECTOR2: return "Vector2";
        case ReflectVariable::Type::VECTOR3: return "Vector3";
        case ReflectVariable::Type::VECTOR4: return "Vector4";
        case ReflectVariable::Type::QUATERNION: return "Quaternion";
        default: break;
    }
    return "None";
}

ReflectVariable::Type ReflectVariable::GetTypeFromString(const String &typeStr)
{
    if (typeStr == "float")
    {
        return ReflectVariable::Type::FLOAT;
    }
    else if (typeStr == "double")
    {
        return ReflectVariable::Type::DOUBLE;
    }
    else if (typeStr == "int")
    {
        return ReflectVariable::Type::INT;
    }
    else if (typeStr == "bool")
    {
        return ReflectVariable::Type::BOOL;
    }
    else if (typeStr == "Color")
    {
        return ReflectVariable::Type::COLOR;
    }
    else if (typeStr == "String")
    {
        return ReflectVariable::Type::STRING;
    }
    else if (typeStr == "Vector2")
    {
        return ReflectVariable::Type::VECTOR2;
    }
    else if (typeStr == "Vector3")
    {
        return ReflectVariable::Type::VECTOR3;
    }
    else if (typeStr == "Vector4")
    {
        return ReflectVariable::Type::VECTOR4;
    }
    else if (typeStr == "Quaternion")
    {
        return ReflectVariable::Type::QUATERNION;
    }
    return ReflectVariable::Type::NONE;
}

bool ReflectVariable::ExistsType(const String &typeStr)
{
    return (ReflectVariable::GetTypeFromString(typeStr) !=
            ReflectVariable::Type::NONE);
}

bool ReflectVariable::operator==(const ReflectVariable &rhs) const
{
    return GetName() == rhs.GetName() && GetCodeName() == rhs.GetCodeName() &&
           GetType() == rhs.GetType() && GetInitValue() == rhs.GetInitValue();
}

bool ReflectVariable::operator!=(const ReflectVariable &rhs) const
{
    return !(*this == rhs);
}

String ReflectVariable::ToString() const
{
    return "(" + GetName() + ", " +
           ReflectVariable::GetTypeToString(GetType()) + ", " + GetCodeName() +
           " = " + GetInitValue() + ")";
}
