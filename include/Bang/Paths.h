#ifndef PATHS_H
#define PATHS_H

#include "Bang/Path.h"
#include "Bang/BinType.h"

NAMESPACE_BANG_BEGIN

#define EPATH(path)  Paths::CreateEnginePath(path)  // Engine assets path
#define PPATH(path)  Paths::CreateProjectPath(path) // Project assets path

class Paths
{
public:
    void InitPaths(const Path &engineRootPath);

    static Path GetHome();
    static Path GetEngineSrcDir();
    static Path GetExecutableDir();
    static Path GetExecutablePath();
    static Path GetEngineIncludeDir();
    static const Path& GetEngineDir();
    static const Path& GetEngineAssetsDir();
    static Path GetEngineBuildDir();
    static Path GetEngineResourcesDir();
    static Path GetEngineLibrariesDir(BinType binaryType);
    static bool IsEnginePath(const Path &path);

    static Path CreateEnginePath(const String &path);
    static Path CreateProjectPath(const String &path);

    static List<Path> GetEngineIncludeDirs();
    static List<Path> GetAllProjectSubDirs();
    static List<Path> GetProjectIncludeDirs();

    static const Path& GetProjectDir();
    static Path GetProjectAssetsDir();
    static Path GetProjectLibrariesDir();

    static void SetEngineRoot(const Path &engineRootDir);

    static Path GetResolvedPath(const Path &path);
    static void SortPathsByName(List<Path> *paths, bool caseSensitive = false);
    static void SortPathsByExtension(List<Path> *paths);
    static void FilterByExtension(List<Path> *paths,
                                  const Array<String>& extensions);
    static void RemoveFilesFromList(List<Path> *paths);
    static void RemoveDirectoriesFromList(List<Path> *paths);

    static void SetProjectRoot(const Path &projectRootDir);

protected:
    Paths();
    virtual ~Paths();

    static Paths* GetInstance();

private:
    Path m_engineRoot  = Path::Empty;
    Path m_projectRoot = Path::Empty;

    // Cached paths (the ones more used), to avoid Path creation every time
    Path m_engineAssetsDir = Path::Empty;

    friend class Application;
};

NAMESPACE_BANG_END

#endif // PATHS_H
