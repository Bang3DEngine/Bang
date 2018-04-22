#include "Bang/ShaderProgram.h"

#include "Bang/GL.h"
#include "Bang/Debug.h"
#include "Bang/Color.h"
#include "Bang/Shader.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"
#include "Bang/Matrix3.h"
#include "Bang/Matrix4.h"
#include "Bang/XMLNode.h"
#include "Bang/Texture2D.h"
#include "Bang/Resources.h"
#include "Bang/GLUniforms.h"
#include "Bang/IconManager.h"
#include "Bang/TextureCubeMap.h"
#include "Bang/TextureUnitManager.h"

USING_NAMESPACE_BANG

ShaderProgram::ShaderProgram()
{
    m_idGL = GL::CreateProgram();
}

ShaderProgram::ShaderProgram(Shader *vShader, Shader *fShader) : ShaderProgram()
{
    Load(vShader, fShader);
}

ShaderProgram::ShaderProgram(Shader *vShader, Shader *gShader, Shader *fShader)
{
    Load(vShader, gShader, fShader);
}

ShaderProgram::ShaderProgram(const Path &vShaderPath,
                             const Path &fShaderPath) : ShaderProgram()
{
    Load(vShaderPath, fShaderPath);
}

ShaderProgram::ShaderProgram(const Path &vShaderPath,
                             const Path &gShaderPath,
                             const Path &fShaderPath)
{
    Load(vShaderPath, gShaderPath, fShaderPath);
}

ShaderProgram::~ShaderProgram()
{
    GL::DeleteProgram(m_idGL);
}

bool ShaderProgram::Load(const Path &vShaderPath, const Path &fShaderPath)
{
    RH<Shader> vShader = Resources::Load<Shader>(vShaderPath);
    RH<Shader> fShader = Resources::Load<Shader>(fShaderPath);
    return Load(vShader.Get(), fShader.Get());
}

bool ShaderProgram::Load(const Path &vShaderPath,
                         const Path &gShaderPath,
                         const Path &fShaderPath)
{
    RH<Shader> vShader = Resources::Load<Shader>(vShaderPath);
    RH<Shader> gShader = Resources::Load<Shader>(gShaderPath);
    RH<Shader> fShader = Resources::Load<Shader>(fShaderPath);
    return Load(vShader.Get(), gShader.Get(), fShader.Get());
}

bool ShaderProgram::Load(Shader* vShader, Shader* fShader)
{
    if(!vShader || !fShader) { return false; }
    if (vShader == GetVertexShader() &&
        fShader == GetFragmentShader())
    {
        return true;
    }

    SetVertexShader(vShader);
    SetFragmentShader(fShader);
    return Link();
}

bool ShaderProgram::Load(Shader *vShader, Shader *gShader, Shader *fShader)
{
    if(!vShader || !gShader || !fShader) { return false; }
    if (vShader == GetVertexShader()   &&
        gShader == GetGeometryShader() &&
        fShader == GetFragmentShader())
    {
        return true;
    }

    SetVertexShader(vShader);
    SetGeometryShader(gShader);
    SetFragmentShader(fShader);
    return Link();
}

bool ShaderProgram::Link()
{
    if (!GetVertexShader())
    {
        Debug_Error("Vertex shader not set. Can't link shader program.");
        return false;
    }

    if (!GetFragmentShader())
    {
        Debug_Error("Fragment shader not set. Can't link shader program.");
        return false;
    }

    if (m_idGL > 0) { GL::DeleteProgram(m_idGL); }
    m_isLinked = false;

    m_idGL = GL::CreateProgram();

    GL::AttachShader(m_idGL, GetVertexShader()->GetGLId());
    if (GetGeometryShader())
    {
        GL::AttachShader(m_idGL, GetGeometryShader()->GetGLId());
    }
    GL::AttachShader(m_idGL, GetFragmentShader()->GetGLId());

    if (!GL::LinkProgram(m_idGL))
    {
       Debug_Error("The shader program " << this << "( " <<
                   GetVertexShader()->GetResourceFilepath() << ", " <<
                   GetFragmentShader()->GetResourceFilepath() <<
                   ") did not link: " <<
                   GL::GetProgramErrorMsg(m_idGL));
       GL::DeleteProgram(m_idGL); m_idGL = 0;
       return false;
    }

    m_isLinked = true;

    // Invalidate caches
    m_nameToLocationCache.clear();
    m_uniformCacheBool.clear();
    m_uniformCacheInt.clear();
    m_uniformCacheFloat.clear();
    m_uniformCacheColor.clear();
    m_uniformCacheVector2.clear();
    m_uniformCacheVector3.clear();
    m_uniformCacheVector4.clear();
    m_uniformCacheMatrix3.clear();
    m_uniformCacheMatrix4.clear();
    m_namesToTexture.clear();

    return true;
}

bool ShaderProgram::IsLinked() const
{
    return m_isLinked;
}

GL::BindTarget ShaderProgram::GetGLBindTarget() const
{
    return GL::BindTarget::ShaderProgram;
}

template <template <class T> class Container, class T>
bool SetShaderUniformArray(ShaderProgram *sp,
                           const String &name,
                           const Container<T> &v,
                           bool warn)
{
    ASSERT(GL::IsBound(sp));

    bool update = true;
    if (update)
    {
        int location = sp->GetUniformLocation(name);
        if (location >= 0) { GL::Uniform(location, v); }
        else if (warn) { Debug_Warn("Uniform '" << name << "' not found"); }
        return (location >= 0);
    }
    return true;
}

template<class T, class=TT_NOT_POINTER(T)>
bool SetShaderUniform(ShaderProgram *sp,
                      std::unordered_map<String, T> *cache,
                      const String &name,
                      const T &v,
                      bool warn)
{
    ASSERT(GL::IsBound(sp));

    bool update = true;
    if (cache)
    {
        const auto &it = cache->find(name);
        if (it != cache->end() && it->second == v) { update = false; }
    }

    if (update)
    {
        if (cache) { (*cache)[name] = v; }

        int location = sp->GetUniformLocation(name);
        if (location >= 0) { GL::Uniform(location, v); }
        else if (warn) { Debug_Warn("Uniform '" << name << "' not found"); }
        return (location >= 0);
    }
    return true;
}

bool ShaderProgram::SetInt(const String &name, int v, bool warn)
{
    return SetShaderUniform<int>(this, &m_uniformCacheInt, name, v, warn);
}
bool ShaderProgram::SetBool(const String &name, bool v, bool warn)
{
    return SetShaderUniform(this, &m_uniformCacheBool, name, v, warn);
}
bool ShaderProgram::SetFloat(const String &name, float v, bool warn)
{
    return SetShaderUniform(this, &m_uniformCacheFloat, name, v, warn);
}
bool ShaderProgram::SetColor(const String &name, const Color &v, bool warn)
{
    return SetShaderUniform(this, &m_uniformCacheColor, name, v, warn);
}
bool ShaderProgram::SetVector2(const String &name, const Vector2 &v, bool warn)
{
    return SetShaderUniform(this, &m_uniformCacheVector2, name, v, warn);
}
bool ShaderProgram::SetVector3(const String &name, const Vector3 &v, bool warn)
{
    return SetShaderUniform(this, &m_uniformCacheVector3, name, v, warn);
}
bool ShaderProgram::SetVector4(const String &name, const Vector4 &v, bool warn)
{
    return SetShaderUniform(this, &m_uniformCacheVector4, name, v, warn);
}
bool ShaderProgram::SetMatrix3(const String &name, const Matrix3 &v, bool warn)
{
    // return SetShaderUniform(this, &m_uniformCacheMatrix3, name, v, warn);
    return SetShaderUniform<Matrix3>(this, nullptr, name, v, warn);
}
bool ShaderProgram::SetMatrix4(const String &name, const Matrix4 &v, bool warn)
{
    // return SetShaderUniform(this, &m_uniformCacheMatrix4, name, v, warn);
    return SetShaderUniform<Matrix4>(this, nullptr, name, v, warn);
}
bool ShaderProgram::SetIntArray(const String &name, const Array<int> &v, bool warn)
{
    return SetShaderUniformArray(this, name, v, warn);
}
bool ShaderProgram::SetBoolArray(const String &name, const Array<bool> &v, bool warn)
{
    return SetShaderUniformArray(this, name, v, warn);
}
bool ShaderProgram::SetFloatArray(const String &name, const Array<float> &v, bool warn)
{
    return SetShaderUniformArray(this, name, v, warn);
}
bool ShaderProgram::SetColorArray(const String &name, const Array<Color> &v, bool warn)
{
    return SetShaderUniformArray(this, name, v, warn);
}
bool ShaderProgram::SetVector2Array(const String &name, const Array<Vector2> &v, bool warn)
{
    return SetShaderUniformArray(this, name, v, warn);
}
bool ShaderProgram::SetVector3Array(const String &name, const Array<Vector3> &v, bool warn)
{
    return SetShaderUniformArray(this, name, v, warn);
}
bool ShaderProgram::SetVector4Array(const String &name, const Array<Vector4> &v, bool warn)
{
    return SetShaderUniformArray(this, name, v, warn);
}
bool ShaderProgram::SetMatrix3Array(const String &name, const Array<Matrix3> &v, bool warn)
{
    return SetShaderUniformArray(this, name, v, warn);
}
bool ShaderProgram::SetMatrix4Array(const String &name, const Array<Matrix4> &v, bool warn)
{
    return SetShaderUniformArray(this, name, v, warn);
}
bool ShaderProgram::SetTexture(const String &name, Texture *texture, bool warn)
{
    if (texture)
    {
        if (GetUniformLocation(name) < 0)
        {
            if (warn)
            {
                Debug_Warn("Texture uniform '" << name << "' not found.");
            }
            return false;
        }

        // Texture name is in shader program, and texture is not null
        bool needToRefreshTexture;
        auto it = m_namesToTexture.find(name);
        if (it != m_namesToTexture.end())
        {
            // Texture name was already being used...
            Texture *oldTexture = it->second;
            if (texture != oldTexture)
            {
                // We are changing an existing texture. Unregister listener
                oldTexture->EventEmitter<IDestroyListener>::UnRegisterListener(this);
                needToRefreshTexture = true;
            }
            else
            {
                // Nothing to do. We already had this tex assigned to this name
                needToRefreshTexture = false;
            }
        }
        else { needToRefreshTexture = true; }

        if (needToRefreshTexture)
        {
            // Texture name was not being used. Register and all stuff
            m_namesToTexture[name] = texture;

            // Register listener to keep track when it is destroyed
            texture->EventEmitter<IDestroyListener>::RegisterListener(this);
        }

        if (GL::IsBound(this)) { BindTextureToFreeUnit(name, texture); }
    }
    else
    {
        // Texture is null or shader program is not using this name.
        // Erase it from name to texture
        // m_namesToTexture.erase(name);
    }

    if (m_namesToTexture.size() >= TextureUnitManager::GetMaxTextureUnits())
    {
        Debug_Error("You are using too many textures at once. Maximum is: " <<
                    TextureUnitManager::GetMaxTextureUnits());
    }

    return true;
}
bool ShaderProgram::SetTexture2D(const String &name,
                                 Texture2D *texture,
                                 bool warn)
{
    return SetTexture(name, SCAST<Texture*>(texture), warn);
}
bool ShaderProgram::SetTextureCubeMap(const String &name,
                                      TextureCubeMap *textureCubeMap,
                                      bool warn)
{
    return SetTexture(name, SCAST<Texture*>(textureCubeMap), warn);
}

bool ShaderProgram::SetShader(Shader *shader, GL::ShaderType type)
{
    if (shader && shader->GetType() != type)
    {
        String typeName = (type == GL::ShaderType::Vertex    ? "Vertex" :
                          (type == GL::ShaderType::Geometry ? "Geometry" :
                                                               "Fragment"));
        Debug_Error("You are trying to set as " << typeName << " shader a "
                    "non-" << typeName << " shader.");
        return false;
    }

    if (GetShader(type))
    {
        GetShader(type)->EventEmitter<IResourceListener>::UnRegisterListener(this);
    }

    switch (type)
    {
        case GL::ShaderType::Vertex:   p_vShader.Set(shader); break;
        case GL::ShaderType::Geometry: p_gShader.Set(shader); break;
        case GL::ShaderType::Fragment: p_fShader.Set(shader); break;
    }

    if (GetShader(type))
    {
        GetShader(type)->EventEmitter<IResourceListener>::RegisterListener(this);
    }

    return true;
}

bool ShaderProgram::SetVertexShader(Shader* vertexShader)
{
    return SetShader(vertexShader, GL::ShaderType::Vertex);
}

bool ShaderProgram::SetGeometryShader(Shader *geometryShader)
{
    return SetShader(geometryShader, GL::ShaderType::Geometry);
}

bool ShaderProgram::SetFragmentShader(Shader* fragmentShader)
{
    return SetShader(fragmentShader, GL::ShaderType::Fragment);
}

Shader *ShaderProgram::GetShader(GL::ShaderType type) const
{
    switch (type)
    {
        case GL::ShaderType::Vertex:   return p_vShader.Get();
        case GL::ShaderType::Geometry: return p_gShader.Get();
        case GL::ShaderType::Fragment: return p_fShader.Get();
    }

    ASSERT(false);
    return nullptr;
}
Shader* ShaderProgram::GetVertexShader()   const { return p_vShader.Get(); }
Shader *ShaderProgram::GetGeometryShader() const { return p_gShader.Get(); }
Shader* ShaderProgram::GetFragmentShader() const { return p_fShader.Get(); }

GLint ShaderProgram::GetUniformLocation(const String &name) const
{
    auto it = m_nameToLocationCache.find(name);
    if (it != m_nameToLocationCache.end()) { return it->second; }

    const int location = GL::GetUniformLocation(m_idGL, name);
    m_nameToLocationCache[name] = location;
    return location;
}

void ShaderProgram::Import(const Path &)
{
    // Nothing to import from filepath
}

void ShaderProgram::Bind() const
{
    #ifdef DEBUG
    if (!IsLinked())
    {
        Debug_Warn("Binding a non-linked shader!");
        return;
    }
    #endif

    // Debug_Log("Binding " << this << " =========================================");
    GL::Bind(this);
    ShaderProgram* noConstThis = const_cast<ShaderProgram*>(this);
    GLUniforms::SetAllUniformsToShaderProgram(noConstThis);
    noConstThis->BindAllTexturesToUnits();
}

void ShaderProgram::UnBind() const
{
    GL::UnBind(this);
}

void ShaderProgram::BindAllTexturesToUnits()
{
    ASSERT(GL::IsBound(this));

    for (const auto &pair : m_namesToTexture)
    {
        const String  &texName = pair.first;
        Texture *texture = pair.second;
        BindTextureToFreeUnit(texName, texture);
    }
}

void ShaderProgram::BindTextureToFreeUnit(const String &textureName,
                                          Texture *texture)
{
    uint unit = TextureUnitManager::BindTextureToUnit(texture);
    SetInt(textureName, unit, false); // Assign unit to sampler
    // Debug_Log("Binding " << textureName << " to " << unit);
}

void ShaderProgram::UnBindAllTexturesFromUnits() const
{
    TextureUnitManager::UnBindAllTexturesFromAllUnits();
}

void ShaderProgram::OnDestroyed(EventEmitter<IDestroyListener> *object)
{
    Array< std::pair<String, Texture*> > entriesToSetToDefaultTex;
    Texture *destroyedTex = DCAST<Texture*>( object );
    for (auto it = m_namesToTexture.begin(); it != m_namesToTexture.end(); )
    {
        Texture *tex = it->second;
        if (tex == destroyedTex)
        {
            const String &name = it->first;
            entriesToSetToDefaultTex.PushBack(std::make_pair(name, tex));

            it = m_namesToTexture.erase(it);
            // Dont break, in case it has obj texture several times
        }
        else { ++it; }
    }

    // Set default textures to those removed entries
    for (const auto &pair : entriesToSetToDefaultTex)
    {
        const String &name = pair.first;
        Texture *tex = pair.second;
        if (DCAST<Texture2D*>(tex))
        {
            SetTexture2D(name, IconManager::GetWhiteTexture().Get(), false);
        }
        else if (DCAST<TextureCubeMap*>(tex))
        {
            SetTextureCubeMap(name, IconManager::GetWhiteTextureCubeMap().Get(),
                              false);
        }
    }
}

void ShaderProgram::OnImported(Resource *res)
{
    // When used shader is imported, link shaderProgram
    ASSERT(res == GetVertexShader()   ||
           res == GetGeometryShader() ||
           res == GetFragmentShader());
    Link();
}
