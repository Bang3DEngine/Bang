#ifndef REFLECTSTRUCT_H
#define REFLECTSTRUCT_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/Map.h"
#include "Bang/ReflectVariable.h"
#include "Bang/String.h"

namespace Bang
{
class ReflectStruct
{
public:
    ReflectStruct();
    virtual ~ReflectStruct();

    void AddVariable(const ReflectVariable &reflVar);
    void AddEnumField(const String &enumName, const String &enumFieldName);
    void AddEnumFieldValue(const String &enumName,
                           const String &enumFieldName,
                           uint enumFieldValue);
    template <class T>
    void AddEnumFieldValue(const String &enumName,
                           const String &enumFieldName,
                           T enumFieldValue);
    void Clear();

    MetaNode GetMeta() const;
    const String &GetStructName() const;
    const String &GetStructVariableName() const;
    const Array<ReflectVariable> &GetVariables() const;
    ReflectVariable *GetReflectVariablePtr(const String &varName);
    const ReflectVariable *GetReflectVariablePtr(const String &varName) const;
    const Map<String, uint> &GetEnumFields(const String &enumName) const;

    bool EqualsWithoutValue(const ReflectStruct &rhs) const;
    bool operator==(const ReflectStruct &rhs) const;
    bool operator!=(const ReflectStruct &rhs) const;

private:
    String m_structName = "";
    String m_structVariableName = "";
    Array<ReflectVariable> m_variables;
    mutable Map<String, Map<String, uint>> m_enumFieldValues;
    mutable Map<String, uint> m_lastAddedEnumFieldValues;

    static void FromString(String::Iterator structBegin,
                           String::Iterator structEnd,
                           ReflectStruct *outStruct,
                           bool *success);

    void SetStructName(const String &structName);
    void SetStructVariableName(const String &structVarName);

    String GetReflectVarCode() const;
    String GetReflectCode() const;

    friend class BangPreprocessor;
};

template <class T>
void ReflectStruct::AddEnumFieldValue(const String &enumName,
                                      const String &enumFieldName,
                                      T enumFieldValue)
{
    AddEnumFieldValue(enumName, enumFieldName, SCAST<uint>(enumFieldValue));
}
}

#endif  // REFLECTSTRUCT_H
