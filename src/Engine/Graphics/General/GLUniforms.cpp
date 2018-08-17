#include "Bang/GLUniforms.h"

#include "Bang/Camera.h"
#include "Bang/Settings.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/TextureFactory.h"
#include "Bang/ShaderProgram.h"

USING_NAMESPACE_BANG

const String GLUniforms::UniformName_ReceivesShadows = "B_ReceivesShadows";
const String GLUniforms::UniformName_MaterialAlbedoColor = "B_MaterialAlbedoColor";
const String GLUniforms::UniformName_AlbedoUvOffset = "B_AlbedoUvOffset";
const String GLUniforms::UniformName_AlbedoUvMultiply = "B_AlbedoUvMultiply";
const String GLUniforms::UniformName_MaterialReceivesLighting = "B_MaterialReceivesLighting";
const String GLUniforms::UniformName_AlbedoTexture = "B_AlbedoTexture";
const String GLUniforms::UniformName_AlphaCutoff = "B_AlphaCutoff";
const String GLUniforms::UniformName_HasAlbedoTexture = "B_HasAlbedoTexture";
const String GLUniforms::UniformName_MaterialRoughness = "B_MaterialRoughness";
const String GLUniforms::UniformName_MaterialMetalness = "B_MaterialMetalness";
const String GLUniforms::UniformName_NormalMapUvOffset = "B_NormalMapUvOffset";
const String GLUniforms::UniformName_NormalMapUvMultiply = "B_NormalMapUvMultiply";
const String GLUniforms::UniformName_NormalMapMultiplyFactor = "B_NormalMapMultiplyFactor";
const String GLUniforms::UniformName_BRDF_LUT = "B_BRDF_LUT";
const String GLUniforms::UniformName_RoughnessTexture = "B_RoughnessTexture";
const String GLUniforms::UniformName_MetalnessTexture = "B_MetalnessTexture";
const String GLUniforms::UniformName_NormalMapTexture = "B_NormalMapTexture";
const String GLUniforms::UniformName_HasNormalMapTexture = "B_HasNormalMapTexture";
const String GLUniforms::UniformName_TimeSeconds = "B_TimeSeconds";
const String GLUniforms::UniformName_Model = "B_Model";
const String GLUniforms::UniformName_ModelInv = "B_ModelInv";
const String GLUniforms::UniformName_Normal = "B_Normal";
const String GLUniforms::UniformName_View = "B_View";
const String GLUniforms::UniformName_ViewInv = "B_ViewInv";
const String GLUniforms::UniformName_Projection = "B_Projection";
const String GLUniforms::UniformName_ProjectionInv = "B_ProjectionInv";
const String GLUniforms::UniformName_ProjectionView = "B_ProjectionView";
const String GLUniforms::UniformName_PVMInv = "B_PVMInv";
const String GLUniforms::UniformName_PVM = "B_PVM";
const String GLUniforms::UniformName_Camera_WorldForward = "B_Camera_WorldForward";
const String GLUniforms::UniformName_Camera_WorldPos = "B_Camera_WorldPos";
const String GLUniforms::UniformName_Camera_ClearColor = "B_Camera_ClearColor";
const String GLUniforms::UniformName_Camera_ClearMode = "B_Camera_ClearMode";
const String GLUniforms::UniformName_SkyBox = "B_SkyBox";
const String GLUniforms::UniformName_SkyBoxSpecular = "B_SkyBoxDiffuse";
const String GLUniforms::UniformName_SkyBoxDiffuse = "B_SkyBoxSpecular";
const String GLUniforms::UniformName_Viewport_MinPos = "B_Viewport_MinPos";
const String GLUniforms::UniformName_Viewport_Size = "B_Viewport_Size";

GLUniforms::ViewportUniforms *GLUniforms::GetViewportUniforms()
{
    return &GLUniforms::GetActive()->m_viewportUniforms;
}

GLUniforms::MatrixUniforms *GLUniforms::GetMatrixUniforms()
{
    return &GLUniforms::GetActive()->m_matrixUniforms;
}

Matrix4 GLUniforms::GetCanvasProjectionMatrix()
{
    const Vector2i vpSize = Vector2i::Max(GL::GetViewportSize(), Vector2i::One);
    const Matrix4   ortho = Matrix4::Ortho(0, vpSize.x, 0, vpSize.y, 1, -1);
    return ortho;
}

void GLUniforms::SetAllUniformsToShaderProgram(ShaderProgram *sp)
{
    ASSERT (GL::IsBound(sp->GetGLBindTarget(), sp->GetGLId()));

    MatrixUniforms *matrices = GLUniforms::GetMatrixUniforms();

    sp->SetFloat(GLUniforms::UniformName_TimeSeconds,
                 Time::GetEllapsed_Seconds(),
                 false);
    sp->SetMatrix4(GLUniforms::UniformName_Model,          matrices->model,    false);
    sp->SetMatrix4(GLUniforms::UniformName_ModelInv,       matrices->modelInv, false);
    sp->SetMatrix4(GLUniforms::UniformName_Normal,         matrices->normal,   false);
    sp->SetMatrix4(GLUniforms::UniformName_View,           matrices->view,     false);
    sp->SetMatrix4(GLUniforms::UniformName_ViewInv,        matrices->viewInv,  false);
    sp->SetMatrix4(GLUniforms::UniformName_Projection,     matrices->proj,     false);
    sp->SetMatrix4(GLUniforms::UniformName_ProjectionInv,  matrices->projInv,  false);
    sp->SetMatrix4(GLUniforms::UniformName_ProjectionView, matrices->projView, false);
    sp->SetMatrix4(GLUniforms::UniformName_PVMInv,         matrices->pvmInv,   false);
    sp->SetMatrix4(GLUniforms::UniformName_PVM,            matrices->pvm,      false);

    Camera *cam = Camera::GetActive();
    Transform *camTR = (cam ? cam->GetGameObject()->GetTransform() : nullptr);
    sp->SetVector3(GLUniforms::UniformName_Camera_WorldForward,
                   (camTR ? camTR->GetForward() : Vector3::Zero),
                   false);
    sp->SetVector3(GLUniforms::UniformName_Camera_WorldPos,
                   (camTR ? camTR->GetPosition() : Vector3::Zero),
                   false);
    sp->SetColor(GLUniforms::UniformName_Camera_ClearColor,
                 (cam ? cam->GetClearColor() : Color::Pink),
                 false);
    sp->SetBool(GLUniforms::UniformName_Camera_ClearMode,
                cam ? int(cam->GetClearMode()) : -1,
                false);

    TextureCubeMap *skyBox    = cam ? cam->GetSkyBoxTexture()         : nullptr;
    TextureCubeMap *sSkyBox   = cam ? cam->GetSpecularSkyBoxTexture() : nullptr;
    TextureCubeMap *dSkyBox   = cam ? cam->GetDiffuseSkyBoxTexture()  : nullptr;
    sp->SetTextureCubeMap(GLUniforms::UniformName_SkyBox,         skyBox,  false);
    sp->SetTextureCubeMap(GLUniforms::UniformName_SkyBoxSpecular, sSkyBox, false);
    sp->SetTextureCubeMap(GLUniforms::UniformName_SkyBoxDiffuse,  dSkyBox, false);

    ViewportUniforms *viewportUniforms = GLUniforms::GetViewportUniforms();
    sp->SetVector2(GLUniforms::UniformName_Viewport_MinPos, viewportUniforms->minPos, false);
    sp->SetVector2(GLUniforms::UniformName_Viewport_Size,   viewportUniforms->size,   false);
}

void GLUniforms::SetModelMatrix(const Matrix4 &model)
{
    MatrixUniforms *matrices = GLUniforms::GetMatrixUniforms();
    if (model != matrices->model)
    {
        matrices->model = model;
        matrices->modelInv = model.Inversed();
        GLUniforms::UpdatePVMMatrix();
    }

}
void GLUniforms::SetViewMatrix(const Matrix4 &view)
{
    MatrixUniforms *matrices = GLUniforms::GetMatrixUniforms();
    if (view != matrices->view)
    {
        matrices->view    = view;
        matrices->viewInv = view.Inversed();
        GLUniforms::UpdatePVMMatrix();
    }
}
void GLUniforms::SetProjectionMatrix(const Matrix4 &projection)
{
    MatrixUniforms *matrices = GLUniforms::GetMatrixUniforms();
    if (projection != matrices->proj)
    {
        matrices->proj    = projection;
        matrices->projInv = projection.Inversed();
        GLUniforms::UpdatePVMMatrix();
    }
}

void GLUniforms::UpdatePVMMatrix()
{
    MatrixUniforms *matrices = GLUniforms::GetMatrixUniforms();
    GLUniforms *glu = GLUniforms::GetActive();

    const Matrix4 &model = matrices->model;
    Matrix4 viewModel = matrices->view * matrices->model;

    Matrix4 normalMatrix = model.Inversed().Transposed();
    matrices->normal = normalMatrix;

    Matrix4 pvmMatrix;
    Matrix4 projViewMatrix;
    switch (glu->GetViewProjMode())
    {
        case GL::ViewProjMode::WORLD:
            pvmMatrix = matrices->proj * viewModel;
            projViewMatrix = matrices->proj * matrices->view;
        break;

        case GL::ViewProjMode::CANVAS:
        {
            Matrix4 proj = GLUniforms::GetCanvasProjectionMatrix();
            pvmMatrix = proj * model;
            projViewMatrix = proj * matrices->view;
        }
        break;
    }
    matrices->projView    = projViewMatrix;
    matrices->pvm = pvmMatrix;
    matrices->pvmInv = matrices->pvm.Inversed();
}

void GLUniforms::SetViewProjMode(GL::ViewProjMode viewProjMode)
{
    if (viewProjMode != GetViewProjMode())
    {
        m_viewProjMode = viewProjMode;
        GLUniforms::UpdatePVMMatrix();
    }
}

const Matrix4 &GLUniforms::GetModelMatrix()
{
    return GLUniforms::GetActive()->GetMatrixUniforms()->model;
}

const Matrix4 &GLUniforms::GetViewMatrix()
{
    return GLUniforms::GetActive()->GetMatrixUniforms()->view;
}

Matrix4 GLUniforms::GetProjectionMatrix()
{
    GL::ViewProjMode vpm = GLUniforms::GetActive()->GetViewProjMode();
    return GLUniforms::GetProjectionMatrix(vpm);
}

Matrix4 GLUniforms::GetProjectionMatrix(GL::ViewProjMode viewProjMode)
{
    GLUniforms *glu = GLUniforms::GetActive();
    return (viewProjMode == GL::ViewProjMode::WORLD) ?
                glu->GetMatrixUniforms()->proj :
                GLUniforms::GetCanvasProjectionMatrix();
}

GL::ViewProjMode GLUniforms::GetViewProjMode() const
{
    return m_viewProjMode;
}

GLUniforms *GLUniforms::GetActive()
{
    return GL::GetInstance()->GetGLUniforms();
}

void GLUniforms::OnViewportChanged(const AARecti &newViewport)
{
    ViewportUniforms *vpUnifs = GLUniforms::GetViewportUniforms();
    vpUnifs->minPos = Vector2(newViewport.GetMin());
    vpUnifs->size   = Vector2(newViewport.GetSize());
    GLUniforms::UpdatePVMMatrix();
}

