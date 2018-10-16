#include "Bang/Extensions.h"

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

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
String Extensions::GetAnimatorStateMachineExtension()
{
    return "banimsm";
}
String Extensions::GetAnimationExtension()
{
    return "banim";
}
String Extensions::GetTextureCubeMapExtension()
{
    return "texcm";
}
String Extensions::GetShaderProgramExtension()
{
    return "bshaderprogram";
}

Array<String> Extensions::GetTTFExtensions()
{
    return {"ttf"};
}

Array<String> Extensions::GetImageExtensions()
{
    return {"png", "jpg", "jpeg", "tga", "bmp", "tiff", "dds"};
}

Array<String> Extensions::GetModelExtensions()
{
    return {"obj", "ply", "fbx", "dae", "3ds", "gltf", "glb",
            "blend", "3ds", "ase", "ifc", "xgl", "zgl", "dxf",
            "lwo", "lws", "lxo", "stl", "x", "ac", "ms3d", "cob",
            "scn", "bvh", "csm", "mdl", "md2", "md3", "pk3", "mdc",
            "md5", "md5mesh", "md5anim", "smd", "vta", "ogex", "3d", "b3d",
            "q3d", "q3s", "nff", "off", "raw", "ter", "mdl", "hmp", "ndo"};
}

Array<String> Extensions::GetShaderExtensions()
{
    return {"vert", "geom", "frag", "glsl"};
}
Array<String> Extensions::GetVertexShaderExtensions()
{
    return {"vert"};
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

NAMESPACE_BANG_END
