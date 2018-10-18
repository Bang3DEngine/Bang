#include "Bang/ReflectStruct.h"

#include <iostream>
#include <string>

#include "Bang/Array.tcc"
#include "Bang/BangPreprocessor.h"
#include "Bang/StreamOperators.h"

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
    String::Iterator structPropertyListBegin, structPropertyListEnd;
    BP::GetNextScope(structBegin,
                     structEnd,
                     &structPropertyListBegin,
                     &structPropertyListEnd,
                     '(',
                     ')');

    String propertyListStr(structPropertyListBegin + 1,
                           structPropertyListEnd - 1);
    Array<String> propertyList = propertyListStr.Split<Array>(',', true);
    if (propertyList.Size() == 0)
    {
        std::cerr << "BP Error: BP_CLASS has 0 properties, but must have at"
                     "least a name"
                  << std::endl;
        return;
    }
    outStruct->SetStructName(propertyList[0]);

    // Find and skip "class"/"struct"
    String::Iterator structKeywordBegin, structKeywordEnd;
    BP::FindNextWord(structPropertyListEnd,
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
        String::Iterator propertyBegin =
            BP::Find(it, structEnd, BP::RVariablePrefixes);
        if (propertyBegin == structEnd)
        {
            break;
        }

        String::Iterator propertyEnd = propertyBegin;
        BP::SkipUntilNext(&propertyEnd, structEnd, {";"});
        if (propertyEnd == structEnd)
        {
            break;
        }
        propertyEnd += 1;

        ReflectVariable bProperty;
        ReflectVariable::FromString(
            propertyBegin, propertyEnd, &bProperty, success);
        outStruct->AddVariable(bProperty);

        it = propertyEnd;
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

String ReflectStruct::GetInitializationCode() const
{
    int i = 0;
    String src = "";
    for (const ReflectVariable &rVar : GetVariables())
    {
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
        src += varInitCode;
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
