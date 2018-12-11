#include "Bang/ReflectStruct.h"

#include <iostream>
#include <string>

#include "Bang/Array.tcc"
#include "Bang/BangPreprocessor.h"
#include "Bang/MetaNode.h"
#include "Bang/ReflectMacros.h"
#include "Bang/StreamOperators.h"
#include "Bang/StreamOperators.h"
#include "Bang/Variant.h"

using namespace Bang;

using BP = BangPreprocessor;

ReflectStruct::ReflectStruct()
{
}

ReflectStruct::~ReflectStruct()
{
}

void ReflectStruct::FromString(String::Iterator structBegin,
                               String::Iterator structEnd,
                               ReflectStruct *outStruct,
                               bool *success)
{
    *success = false;

    // Parse the struct/class reflected annotation
    String::Iterator structVariableListBegin, structVariableListEnd;
    BP::GetNextScope(structBegin,
                     structEnd,
                     &structVariableListBegin,
                     &structVariableListEnd,
                     '(',
                     ')');

    String variableListStr(structVariableListBegin + 1,
                           structVariableListEnd - 1);
    Array<String> variableList = variableListStr.Split<Array>(',', true);
    if (variableList.Size() == 0)
    {
        std::cerr << "BP Error: BP_CLASS has 0 properties, but must have at"
                     "least a name"
                  << std::endl;
        return;
    }
    outStruct->SetStructName(variableList[0]);

    // Find and skip "class"/"struct"
    String::Iterator structKeywordBegin, structKeywordEnd;
    BP::FindNextWord(structVariableListEnd,
                     structEnd,
                     &structKeywordBegin,
                     &structKeywordEnd);
    String keyword(structKeywordBegin, structKeywordEnd);
    if (keyword != "class" && keyword != "struct")
    {
        std::cerr << "BP Error: 'class' or 'struct' keyword expected after"
                     " BANG_CLASS(...)"
                  << std::endl;
        return;
    }

    // Get class/struct variable name (XXX in  "class XXX { ... }")
    String::Iterator structVarNameBegin, structVarNameEnd;
    BP::FindNextWord(
        structKeywordEnd, structEnd, &structVarNameBegin, &structVarNameEnd);
    outStruct->SetStructVariableName(
        String(structVarNameBegin, structVarNameEnd));

    String::Iterator it = structVarNameEnd;
    while (it != structEnd)
    {
        String::Iterator variableBegin =
            BP::Find(it, structEnd, BP::RVariablePrefixes);
        if (variableBegin == structEnd)
        {
            break;
        }

        String::Iterator variableEnd = variableBegin;
        BP::SkipUntilNext(&variableEnd, structEnd, {";"});
        if (variableEnd == structEnd)
        {
            break;
        }
        variableEnd += 1;

        ReflectVariable reflVar;
        ReflectVariable::FromString(
            variableBegin, variableEnd, &reflVar, success);
        outStruct->AddVariable(reflVar);

        it = variableEnd;
    }

    *success = true;
}

void ReflectStruct::SetStructName(const String &structName)
{
    m_structName = structName;
}

void ReflectStruct::SetStructVariableName(const String &structVarName)
{
    m_structVariableName = structVarName;
}

void ReflectStruct::AddVariable(const ReflectVariable &reflVar)
{
    m_variables.PushBack(reflVar);
}

void ReflectStruct::AddEnumField(const String &enumName,
                                 const String &enumFieldName)
{
    uint enumFieldValue = 0;
    if (!m_lastAddedEnumFieldValues.ContainsKey(enumName))
    {
        enumFieldValue = m_lastAddedEnumFieldValues[enumName] + 1;
    }
    AddEnumFieldValue(enumName, enumFieldName, enumFieldValue);
}

void ReflectStruct::AddEnumFieldValue(const String &enumName,
                                      const String &enumFieldName,
                                      uint enumFieldValue)
{
    m_enumFieldValues[enumName][enumFieldName] = enumFieldValue;
    m_lastAddedEnumFieldValues[enumName] = enumFieldValue;
}

void ReflectStruct::Clear()
{
    m_structName = "";
    m_structVariableName = "";
    m_variables.Clear();
    m_enumFieldValues.Clear();
    m_lastAddedEnumFieldValues.Clear();
}

MetaNode ReflectStruct::GetMeta() const
{
    MetaNode meta;
    for (const ReflectVariable &reflVar : GetVariables())
    {
        meta.Set(reflVar.GetName(), reflVar.GetCurrentValue());
    }
    return meta;
}

String ReflectStruct::GetReflectVarCode() const
{
    String src = "";
    for (const ReflectVariable &var : GetVariables())
    {
        Variant::Type varType = var.GetVariant().GetType();
        if (varType == Variant::Type::NONE)
        {
            continue;
        }

        String varReflectionCode = R"VERBATIM(
               ReflectVar<VAR_TYPE>(
                           "VAR_REFL_NAME",
                           [this](VAR_TYPE x) { VAR_NAME = MAYBE_CAST(VAR_X); },
                           [this]() { return MAYBE_CONSTRUCTOR(VAR_NAME); },
                                   HINTS);
            )VERBATIM";

        if (varType == Variant::Type::OBJECT_PTR)
        {
            varReflectionCode.ReplaceInSitu(
                "VAR_X",
                "x.GetObjectIn(SceneManager::GetObjectPtrLookupScene())");
            varReflectionCode.ReplaceInSitu(
                "MAYBE_CONSTRUCTOR",
                Variant::GetTypeToString(var.GetVariant().GetType()));
            varReflectionCode.ReplaceInSitu(
                "MAYBE_CAST", "SCAST<" + var.GetTypeString() + ">");
            const String typeStr = var.GetTypeString().Replace("*", "");
            varReflectionCode.ReplaceInSitu(
                "HINTS", "\"" + var.GetHints().GetHintsString() + "\"");
        }
        else
        {
            varReflectionCode.ReplaceInSitu("VAR_X", "x");
            varReflectionCode.ReplaceInSitu("MAYBE_CONSTRUCTOR", "");
            varReflectionCode.ReplaceInSitu("MAYBE_CAST", "");
            varReflectionCode.ReplaceInSitu("HINTS", "\"\"");
        }

        varReflectionCode.ReplaceInSitu(
            "SET_FUNC", "Set<" + Variant::GetTypeToString(varType) + ">");
        varReflectionCode.ReplaceInSitu("VAR_REFL_NAME", var.GetName());
        varReflectionCode.ReplaceInSitu("VAR_NAME", var.GetCodeName());
        varReflectionCode.ReplaceInSitu(
            "VAR_TYPE", Variant::GetTypeToString(var.GetVariant().GetType()));

        src += varReflectionCode;
    }
    return src;
}

String ReflectStruct::GetReflectCode() const
{
    String src = R"VERBATIM(
     void Reflect() override
     {
        INIT_CODE
     }
    )VERBATIM";

    src.ReplaceInSitu("REFLECT_VAR_NAME", BP::GetReflectionInfoPtrFuncName);
    src.ReplaceInSitu("INIT_CODE", GetReflectVarCode());
    return src;
}

const String &ReflectStruct::GetStructName() const
{
    return m_structName;
}

const String &ReflectStruct::GetStructVariableName() const
{
    return m_structVariableName;
}

const Array<ReflectVariable> &ReflectStruct::GetVariables() const
{
    return m_variables;
}

ReflectVariable *ReflectStruct::GetReflectVariablePtr(const String &varName)
{
    for (ReflectVariable &var : m_variables)
    {
        if (var.GetName() == varName)
        {
            return &var;
        }
    }
    return nullptr;
}

const ReflectVariable *ReflectStruct::GetReflectVariablePtr(
    const String &varName) const
{
    return const_cast<ReflectStruct *>(this)->GetReflectVariablePtr(varName);
}

const Map<String, uint> &ReflectStruct::GetEnumFields(
    const String &enumName) const
{
    return m_enumFieldValues[enumName];
}

bool ReflectStruct::EqualsWithoutValue(const ReflectStruct &rhs) const
{
    if (GetVariables().Size() != rhs.GetVariables().Size())
    {
        return false;
    }

    for (uint i = 0; i < rhs.GetVariables().Size(); ++i)
    {
        if (!GetVariables()[i].EqualsWithoutValue(rhs.GetVariables()[i]))
        {
            return false;
        }
    }

    return GetStructName() == rhs.GetStructName() &&
           GetStructVariableName() == rhs.GetStructVariableName();
}

bool ReflectStruct::operator==(const ReflectStruct &rhs) const
{
    return GetStructName() == rhs.GetStructName() &&
           GetStructVariableName() == rhs.GetStructVariableName() &&
           GetVariables() == rhs.GetVariables();
}

bool ReflectStruct::operator!=(const ReflectStruct &rhs) const
{
    return !(*this == rhs);
}
