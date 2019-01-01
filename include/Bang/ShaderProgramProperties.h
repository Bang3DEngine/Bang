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
    void SetWireframe(bool wireframe);
    void SetLineWidth(float lineWidth);

    bool GetWireframe() const;
    float GetLineWidth() const;
    GL::CullFaceExt GetCullFace() const;

private:
    bool m_wireframe = false;
    float m_lineWidth = 1.0f;
    GL::CullFaceExt m_cullFace = GL::CullFaceExt::NONE;
};
}

#endif  // SHADERPROGRAMPROPERTIES_H
