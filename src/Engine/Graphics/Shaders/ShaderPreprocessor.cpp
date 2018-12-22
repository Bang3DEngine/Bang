#include "Bang/ShaderPreprocessor.h"

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/CodePreprocessor.h"
#include "Bang/Path.h"
#include "Bang/Paths.h"
#include "Bang/Assets.h"
#include "Bang/String.h"

using namespace Bang;

const String ShaderPreprocessor::GLSLVersionString = "#version 330 core";

void ShaderPreprocessor::PreprocessCode(String *shaderSourceCode)
{
    const Path engShadersDir = Paths::GetEngineAssetsDir().Append("Shaders");

    Array<Path> includeDirs;
    includeDirs.PushBack(engShadersDir);
    includeDirs.PushBack(engShadersDir.Append("Include"));
    if (Assets::GetInstance())
    {
        Array<Path> lookUpPaths = Assets::GetInstance()->GetLookUpPaths();
        for (const Path &p : lookUpPaths)
        {
            includeDirs.PushBack(p);
        }
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
