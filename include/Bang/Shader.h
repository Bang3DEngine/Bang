#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/GLObject.h"
#include "Bang/Path.h"
#include "Bang/Resource.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class Path;

class Shader : public GLObject,
               public Resource
{
    RESOURCE(Shader)

public:
    Shader();
    Shader(GL::ShaderType t);
    virtual ~Shader() override;

    bool Compile();

    const String& GetSourceCode() const;
    GL::ShaderType GetType() const;
    GL::BindTarget GetGLBindTarget() const override;

    // Resource
    virtual void Import(const Path &shaderFilepath) override;

private:
    GL::ShaderType m_type;
    String m_sourceCode = "";

    void RetrieveType(const Path &shaderPath);
};

NAMESPACE_BANG_END

#endif // SHADER_H
