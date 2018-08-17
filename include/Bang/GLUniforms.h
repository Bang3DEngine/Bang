#ifndef GLUNIFORMS_H
#define GLUNIFORMS_H

#include "Bang/GL.h"
#include "Bang/Array.h"
#include "Bang/Matrix4.h"
#include "Bang/Vector2.h"
#include "Bang/TypeMap.h"
#include "Bang/UniformBuffer.h"
#include "Bang/TextureCubeMap.h"

NAMESPACE_BANG_BEGIN

FORWARD class ShaderProgram;

class GLUniforms
{
public:
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

    struct MatrixUniforms
    {
        Matrix4 model;
        Matrix4 modelInv;
        Matrix4 normal;
        Matrix4 view;
        Matrix4 viewInv;
        Matrix4 proj;
        Matrix4 projInv;
        Matrix4 projView;
        Matrix4 pvm;
        Matrix4 pvmInv;
    };

    struct ViewportUniforms
    {
        Vector2 minPos;
        Vector2 size;
    };

    template <class T>
    struct GLSLVar
    {
        String name = "";
        T value = T();
        int size = -1;
        Array<T> values;

        GLSLVar(const String &name_, const T &value_, int size_)
            : name(name_),
              value(value_),
              size(size_)
        {
        }

        GLSLVar() {}
    };

    template <class T>
    static T GetUniform(GLId program, int uniformLocation);

    template <class T>
    static T GetUniform(GLId program, const String &uniformName);

    template <class T>
    static Array<T> GetUniformArray(GLId program, const String &uniformName,
                                    int numElements);

    template <class T>
    static Array<T> GetUniformArray(GLId program, int uniformLocation,
                                    int numElements);

    template <class T>
    static T GetUniform(const String &uniformName);

    template <class T>
    static GLSLVar<T> GetUniformAt(GLId shaderProgramId, GLuint uniformIndex);

    static void SetAllUniformsToShaderProgram(ShaderProgram *sp);

    static void SetModelMatrix(const Matrix4 &model);
    static void SetViewMatrix(const Matrix4 &view);
    static void SetProjectionMatrix(const Matrix4 &projection);
    static void UpdatePVMMatrix();

    void SetViewProjMode(GL::ViewProjMode viewProjMode);
    GL::ViewProjMode GetViewProjMode() const;

    static const Matrix4 &GetModelMatrix();
    static const Matrix4 &GetViewMatrix();
    static       Matrix4  GetProjectionMatrix();
    static       Matrix4  GetProjectionMatrix(GL::ViewProjMode viewProjMode);

    static GLUniforms *GetActive();

private:
    ViewportUniforms m_viewportUniforms;
    MatrixUniforms m_matrixUniforms;
    GL::ViewProjMode m_viewProjMode = GL::ViewProjMode::CANVAS;

    GLUniforms() = default;
    virtual ~GLUniforms() = default;

    static void OnViewportChanged(const AARecti &newViewport);

    static ViewportUniforms* GetViewportUniforms();
    static MatrixUniforms *GetMatrixUniforms();
    static Matrix4 GetCanvasProjectionMatrix();

    friend class GL;
};

NAMESPACE_BANG_END

#include "Bang/GLUniforms.tcc"

#endif // GLUNIFORMS_H

