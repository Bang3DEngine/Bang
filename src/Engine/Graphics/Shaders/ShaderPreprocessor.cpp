#include "Bang/ShaderPreprocessor.h"

#include "Bang/List.h"
#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/CodePreprocessor.h"

USING_NAMESPACE_BANG

const String ShaderPreprocessor::GLSLVersionString = "#version 330 core";

void ShaderPreprocessor::PreprocessCode(String *shaderSourceCode)
{
    List<Path> includeDirs;
    includeDirs.PushBack(EPATH("Shaders"));
    includeDirs.PushBack(EPATH("Shaders/Include"));
    if (Resources::GetInstance())
    {
        Array<Path> lookUpPaths = Resources::GetInstance()->GetLookUpPaths();
        for (const Path &p : lookUpPaths) { includeDirs.PushBack(p); }
    }

    bool addVersion = !shaderSourceCode->BeginsWith("#version");
    CodePreprocessor::PreprocessCode(shaderSourceCode, includeDirs);

    String &code = *shaderSourceCode;
    if (addVersion)
    {
        code.Prepend(ShaderPreprocessor::GLSLVersionString + "\n");
    }
}

ShaderPreprocessor::ShaderPreprocessor()
{
}
