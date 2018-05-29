#ifndef PATH_H
#define PATH_H

#include "Bang/Array.h"
#include "Bang/Flags.h"
#include "Bang/String.h"
#include "Bang/IToString.h"

NAMESPACE_BANG_BEGIN

class Path : public IToString
{
public:
    enum FindFlag
    {
        NONE      = 0,
        RECURSIVE = 1,
        HIDDEN    = 2,

        SIMPLE           = FindFlag::NONE,
        SIMPLE_HIDDEN    = FindFlag::SIMPLE | FindFlag::HIDDEN,
        RECURSIVE_HIDDEN = FindFlag::RECURSIVE | FindFlag::HIDDEN,
        DEFAULT          = FindFlag::SIMPLE_HIDDEN
    };
    CREATE_FLAGS(FindFlags, FindFlag);

    static const Path Empty;

    Path();
    Path(const Path &path);
    explicit Path(const String &absolutePath);
    void SetPath(const String &path);

    bool IsDir() const;
    bool IsFile() const;
    bool Exists() const;


    List<Path> GetFiles(FindFlags findFlags = FindFlag::DEFAULT,
                        const Array<String> &extensions = {}) const;
    List<Path> GetSubDirectories(FindFlags findFlags) const;
    List<Path> GetSubPaths(FindFlags findFlags) const;

    uint64_t GetModificationTimeSeconds() const;

    Path GetDirectory() const;
    String GetName() const;
    bool IsAbsolute() const;
    String GetNameExt() const;
    String GetExtension() const;
    Array<String> GetExtensions() const;
    Path GetRelativePath(const Path &prefix) const;
    const String& GetAbsolute() const;
    Path GetDuplicatePath() const;

    virtual String ToString() const override;
    bool IsEmpty() const;

    bool BeginsWith(const Path &path) const;
    bool BeginsWith(const String &path) const;

    Path Append(const Path& path) const;
    Path Append(const String& str) const;
    Path AppendRaw(const String& str) const;
    Path AppendExtension(const String& extension) const;

    bool IsHiddenFile() const;

    Path WithHidden(bool hidden) const;
    Path WithNameExt(const String& name, const String& extension = "") const;
    Path WithExtension(const String& extension) const;

    bool HasExtension(const String &extension) const;
    bool HasExtension(const Array<String> &extensions) const;

    explicit operator String() const;
    bool operator!=(const Path &rhs) const;
    bool operator==(const Path &rhs) const;
    bool operator<(const Path &rhs) const;

    static Path EmptyPath();

private:
    String m_absolutePath = "";

    static Path GetNextDuplicatePath(const Path &path);
};

NAMESPACE_BANG_END

// Hash for Path
namespace std
{
    template <>
    struct hash<Bang::Path>
    {
        std::size_t operator()(const Bang::Path& path) const
        {
            return std::hash<std::string>()(path.GetAbsolute());
        }
    };
}

#endif // PATH_H
