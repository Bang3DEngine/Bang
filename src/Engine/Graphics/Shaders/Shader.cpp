#include "Bang/Shader.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include "Bang/GL.h"
#include "Bang/File.h"
#include "Bang/Path.h"
#include "Bang/Array.h"
#include "Bang/Debug.h"
#include "Bang/ShaderPreprocessor.h"

USING_NAMESPACE_BANG

Shader::Shader(GL::ShaderType t) : m_type(t)
{
}

Shader::Shader() : Shader(GL::ShaderType::VERTEX)
{
}

void Shader::Import(const Path& shaderFilepath)
{
    if (!shaderFilepath.IsFile())
    {
        Debug_Error("Shader '" << shaderFilepath << "' does not exist.");
        return;
    }

    RetrieveType(shaderFilepath);
    m_sourceCode = File::GetContents(shaderFilepath);
    ShaderPreprocessor::PreprocessCode(&m_sourceCode);

    m_idGL = GL::CreateShader(m_type);

    GL::ShaderSource(m_idGL, m_sourceCode);
    if ( !GL::CompileShader(m_idGL) )
    {
        Debug_Error("Failed to compile shader: '"  << shaderFilepath
                    << "': " << std::endl << GL::GetShaderErrorMsg(m_idGL));
        GL::DeleteShader(m_idGL);
    }
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

const String& Shader::GetSourceCode() const
{
    return m_sourceCode;
}

GL::ShaderType Shader::GetType() const
{
    return m_type;
}
