#include "Bang/BPReflectedVariable.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "Bang/Array.tcc"
#include "Bang/BangPreprocessor.h"

using namespace Bang;

using BP = BangPreprocessor;

BPReflectedVariable::BPReflectedVariable()
{
}

void BPReflectedVariable::FromString(String::Iterator propBegin,
                                     String::Iterator propEnd,
                                     BPReflectedVariable *outReflectedVar,
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
    if (!BPReflectedVariable::ExistsType(variableTypeStr))
    {
        std::cerr << "BP Error: Expected a variable type,"
                     "but got '"
                  << variableTypeStr << "'" << std::endl;
        return;
    }

    outReflectedVar->SetType(
        BPReflectedVariable::GetTypeFromString(variableTypeStr));

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

String BPReflectedVariable::GetInitializationCode(
    const String &rvarInitVarName) const
{
    String src = R"VERBATIM(
            RVAR_VARIABLE_NAME.SetName("RVAR_NAME");
            RVAR_VARIABLE_NAME.SetType(BPReflectedVariable::Type::VARIABLE_TYPE);
            RVAR_VARIABLE_NAME.SetCodeName("VARIABLE_CODE_NAME");
            RVAR_VARIABLE_NAME.SetInitValue("VARIABLE_INIT_VALUE");
    )VERBATIM";
    src.ReplaceInSitu("RVAR_VARIABLE_NAME", rvarInitVarName);
    src.ReplaceInSitu("RVAR_NAME", GetName());
    src.ReplaceInSitu(
        "VARIABLE_TYPE",
        BPReflectedVariable::GetTypeToString(GetType()).ToUpper());
    src.ReplaceInSitu("VARIABLE_CODE_NAME", GetCodeName());
    src.ReplaceInSitu("VARIABLE_INIT_VALUE", GetInitValue());
    return src;
}

void BPReflectedVariable::SetName(const String &name)
{
    m_name = name;
}

void BPReflectedVariable::SetType(BPReflectedVariable::Type type)
{
    m_variableType = type;
}

void BPReflectedVariable::SetCodeName(const String &varCodeName)
{
    m_codeName = varCodeName;
}

void BPReflectedVariable::SetInitValue(const String &initValue)
{
    m_initValue = initValue;
}

const String &BPReflectedVariable::GetName() const
{
    return m_name;
}

BPReflectedVariable::Type BPReflectedVariable::GetType() const
{
    return m_variableType;
}

const String &BPReflectedVariable::GetCodeName() const
{
    return m_codeName;
}

const String &BPReflectedVariable::GetInitValue() const
{
    return m_initValue;
}

String BPReflectedVariable::GetTypeToString(BPReflectedVariable::Type type)
{
    switch (type)
    {
        case BPReflectedVariable::Type::FLOAT: return "float";
        case BPReflectedVariable::Type::DOUBLE: return "double";
        case BPReflectedVariable::Type::INT: return "int";
        case BPReflectedVariable::Type::BOOL: return "bool";
        case BPReflectedVariable::Type::COLOR: return "Color";
        case BPReflectedVariable::Type::STRING: return "String";
        case BPReflectedVariable::Type::VECTOR2: return "Vector2";
        case BPReflectedVariable::Type::VECTOR3: return "Vector3";
        case BPReflectedVariable::Type::VECTOR4: return "Vector4";
        case BPReflectedVariable::Type::QUATERNION: return "Quaternion";
        default: break;
    }
    return "None";
}

BPReflectedVariable::Type BPReflectedVariable::GetTypeFromString(
    const String &typeStr)
{
    if (typeStr == "float")
    {
        return BPReflectedVariable::Type::FLOAT;
    }
    else if (typeStr == "double")
    {
        return BPReflectedVariable::Type::DOUBLE;
    }
    else if (typeStr == "int")
    {
        return BPReflectedVariable::Type::INT;
    }
    else if (typeStr == "bool")
    {
        return BPReflectedVariable::Type::BOOL;
    }
    else if (typeStr == "Color")
    {
        return BPReflectedVariable::Type::COLOR;
    }
    else if (typeStr == "String")
    {
        return BPReflectedVariable::Type::STRING;
    }
    else if (typeStr == "Vector2")
    {
        return BPReflectedVariable::Type::VECTOR2;
    }
    else if (typeStr == "Vector3")
    {
        return BPReflectedVariable::Type::VECTOR3;
    }
    else if (typeStr == "Vector4")
    {
        return BPReflectedVariable::Type::VECTOR4;
    }
    else if (typeStr == "Quaternion")
    {
        return BPReflectedVariable::Type::QUATERNION;
    }
    return BPReflectedVariable::Type::NONE;
}

bool BPReflectedVariable::ExistsType(const String &typeStr)
{
    return (BPReflectedVariable::GetTypeFromString(typeStr) !=
            BPReflectedVariable::Type::NONE);
}

bool BPReflectedVariable::operator==(const BPReflectedVariable &rhs) const
{
    return GetName() == rhs.GetName() && GetCodeName() == rhs.GetCodeName() &&
           GetType() == rhs.GetType() && GetInitValue() == rhs.GetInitValue();
}

bool BPReflectedVariable::operator!=(const BPReflectedVariable &rhs) const
{
    return !(*this == rhs);
}

String BPReflectedVariable::ToString() const
{
    return "(" + GetName() + ", " +
           BPReflectedVariable::GetTypeToString(GetType()) + ", " +
           GetCodeName() + " = " + GetInitValue() + ")";
}
