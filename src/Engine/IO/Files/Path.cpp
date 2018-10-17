#include "Bang/Path.h"

#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Assert.h"
#include "Bang/Extensions.h"

using namespace Bang;

const Path Path::Empty;

Path::Path()
{
}

Path::Path(const Path &path) : Path(path.GetAbsolute())
{
}

Path::Path(const String &absolutePath)
{
    SetPath(absolutePath);
}

void Path::SetPath(const String &path)
{
    m_absolutePath = path;
    if(!m_absolutePath.IsEmpty() &&
       m_absolutePath.At(m_absolutePath.Size() - 1) == '/')
    {
        m_absolutePath.Remove(m_absolutePath.Size() - 1, m_absolutePath.Size());
    }

    if(m_absolutePath.BeginsWith("./"))
    {
        m_absolutePath.Remove(0, 1);
    }
}

bool Path::IsDir() const
{
    struct stat path_stat;
    if(stat(GetAbsolute().ToCString(), &path_stat) == 0)
    {
        return S_ISDIR(path_stat.st_mode);
    }
    return false;
}

bool Path::IsFile() const
{
    struct stat path_stat;
    if(stat(GetAbsolute().ToCString(), &path_stat) == 0)
    {
        return S_ISREG(path_stat.st_mode);
    }
    return false;
}

bool Path::Exists() const
{
    struct stat path_stat;
    return (stat(GetAbsolute().ToCString(), &path_stat) == 0);
}

bool Path::IsSubPathOf(const Path &path) const
{
    return GetAbsolute().BeginsWith(path.GetAbsolute());
}

Array<Path> Path::GetFiles(FindFlags findFlags,
                           const Array<String> &extensions) const
{
    Array<Path> subFilesArray;
    if(IsDir())
    {
        Array<Path> subPathsArray = GetSubPaths(findFlags);
        for(const Path &subPath : subPathsArray)
        {
            bool extMatches =
                extensions.IsEmpty() || subPath.HasExtension(extensions);
            if(subPath.IsDir())
            {
                if(findFlags.IsOn(FindFlag::RECURSIVE))
                {
                    subFilesArray.PushBack(
                        subPath.GetFiles(findFlags, extensions));
                }
            }
            else if(/*subPath.IsFile() &&*/ extMatches)
            {
                subFilesArray.PushBack(subPath);
            }
        }
    }
    return subFilesArray;
}

Array<Path> Path::GetSubDirectories(FindFlags findFlags) const
{
    Array<Path> subDirsArray;
    if(IsDir())
    {
        Array<Path> subPathsArray = GetSubPaths(findFlags);
        for(const Path &subPath : subPathsArray)
        {
            if(subPath.IsDir())
            {
                subDirsArray.PushBack(subPath);
                if(findFlags.IsOn(FindFlag::RECURSIVE))
                {
                    subDirsArray.PushBack(subPath.GetSubDirectories(findFlags));
                }
            }
        }
    }
    return subDirsArray;
}

Array<Path> Path::GetSubPaths(FindFlags findFlags) const
{
    Array<Path> subPathsArray;

    struct dirent *dir;
    if(DIR *d = opendir(GetAbsolute().ToCString()))
    {
        while((dir = readdir(d)) != nullptr)
        {
            String subName = dir->d_name;
            if(findFlags.IsOn(FindFlag::HIDDEN) || !subName.BeginsWith("."))
            {
                if(subName != "." && subName != "..")
                {
                    Path subPath = this->Append(subName);
                    subPathsArray.PushBack(subPath);
                }
            }
        }
        closedir(d);
    }

    return subPathsArray;
}

Time Path::GetModificationTime() const
{
    Time time;

    struct stat attr;
    if(stat(GetAbsolute().ToCString(), &attr) == 0)
    {
        time.SetMillis(attr.st_mtim.tv_sec);
    }

    return time;
}

String Path::GetName() const
{
    String name = "";
    bool iteratingFirstDots = true;  // Treat hidden files "....foo.txt.bang"
    const String nameExt = GetNameExt();
    for(int i = 0; i < nameExt.Size(); ++i)
    {
        const char c = nameExt[i];
        if(iteratingFirstDots)
        {
            if(c != '.')
            {
                iteratingFirstDots = false;
            }
        }

        if(!iteratingFirstDots)
        {
            if(c != '.')
            {
                name += c;
            }
            else
            {
                break;
            }
        }
    }
    return name;
}

bool Path::IsAbsolute() const
{
    return GetAbsolute().BeginsWith("/");
}

String Path::GetNameExt() const
{
    if(IsEmpty())
    {
        return "";
    }

    String filename = GetAbsolute();
    const size_t lastSlash = GetAbsolute().RFind('/');
    if(lastSlash != String::npos)
    {
        filename = GetAbsolute().SubString(lastSlash + 1);
    }
    return filename;
}

String Path::GetExtension() const
{
    String fullExtension = "";
    bool firstDotFound = false;
    const String &absPath = GetAbsolute();
    for(int i = 0; i < absPath.Size(); ++i)
    {
        char c = absPath[i];
        if(firstDotFound)
        {
            fullExtension += c;
        }
        else
        {
            if(c == '.')
            {
                firstDotFound = true;
            }
        }
    }
    return fullExtension;
}

String Path::GetLastExtension() const
{
    String lastExtension = "";
    const String &absPath = GetAbsolute();
    for(int i = (absPath.Size() - 1); i >= 0; --i)
    {
        char c = absPath[i];
        if(c == '.')
        {
            break;
        }
        lastExtension.Prepend(c);
    }
    return lastExtension;
}

Array<String> Path::GetExtensions() const
{
    Array<String> parts;
    if(!IsEmpty())
    {
        parts = GetNameExt().Split<Array>('.');
        if(!parts.IsEmpty())
        {
            parts.PopFront();
        }
    }
    return parts;
}

Path Path::GetRelativePath(const Path &prefix) const
{
    const String &absolute = GetAbsolute();
    String relative = absolute;
    if(absolute.BeginsWith(prefix.GetAbsolute()))
    {
        relative.Remove(0, prefix.GetAbsolute().Size());
    }
    return Path(relative);
}

Path Path::GetDirectory() const
{
    if(IsEmpty())
    {
        return Path::Empty;
    }

    const size_t lastSlash = GetAbsolute().RFind('/');
    if(lastSlash != String::npos)
    {
        return Path(GetAbsolute().SubString(0, lastSlash - 1));
    }
    return Path(".");
}

const String &Path::GetAbsolute() const
{
    return m_absolutePath;
}

Path Path::GetDuplicatePath() const
{
    return GetDuplicatePath(*this);
}

Path Path::GetDuplicatePath(const Path &path)
{
    if(path.IsEmpty())
    {
        return Path::Empty;
    }

    Path resultPath = path;
    while(resultPath.Exists())
    {
        resultPath = Path::GetNextDuplicatePath(resultPath);
    }
    return resultPath;
}

bool Path::IsEmpty() const
{
    return GetAbsolute().IsEmpty();
}

bool Path::BeginsWith(const Path &path) const
{
    return BeginsWith(path.GetAbsolute());
}

bool Path::BeginsWith(const String &path) const
{
    return GetAbsolute().BeginsWith(path);
}

Path Path::Append(const Path &pathRHS) const
{
    String str = pathRHS.GetAbsolute();
    if(str.BeginsWith("./"))
    {
        str.Remove(0, 1);
    }

    while(str.BeginsWith("/"))
    {
        str.Remove(0, 1);
    }

    return this->AppendRaw("/" + str);
}

Path Path::Append(const String &str) const
{
    return Path(GetAbsolute()).Append(Path(str));
}

Path Path::AppendRaw(const String &str) const
{
    return Path(GetAbsolute() + str);
}

Path Path::AppendExtension(const String &extension) const
{
    if(HasExtension(extension) || extension.IsEmpty())
    {
        return (*this);
    }
    return Path(GetAbsolute() + "." + extension);
}

bool Path::IsHiddenFile() const
{
    return IsFile() && GetNameExt().BeginsWith(".");
}

Path Path::WithHidden(bool hidden) const
{
    String nameExt = GetNameExt();
    if(hidden && !nameExt.BeginsWith("."))
    {
        nameExt.Insert(0, ".");
    }

    if(!hidden && nameExt.BeginsWith("."))
    {
        nameExt.Remove(0, 1);
    }

    return GetDirectory().Append(nameExt);
}

Path Path::WithNameExt(const String &name, const String &extension) const
{
    return GetDirectory().Append(name).AppendExtension(extension);
}

Path Path::WithExtension(const String &extension) const
{
    return Path(GetDirectory().Append(GetName()).AppendExtension(extension));
}

bool Path::HasExtension(const String &extensions) const
{
    Array<String> extensionsArray = extensions.Split<Array>(' ', true);
    return HasExtension(extensionsArray);
}

bool Path::HasExtension(const Array<String> &extensions) const
{
    return Extensions::Equals(GetExtension(), extensions);
}

Path::operator String() const
{
    return GetAbsolute();
}

bool Path::operator!=(const Path &rhs) const
{
    return !(*this == rhs);
}

bool Path::operator==(const Path &rhs) const
{
    return GetAbsolute() == rhs.GetAbsolute();
}

bool Path::operator<(const Path &rhs) const
{
    return GetAbsolute() < rhs.GetAbsolute();
}

Path Path::GetNextDuplicatePath(const Path &filepath)
{
    if(filepath.IsEmpty())
    {
        return Path::Empty;
    }

    Path fileDir = filepath.GetDirectory();
    String fileName = filepath.GetName();
    String fileExtension = filepath.GetExtension();

    String duplicateFileName = GetNextDuplicateString(fileName);

    Path result =
        fileDir.Append(duplicateFileName).AppendExtension(fileExtension);
    return result;
}

String Path::GetDuplicateStringWithExtension(
    const String &string,
    const Array<String> &existingStrings)
{
    if(string.IsEmpty())
    {
        return "";
    }

    Path resultPath = Path(string);
    const Array<Path> existingPaths = existingStrings.To<Array, Path>();
    while(existingPaths.Contains(resultPath))
    {
        resultPath = Path(Path::GetNextDuplicatePath(resultPath).GetNameExt());
    }

    return resultPath.GetAbsolute();
}

String Path::GetDuplicateString(const String &string,
                                const Array<String> &existingStrings)
{
    if(string.IsEmpty())
    {
        return "";
    }

    String resultString = string;
    while(existingStrings.Contains(resultString))
    {
        resultString = Path::GetNextDuplicateString(resultString);
    }
    return resultString;
}

String Path::GetNextDuplicateString(const String &string)
{
    int duplicationNumber = 1;
    String duplicateString = string;
    Array<String> splitted = duplicateString.Split<Array>('_');
    if(splitted.Size() > 1)
    {
        bool ok = false;
        String numberString = splitted[splitted.Size() - 1];
        int readNumber = String::ToInt(numberString, &ok);
        if(ok)
        {
            duplicationNumber = readNumber + 1;
            splitted.PopBack();

            int lastUnderscorePos = duplicateString.RFind('_');
            if(lastUnderscorePos != -1)  // Strip _[number] from fileName
            {
                duplicateString =
                    duplicateString.SubString(0, lastUnderscorePos - 1);
            }
        }
    }

    duplicateString =
        (duplicateString + "_" + String::ToString(duplicationNumber));
    ASSERT(duplicateString != string);
    return duplicateString;
}

Path Path::EmptyPath()
{
    return Path();
}
