#ifndef SHADER_H
#define SHADER_H

#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/GLObject.h"
#include "Bang/String.h"

namespace Bang
{
class Path;

class Shader : public GLObject, public Asset
{
    ASSET(Shader)

public:
    Shader();
    Shader(GL::ShaderType t);
    virtual ~Shader() override;

    void SetSourceCode(const String &sourceCode);
    void SetType(GL::ShaderType type);
    bool CompileIfNeeded();
    bool Compile();

    void DeleteShader();
    bool IsCompiled() const;
    const String &GetSourceCode() const;
    const String &GetPreprocessedSourceCode() const;
    GL::ShaderType GetType() const;
    GL::BindTarget GetGLBindTarget() const override;

    // Asset
    virtual void Import(const Path &shaderFilepath) override;

private:
    bool m_compiled = false;
    String m_sourceCode = "";
    String m_preprocessedSourceCode = "";
    GL::ShaderType m_type = Undef<GL::ShaderType>();

    void CommitShaderSourceCode();
};
}

#endif  // SHADER_H
