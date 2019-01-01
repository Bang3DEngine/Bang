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

GL::CullFaceExt ShaderProgramProperties::GetCullFace() const
{
    return m_cullFace;
}
