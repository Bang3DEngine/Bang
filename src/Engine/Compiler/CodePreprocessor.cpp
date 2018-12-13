#include "Bang/CodePreprocessor.h"

#include <iterator>
#include <list>
#include <ostream>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Debug.h"
#include "Bang/File.h"
#include "Bang/List.h"
#include "Bang/List.tcc"
#include "Bang/Path.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

using namespace Bang;

String CodePreprocessor::GetIncludeString(const String &includeDirective)
{
    Array<String> includeParts = includeDirective.Split<Array>('"', true);
    if (includeParts.Size() < 2)
    {
        includeParts = includeDirective.Split<Array>('<', true);
        if (includeParts.Size() < 2)
        {
            return "";
        }
    }
    return includeParts[1];
}

Path CodePreprocessor::GetIncludePath(const String &includeDirective,
                                      const Array<Path> &includeDirs)
{
    Array<Path> includeDirPathCombinations =
        CodePreprocessor::GetIncludePathCombinations(includeDirective,
                                                     includeDirs);

    // Search if the include file combined with any of the include paths
    // gives us an existing file. Return the first found.
    for (const Path &includePath : includeDirPathCombinations)
    {
        if (includePath.IsFile())
        {
            return includePath;
        }
    }

    Debug_Error("Could not find include: '" << includeDirective << "'. "
                                            << "Using directories: "
                                            << includeDirs);
    return Path::Empty();
}

Array<Path> CodePreprocessor::GetIncludePathCombinations(
    const String &includeDirective,
    const Array<Path> &includeDirs)
{
    String includeStr = CodePreprocessor::GetIncludeString(includeDirective);

    Array<Path> combinations;
    for (const Path &includeDirPath : includeDirs)
    {
        Path completeFilepath = includeDirPath.Append(includeStr);
        combinations.PushBack(completeFilepath);
    }

    return combinations;
}

String CodePreprocessor::GetIncludeContents(const String &includeDirective,
                                            const Array<Path> &includeDirs)
{
    Path includePath =
        CodePreprocessor::GetIncludePath(includeDirective, includeDirs);
    if (includePath.IsFile())
    {
        return File::GetContents(includePath);
    }
    return "";
}

void CodePreprocessor::PreprocessCode(String *srcCode,
                                      const Array<Path> &includeDirs)
{
    String &outputCode = *srcCode;

    Array<String> linesArray = outputCode.Split<Array>('\n');
    List<String> lines = linesArray.To<List>();

    // Keep track of the user's source line number, to use #line directive
    int originalLineNum = 1;
    for (auto it = lines.Begin(); it != lines.End();)
    {
        String line = (*it).Trim();
        if (line.BeginsWith("#include"))
        {
            it = lines.Remove(it);
            String includeContent = GetIncludeContents(line, includeDirs);
            CodePreprocessor::PreprocessCode(&includeContent, includeDirs);

            List<String> contentLines = includeContent.Split<List>('\n', false);
            contentLines.PushBack("#line " + String(originalLineNum - 1));
            lines.Splice(
                it, contentLines, contentLines.Begin(), contentLines.End());
            std::advance(it, contentLines.Size());
        }
        else
        {
            ++it;
        }

        ++originalLineNum;
    }

    if (lines.Size() > 0)
    {
        if (lines.Front().BeginsWith("#version"))
        {
            lines.Insert("#line 1\n", 1);
        }
        else
        {
            lines.Insert("#line 0\n", 0);
        }

        outputCode = String::Join(lines, "\n");
    }
}

Array<String> CodePreprocessor::GetSourceIncludeDirectives(
    const String &srcCode)
{
    Array<String> includes;
    Array<String> lines = srcCode.Split<Array>('\n');
    for (const String &line : lines)
    {
        if (line.Replace(" ", "").BeginsWith("#include"))
        {
            includes.PushBack(line);
        }
    }
    return includes;
}

Array<Path> CodePreprocessor::GetSourceIncludePaths(
    const Path &srcPath,
    const Array<Path> &includeDirs,
    bool onlyExisting)
{
    if (!srcPath.IsFile())
    {
        return Array<Path>();
    }
    return CodePreprocessor::GetSourceIncludePaths(
        File::GetContents(srcPath), includeDirs, onlyExisting);
}

Array<Path> CodePreprocessor::GetSourceIncludePaths(
    const String &srcCode,
    const Array<Path> &includeDirs,
    bool onlyExisting)
{
    Array<Path> sourceIncludePaths;

    Array<String> includeDirectives =
        CodePreprocessor::GetSourceIncludeDirectives(srcCode);
    for (const String &includeDirective : includeDirectives)
    {
        Path incPath;
        if (!onlyExisting)
        {
            CodePreprocessor::GetIncludePath(includeDirective, includeDirs);
            if (incPath.IsFile())
            {
                sourceIncludePaths.PushBack(incPath);
            }
        }
        else
        {
            Array<Path> combinations =
                CodePreprocessor::GetIncludePathCombinations(includeDirective,
                                                             includeDirs);
            sourceIncludePaths.PushBack(combinations);
        }
    }
    return sourceIncludePaths;
}

CodePreprocessor::CodePreprocessor()
{
}
