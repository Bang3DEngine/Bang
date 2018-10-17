#ifndef GLUNIFORMS_H
#define GLUNIFORMS_H

#include <GL/glew.h>

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/Matrix4.tcc"
#include "Bang/NeededUniformFlags.h"
#include "Bang/String.h"
#include "Bang/UniformBuffer.h"
#include "Bang/UniformBuffer.tcc"

namespace Bang
{
class Color;
}  // namespace Bang

namespace Bang
{
class ShaderProgram;

enum class CameraClearMode;

class GLUniforms
{
public:
    static const String UniformBlockName_Camera;
    static const String UniformName_ReceivesShadows;
    static const String UniformName_MaterialAlbedoColor;
    static const String UniformName_AlbedoUvOffset;
    static const String UniformName_AlbedoUvMultiply;
    static const String UniformName_MaterialReceivesLighting;
    static const String UniformName_AlbedoTexture;
    static const String UniformName_AlphaCutoff;
    static const String UniformName_HasAlbedoTexture;
    static const String UniformName_MaterialRoughness;
    static const String UniformName_MaterialMetalness;
    static const String UniformName_NormalMapUvOffset;
    static const String UniformName_NormalMapUvMultiply;
    static const String UniformName_NormalMapMultiplyFactor;
    static const String UniformName_BRDF_LUT;
    static const String UniformName_RoughnessTexture;
    static const String UniformName_MetalnessTexture;
    static const String UniformName_NormalMapTexture;
    static const String UniformName_HasNormalMapTexture;
    static const String UniformName_TimeSeconds;
    static const String UniformName_Model;
    static const String UniformName_ModelInv;
    static const String UniformName_Normal;
    static const String UniformName_View;
    static const String UniformName_ViewInv;
    static const String UniformName_Projection;
    static const String UniformName_ProjectionInv;
    static const String UniformName_ProjectionView;
    static const String UniformName_PVMInv;
    static const String UniformName_PVM;
    static const String UniformName_Camera_WorldForward;
    static const String UniformName_Camera_WorldPos;
    static const String UniformName_Camera_ClearColor;
    static const String UniformName_Camera_ClearMode;
    static const String UniformName_SkyBox;
    static const String UniformName_SkyBoxSpecular;
    static const String UniformName_SkyBoxDiffuse;
    static const String UniformName_Viewport_MinPos;
    static const String UniformName_Viewport_Size;

    struct ModelMatrixUniforms
    {
        Matrix4 model;
        Matrix4 modelInv;
        Matrix4 normal;
        Matrix4 pvm;
        Matrix4 pvmInv;
    };

    struct CameraUniforms
    {
        Matrix4 view;
        Matrix4 viewInv;
        Matrix4 proj;
        Matrix4 projInv;
        Matrix4 projView;

        Vector4 worldPos;
        Vector4 clearColor;

        Vector2 viewportMinPos;
        Vector2 viewportSize;
        int clearMode;
    };

    template <class T>
    struct GLSLVar
    {
        String name = "";
        T value = T();
        int size = -1;
        Array<T> values;

        GLSLVar() = default;
        GLSLVar(const String &name_, const T &value_, int size_)
            : name(name_), value(value_), size(size_)
        {
        }
    };

    template <class T>
    static T GetUniform(GLId program, int uniformLocation);

    template <class T>
    static T GetUniform(GLId program, const String &uniformName);

    template <class T>
    static Array<T> GetUniformArray(GLId program,
                                    const String &uniformName,
                                    int numElements);

    template <class T>
    static Array<T> GetUniformArray(GLId program,
                                    int uniformLocation,
                                    int numElements);

    template <class T>
    static T GetUniform(const String &uniformName);

    template <class T>
    static GLSLVar<T> GetUniformAt(GLId shaderProgramId, GLuint uniformIndex);

    void BindUniformBuffers(ShaderProgram *shaderProgram);
    static void SetAllUniformsToShaderProgram(
        ShaderProgram *sp,
        NeededUniformFlags neededUniforms = NeededUniformFlag::ALL);

    static void SetCameraWorldPosition(const Vector3 &camWorldPosition);
    static void SetCameraClearColor(const Color &camClearColor);
    static void SetCameraClearMode(const CameraClearMode &camClearMode);
    static void SetModelMatrix(const Matrix4 &model);
    static void SetViewMatrix(const Matrix4 &view);
    static void SetProjectionMatrix(const Matrix4 &projection);

    void SetViewProjMode(GL::ViewProjMode viewProjMode);
    GL::ViewProjMode GetViewProjMode() const;

    static const Matrix4 &GetModelMatrix();
    static const Matrix4 &GetViewMatrix();
    static Matrix4 GetProjectionMatrix();
    static Matrix4 GetProjectionMatrix(GL::ViewProjMode viewProjMode);
    static Matrix4 CalculateNormalMatrix(const Matrix4 &modelMatrix);

    static GLUniforms *GetActive();

private:
    bool m_cameraUniformBufferOutdated = true;
    UniformBuffer<CameraUniforms> m_cameraUniformBuffer;
    static void UpdatePVMMatrix();

    CameraUniforms m_cameraUniforms;
    ModelMatrixUniforms m_matrixUniforms;
    GL::ViewProjMode m_viewProjMode = GL::ViewProjMode::CANVAS;

    GLUniforms();
    virtual ~GLUniforms() = default;

    static void OnViewportChanged(const AARecti &newViewport);

    static ModelMatrixUniforms *GetModelMatricesUniforms();
    static Matrix4 GetCanvasProjectionMatrix();

    friend class GL;
};
}

#endif  // GLUNIFORMS_H
