#include "Bang/Shader.h"

#include <fstream>

#include "Bang/Debug.h"
#include "Bang/File.h"
#include "Bang/GL.h"
#include "Bang/Path.h"
#include "Bang/ShaderPreprocessor.h"
#include "Bang/StreamOperators.h"

using namespace Bang;

Shader::Shader(GL::ShaderType t) : m_type(t)
{
}

Shader::~Shader()
{
    if (m_idGL > 0)
    {
        GL::DeleteShader(m_idGL);
    }
}

Shader::Shader() : Shader(GL::ShaderType::VERTEX)
{
}

void Shader::Import(const Path &shaderFilepath)
{
    if (!shaderFilepath.IsFile())
    {
        Debug_Error("Shader file '" << shaderFilepath << "' does not exist.");
        return;
    }
    Compile();
}

void Shader::RetrieveType(const Path &shaderPath)
{
    if (shaderPath.GetExtension().Contains("vert"))
    {
        m_type = GL::ShaderType::VERTEX;
    }
    else if (shaderPath.GetExtension().Contains("geom"))
    {
        m_type = GL::ShaderType::GEOMETRY;
    }
    else
    {
        m_type = GL::ShaderType::FRAGMENT;
    }
}

GL::BindTarget Shader::GetGLBindTarget() const
{
    return GL::BindTarget::NONE;
}

bool Shader::Compile()
{
    Path shaderFilepath = GetResourceFilepath();
    if (shaderFilepath.IsFile())
    {
        RetrieveType(shaderFilepath);
        m_sourceCode = File::GetContents(shaderFilepath);
    }

    m_idGL = GL::CreateShader(m_type);

    m_processedSourceCode = m_sourceCode;
    ShaderPreprocessor::PreprocessCode(&m_processedSourceCode);

    GL::ShaderSource(m_idGL, m_processedSourceCode);
    if (!GL::CompileShader(m_idGL))
    {
        Debug_Error(
            "Failed to compile shader: '" << GetResourceFilepath() << "': "
                                          << std::endl
                                          << GL::GetShaderErrorMsg(m_idGL));
        GL::DeleteShader(m_idGL);
        return false;
    }
    return true;
}

const String &Shader::GetSourceCode() const
{
    return m_sourceCode;
}

const String &Shader::GetProcessedSourceCode() const
{
    return m_processedSourceCode;
}

GL::ShaderType Shader::GetType() const
{
    return m_type;
}
