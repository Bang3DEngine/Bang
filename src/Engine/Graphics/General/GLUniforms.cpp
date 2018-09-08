#include "Bang/GLUniforms.h"

#include "Bang/Camera.h"
#include "Bang/Settings.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/TextureFactory.h"

USING_NAMESPACE_BANG

const String GLUniforms::UniformBlockName_Camera = "B_CameraUniformBuffer";
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
const String GLUniforms::UniformName_Camera_WorldPos = "B_Camera_WorldPos";
const String GLUniforms::UniformName_Camera_ClearColor = "B_Camera_ClearColor";
const String GLUniforms::UniformName_Camera_ClearMode = "B_Camera_ClearMode";
const String GLUniforms::UniformName_SkyBox = "B_SkyBox";
const String GLUniforms::UniformName_SkyBoxDiffuse = "B_SkyBoxDiffuse";
const String GLUniforms::UniformName_SkyBoxSpecular = "B_SkyBoxSpecular";
const String GLUniforms::UniformName_Viewport_MinPos = "B_Viewport_MinPos";
const String GLUniforms::UniformName_Viewport_Size = "B_Viewport_Size";

GLUniforms::GLUniforms()
{
    m_cameraUniformBuffer.SetBindingPoint(0);
}

GLUniforms::ModelMatrixUniforms *GLUniforms::GetModelMatricesUniforms()
{
    return &GLUniforms::GetActive()->m_matrixUniforms;
}

Matrix4 GLUniforms::GetCanvasProjectionMatrix()
{
    const Vector2i vpSize = Vector2i::Max(GL::GetViewportSize(), Vector2i::One);
    const Matrix4   ortho = Matrix4::Ortho(0, vpSize.x, 0, vpSize.y, 1, -1);
    return ortho;
}

void GLUniforms::BindUniformBuffers(ShaderProgram *sp)
{
    m_cameraUniformBuffer.Bind();
    GL::BindUniformBlock(sp->GetGLId(),
                         GLUniforms::UniformBlockName_Camera,
                         m_cameraUniformBuffer.GetBindingPoint());
    m_cameraUniformBuffer.UnBind();
}

void GLUniforms::SetAllUniformsToShaderProgram(ShaderProgram *sp)
{
    ASSERT (GL::IsBound(sp->GetGLBindTarget(), sp->GetGLId()));

    ModelMatrixUniforms *matrices = GLUniforms::GetModelMatricesUniforms();

    sp->SetDouble(GLUniforms::UniformName_TimeSeconds, Time::GetEllapsed_Seconds());
    sp->SetMatrix4(GLUniforms::UniformName_Model,          matrices->model);
    sp->SetMatrix4(GLUniforms::UniformName_ModelInv,       matrices->modelInv);
    sp->SetMatrix4(GLUniforms::UniformName_Normal,         matrices->normal);
    sp->SetMatrix4(GLUniforms::UniformName_PVM,            matrices->pvm);
    sp->SetMatrix4(GLUniforms::UniformName_PVMInv,         matrices->pvmInv);

    Camera *cam = Camera::GetActive();

    TextureCubeMap *skyBox  = cam ? cam->GetSkyBoxTexture()         : nullptr;
    TextureCubeMap *sSkyBox = cam ? cam->GetSpecularSkyBoxTexture() : nullptr;
    TextureCubeMap *dSkyBox = cam ? cam->GetDiffuseSkyBoxTexture()  : nullptr;
    sp->SetTextureCubeMap(GLUniforms::UniformName_SkyBox,         skyBox);
    sp->SetTextureCubeMap(GLUniforms::UniformName_SkyBoxSpecular, sSkyBox);
    sp->SetTextureCubeMap(GLUniforms::UniformName_SkyBoxDiffuse,  dSkyBox);
}

void GLUniforms::SetCameraWorldPosition(const Vector3 &camWorldPosition)
{
    if (GLUniforms *glu = GLUniforms::GetActive())
    {
        glu->m_cameraUniforms.worldPos = Vector4(camWorldPosition, 0);
        glu->m_cameraUniformBuffer.SetSubData(Vector4(camWorldPosition, 0),
                                              5 * sizeof(Matrix4) +
                                              0 * sizeof(Vector4));
    }
}

void GLUniforms::SetCameraClearColor(const Color &camClearColor)
{
    if (GLUniforms *glu = GLUniforms::GetActive())
    {
        glu->m_cameraUniforms.clearColor = camClearColor.ToVector4();
        glu->m_cameraUniformBuffer.SetSubData(camClearColor.ToVector4(),
                                              5 * sizeof(Matrix4) +
                                              1 * sizeof(Vector4));
    }

}
void GLUniforms::OnViewportChanged(const AARecti &newViewport)
{
    if (GLUniforms *glu = GLUniforms::GetActive())
    {
        Vector2 vpMinPos = Vector2(newViewport.GetMin());
        Vector2 vpSize   = Vector2(newViewport.GetSize());
        glu->m_cameraUniforms.viewportMinPos = vpMinPos;
        glu->m_cameraUniforms.viewportSize   = vpSize;
        glu->m_cameraUniformBuffer.SetSubData(vpMinPos, 5 * sizeof(Matrix4) +
                                                        2 * sizeof(Vector4));
        glu->m_cameraUniformBuffer.SetSubData(vpSize, 5 * sizeof(Matrix4) +
                                                      2 * sizeof(Vector4) +
                                                      1 * sizeof(Vector2));
        GLUniforms::UpdatePVMMatrix();
    }
}

void GLUniforms::SetCameraClearMode(const CameraClearMode &camClearMode)
{
    if (GLUniforms *glu = GLUniforms::GetActive())
    {
        glu->m_cameraUniforms.clearMode = SCAST<int>(camClearMode);
        glu->m_cameraUniformBuffer.SetSubData(camClearMode,
                                              5 * sizeof(Matrix4) +
                                              2 * sizeof(Vector4) +
                                              2 * sizeof(Vector2));
    }
}

void GLUniforms::SetModelMatrix(const Matrix4 &model)
{
    ModelMatrixUniforms *matrices = GLUniforms::GetModelMatricesUniforms();
    if (model != matrices->model)
    {
        matrices->model = model;
        matrices->modelInv = model.Inversed();
        GLUniforms::UpdatePVMMatrix();
    }

}
void GLUniforms::SetViewMatrix(const Matrix4 &view)
{
    if (GLUniforms *glu = GLUniforms::GetActive())
    {
        if (view != glu->m_cameraUniforms.view)
        {
            Matrix4 viewInv = view.Inversed();
            glu->m_cameraUniforms.view    = view;
            glu->m_cameraUniforms.viewInv = viewInv;
            glu->m_cameraUniformBuffer.SetSubData(view,    0 * sizeof(Matrix4));
            glu->m_cameraUniformBuffer.SetSubData(viewInv, 1 * sizeof(Matrix4));
            GLUniforms::UpdatePVMMatrix();
        }
    }
}
void GLUniforms::SetProjectionMatrix(const Matrix4 &projection)
{
    if (GLUniforms *glu = GLUniforms::GetActive())
    {
        if (projection != glu->m_cameraUniforms.proj)
        {
            Matrix4 projectionInv = projection.Inversed();
            glu->m_cameraUniforms.proj    = projection;
            glu->m_cameraUniforms.projInv = projectionInv;
            glu->m_cameraUniformBuffer.SetSubData(projection,    2 * sizeof(Matrix4));
            glu->m_cameraUniformBuffer.SetSubData(projectionInv, 3 * sizeof(Matrix4));
            GLUniforms::UpdatePVMMatrix();
        }
    }
}

void GLUniforms::UpdatePVMMatrix()
{
    ModelMatrixUniforms *matrices = GLUniforms::GetModelMatricesUniforms();
    GLUniforms *glu = GLUniforms::GetActive();

    const Matrix4 &model = matrices->model;
    Matrix4 viewModel = (glu->m_cameraUniforms.view * matrices->model);

    Matrix4 normalMatrix = model.Inversed().Transposed();
    matrices->normal = normalMatrix;

    Matrix4 pvmMatrix;
    Matrix4 projViewMatrix;
    switch (glu->GetViewProjMode())
    {
        case GL::ViewProjMode::WORLD:
            pvmMatrix = glu->m_cameraUniforms.proj * viewModel;
            projViewMatrix = glu->m_cameraUniforms.proj *
                             glu->m_cameraUniforms.view;
        break;

        case GL::ViewProjMode::CANVAS:
        {
            Matrix4 canvasProj = GLUniforms::GetCanvasProjectionMatrix();
            pvmMatrix = canvasProj * model;
            projViewMatrix = canvasProj * glu->m_cameraUniforms.view;
        }
        break;
    }

    glu->m_cameraUniforms.projView = projViewMatrix;
    glu->m_cameraUniformBuffer.SetSubData(projViewMatrix, 4 * sizeof(Matrix4));
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
    return GLUniforms::GetActive()->GetModelMatricesUniforms()->model;
}

const Matrix4 &GLUniforms::GetViewMatrix()
{
    return GLUniforms::GetActive()->m_cameraUniforms.view;
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
                glu->m_cameraUniforms.proj :
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

