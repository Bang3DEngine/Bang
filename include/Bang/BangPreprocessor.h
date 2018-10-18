#ifndef BANGPREPROCESSOR_H
#define BANGPREPROCESSOR_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/String.h"

namespace Bang
{
class ReflectStruct;
class Path;

class BangPreprocessor
{
public:
    const static Array<String> Modifiers;
    const static Array<String> RVariablePrefixes;
    const static Array<String> RStructPrefixes;
    const static String ReflectDefinitionsDefineName;
    const static String GetReflectionInfoPtrFuncName;

    static void Preprocess(const Path &filepath);
    static void Preprocess(const String &source,
                           String *reflectionHeaderSource,
                           bool *preprocessedSomething);
    static Array<ReflectStruct> GetReflectStructs(const Path &sourcePath);
    static Array<ReflectStruct> GetReflectStructs(const String &source);

    BangPreprocessor() = delete;

    static void RemoveComments(String *source);

    static String::Iterator Find(String::Iterator begin,
                                 String::Iterator end,
                                 const Array<String> &toFindList);

    static void GetNextScope(String::Iterator begin,
                             String::Iterator end,
                             String::Iterator *scopeBegin,
                             String::Iterator *scopeEnd,
                             char openingBrace,
                             char closingBrace);

    static void SkipBlanks(String::Iterator *it, String::Iterator end);
    static void SkipUntilNextBlank(String::Iterator *it, String::Iterator end);
    static void SkipUntilNext(String::Iterator *it,
                              String::Iterator end,
                              const Array<String> &particles);

    static void FindNextWord(String::Iterator begin,
                             String::Iterator end,
                             String::Iterator *wordBegin,
                             String::Iterator *wordEnd);
};
}

#endif  // BANGPREPROCESSOR_H
