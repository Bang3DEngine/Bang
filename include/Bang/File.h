#ifndef FILE_H
#define FILE_H

#include <cstddef>

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/Path.h"
#include "Bang/String.h"
#include "Bang/USet.h"

namespace Bang
{
class File
{
public:
    File();
    File(const Path &filepath);
    File(const String &filepath);
    ~File();

    String GetContents() const;
    const Path &GetPath() const;

    static bool Remove(const Path &path);
    static bool CreateDir(const Path &dirPath);
    static bool Rename(const Path &srcPath, const Path &dstPath);

    static bool Duplicate(const Path &srcPath, const Path &dstPath);
    static bool DuplicateFile(const Path &srcFilepath,
                              const Path &dstFilepath,
                              bool overwrite = true);
    static bool DuplicateDir(const Path &srcDirpath,
                             const Path &dstDirpath,
                             bool overwrite = true);
    static void AddExecutablePermission(const Path &path);

    static String GetContents(const Path &filepath);
    static void Write(const Path &filepath, const String &contents);
    static void Write(const Path &filepath, const Array<String> &lines);
    static void Write(const Path &filepath, const List<String> &lines);
    static void Write(const Path &filepath,
                      const Byte *bytes,
                      std::size_t bytesSize);

protected:
    Path m_path;

    static bool DuplicateDir(const Path &srcDirpath,
                             const Path &dstDirpath,
                             USet<Path> &pathsToIgnore,
                             bool overwrite = true);
};
}  // namespace Bang

#endif  // FILE_H
