#ifndef LIBRARY_H
#define LIBRARY_H

#include "Bang/BangDefines.h"
#include "Bang/Path.h"
#include "Bang/String.h"

namespace Bang
{
class Library
{
public:
    Library();
    Library(const Path &libPath);
    ~Library();

    bool Load();
    bool UnLoad();
    void SetLibraryPath(const Path &libPath);

    template <class T>
    T Get(const String &symbolName)
    {
        return RCAST<T>(GetSymbol(symbolName));
    }

    bool IsLoaded() const;
    void *GetSymbol(const String &symbolName);

    const Path &GetLibraryPath() const;
    const String &GetErrorString() const;

private:
    Path m_libPath;
    void *m_libHandle = nullptr;
    String m_errorString;

    void ClearError();
    void FetchError();
    bool TheresError() const;
};
}

#endif  // LIBRARY_H
