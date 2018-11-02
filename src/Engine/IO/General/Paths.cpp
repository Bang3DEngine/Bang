#include "Bang/Paths.h"

#include <limits.h>

#ifdef __linux__
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
constexpr char Separator[] = "/";
constexpr char SeparatorC = '/';
#elif _WIN32
#include <Windows.h>
#include <stdlib.h>
#include "Bang/WinUndef.h"
constexpr char Separator[] = "\\";
constexpr char SeparatorC = '\\';
#endif

#include "Bang/Application.h"
#include "Bang/Array.tcc"
#include "Bang/Containers.h"
#include "Bang/Debug.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

using namespace Bang;

Paths::Paths()
{
}

Paths::~Paths()
{
}

void Paths::InitPaths(const Path &engineRootPath)
{
    m_engineRoot = Path::Empty;
    if (!engineRootPath.IsEmpty())
    {
        Paths::SetEngineRoot(engineRootPath);
    }

    m_engineAssetsDir = Paths::GetEngineDir().Append("Assets");
    if (GetEngineAssetsDir().IsDir())
    {
        Debug_Log("Picking as Paths Bang Root: '" << GetEngineDir() << "'");
    }
    else
    {
        Debug_Error("Could not find the Bang root directory! Current set to: '"
                    << GetEngineDir());
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
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    String exePath(std::string(result, (count > 0) ? count : 0));
    return Path(exePath);

#elif _WIN32

    HMODULE hModule = GetModuleHandleW(NULL);
    WCHAR path[_MAX_PATH];
    GetModuleFileNameW(hModule, path, _MAX_PATH);

    std::wstring wsPath(path);
    std::string sPath(wsPath.begin(), wsPath.end());

    return Path(String(sPath));

#endif
}

Path Paths::GetEngineIncludeDir()
{
    return Paths::GetEngineDir().Append("include");
}

const Path &Paths::GetEngineDir()
{
    return Paths::GetInstance()->m_engineRoot;
}
const Path &Paths::GetEngineAssetsDir()
{
    return Paths::GetInstance()->m_engineAssetsDir;
}

Path Paths::GetEngineBuildDir()
{
    return Paths::GetEngineDir().Append("Build");
}

Path Paths::GetEngineLibrariesDir()
{
    return GetEngineDir().Append("Libraries").Append(Paths::GetBuildType());
}

const String &Paths::GetBuildType()
{
    static const String BuildType = BANG_BUILD_TYPE;
    return BuildType;
}

Path Paths::CreateEnginePath(const String &path)
{
    return Paths::GetEngineAssetsDir().Append(path);
}

Array<Path> Paths::GetAllProjectSubDirs()
{
    Array<Path> subdirs =
        Paths::GetProjectDir().GetSubDirectories(FindFlag::RECURSIVE);
    subdirs.PushFront(Paths::GetProjectDir());
    return subdirs;
}

Array<Path> Paths::GetProjectIncludeDirs()
{
    Array<Path> incDirs =
        Paths::GetProjectAssetsDir().GetSubDirectories(FindFlag::RECURSIVE);
    incDirs.PushBack(Paths::GetProjectAssetsDir());
    return incDirs;
}

const Path &Paths::GetProjectDir()
{
    return Paths::GetInstance()->m_projectRoot;
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
    Paths::GetInstance()->m_projectRoot = projectRootDir;
}

Array<Path> Paths::GetEngineIncludeDirs()
{
    Array<Path> incPaths;
    incPaths.PushBack(Paths::GetEngineIncludeDir());
    incPaths.PushBack(Path("/usr/include"));
    incPaths.PushBack(Path("/usr/include/SDL2"));

    Path physxRootDir = Paths::GetEngineDir().Append(
        "Compile/CompileDependencies/ThirdParty/PhysX/");
    Array<Path> physxDirs = physxRootDir.GetSubDirectories(FindFlag::RECURSIVE);
    for (const Path &physxDir : physxDirs)
    {
        incPaths.PushBack(physxDir);
    }
    return incPaths;
}

bool Paths::IsEnginePath(const Path &path)
{
    return path.BeginsWith(String(Paths::GetEngineDir()) + String(Separator));
}

void Paths::SetEngineRoot(const Path &engineRootDir)
{
    Paths::GetInstance()->m_engineRoot = engineRootDir;
}

Path Paths::GetResolvedPath(const Path &path_)
{
    Path path = path_;
    if (!path.IsAbsolute())
    {
        path = Paths::GetExecutableDir().Append(path);
    }

    Array<String> pathParts = path.GetAbsolute().Split<Array>(SeparatorC);
    pathParts.RemoveAll(".");

    int skipNext = 0;
    Array<String> resolvedPathParts;
    for (auto it = pathParts.RBegin(); it != pathParts.REnd(); ++it)
    {
        const String &pathPart = *it;
        if (pathPart == "..")
        {
            ++skipNext;
        }
        else
        {
            if (skipNext > 0)
            {
                --skipNext;
            }
            else
            {
                resolvedPathParts.PushFront(pathPart);
            }
        }
    }
    Path resolvedPath = Path(String::Join(resolvedPathParts, Separator));
    return resolvedPath;
}

void Paths::SortPathsByName(Array<Path> *paths, bool caseSensitive)
{
    if (!paths->IsEmpty())
    {
        Array<Path> pathsArr;
        pathsArr.PushBack(paths->Begin(), paths->End());
        Containers::StableSort(
            paths->Begin(),
            paths->End(),
            [caseSensitive](const Path &lhs, const Path &rhs) {
                return caseSensitive ? lhs.GetNameExt() < rhs.GetNameExt()
                                     : (lhs.GetNameExt().ToUpper() <
                                        rhs.GetNameExt().ToUpper());
            });
    }
}

void Paths::SortPathsByExtension(Array<Path> *paths)
{
    if (!paths->IsEmpty())
    {
        Containers::StableSort(
            paths->Begin(), paths->End(), [](const Path &lhs, const Path &rhs) {
                return lhs.GetExtension().ToUpper() <
                       rhs.GetExtension().ToUpper();
            });
    }
}

void Paths::FilterByExtension(Array<Path> *paths,
                              const Array<String> &extensions)
{
    for (auto it = paths->Begin(); it != paths->End();)
    {
        const Path &p = *it;
        if (p.IsFile() && !p.HasExtension(extensions))
        {
            it = paths->Remove(it);
        }
        else
        {
            ++it;
        }
    }
}

void Paths::RemoveFilesFromArray(Array<Path> *paths)
{
    for (auto it = paths->Begin(); it != paths->End();)
    {
        const Path &p = *it;
        if (p.IsFile())
        {
            it = paths->Remove(it);
        }
        else
        {
            ++it;
        }
    }
}

void Paths::RemoveDirectoriesFromArray(Array<Path> *paths)
{
    for (auto it = paths->Begin(); it != paths->End();)
    {
        const Path &p = *it;
        if (p.IsDir())
        {
            it = paths->Remove(it);
        }
        else
        {
            ++it;
        }
    }
}

Paths *Paths::GetInstance()
{
    return Application::GetInstance()->GetPaths();
}
