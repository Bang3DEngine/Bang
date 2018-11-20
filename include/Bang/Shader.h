#ifndef SHADER_H
#define SHADER_H

#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/GLObject.h"
#include "Bang/Resource.h"
#include "Bang/String.h"

namespace Bang
{
class Path;

class Shader : public GLObject, public Resource
{
    RESOURCE(Shader)

public:
    Shader();
    Shader(GL::ShaderType t);
    virtual ~Shader() override;

    bool Compile();

    const String &GetSourceCode() const;
    const String &GetProcessedSourceCode() const;
    GL::ShaderType GetType() const;
    GL::BindTarget GetGLBindTarget() const override;

    // Resource
    virtual void Import(const Path &shaderFilepath) override;

private:
    GL::ShaderType m_type;
    String m_sourceCode = "";
    String m_processedSourceCode = "";

    void RetrieveType(const Path &shaderPath);
};
}

#endif  // SHADER_H
