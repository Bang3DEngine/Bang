#include "Bang/ReflectStruct.h"

#include <iostream>
#include <string>

#include "Bang/Array.tcc"
#include "Bang/BangPreprocessor.h"
#include "Bang/MetaNode.h"
#include "Bang/StreamOperators.h"
#include "Bang/StreamOperators.h"
#include "Bang/Variant.h"

using namespace Bang;

using BP = BangPreprocessor;

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

void ReflectStruct::AddVariable(const ReflectVariable &prop)
{
    m_variables.PushBack(prop);
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

MetaNode ReflectStruct::GetMeta() const
{
    MetaNode meta;
    for (const ReflectVariable &reflVar : GetVariables())
    {
        meta.Set(reflVar.GetName(), reflVar.GetCurrentValue());
    }
    return meta;
}

String ReflectStruct::GetInitializationCode() const
{
    String src = "";
    for (const ReflectVariable &var : GetVariables())
    {
        String varReflectionCode = R"VERBATIM(
               ReflectVar<VAR_TYPE>(
                           "VAR_REFL_NAME",
                           [this](VAR_TYPE x) { VAR_NAME = x; },
                           [this]() { return VAR_NAME; });
            )VERBATIM";

        Variant::Type varType = var.GetVariant().GetType();
        if (varType == Variant::Type::NONE)
        {
            continue;
        }
        varReflectionCode.ReplaceInSitu(
            "SET_FUNC", "Set<" + Variant::GetTypeToString(varType) + ">");
        varReflectionCode.ReplaceInSitu("VAR_REFL_NAME", var.GetName());
        varReflectionCode.ReplaceInSitu("VAR_NAME", var.GetCodeName());
        varReflectionCode.ReplaceInSitu(
            "VAR_TYPE", Variant::GetTypeToString(var.GetVariant().GetType()));

        /*
        varsSetsSrc += varSetSrc;

        String varInitCode = R"VERBATIM(
              ReflectVariable RVAR_NAME;
              RVAR_INIT_CODE
              REFL_INFO->AddVariable(RVAR_NAME);
          )VERBATIM";

        String rVarName = "var" + String(i++);
        varInitCode.ReplaceInSitu("RVAR_NAME", rVarName);
        varInitCode.ReplaceInSitu("RVAR_INIT_CODE",
                                  rVar.GetInitializationCode(rVarName));
        varInitCode.ReplaceInSitu("REFL_INFO",
                                  BP::GetReflectionInfoPtrFuncName);
                                  */
        src += varReflectionCode;
    }
    return src;
}

String ReflectStruct::GetGetReflectionInfoCode() const
{
    String src = R"VERBATIM(
     void Reflect() override
     {
        INIT_CODE
     }
    )VERBATIM";

    src.ReplaceInSitu("REFLECT_VAR_NAME", BP::GetReflectionInfoPtrFuncName);
    src.ReplaceInSitu("INIT_CODE", GetInitializationCode());
    return src;
}

String ReflectStruct::GetWriteReflectionCode() const
{
    String src = R"VERBATIM(
        void ExportMeta(MetaNode *metaNode) const override
        {
            VARS_SETS
        }
        )VERBATIM";

    String varsSetsSrc = "";
    for (const ReflectVariable &var : GetVariables())
    {
        String varSetSrc = R"VERBATIM(
                    metaNode->SET_FUNC("VAR_REFL_NAME", VAR_NAME);
            )VERBATIM";

        Variant::Type varType = var.GetVariant().GetType();
        if (varType == Variant::Type::NONE)
        {
            continue;
        }
        varSetSrc.ReplaceInSitu(
            "SET_FUNC", "Set<" + Variant::GetTypeToString(varType) + ">");
        varSetSrc.ReplaceInSitu("VAR_REFL_NAME", var.GetName());
        varSetSrc.ReplaceInSitu("VAR_NAME", var.GetCodeName());

        varsSetsSrc += varSetSrc;
    }
    src.ReplaceInSitu("VARS_SETS", varsSetsSrc);
    return src;
}

String ReflectStruct::GetReadReflectionCode() const
{
    String src = R"VERBATIM(
        void ImportMeta(const MetaNode &metaNode) override
        {
            VARS_GETS
        }
        )VERBATIM";

    String varsGetsSrc = "";
    for (const ReflectVariable &var : GetVariables())
    {
        String varGetSrc = R"VERBATIM(
                    VAR_NAME = metaNode.GET_FUNC("VAR_REFL_NAME");
            )VERBATIM";

        Variant::Type varType = var.GetVariant().GetType();
        if (varType == Variant::Type::NONE)
        {
            continue;
        }
        varGetSrc.ReplaceInSitu(
            "GET_FUNC", "Get<" + Variant::GetTypeToString(varType) + ">");
        varGetSrc.ReplaceInSitu("VAR_REFL_NAME", var.GetName());
        varGetSrc.ReplaceInSitu("VAR_NAME", var.GetCodeName());

        varsGetsSrc += varGetSrc;
    }
    src.ReplaceInSitu("VARS_GETS", varsGetsSrc);
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

const Map<String, uint> &ReflectStruct::GetEnumFields(
    const String &enumName) const
{
    return m_enumFieldValues[enumName];
}
