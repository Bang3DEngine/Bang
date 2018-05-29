#ifndef FILE_H
#define FILE_H

#include "Bang/USet.h"
#include "Bang/Path.h"

NAMESPACE_BANG_BEGIN

class File
{
public:
    File();
    File(const Path &filepath);
    File(const String &filepath);
    virtual ~File();

    String GetContents() const;
    const Path& GetPath() const;

    static bool Remove(const Path &path);
    static bool CreateDirectory(const Path &dirPath);
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

protected:
    Path m_path;

    static bool DuplicateDir(const Path &srcDirpath,
                             const Path &dstDirpath,
                             USet<Path> &pathsToIgnore,
                             bool overwrite = true);
};

NAMESPACE_BANG_END

#endif // FILE_H
