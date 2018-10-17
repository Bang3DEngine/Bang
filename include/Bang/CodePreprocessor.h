#ifndef CODEPREPROCESSOR_H
#define CODEPREPROCESSOR_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/Path.h"
#include "Bang/String.h"

namespace Bang
{
class CodePreprocessor
{
public:
    static void PreprocessCode(String *srcCode,
                               const Array<Path> &includePaths);

    static Array<String> GetSourceIncludeDirectives(const String &srcCode);
    static Array<Path> GetSourceIncludePaths(const Path &srcPath,
                                             const Array<Path> &includeDirs,
                                             bool onlyExisting = true);
    static Array<Path> GetSourceIncludePaths(const String &srcCode,
                                             const Array<Path> &includeDirs,
                                             bool onlyExisting = true);

protected:
    static String GetIncludeString(const String &includeDirective);
    static Path GetIncludePath(const String &includeDirective,
                               const Array<Path> &includeDirs);
    static Array<Path> GetIncludePathCombinations(
        const String &includeDirective,
        const Array<Path> &includeDirs);
    static String GetIncludeContents(const String &includeDirective,
                                     const Array<Path> &includeDirs);

private:
    CodePreprocessor();
};
}

#endif  // CODEPREPROCESSOR_H
