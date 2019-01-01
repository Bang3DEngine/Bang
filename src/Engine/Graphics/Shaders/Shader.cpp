#include "Bang/Shader.h"

#include <fstream>

#include "Bang/Debug.h"
#include "Bang/File.h"
#include "Bang/GL.h"
#include "Bang/Path.h"
#include "Bang/ShaderPreprocessor.h"
#include "Bang/StreamOperators.h"

using namespace Bang;

Shader::Shader(GL::ShaderType t)
{
    SetType(t);
}

Shader::~Shader()
{
    if (GetGLId() > 0)
    {
        GL::DeleteShader(GetGLId());
    }
}

void Shader::SetSourceCode(const String &sourceCode)
{
    m_sourceCode = sourceCode;
}

void Shader::SetType(GL::ShaderType type)
{
    m_type = type;
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
        SetType(GL::ShaderType::VERTEX);
    }
    else if (shaderPath.GetExtension().Contains("geom"))
    {
        SetType(GL::ShaderType::GEOMETRY);
    }
    else
    {
        SetType(GL::ShaderType::FRAGMENT);
    }
}

GL::BindTarget Shader::GetGLBindTarget() const
{
    return GL::BindTarget::NONE;
}

bool Shader::Compile()
{
    Path shaderFilepath = GetAssetFilepath();
    if (shaderFilepath.IsFile())
    {
        RetrieveType(shaderFilepath);
        SetSourceCode(File::GetContents(shaderFilepath));
    }

    m_idGL = GL::CreateShader(GetType());

    m_processedSourceCode = GetSourceCode();
    ShaderPreprocessor::PreprocessCode(&m_processedSourceCode);

    GL::ShaderSource(GetGLId(), GetProcessedSourceCode());
    if (!GL::CompileShader(GetGLId()))
    {
        Debug_Error(
            "Failed to compile shader: '" << GetAssetFilepath() << "': "
                                          << std::endl
                                          << GL::GetShaderErrorMsg(GetGLId()));
        GL::DeleteShader(GetGLId());
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
