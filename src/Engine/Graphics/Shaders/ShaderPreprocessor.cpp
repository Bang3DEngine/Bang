#include "Bang/ShaderPreprocessor.h"

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Assets.h"
#include "Bang/CodePreprocessor.h"
#include "Bang/Path.h"
#include "Bang/Paths.h"
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

String ShaderPreprocessor::GetSourceCodeSection(const String &sourceCode,
                                                GL::ShaderType shaderType)
{
    const String VertexKeyWord = "#vertex";
    const String GeometryKeyWord = "#geometry";
    const String FragmentKeyWord = "#fragment";
    const Array<String> keywords = {
        VertexKeyWord, GeometryKeyWord, FragmentKeyWord};

    uint keywordIndex = -1u;
    switch (shaderType)
    {
        case GL::ShaderType::VERTEX: keywordIndex = 0; break;
        case GL::ShaderType::GEOMETRY: keywordIndex = 1; break;
        case GL::ShaderType::FRAGMENT: keywordIndex = 2; break;
    }

    String sourceCodeSection = "";
    if (keywordIndex != -1u)
    {
        const String keyword = keywords[keywordIndex];
        long keywordIndex = sourceCode.IndexOf(keyword);
        if (keywordIndex != -1)
        {
            long beginCodeIndex = sourceCode.IndexOf("\n", keywordIndex);
            if (beginCodeIndex != -1)
            {
                ++beginCodeIndex;
                long endCodeIndex = -1;
                for (uint i = 0; i < keywords.Size(); ++i)
                {
                    if (i != keywordIndex)
                    {
                        endCodeIndex =
                            sourceCode.IndexOf(keywords[i], beginCodeIndex);
                        if (endCodeIndex != -1)
                        {
                            --endCodeIndex;
                            break;
                        }
                    }
                }

                sourceCodeSection = sourceCode.SubString(
                    beginCodeIndex,
                    (endCodeIndex >= 0 ? endCodeIndex : String::npos));
            }
        }
    }

    return sourceCodeSection;
}

ShaderPreprocessor::ShaderPreprocessor()
{
}
