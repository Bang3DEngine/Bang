#ifndef SHADERPREPROCESSOR_H
#define SHADERPREPROCESSOR_H

#include "Bang/BangDefines.h"
#include "Bang/GL.h"

namespace Bang
{
class String;

class ShaderPreprocessor
{
public:
    static void PreprocessCode(String *shaderSourceCode);
    static String GetSourceCodeSection(const String &sourceCode,
                                       GL::ShaderType shaderType);

protected:
    static const String GLSLVersionString;

private:
    ShaderPreprocessor();
};
}

#endif  // SHADERPREPROCESSOR_H
