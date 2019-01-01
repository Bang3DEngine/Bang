#ifndef SHADERPREPROCESSOR_H
#define SHADERPREPROCESSOR_H

#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/ShaderProgramProperties.h"

namespace Bang
{
class String;

enum class ShaderSection
{
    VERTEX = 0,
    GEOMETRY,
    FRAGMENT,
    PROPERTIES
};

class ShaderPreprocessor
{
public:
    ShaderPreprocessor() = delete;

    static void PreprocessCode(String *shaderSourceCode);
    static String GetSectionSourceCode(const String &sourceCode,
                                       GL::ShaderType shaderType);
    static String GetSectionSourceCode(const String &sourceCode,
                                       ShaderSection shaderSection);

    static ShaderProgramProperties GetShaderProperties(
        const String &sourceCode);

private:
    static const String GLSLVersionString;
    static Array<String> GetSectionKeywords();
};
}

#endif  // SHADERPREPROCESSOR_H
