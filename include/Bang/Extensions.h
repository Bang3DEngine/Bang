#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/String.h"

namespace Bang
{
class Extensions
{
public:
    static Array<String> GetExtension(const String &className);

    static String GetObjExtension();
    static String GetStaticLibExtension();
    static String GetDynamicLibExtension();

    static String GetMeshExtension();
    static String GetSceneExtension();
    static String GetPrefabExtension();
    static String GetProjectExtension();
    static String GetMaterialExtension();
    static String GetAnimationExtension();
    static String GetShaderProgramExtension();
    static String GetUnifiedShaderExtension();
    static String GetTextureCubeMapExtension();
    static String GetPhysicsMaterialExtension();
    static String GetAnimatorLayerMaskExtension();
    static String GetAnimatorStateMachineExtension();

    static Array<String> GetFontExtensions();
    static Array<String> GetImageExtensions();
    static Array<String> GetModelExtensions();
    static Array<String> GetShaderExtensions();
    static Array<String> GetAudioClipExtensions();
    static Array<String> GetTextFileExtensions();
    static Array<String> GetBehaviourExtensions();
    static Array<String> GetSourceFileExtensions();
    static Array<String> GetVertexShaderExtensions();
    static Array<String> GetGeometryShaderExtensions();
    static Array<String> GetFragmentShaderExtensions();
    static Array<String> GetSharedLibraryFileExtensions();

    static String GetDefaultBehaviourHeaderExtension();
    static String GetDefaultBehaviourSourceExtension();

    static bool Equals(const String &extension,
                       const Array<String> &extensions);
    static bool Equals(const String &extensionSrc, const String &extensionDst);

    Extensions() = delete;
};
}  // namespace Bang

#endif  // EXTENSIONS_H
