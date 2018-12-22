#ifndef PATHS_H
#define PATHS_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/Path.h"
#include "Bang/String.h"

namespace Bang
{
#define EPATH(path) Paths::CreateEnginePath(path)   // Engine assets path
#define PPATH(path) Paths::CreateProjectPath(path)  // Project assets path

class Paths
{
public:
    void InitPaths(const Path &engineRootPath);
    void InitPathsAfterInitingSettings();

    static Path GetHome();
    static Path GetEngineSrcDir();
    static Path GetExecutableDir();
    static Path GetExecutablePath();
    static Path GetEngineIncludeDir();
    static const Path &GetEngineDir();
    static const Path &GetEngineAssetsDir();
    static Path GetEngineBuildDir();
    static Path GetEngineLibrariesDir();
    static const String &GetBuildType();
    static bool IsEnginePath(const Path &path);

    static Path CreateEnginePath(const String &path);
    static Path CreateProjectPath(const String &path);

    static Path GetCompilerPath();
    static Path GetLinkerPath();
    static Path GetMSVCConfigureArchitectureBatPath();
    static Array<Path> GetEngineIncludeDirs();
    static Array<Path> GetAllProjectSubDirs();
    static Array<Path> GetProjectIncludeDirs();
    static const Array<Path> &GetCompilerIncludePaths();

    static const Path &GetProjectDir();
    static Path GetProjectAssetsDir();
    static Path GetProjectLibrariesDir();

    static void SetEngineRoot(const Path &engineRootDir);

    static Path GetResolvedPath(const Path &path);
    static void SortPathsByName(Array<Path> *paths, bool caseSensitive = false);
    static void SortPathsByExtension(Array<Path> *paths);
    static void FilterByExtension(Array<Path> *paths,
                                  const Array<String> &extensions);
    static void RemoveFilesFromArray(Array<Path> *paths);
    static void RemoveDirectoriesFromArray(Array<Path> *paths);

    static void SetProjectRoot(const Path &projectRootDir);
    static void SetCompilerPath(const Path &compilerPath);
    static void SetLinkerPath(const Path &linkerPath);
    static void SetMSVCConfigureArchitecturePath(const Path &msvcConfArchPath);
    static void AddCompilerIncludePath(const Path &compilerIncludePath);

protected:
    Paths();
    virtual ~Paths();

    static Paths *GetInstance();

private:
    Path m_engineRootPath = Path::Empty();
    Path m_projectRootPath = Path::Empty();
    Path m_engineAssetsDir = Path::Empty();

    Array<Path> m_compilerIncludePaths;
    Path m_compilerPath = Path::Empty();
    Path m_linkerPath = Path::Empty();
    Path m_msvcConfigureArchitecturePath = Path::Empty();

    void FindCompilerPaths(Path *compilerPath,
                           Path *linkerPath,
                           Path *msvcConfigureArchitectureBatPath,
                           Array<Path> *includePaths);

    friend class Application;
    friend class Settings;
};
}  // namespace Bang

#endif  // PATHS_H
