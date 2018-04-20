#include "Bang/CubeMapIrradianceGenerator.h"

#include "Bang/Mesh.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Shader.h"
#include "Bang/Camera.h"
#include "Bang/Matrix4.h"
#include "Bang/GEngine.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/GLUniforms.h"
#include "Bang/Framebuffer.h"
#include "Bang/IconManager.h"
#include "Bang/MeshFactory.h"
#include "Bang/MeshRenderer.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

RH<TextureCubeMap>
CubeMapIrradianceGenerator::GenerateIrradianceCubeMap(
                                                TextureCubeMap *textureCubeMap)
{
    constexpr uint IrradianceCubeMapSize = 32;
    constexpr uint ICMSize = IrradianceCubeMapSize;

    // Save OpenGL state
    const AARecti prevVP = GL::GetViewportRect();
    const Matrix4 &prevModel = GLUniforms::GetModelMatrix();
    const Matrix4 &prevView  = GLUniforms::GetViewMatrix();
    const Matrix4 &prevProj  = GLUniforms::GetProjectionMatrix();
    const  GLId prevBoundFB = GL::GetBoundId(GL::BindTarget::Framebuffer);
    const  GLId prevBoundSP = GL::GetBoundId(GL::BindTarget::ShaderProgram);
    bool wasCullEnabled = GL::IsEnabled(GL::Test::CullFace);

    // Create cube map
    RH<TextureCubeMap> irradianceCubeMapRH = Resources::Create<TextureCubeMap>();
    TextureCubeMap *irradianceCubeMap = irradianceCubeMapRH.Get();

    // Create shader program
    ShaderProgram *sp = ShaderProgramFactory::Get(
                EPATH("Shaders/CubeMapIrradianceGenerator.vert"),
                EPATH("Shaders/CubeMapIrradianceGenerator.geom"),
                EPATH("Shaders/CubeMapIrradianceGenerator.frag"));

    // Create framebuffer and shader program
    Framebuffer *fb = new Framebuffer();
    fb->SetAttachmentTexture(irradianceCubeMap, GL::Attachment::Color0);
    fb->Resize(ICMSize, ICMSize);
    fb->Bind();
    fb->SetAllDrawBuffers();

    sp->Bind();
    sp->SetTextureCubeMap("B_InputCubeMap", textureCubeMap);
    GL::SetViewport(0, 0, ICMSize, ICMSize);
    GEngine::GetActive()->RenderViewportPlane();

    fb->UnBind();
    delete fb;

    // Restore OpenGL state
    GL::SetViewport(prevVP);
    GL::SetEnabled(GL::Test::CullFace, wasCullEnabled);
    GL::SetColorMask(true, true, true, true);
    GLUniforms::SetModelMatrix(prevModel);
    GLUniforms::SetViewMatrix(prevView);
    GLUniforms::SetProjectionMatrix(prevProj);
    GL::Bind(GL::BindTarget::Framebuffer,   prevBoundFB);
    GL::Bind(GL::BindTarget::ShaderProgram, prevBoundSP);

    return irradianceCubeMapRH;
}
