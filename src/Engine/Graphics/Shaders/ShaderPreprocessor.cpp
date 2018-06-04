#include "Bang/ShaderPreprocessor.h"

#include "Bang/List.h"
#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/CodePreprocessor.h"

USING_NAMESPACE_BANG

const String ShaderPreprocessor::GLSLVersionString = "#version 330 core";

void ShaderPreprocessor::PreprocessCode(String *shaderSourceCode)
{
    const Path engShadersDir = Paths::GetEngineAssetsDir().Append("Shaders");

    List<Path> includeDirs;
    includeDirs.PushBack(engShadersDir);
    includeDirs.PushBack(engShadersDir.Append("Include"));
    if (Resources::GetActive())
    {
        Array<Path> lookUpPaths = Resources::GetActive()->GetLookUpPaths();
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
