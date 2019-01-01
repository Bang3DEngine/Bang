#include "Bang/Shader.h"

#include <fstream>

#include "Bang/Debug.h"
#include "Bang/File.h"
#include "Bang/GL.h"
#include "Bang/Path.h"
#include "Bang/ShaderPreprocessor.h"
#include "Bang/StreamOperators.h"

using namespace Bang;

Shader::Shader() : Shader(GL::ShaderType::VERTEX)
{
}

Shader::Shader(GL::ShaderType t)
{
    SetType(t);
}

Shader::~Shader()
{
    DeleteShader();
}

void Shader::SetSourceCode(const String &sourceCode)
{
    if (sourceCode != GetSourceCode())
    {
        m_compiled = false;
        m_sourceCode = sourceCode;

        String preprocessedSourceCode = sourceCode;
        ShaderPreprocessor::PreprocessCode(&preprocessedSourceCode);
        m_preprocessedSourceCode = preprocessedSourceCode;

        CommitShaderSourceCode();
    }
}

void Shader::SetType(GL::ShaderType type)
{
    if (type != GetType())
    {
        m_type = type;

        DeleteShader();
        m_idGL = GL::CreateShader(GetType());
        CommitShaderSourceCode();
    }
}

bool Shader::CompileIfNeeded()
{
    if (!IsCompiled())
    {
        return Compile();
    }
    return true;
}

void Shader::Import(const Path &shaderFilepath)
{
    if (shaderFilepath.IsFile())
    {
        SetSourceCode(File::GetContents(shaderFilepath));
    }
    else
    {
        Debug_Error("Shader file '" << shaderFilepath << "' does not exist.");
    }
}

void Shader::CommitShaderSourceCode()
{
    if (GetGLId() > 0)
    {
        GL::ShaderSource(GetGLId(), GetPreprocessedSourceCode());
    }
}

GL::BindTarget Shader::GetGLBindTarget() const
{
    return GL::BindTarget::NONE;
}

bool Shader::Compile()
{
    m_compiled = GL::CompileShader(GetGLId());
    if (!IsCompiled())
    {
        Debug_Error(
            "Failed to compile shader: '" << GetAssetFilepath() << "': "
                                          << std::endl
                                          << GL::GetShaderErrorMsg(GetGLId()));
    }
    return IsCompiled();
}

void Shader::DeleteShader()
{
    if (GetGLId() > 0)
    {
        GL::DeleteShader(GetGLId());
    }
    m_compiled = false;
}

bool Shader::IsCompiled() const
{
    return m_compiled;
}

const String &Shader::GetSourceCode() const
{
    return m_sourceCode;
}

const String &Shader::GetPreprocessedSourceCode() const
{
    return m_preprocessedSourceCode;
}

GL::ShaderType Shader::GetType() const
{
    return m_type;
}
