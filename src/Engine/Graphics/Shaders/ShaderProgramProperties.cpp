#include "Bang/ShaderProgramProperties.h"

using namespace Bang;

ShaderProgramProperties::ShaderProgramProperties()
{
}

ShaderProgramProperties::~ShaderProgramProperties()
{
}

void ShaderProgramProperties::SetCullFace(GL::CullFaceExt cullFace)
{
    m_cullFace = cullFace;
}

void ShaderProgramProperties::SetWireframe(bool wireframe)
{
    m_wireframe = wireframe;
}

void ShaderProgramProperties::SetLineWidth(float lineWidth)
{
    m_lineWidth = lineWidth;
}

bool ShaderProgramProperties::GetWireframe() const
{
    return m_wireframe;
}

float ShaderProgramProperties::GetLineWidth() const
{
    return m_lineWidth;
}

GL::CullFaceExt ShaderProgramProperties::GetCullFace() const
{
    return m_cullFace;
}
