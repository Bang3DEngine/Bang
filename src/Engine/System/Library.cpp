#include "Bang/Library.h"

#ifdef __linux__
#include <dlfcn.h>
#elif _WIN32
#endif

using namespace Bang;

Library::Library()
{
}

Library::Library(const Path &libPath)
{
    SetLibraryPath(libPath);
}

Library::~Library()
{
    UnLoad();
}

bool Library::Load()
{
    ClearError();
#ifdef __linux__
    m_libHandle = dlopen(GetLibraryPath().GetAbsolute().ToCString(),
                         // RTLD_NOW | RTLD_GLOBAL);
                         RTLD_NOW | RTLD_LOCAL);
#elif _WIN32

#endif

    FetchError();
    return !TheresError();
}

bool Library::UnLoad()
{
    ClearError();
    if (IsLoaded())
    {
#ifdef __linux
        dlclose(m_libHandle);
#endif
    }
    FetchError();
    return !TheresError();
}

void Library::SetLibraryPath(const Path &libPath)
{
    m_libPath = libPath;
}

bool Library::IsLoaded() const
{
    return (m_libHandle != nullptr);
}

void *Library::GetSymbol(const String &symbolName)
{
    ClearError();
    void *symbolAddress = nullptr;

#ifdef __linux__
    symbolAddress = dlsym(m_libHandle, symbolName.ToCString());
#elif _WIN32
#endif

    FetchError();
    return symbolAddress;
}

const Path &Library::GetLibraryPath() const
{
    return m_libPath;
}

const String &Library::GetErrorString() const
{
    return m_errorString;
}

void Library::ClearError()
{
    FetchError();
    m_errorString = "";
}

void Library::FetchError()
{
    const char *error = nullptr;

#ifdef __linux
    error = dlerror();
    if (error)
    {
        m_errorString = String(error);
    }
    else
    {
        m_errorString = "";
    }
#elif _WIN32
#endif
}

bool Library::TheresError() const
{
    return !m_errorString.IsEmpty();
}
