#include "Bang/BPReflectedStruct.h"

#include <iostream>
#include <string>

#include "Bang/Array.tcc"
#include "Bang/BangPreprocessor.h"
#include "Bang/StreamOperators.h"

using namespace Bang;

using BP = BangPreprocessor;

BPReflectedStruct::BPReflectedStruct()
{
}

BPReflectedStruct::~BPReflectedStruct()
{
}

bool BPReflectedStruct::operator==(const BPReflectedStruct &rhs) const
{
    return GetStructName() == rhs.GetStructName() &&
           GetStructVariableName() == rhs.GetStructVariableName() &&
           GetVariables() == rhs.GetVariables();
}

bool BPReflectedStruct::operator!=(const BPReflectedStruct &rhs) const
{
    return !(*this == rhs);
}

void BPReflectedStruct::FromString(String::Iterator structBegin,
                                   String::Iterator structEnd,
                                   BPReflectedStruct *outStruct,
                                   bool *success)
{
    *success = false;

    // Parse the struct/class reflected annotation
    String::Iterator structPropertyListBegin, structPropertyListEnd;
    BP::GetNextScope(structBegin, structEnd, &structPropertyListBegin,
                     &structPropertyListEnd, '(', ')');

    String propertyListStr(structPropertyListBegin + 1,
                           structPropertyListEnd - 1);
    Array<String> propertyList = propertyListStr.Split<Array>(',', true);
    if(propertyList.Size() == 0)
    {
        std::cerr << "BP Error: BP_CLASS has 0 properties, but must have at"
                     "least a name"
                  << std::endl;
        return;
    }
    outStruct->SetStructName(propertyList[0]);

    // Find and skip "class"/"struct"
    String::Iterator structKeywordBegin, structKeywordEnd;
    BP::FindNextWord(structPropertyListEnd, structEnd, &structKeywordBegin,
                     &structKeywordEnd);
    String keyword(structKeywordBegin, structKeywordEnd);
    if(keyword != "class" && keyword != "struct")
    {
        std::cerr << "BP Error: 'class' or 'struct' keyword expected after"
                     " BANG_CLASS(...)"
                  << std::endl;
        return;
    }

    // Get class/struct variable name (XXX in  "class XXX { ... }")
    String::Iterator structVarNameBegin, structVarNameEnd;
    BP::FindNextWord(structKeywordEnd, structEnd, &structVarNameBegin,
                     &structVarNameEnd);
    outStruct->SetStructVariableName(
        String(structVarNameBegin, structVarNameEnd));

    String::Iterator it = structVarNameEnd;
    while(it != structEnd)
    {
        String::Iterator propertyBegin =
            BP::Find(it, structEnd, BP::RVariablePrefixes);
        if(propertyBegin == structEnd)
        {
            break;
        }

        String::Iterator propertyEnd = propertyBegin;
        BP::SkipUntilNext(&propertyEnd, structEnd, {";"});
        if(propertyEnd == structEnd)
        {
            break;
        }
        propertyEnd += 1;

        BPReflectedVariable bProperty;
        BPReflectedVariable::FromString(propertyBegin, propertyEnd, &bProperty,
                                        success);
        outStruct->AddVariable(bProperty);

        it = propertyEnd;
    }

    *success = true;
}

void BPReflectedStruct::SetStructName(const String &structName)
{
    m_structName = structName;
}

void BPReflectedStruct::SetStructVariableName(const String &structVarName)
{
    m_structVariableName = structVarName;
}

void BPReflectedStruct::AddVariable(const BPReflectedVariable &prop)
{
    m_variables.PushBack(prop);
}

String BPReflectedStruct::GetInitializationCode() const
{
    int i = 0;
    String src = "";
    for(const BPReflectedVariable &rVar : GetVariables())
    {
        String varInitCode = R"VERBATIM(
              BPReflectedVariable RVAR_NAME;
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

String BPReflectedStruct::GetGetReflectionInfoCode() const
{
    String src = R"VERBATIM(
     void Reflect() const override
     {
        INIT_CODE
     }
    )VERBATIM";

    src.ReplaceInSitu("REFLECT_VAR_NAME", BP::GetReflectionInfoPtrFuncName);
    src.ReplaceInSitu("INIT_CODE", GetInitializationCode());
    return src;
}

String BPReflectedStruct::GetWriteReflectionCode() const
{
    String src = R"VERBATIM(
        void ExportMeta(MetaNode *metaNode) const override
        {
            VARS_SETS
        }
        )VERBATIM";

    String varsSetsSrc = "";
    for(const BPReflectedVariable &var : GetVariables())
    {
        String varSetSrc = R"VERBATIM(
                    metaNode->SET_FUNC("VAR_REFL_NAME", VAR_NAME);
            )VERBATIM";

        BPReflectedVariable::Type varType = var.GetType();
        if(varType == BPReflectedVariable::Type::NONE)
        {
            continue;
        }
        varSetSrc.ReplaceInSitu(
            "SET_FUNC",
            "Set<" + BPReflectedVariable::GetTypeToString(varType) + ">");
        varSetSrc.ReplaceInSitu("VAR_REFL_NAME", var.GetName());
        varSetSrc.ReplaceInSitu("VAR_NAME", var.GetCodeName());

        varsSetsSrc += varSetSrc;
    }
    src.ReplaceInSitu("VARS_SETS", varsSetsSrc);
    return src;
}

String BPReflectedStruct::GetReadReflectionCode() const
{
    String src = R"VERBATIM(
        void ImportMeta(const MetaNode &metaNode) override
        {
            VARS_GETS
        }
        )VERBATIM";

    String varsGetsSrc = "";
    for(const BPReflectedVariable &var : GetVariables())
    {
        String varGetSrc = R"VERBATIM(
                    VAR_NAME = metaNode.GET_FUNC("VAR_REFL_NAME");
            )VERBATIM";

        BPReflectedVariable::Type varType = var.GetType();
        if(varType == BPReflectedVariable::Type::NONE)
        {
            continue;
        }
        varGetSrc.ReplaceInSitu(
            "GET_FUNC",
            "Get<" + BPReflectedVariable::GetTypeToString(varType) + ">");
        varGetSrc.ReplaceInSitu("VAR_REFL_NAME", var.GetName());
        varGetSrc.ReplaceInSitu("VAR_NAME", var.GetCodeName());

        varsGetsSrc += varGetSrc;
    }
    src.ReplaceInSitu("VARS_GETS", varsGetsSrc);
    return src;
}

const String &BPReflectedStruct::GetStructName() const
{
    return m_structName;
}

const String &BPReflectedStruct::GetStructVariableName() const
{
    return m_structVariableName;
}

const Array<BPReflectedVariable> &BPReflectedStruct::GetVariables() const
{
    return m_variables;
}

String BPReflectedStruct::ToString() const
{
    std::ostringstream oss;
    oss << "{ " << GetStructName() << ", " << GetStructVariableName() << ", "
        << GetVariables() << " }";
    return String(oss.str());
}
