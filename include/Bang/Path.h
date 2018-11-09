#ifndef PATH_H
#define PATH_H

#include <cstddef>
#include <string>
#include <system_error>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Flags.h"
#include "Bang/String.h"
#include "Bang/Time.h"

namespace Bang
{
enum FindFlag
{
    NONE = 0,
    RECURSIVE = 1,
    HIDDEN = 2,

    SIMPLE = FindFlag::NONE,
    SIMPLE_HIDDEN = FindFlag::SIMPLE | FindFlag::HIDDEN,
    RECURSIVE_HIDDEN = FindFlag::RECURSIVE | FindFlag::HIDDEN,
    DEFAULT = FindFlag::SIMPLE_HIDDEN
};
CREATE_FLAGS(FindFlags, FindFlag);

class Path
{
public:
    static const Path &Empty();

    Path();
    Path(const Path &path);
    explicit Path(const String &absolutePath);
    void SetPath(const String &path);

    bool IsDir() const;
    bool IsFile() const;
    bool Exists() const;
    bool IsSubPathOf(const Path &path) const;

    Array<Path> GetFiles(FindFlags findFlags = FindFlag::DEFAULT,
                         const Array<String> &extensions = {}) const;
    Array<Path> GetSubDirectories(FindFlags findFlags) const;
    Array<Path> GetSubPaths(FindFlags findFlags) const;

    Time GetModificationTime() const;

    Path GetDirectory() const;
    String GetName() const;
    bool IsAbsolute() const;
    String GetNameExt() const;
    String GetExtension() const;
    String GetLastExtension() const;
    Array<String> GetExtensions() const;
    Path GetRelativePath(const Path &prefix) const;
    const String &GetAbsolute() const;
    Path GetDuplicatePath() const;
    static Path GetDuplicatePath(const Path &path);
    static Path GetNextDuplicatePath(const Path &path);
    static String GetDuplicateStringWithExtension(
        const String &string,
        const Array<String> &existingStrings);
    static String GetDuplicateString(const String &string,
                                     const Array<String> &existingStrings);
    static String GetNextDuplicateString(const String &string);

    bool IsEmpty() const;

    bool BeginsWith(const Path &path) const;
    bool BeginsWith(const String &path) const;

    Path Append(const Path &path) const;
    Path Append(const String &str) const;
    Path AppendRaw(const String &str) const;
    Path AppendExtension(const String &extension) const;

    bool IsHiddenFile() const;

    Path WithHidden(bool hidden) const;
    Path WithNameExt(const String &name, const String &extension = "") const;
    Path WithExtension(const String &extension) const;

    bool HasExtension(const String &extension) const;
    bool HasExtension(const Array<String> &extensions) const;

    explicit operator String() const;
    bool operator!=(const Path &rhs) const;
    bool operator==(const Path &rhs) const;
    bool operator<(const Path &rhs) const;

private:
    String m_absolutePath = "";
};
}  // namespace Bang

// Hash for Path
namespace std
{
template <>
struct hash<Bang::Path>
{
    std::size_t operator()(const Bang::Path &path) const
    {
        return std::hash<std::string>()(path.GetAbsolute());
    }
};
}  // namespace std

#endif  // PATH_H
