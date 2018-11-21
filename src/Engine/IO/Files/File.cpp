#include "Bang/File.h"

#ifdef __linux__
#include <sys/stat.h>
#endif

#include <errno.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#ifdef __linux__
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif _WIN32
#include <Windows.h>
#endif

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Debug.h"
#include "Bang/List.h"
#include "Bang/List.tcc"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"
#include "Bang/USet.h"
#include "Bang/USet.tcc"

using namespace Bang;

File::File()
{
}

File::File(const Path &filepath)
{
    m_path = filepath;
}

File::File(const String &filepath) : File(Path(filepath))
{
}

File::~File()
{
}

bool File::DuplicateFile(const Path &srcFilepath,
                         const Path &dstFilepath,
                         bool overwrite)
{
    if (!srcFilepath.IsFile())
    {
        return false;
    }
    if (!overwrite && dstFilepath.Exists())
    {
        return false;
    }

    std::ifstream src(srcFilepath.GetAbsolute().ToCString(), std::ios::binary);
    if (!src)
    {
        return false;
    }

    if (overwrite)
    {
        File::Remove(dstFilepath);
    }

    std::ofstream dst(dstFilepath.GetAbsolute().ToCString(), std::ios::binary);
    if (!dst)
    {
        return false;
    }

    dst << src.rdbuf();
    return true;
}

bool File::DuplicateDir(const Path &srcDirpath,
                        const Path &dstDirPath,
                        USet<Path> &pathsToIgnore,
                        bool overwrite)
{
    if (!srcDirpath.IsDir())
    {
        return false;
    }
    if (!overwrite && dstDirPath.Exists())
    {
        return false;
    }
    if (pathsToIgnore.Contains(srcDirpath))
    {
        return true;
    }
    if (!File::CreateDir(dstDirPath))
    {
        return false;
    }

    Array<Path> filepaths = srcDirpath.GetFiles(FindFlag::SIMPLE_HIDDEN);
    for (const Path &filepath : filepaths)
    {
        String fileName = filepath.GetNameExt();
        bool ok = File::DuplicateFile(srcDirpath.Append(fileName),
                                      dstDirPath.Append(fileName),
                                      overwrite);
        if (!ok)
        {
            return false;
        }
    }

    Array<Path> subdirs = srcDirpath.GetSubDirectories(FindFlag::SIMPLE_HIDDEN);
    for (const Path &subdir : subdirs)
    {
        if (subdir.IsSubPathOf(dstDirPath))
        {
            continue;
        }

        Path newSubDirPath = dstDirPath.Append(subdir.GetName());
        pathsToIgnore.Add(newSubDirPath);

        bool ok =
            File::DuplicateDir(subdir, newSubDirPath, pathsToIgnore, overwrite);
        if (!ok)
        {
            return false;
        }
    }
    return true;
}

bool File::DuplicateDir(const Path &srcDirpath,
                        const Path &dstDirPath,
                        bool overwrite)
{
    USet<Path> pathsToIgnore;
    pathsToIgnore.Add(dstDirPath);
    return DuplicateDir(srcDirpath, dstDirPath, pathsToIgnore, overwrite);
}

void File::AddExecutablePermission(const Path &path)
{
#ifdef __linux__
    chmod(path.GetAbsolute().ToCString(), S_IRUSR | S_IXUSR);
#elif _WIN32
#endif
}

bool File::Remove(const Path &path)
{
    if (!path.Exists())
    {
        return false;
    }

    if (path.IsFile())
    {
        return std::remove(path.GetAbsolute().ToCString()) == 0;
    }
    else
    {
        Array<Path> subDirs = path.GetSubDirectories(FindFlag::SIMPLE_HIDDEN);
        for (const Path &subDir : subDirs)
        {
            File::Remove(subDir);
        }

        Array<Path> subFiles = path.GetFiles(FindFlag::SIMPLE_HIDDEN);
        for (const Path &subFile : subFiles)
        {
            File::Remove(subFile);
        }
        return std::remove(path.GetAbsolute().ToCString()) == 0;
    }
}

bool File::CreateDir(const Path &dirPath)
{
    if (dirPath.Exists())
    {
        return true;
    }

#ifdef __linux__
    return mkdir(dirPath.GetAbsolute().ToCString(), 0700) == 0;
#elif _WIN32
    return CreateDirectory(dirPath.GetAbsolute().ToCString(), NULL);
#endif
}

bool File::Rename(const Path &oldPath, const Path &newPath)
{
    if (!oldPath.Exists())
    {
        return false;
    }
    return std::rename(oldPath.GetAbsolute().ToCString(),
                       newPath.GetAbsolute().ToCString()) != 0;
}

bool File::Duplicate(const Path &fromPath, const Path &toPath)
{
    if (fromPath.IsFile())
    {
        return File::DuplicateFile(fromPath, toPath);
    }
    else if (fromPath.IsDir())
    {
        return File::DuplicateDir(fromPath, toPath);
    }
    return false;
}

void File::Write(const Path &filepath, const String &contents)
{
    std::ofstream out(filepath.GetAbsolute());
    if (out)
    {
        out << contents;
        out.close();
    }
}

void File::Write(const Path &filepath, const Array<String> &lines)
{
    File::Write(filepath, String::Join(lines, "\n"));
}

void File::Write(const Path &filepath, const List<String> &lines)
{
    File::Write(filepath, String::Join(lines, "\n"));
}

void File::Write(const Path &filepath, const Byte *bytes, std::size_t bytesSize)
{
    std::ofstream out(filepath.GetAbsolute(), std::ios::binary);
    if (out)
    {
        out.write(RCAST<const char *>(bytes), bytesSize);
        out.close();
    }
}

String File::GetContents(const Path &filepath)
{
    if (!filepath.IsFile())
    {
        return "";
    }

    String contents = "";
    std::ifstream ifs(filepath.GetAbsolute().ToCString());
    if (ifs.is_open() && ifs.good() && !ifs.bad() && !ifs.fail())
    {
        // contents = String((std::istreambuf_iterator<char>(ifs)),
        //                   std::istreambuf_iterator<char>());

        std::string line;
        while (std::getline(ifs, line))
        {
            contents += line;
            contents += "\n";
        }

        ifs.close();
    }
    else
    {
        Debug_Error(
            "Can't open file '" << filepath << "': " << std::strerror(errno));
    }
    return contents;
}

String File::GetContents() const
{
    return File::GetContents(GetPath());
}

const Path &File::GetPath() const
{
    return m_path;
}
