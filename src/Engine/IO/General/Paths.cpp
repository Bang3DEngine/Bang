#include "Bang/Paths.h"

#include <limits.h>

#ifdef __linux__
#include <linux/limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
constexpr char Separator[] = "/";
constexpr char SeparatorC = '/';
#elif _WIN32
#include <Windows.h>

#include <ShlObj.h>
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
    m_engineRootPath = Path::Empty();
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

void Paths::InitPathsAfterInitingSettings()
{
    bool mustFindCompilerPaths = false;
    mustFindCompilerPaths |= !m_compilerPath.IsFile();
    mustFindCompilerPaths |= !m_linkerPath.IsFile();

#ifdef _WIN32
    mustFindCompilerPaths |= !m_msvcConfigureArchitecturePath.IsFile();
#endif

    mustFindCompilerPaths |= m_compilerIncludePaths.IsEmpty();
    for (const Path &includePath : m_compilerIncludePaths)
    {
        mustFindCompilerPaths |= !includePath.Exists();
    }

    if (mustFindCompilerPaths)
    {
        FindCompilerPaths(&m_compilerPath,
                          &m_linkerPath,
                          &m_msvcConfigureArchitecturePath,
                          &m_compilerIncludePaths);
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

Path Paths::GetCompilerPath()
{
    return Paths::GetInstance()->m_compilerPath;
}

Path Paths::GetLinkerPath()
{
    return Paths::GetInstance()->m_linkerPath;
}

Path Paths::GetMSVCConfigureArchitectureBatPath()
{
    return Paths::GetInstance()->m_msvcConfigureArchitecturePath;
}

const Path &Paths::GetEngineDir()
{
    return Paths::GetInstance()->m_engineRootPath;
}
const Path &Paths::GetEngineAssetsDir()
{
    return Paths::GetInstance()->m_engineAssetsDir;
}

Path Paths::GetEngineBuildDir()
{
    return Paths::GetEngineDir().Append("Compile");
}

Path Paths::GetEngineLibrariesDir()
{
    return GetEngineDir().Append("Libraries").Append(Paths::GetBuildType());
}

const String &Paths::GetBuildType()
{
#ifdef DEBUG
    static const String BuildType = "Debug";
#else
    static const String BuildType = "Release";
#endif
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

const Array<Path> &Paths::GetCompilerIncludePaths()
{
    return Paths::GetInstance()->m_compilerIncludePaths;
}

const Path &Paths::GetProjectDir()
{
    return Paths::GetInstance()->m_projectRootPath;
}

Path Paths::GetProjectAssetsDir()
{
    return GetProjectDir().Append("Assets");
}

Path Paths::GetProjectLibrariesDir()
{
    return GetProjectDir().Append("Libraries");
}

void Paths::FindCompilerPaths(Path *compilerPath,
                              Path *linkerPath,
                              Path *msvcConfigureArchitectureBatPath,
                              Array<Path> *includePaths)
{
    // Include paths
    includePaths->Clear();

    Path tpd = Paths::GetEngineDir()
                   .Append("Compile")
                   .Append("CompileDependencies")
                   .Append("ThirdParty");
    {
        includePaths->PushBack(tpd.Append("yaml-cpp").Append("include"));
        includePaths->PushBack(tpd.Append("assimp").Append("include"));
        includePaths->PushBack(tpd.Append("assimp")
                                   .Append("build")
                                   .Append(GetBuildType())
                                   .Append("include"));
        includePaths->PushBack(tpd.Append("openal-soft").Append("include"));
        includePaths->PushBack(tpd.Append("glew-2.1.0").Append("include"));
        includePaths->PushBack(tpd.Append("SDL2-2.0.8").Append("include"));
        includePaths->PushBack(tpd.Append("SDL2_ttf-2.0.14"));
        includePaths->PushBack(tpd.Append("libpng-1.6.34"));
        includePaths->PushBack(tpd.Append("libpng-1.6.34")
                                   .Append("build")
                                   .Append(GetBuildType())
                                   .Append("x64")
                                   .Append("include"));
        includePaths->PushBack(tpd.Append("libjpeg-turbo"));
        includePaths->PushBack(tpd.Append("libjpeg-turbo")
                                   .Append("build")
                                   .Append(GetBuildType())
                                   .Append("x64")
                                   .Append("include"));
        includePaths->PushBack(tpd.Append("libsndfile").Append("include"));

        {
            Path physxRootDir = tpd.Append("PhysX");
            Array<Path> physxDirs =
                physxRootDir.GetSubDirectories(FindFlag::RECURSIVE);
            for (const Path &physxDir : physxDirs)
            {
                includePaths->PushBack(physxDir);
            }
        }
    }
    includePaths->PushBack(Paths::GetEngineDir().Append("include"));
    includePaths->PushBack(Paths::GetEngineBuildDir());

#ifdef __linux__

    // Compiler path
    Array<Path> pathsToTry;
    pathsToTry.PushBack(Path("/usr/bin/g++"));
    pathsToTry.PushBack(Path("/usr/bin/c++"));
    pathsToTry.PushBack(Path("/usr/bin/gcc"));
    for (const Path &path : pathsToTry)
    {
        if (path.IsFile())
        {
            *compilerPath = path;
            break;
        }
    }
    *linkerPath = *compilerPath;

    // Include paths
    includePaths->PushBack(Path("/usr/include"));
    includePaths->PushBack(Path("/usr/include/SDL2"));

#elif _WIN32

    Array<Path> programFilesDirs;
    {
        TCHAR pf[MAX_PATH];
        SHGetSpecialFolderPath(0, pf, CSIDL_PROGRAM_FILES, FALSE);
        programFilesDirs.PushBack(Path(String(pf)));
        SHGetSpecialFolderPath(0, pf, CSIDL_PROGRAM_FILESX86, FALSE);
        programFilesDirs.PushBack(Path(String(pf)));
    }

    for (const Path &programFilesDir : programFilesDirs)
    {
        const Array<Path> programFilesSubDirs =
            programFilesDir.GetSubDirectories(FindFlag::SIMPLE);
        for (const Path &programFilesSubDir : programFilesSubDirs)
        {
            if (programFilesSubDir.GetAbsolute().Contains(
                    "Microsoft Visual Studio"))
            {
                Array<Path> allSubPaths =
                    programFilesSubDir.GetSubPaths(FindFlag::RECURSIVE);
                for (const Path &subPath : allSubPaths)
                {
                    if (subPath.IsFile())
                    {
                        if (subPath.GetDirectory().GetName() == "x64")
                        {
                            if (subPath.GetNameExt() == "cl.exe")
                            {
                                *compilerPath = subPath;
                            }
                            else if (subPath.GetNameExt() == "link.exe")
                            {
                                *linkerPath = subPath;
                            }
                        }

                        if (subPath.GetNameExt() == "vcvarsall.bat")
                        {
                            *msvcConfigureArchitectureBatPath = subPath;
                        }
                    }
                }

                for (const Path &subPath : allSubPaths)
                {
                    if (subPath.IsDir())
                    {
                        if ((subPath.GetAbsolute().Contains("MSVC") &&
                             subPath.GetAbsolute().EndsWith("include")) ||
                            subPath.GetAbsolute().EndsWith(
                                "VC\\Auxiliary\\VS\\include"))
                        {
                            includePaths->PushBack(subPath);
                        }
                    }
                }
            }

            if (programFilesSubDir.GetAbsolute().Contains("Windows Kits"))
            {
                const Array<Path> allSubDirs =
                    programFilesSubDir.GetSubDirectories(FindFlag::RECURSIVE);
                for (const Path &subDir : allSubDirs)
                {
                    if (subDir.GetAbsolute().EndsWith("ucrt") ||
                        subDir.GetAbsolute().EndsWith("um") ||
                        subDir.GetAbsolute().EndsWith("shared") ||
                        subDir.GetAbsolute().EndsWith("winrt"))
                    {
                        includePaths->PushBack(subDir);
                    }
                }
            }
        }
    }
#endif
}

Path Paths::CreateProjectPath(const String &path)
{
    return Paths::GetProjectAssetsDir().Append(path);
}

void Paths::SetProjectRoot(const Path &projectRootDir)
{
    Paths::GetInstance()->m_projectRootPath = projectRootDir;
}

void Paths::SetCompilerPath(const Path &compilerPath)
{
    Paths::GetInstance()->m_compilerPath = compilerPath;
}

void Paths::SetLinkerPath(const Path &linkerPath)
{
    Paths::GetInstance()->m_linkerPath = linkerPath;
}

void Paths::SetMSVCConfigureArchitecturePath(const Path &msvcConfArchPath)
{
    Paths::GetInstance()->m_msvcConfigureArchitecturePath = msvcConfArchPath;
}

void Paths::AddCompilerIncludePath(const Path &compilerIncludePath)
{
    Paths::GetInstance()->m_compilerIncludePaths.PushBack(compilerIncludePath);
}

Array<Path> Paths::GetEngineIncludeDirs()
{
    return Paths::GetInstance()->m_compilerIncludePaths;
}

bool Paths::IsEnginePath(const Path &path)
{
    return path.BeginsWith(String(Paths::GetEngineDir()) + String(Separator));
}

void Paths::SetEngineRoot(const Path &engineRootDir)
{
    Paths::GetInstance()->m_engineRootPath = engineRootDir;
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
