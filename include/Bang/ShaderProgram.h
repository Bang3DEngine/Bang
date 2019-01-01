#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Asset.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/GL.h"
#include "Bang/GLObject.h"
#include "Bang/IEventsAsset.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/Matrix3.h"
#include "Bang/Matrix4.h"
#include "Bang/ShaderProgramProperties.h"
#include "Bang/String.h"
#include "Bang/Texture3D.h"
#include "Bang/UMap.h"

namespace Bang
{
template <class>
class EventEmitter;
class Color;
class Texture3D;
class IEventsDestroy;
class IEventsAsset;
class Path;
class Shader;
class Texture2D;
class Texture;
class TextureCubeMap;

class ShaderProgram : public GLObject,
                      public Asset,
                      public EventListener<IEventsAsset>,
                      public EventListener<IEventsDestroy>
{
    ASSET(ShaderProgram)

public:
    bool Load(const Path &unifiedShaderPath);
    bool Load(const Path &vShaderPath, const Path &fShaderPath);
    bool Load(const Path &vShaderPath,
              const Path &gShaderPath,
              const Path &fShaderPath);
    bool Load(Shader *vShader, Shader *fShader);
    bool Load(Shader *vShader, Shader *gShader, Shader *fShader);

    bool Link();
    bool IsLinked() const;

    void Bind() override;
    void UnBind() override;
    GL::BindTarget GetGLBindTarget() const override;

    bool SetInt(const String &name, int v, bool warn = false);
    bool SetBool(const String &name, bool v, bool warn = false);
    bool SetFloat(const String &name, float v, bool warn = false);
    bool SetDouble(const String &name, double v, bool warn = false);
    bool SetColor(const String &name, const Color &v, bool warn = false);
    bool SetVector2(const String &name, const Vector2 &v, bool warn = false);
    bool SetVector3(const String &name, const Vector3 &v, bool warn = false);
    bool SetVector4(const String &name, const Vector4 &v, bool warn = false);
    bool SetMatrix3(const String &name, const Matrix3 &v, bool warn = false);
    bool SetMatrix4(const String &name, const Matrix4 &v, bool warn = false);
    bool SetTexture2D(const String &name,
                      Texture2D *texture,
                      bool warn = false);
    bool SetTexture3D(const String &name,
                      Texture3D *texture3D,
                      bool warn = false);
    bool SetTextureCubeMap(const String &name,
                           TextureCubeMap *textureCubeMap,
                           bool warn = false);
    bool SetIntArray(const String &name,
                     const Array<int> &v,
                     bool warn = false);
    bool SetBoolArray(const String &name,
                      const Array<bool> &v,
                      bool warn = false);
    bool SetFloatArray(const String &name,
                       const Array<float> &v,
                       bool warn = false);
    bool SetDoubleArray(const String &name,
                        const Array<double> &v,
                        bool warn = false);
    bool SetColorArray(const String &name,
                       const Array<Color> &v,
                       bool warn = false);
    bool SetVector2Array(const String &name,
                         const Array<Vector2> &v,
                         bool warn = false);
    bool SetVector3Array(const String &name,
                         const Array<Vector3> &v,
                         bool warn = false);
    bool SetVector4Array(const String &name,
                         const Array<Vector4> &v,
                         bool warn = false);
    bool SetMatrix3Array(const String &name,
                         const Array<Matrix3> &v,
                         bool warn = false);
    bool SetMatrix4Array(const String &name,
                         const Array<Matrix4> &v,
                         bool warn = false);
    bool SetTexture2DArray(const String &name,
                           const Array<Texture2D *> &v,
                           bool warn = false);

    bool AddShader(Shader *shader);
    bool SetVertexShader(Shader *vertexShader);
    bool SetGeometryShader(Shader *geometryShader);
    bool SetFragmentShader(Shader *fragmentShader);

    const ShaderProgramProperties &GetLoadedProperties() const;
    Shader *GetShader(GL::ShaderType type) const;
    Shader *GetVertexShader() const;
    Shader *GetGeometryShader() const;
    Shader *GetFragmentShader() const;
    const Path &GetUnifiedShaderPath() const;

    GLint GetUniformLocation(const String &name) const;

    // Serializable
    void Reflect() override;

    // Asset
    void Import(const Path &assetFilepath) override;

private:
    AH<Shader> p_vShader;
    AH<Shader> p_gShader;
    AH<Shader> p_fShader;
    Path m_unifiedShaderPath = Path::Empty();
    ShaderProgramProperties m_loadedProperties;
    bool m_isLinked = false;

    UMap<String, int> m_uniformCacheInt;
    UMap<String, bool> m_uniformCacheBool;
    UMap<String, float> m_uniformCacheFloat;
    UMap<String, double> m_uniformCacheDouble;
    UMap<String, Color> m_uniformCacheColor;
    UMap<String, Vector2> m_uniformCacheVector2;
    UMap<String, Vector3> m_uniformCacheVector3;
    UMap<String, Vector4> m_uniformCacheVector4;
    UMap<String, Matrix3> m_uniformCacheMatrix3;
    UMap<String, Matrix4> m_uniformCacheMatrix4;

    mutable UMap<String, int> m_nameToLocationCache;
    mutable UMap<String, Texture *> m_namesToTexture;

    ShaderProgram();
    ShaderProgram(Shader *vShader, Shader *fShader);
    ShaderProgram(Shader *vShader, Shader *gShader, Shader *fShader);
    ShaderProgram(const Path &vShaderPath, const Path &fShaderPath);
    ShaderProgram(const Path &vShaderPath,
                  const Path &gShaderPath,
                  const Path &fShaderPath);
    virtual ~ShaderProgram() override;

    void Bind() const override;
    void UnBind() const override;

    bool SetDefaultTexture2D(const String &name, bool warn = true);
    bool SetDefaultTexture3D(const String &name, bool warn = true);
    bool SetDefaultTextureCubeMap(const String &name, bool warn = true);
    bool SetTexture(const String &name, Texture *texture, bool warn = true);

    void BindAllTexturesToUnits();
    void CheckTextureBindingsValidity() const;
    void BindTextureToFreeUnit(const String &textureName, Texture *texture);
    void UnBindAllTexturesFromUnits() const;

    // IAssetListener
    void OnImported(Asset *res) override;

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;
};
}

#endif  // SHADERPROGRAM_H
