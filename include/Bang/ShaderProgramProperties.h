#ifndef SHADERPROGRAMPROPERTIES_H
#define SHADERPROGRAMPROPERTIES_H

#include "Bang/Bang.h"
#include "Bang/GL.h"
#include "Bang/RenderPass.h"

namespace Bang
{
class ShaderProgramProperties
{
public:
    ShaderProgramProperties();
    ~ShaderProgramProperties();

    void SetCullFace(GL::CullFaceExt cullFace);
    void SetWireframe(bool wireframe);
    void SetLineWidth(float lineWidth);
    void SetRenderPass(RenderPass renderPass);

    bool GetWireframe() const;
    float GetLineWidth() const;
    RenderPass GetRenderPass() const;
    GL::CullFaceExt GetCullFace() const;

private:
    bool m_wireframe = false;
    float m_lineWidth = 1.0f;
    RenderPass m_renderPass = RenderPass::SCENE_OPAQUE;
    GL::CullFaceExt m_cullFace = GL::CullFaceExt::NONE;
};
}

#endif  // SHADERPROGRAMPROPERTIES_H
