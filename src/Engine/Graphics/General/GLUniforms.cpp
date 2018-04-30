#include "Bang/GLUniforms.h"

#include "Bang/Camera.h"
#include "Bang/Settings.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/TextureFactory.h"
#include "Bang/ShaderProgram.h"

USING_NAMESPACE_BANG

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
    sp->SetMatrix4("B_Model",             matrices->model,       false);
    sp->SetMatrix4("B_ModelInv",          matrices->modelInv,    false);
    sp->SetMatrix4("B_Normal",            matrices->normal,      false);
    sp->SetMatrix4("B_View",              matrices->view,        false);
    sp->SetMatrix4("B_ViewInv",           matrices->viewInv,     false);
    sp->SetMatrix4("B_Projection",        matrices->proj,        false);
    sp->SetMatrix4("B_ProjectionInv",     matrices->projInv,     false);
    sp->SetMatrix4("B_ProjectionView",    matrices->projView,    false);
    sp->SetMatrix4("B_ProjectionViewInv", matrices->projViewInv, false);
    sp->SetMatrix4("B_PVM",               matrices->pvm,         false);
    sp->SetMatrix4("B_PVMInv",            matrices->pvmInv,      false);

    sp->SetFloat("B_AmbientLight", Settings::GetAmbientLight(), false);

    Camera *cam = Camera::GetActive();
    Transform *camTR = (cam ? cam->GetGameObject()->GetTransform() : nullptr);
    sp->SetFloat("B_Camera_ZNear",  (cam ? cam->GetZNear() : 0.0f), false);
    sp->SetFloat("B_Camera_ZFar",   (cam ?  cam->GetZFar() : 0.0f), false);
    sp->SetVector3("B_Camera_Forward",
                   (camTR ? camTR->GetForward() : Vector3::Zero), false);
    sp->SetVector3("B_Camera_WorldPos",
                   (camTR ? camTR->GetPosition() : Vector3::Zero), false);
    sp->SetColor("B_Camera_ClearColor",
                 (cam ? cam->GetClearColor() : Color::Pink), false);

    TextureCubeMap *skyBox    = cam ? cam->GetSkyBoxTexture()         : nullptr;
    TextureCubeMap *sSkyBox   = cam ? cam->GetSpecularSkyBoxTexture() : nullptr;
    TextureCubeMap *dSkyBox   = cam ? cam->GetDiffuseSkyBoxTexture()  : nullptr;
    sp->SetTextureCubeMap("B_SkyBox",         skyBox,  false);
    sp->SetTextureCubeMap("B_SkyBoxSpecular", sSkyBox, false);
    sp->SetTextureCubeMap("B_SkyBoxDiffuse",  dSkyBox, false);
    sp->SetBool("B_Camera_ClearMode", cam ? int(cam->GetClearMode()) : -1, false);

    ViewportUniforms *viewportUniforms = GLUniforms::GetViewportUniforms();
    sp->SetVector2("B_Viewport_MinPos", viewportUniforms->minPos, false);
    sp->SetVector2("B_Viewport_Size",   viewportUniforms->size,   false);
}

void GLUniforms::SetModelMatrix(const Matrix4 &model)
{
    MatrixUniforms *matrices = GLUniforms::GetMatrixUniforms();
    if (model != matrices->model)
    {
        matrices->model = model;
        matrices->modelInv = model.Inversed();
        UpdatePVMMatrix();
    }

}
void GLUniforms::SetViewMatrix(const Matrix4 &view)
{
    MatrixUniforms *matrices = GLUniforms::GetMatrixUniforms();
    if (view != matrices->view)
    {
        matrices->view    = view;
        matrices->viewInv = view.Inversed();
        UpdatePVMMatrix();
    }
}
void GLUniforms::SetProjectionMatrix(const Matrix4 &projection)
{
    MatrixUniforms *matrices = GLUniforms::GetMatrixUniforms();
    if (projection != matrices->proj)
    {
        matrices->proj    = projection;
        matrices->projInv = projection.Inversed();
        UpdatePVMMatrix();
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
        case GL::ViewProjMode::World:
            pvmMatrix = matrices->proj * viewModel;
            projViewMatrix = matrices->proj * matrices->view;
        break;

        case GL::ViewProjMode::Canvas:
        {
            Matrix4 proj = GLUniforms::GetCanvasProjectionMatrix();
            pvmMatrix = proj * model;
            projViewMatrix = proj * matrices->view;
        }
        break;
    }
    matrices->projView    = projViewMatrix;
    matrices->projViewInv = matrices->projView.Inversed();
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
    GLUniforms *glu = GLUniforms::GetActive();
    return (glu->GetViewProjMode() == GL::ViewProjMode::World) ?
                glu->GetMatrixUniforms()->proj :
                GLUniforms::GetCanvasProjectionMatrix();
}

GL::ViewProjMode GLUniforms::GetViewProjMode() const
{
    return m_viewProjMode;
}

GLUniforms *GLUniforms::GetActive()
{
    return GL::GetActive()->GetGLUniforms();
}

void GLUniforms::OnViewportChanged(const AARecti &newViewport)
{
    ViewportUniforms *vpUnifs = GLUniforms::GetViewportUniforms();
    vpUnifs->minPos = Vector2(newViewport.GetMin());
    vpUnifs->size   = Vector2(newViewport.GetSize());
    GLUniforms::UpdatePVMMatrix();
}

