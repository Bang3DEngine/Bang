#include "Bang/Extensions.h"

#include "Bang/Animation.h"
#include "Bang/AnimatorLayerMask.h"
#include "Bang/AnimatorStateMachine.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/AudioClip.h"
#include "Bang/Behaviour.h"
#include "Bang/Font.h"
#include "Bang/Material.h"
#include "Bang/Model.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/Prefab.h"
#include "Bang/Scene.h"
#include "Bang/Shader.h"
#include "Bang/ShaderProgram.h"
#include "Bang/String.h"
#include "Bang/Texture2D.h"
#include "Bang/Texture3D.h"
#include "Bang/TextureCubeMap.h"

namespace Bang
{
Array<String> Extensions::GetExtension(const String &className)
{
    if (className == Model::GetClassNameStatic())
    {
        return GetModelExtensions();
    }
    else if (className == Mesh::GetClassNameStatic())
    {
        return {GetMeshExtension()};
    }
    else if (className == Animation::GetClassNameStatic())
    {
        return {GetAnimationExtension()};
    }
    else if (className == Shader::GetClassNameStatic())
    {
        return {GetShaderExtensions()};
    }
    else if (className == ShaderProgram::GetClassNameStatic())
    {
        return {GetShaderProgramExtension()};
    }
    else if (className == Texture2D::GetClassNameStatic())
    {
        return {GetImageExtensions()};
    }
    else if (className == TextureCubeMap::GetClassNameStatic())
    {
        return {GetTextureCubeMapExtension()};
    }
    else if (className == PhysicsMaterial::GetClassNameStatic())
    {
        return {GetPhysicsMaterialExtension()};
    }
    else if (className == Material::GetClassNameStatic())
    {
        return {GetMaterialExtension()};
    }
    else if (className == AudioClip::GetClassNameStatic())
    {
        return {GetTextureCubeMapExtension()};
    }
    else if (className == Font::GetClassNameStatic())
    {
        return {GetFontExtensions()};
    }
    else if (className == Prefab::GetClassNameStatic())
    {
        return {GetPrefabExtension()};
    }
    else if (className == Scene::GetClassNameStatic())
    {
        return {GetSceneExtension()};
    }
    else if (className == AnimatorLayerMask::GetClassNameStatic())
    {
        return {GetAnimatorLayerMaskExtension()};
    }
    else if (className == AnimatorStateMachine::GetClassNameStatic())
    {
        return {GetAnimatorStateMachineExtension()};
    }
    else if (className == Behaviour::GetClassNameStatic())
    {
        return {GetBehaviourExtensions()};
    }
    return {};
}

String Extensions::GetObjExtension()
{
#ifdef __linux__
    return "o";
#else
    return "obj";
#endif
}

String Extensions::GetStaticLibExtension()
{
#ifdef __linux__
    return "a";
#else
    return "lib";
#endif
}

String Extensions::GetDynamicLibExtension()
{
#ifdef __linux__
    return "so";
#else
    return "dll";
#endif
}

String Extensions::GetMeshExtension()
{
    return "bmesh";
}
String Extensions::GetSceneExtension()
{
    return "bscene";
}
String Extensions::GetPrefabExtension()
{
    return "bprefab";
}
String Extensions::GetProjectExtension()
{
    return "bproject";
}
String Extensions::GetMaterialExtension()
{
    return "bmat";
}
String Extensions::GetPhysicsMaterialExtension()
{
    return "bphmat";
}

String Extensions::GetAnimatorLayerMaskExtension()
{
    return "banimlm";
}
String Extensions::GetAnimatorStateMachineExtension()
{
    return "banimsm";
}
String Extensions::GetAnimationExtension()
{
    return "banim";
}

String Extensions::GetShaderProgramExtension()
{
    return "bshaderprogram";
}

String Extensions::GetUnifiedShaderExtension()
{
    return "bushader";
}

String Extensions::GetTextureCubeMapExtension()
{
    return "texcm";
}

Array<String> Extensions::GetFontExtensions()
{
    return {"ttf"};
}

Array<String> Extensions::GetImageExtensions()
{
    return {"png", "jpg", "jpeg", "tga", "bmp", "tiff", "dds"};
}

Array<String> Extensions::GetModelExtensions()
{
    return {"obj",     "ply", "fbx", "dae",  "3ds",  "gltf", "glb", "blend",
            "3ds",     "ase", "ifc", "xgl",  "zgl",  "dxf",  "lwo", "lws",
            "lxo",     "stl", "x",   "ac",   "ms3d", "cob",  "scn", "bvh",
            "csm",     "mdl", "md2", "md3",  "pk3",  "mdc",  "md5", "md5mesh",
            "md5anim", "smd", "vta", "ogex", "3d",   "b3d",  "q3d", "q3s",
            "nff",     "off", "raw", "ter",  "mdl",  "hmp",  "ndo"};
}

Array<String> Extensions::GetShaderExtensions()
{
    Array<String> extensions;
    extensions.PushBack(GetVertexShaderExtensions());
    extensions.PushBack(GetGeometryShaderExtensions());
    extensions.PushBack(GetFragmentShaderExtensions());
    extensions.PushBack(GetUnifiedShaderExtension());
    extensions.PushBack("glsl");
    return extensions;
}
Array<String> Extensions::GetVertexShaderExtensions()
{
    return {"vert"};
}

Array<String> Extensions::GetGeometryShaderExtensions()
{
    return {"geom"};
}
Array<String> Extensions::GetFragmentShaderExtensions()
{
    return {"frag"};
}

Array<String> Extensions::GetAudioClipExtensions()
{
    return {"ogg", "wav"};
}

Array<String> Extensions::GetTextFileExtensions()
{
    return {"txt"};
}

Array<String> Extensions::GetBehaviourExtensions()
{
    return {"c", "cpp", "h", "hpp"};
}

Array<String> Extensions::GetSourceFileExtensions()
{
    return {"c", "cpp"};
}

Array<String> Extensions::GetSharedLibraryFileExtensions()
{
    return {"so", "dll"};
}

String Extensions::GetDefaultBehaviourHeaderExtension()
{
    return "h";
}

String Extensions::GetDefaultBehaviourSourceExtension()
{
    return "cpp";
}

bool Extensions::Equals(const String &fullExtension,
                        const Array<String> &otherExtensions)
{
    Array<String> extensions = fullExtension.Split<Array>('.');
    for (const String &otherExtension : otherExtensions)
    {
        for (const String &extension : extensions)
        {
            if (extension.EqualsNoCase(otherExtension))
            {
                return true;
            }
        }
    }
    return false;
}

bool Extensions::Equals(const String &extensionSrc, const String &extensionDst)
{
    Array<String> extensionsDst = {{extensionDst}};
    return Extensions::Equals(extensionSrc, extensionsDst);
}
}  // namespace Bang
