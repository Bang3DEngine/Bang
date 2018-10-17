#ifndef SHADERPREPROCESSOR_H
#define SHADERPREPROCESSOR_H

#include "Bang/BangDefines.h"

namespace Bang
{
class String;

class ShaderPreprocessor
{
public:
    static void PreprocessCode(String *shaderSourceCode);

protected:
    static const String GLSLVersionString;

private:
    ShaderPreprocessor();
};
}

#endif  // SHADERPREPROCESSOR_H
