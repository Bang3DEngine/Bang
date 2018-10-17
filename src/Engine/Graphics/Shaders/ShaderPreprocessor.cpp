#include "Bang/ShaderPreprocessor.h"

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/CodePreprocessor.h"
#include "Bang/Path.h"
#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/String.h"

using namespace Bang;

const String ShaderPreprocessor::GLSLVersionString = "#version 330 core";

void ShaderPreprocessor::PreprocessCode(String *shaderSourceCode)
{
    Array<Path> includeDirs;
    includeDirs.PushBack(EPATH("Shaders"));
    includeDirs.PushBack(EPATH("Shaders/Include"));
    if(Resources::GetInstance())
    {
        Array<Path> lookUpPaths = Resources::GetInstance()->GetLookUpPaths();
        for(const Path &p : lookUpPaths)
        {
            includeDirs.PushBack(p);
        }
    }

    bool addVersion = !shaderSourceCode->BeginsWith("#version");
    CodePreprocessor::PreprocessCode(shaderSourceCode, includeDirs);

    String &code = *shaderSourceCode;
    if(addVersion)
    {
        code.Prepend(ShaderPreprocessor::GLSLVersionString + "\n");
    }
}

ShaderPreprocessor::ShaderPreprocessor()
{
}
