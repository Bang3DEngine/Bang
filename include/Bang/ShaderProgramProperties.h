#ifndef SHADERPROGRAMPROPERTIES_H
#define SHADERPROGRAMPROPERTIES_H

#include "Bang/Bang.h"
#include "Bang/GL.h"

namespace Bang
{
class ShaderProgramProperties
{
public:
    ShaderProgramProperties();
    virtual ~ShaderProgramProperties();

    void SetCullFace(GL::CullFaceExt cullFace);

    GL::CullFaceExt GetCullFace() const;

private:
    GL::CullFaceExt m_cullFace = GL::CullFaceExt::NONE;
};
}

#endif  // SHADERPROGRAMPROPERTIES_H
