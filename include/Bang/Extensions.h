#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

class Extensions
{
public:
    static String GetMeshExtension();
    static String GetSceneExtension();
    static String GetPrefabExtension();
    static String GetProjectExtension();
    static String GetMaterialExtension();
    static String GetAnimationExtension();
    static String GetShaderProgramExtension();
    static String GetTextureCubeMapExtension();
    static String GetPhysicsMaterialExtension();
    static String GetAnimatorStateMachineExtension();

    static Array<String> GetTTFExtensions();
    static Array<String> GetImageExtensions();
    static Array<String> GetModelExtensions();
    static Array<String> GetShaderExtensions();
    static Array<String> GetAudioClipExtensions();
    static Array<String> GetTextFileExtensions();
    static Array<String> GetBehaviourExtensions();
    static Array<String> GetSourceFileExtensions();
    static Array<String> GetVertexShaderExtensions();
    static Array<String> GetFragmentShaderExtensions();
    static Array<String> GetSharedLibraryFileExtensions();

    static String GetDefaultBehaviourHeaderExtension();
    static String GetDefaultBehaviourSourceExtension();

    static bool Equals(const String &extension, const Array<String> &extensions);
    static bool Equals(const String &extensionSrc, const String &extensionDst);

    Extensions() = delete;
};

NAMESPACE_BANG_END

#endif // EXTENSIONS_H
