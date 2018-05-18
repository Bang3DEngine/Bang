#include "Bang/Paths.h"

#include <limits.h>

#ifdef __linux__
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#elif _WIN32
#endif

#include "Bang/Debug.h"
#include "Bang/String.h"
#include "Bang/Application.h"
#include "Bang/SystemUtils.h"

USING_NAMESPACE_BANG

Paths::Paths()
{
}

Paths::~Paths()
{

}

void Paths::InitPaths(const Path &engineRootPath)
{
    c_engineRoot = Path::Empty;
    if (!engineRootPath.IsEmpty()) { Paths::SetEngineRoot(engineRootPath); }

    if (GetEngineAssetsDir().IsDir())
    {
        Debug_Log("Picking as Paths Bang Root: '" << GetEngineDir() << "'");
    }
    else
    {
        Debug_Error("Could not find the Bang root directory! Current set to: '" <<
                    GetEngineDir());
        Application::Exit(1, true);
    }
}

Path Paths::GetHome()
{
    Path homePath;

    #ifdef __linux__
    
    struct passwd *pw = getpwuid(getuid());
    homePath = Path(pw->pw_dir);
    
    #elif _WIN32

    #endif

    return homePath;
}

Path Paths::GetEngineSrcDir()
{
    return Paths::GetEngineDir().Append("src");
}

Path Paths::GetExecutableDir()
{
    return Paths::GetExecutablePath().GetDirectory();
}

Path Paths::GetExecutablePath()
{
    #ifdef __linux__
    
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX );
    String exePath( std::string(result, (count > 0) ? count : 0) );
    return Path(exePath);

    #elif _WIN32

    return Path::Empty;

    #endif
}

Path Paths::GetEngineIncludeDir()
{
    return Paths::GetEngineDir().Append("include");
}

const Path &Paths::GetEngineDir()
{
    return Paths::GetInstance()->c_engineRoot;
}
Path Paths::GetEngineAssetsDir()
{
    return Paths::GetEngineDir().Append("Assets");
}

Path Paths::GetEngineBuildDir()
{
    return Paths::GetEngineDir().Append("Build");
}

Path Paths::GetEngineLibrariesDir(BinType binaryType)
{
    return GetEngineDir().
           Append("bin").
           Append(binaryType == BinType::Debug ? "Debug" : "Release").
           Append("lib");
}

Path Paths::CreateEnginePath(const String &path)
{
    return Paths::GetEngineAssetsDir().Append(path);
}


List<Path> Paths::GetAllProjectSubDirs()
{
    List<Path> subdirs = Paths::GetProjectDir()
                         .GetSubDirectories(Path::FindFlag::Recursive);
    subdirs.PushFront(Paths::GetProjectDir());
    return subdirs;
}

List<Path> Paths::GetProjectIncludeDirs()
{
    List<Path> incDirs = Paths::GetProjectAssetsDir()
                        .GetSubDirectories(Path::FindFlag::Recursive);
    incDirs.PushBack( Paths::GetProjectAssetsDir() );
    return incDirs;
}

const Path &Paths::GetProjectDir()
{
    return Paths::GetInstance()->c_projectRoot;
}

Path Paths::GetProjectAssetsDir()
{
    return GetProjectDir().Append("Assets");
}

Path Paths::GetProjectLibrariesDir()
{
    return GetProjectDir().Append("Libraries");
}

Path Paths::CreateProjectPath(const String &path)
{
    return Paths::GetProjectAssetsDir().Append(path);
}

void Paths::SetProjectRoot(const Path &projectRootDir)
{
    Paths::GetInstance()->c_projectRoot = projectRootDir;
}

List<Path> Paths::GetEngineIncludeDirs()
{
    List<Path> incPaths;
    incPaths.PushBack( Paths::GetEngineIncludeDir() );
    return incPaths;
}

bool Paths::IsEnginePath(const Path &path)
{
    return path.BeginsWith( String(Paths::GetEngineDir()) + String("/") );
}

void Paths::SetEngineRoot(const Path &engineRootDir)
{
    Paths::GetInstance()->c_engineRoot = engineRootDir;
}

Path Paths::GetResolvedPath(const Path &path_)
{
    Path path = path_;
    if (!path.IsAbsolute()) { path = Paths::GetExecutableDir().Append(path); }

    List<String> pathParts = path.GetAbsolute().Split<List>('/');
    pathParts.RemoveAll(".");

    int skipNext = 0;
    List<String> resolvedPathParts;
    for (auto it = pathParts.RBegin(); it != pathParts.REnd(); ++it)
    {
        const String &pathPart = *it;
        if (skipNext > 0)  { --skipNext;  continue; }

        if (pathPart == "..") { ++skipNext; }
        else { resolvedPathParts.PushFront(pathPart); }
    }
    Path resolvedPath = Path( String::Join(resolvedPathParts, "/") );
    return resolvedPath;
}

void Paths::SortPathsByName(List<Path> *paths)
{
    Array<Path> pathsArr;
    pathsArr.PushBack(paths->Begin(), paths->End());
    std::stable_sort(
        pathsArr.Begin(), pathsArr.End(),
        [](const Path &lhs, const Path &rhs)
        {
            return lhs.GetNameExt() < rhs.GetNameExt();
        }
    );

    paths->Clear();
    paths->Insert(paths->End(), pathsArr.Begin(), pathsArr.End());
}

void Paths::SortPathsByExtension(List<Path> *paths)
{
    Array<Path> pathsArr;
    pathsArr.PushBack(paths->Begin(), paths->End());
    std::stable_sort(
        pathsArr.Begin(), pathsArr.End(),
        [](const Path &lhs, const Path &rhs)
        {
            return lhs.GetExtension() < rhs.GetExtension();
        }
    );

    paths->Clear();
    paths->Insert(paths->End(), pathsArr.Begin(), pathsArr.End());
}

void Paths::FilterByExtension(List<Path> *paths, const Array<String>& extensions)
{
    for (auto it = paths->Begin(); it != paths->End(); )
    {
        const Path &p = *it;
        if ( p.IsFile() && !p.HasExtension(extensions) )
        {
            it = paths->Remove(it);
        }
        else { ++it; }
    }
}

void Paths::RemoveFilesFromList(List<Path> *paths)
{
    for (auto it = paths->Begin(); it != paths->End(); )
    {
        const Path &p = *it;
        if (p.IsFile()) { it = paths->Remove(it); }
        else { ++it; }
    }
}

void Paths::RemoveDirectoriesFromList(List<Path> *paths)
{
    for (auto it = paths->Begin(); it != paths->End(); )
    {
        const Path &p = *it;
        if (p.IsDir()) { it = paths->Remove(it); }
        else { ++it; }
    }
}


Paths *Paths::GetInstance()
{
    return Application::GetInstance()->GetPaths();
}
