#ifndef REFLECTSTRUCT_H
#define REFLECTSTRUCT_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/ReflectVariable.h"
#include "Bang/String.h"

namespace Bang
{
class ReflectStruct
{
public:
    ReflectStruct() = default;
    ~ReflectStruct() = default;

    void AddVariable(const ReflectVariable &prop);

    MetaNode GetMeta() const;
    const String &GetStructName() const;
    const String &GetStructVariableName() const;
    const Array<ReflectVariable> &GetVariables() const;

    bool operator==(const ReflectStruct &rhs) const;
    bool operator!=(const ReflectStruct &rhs) const;

private:
    String m_structName = "";
    String m_structVariableName = "";
    Array<ReflectVariable> m_variables;

    static void FromString(String::Iterator structBegin,
                           String::Iterator structEnd,
                           ReflectStruct *outStruct,
                           bool *success);

    void SetStructName(const String &structName);
    void SetStructVariableName(const String &structVarName);

    String GetInitializationCode() const;
    String GetGetReflectionInfoCode() const;
    String GetWriteReflectionCode() const;
    String GetReadReflectionCode() const;

    friend class BangPreprocessor;
};
}

#endif  // REFLECTSTRUCT_H
