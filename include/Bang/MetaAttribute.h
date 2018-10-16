#ifndef METAATTRIBUTE_H
#define METAATTRIBUTE_H

#include <ostream>

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/IToString.h"
#include "Bang/Path.h"
#include "Bang/Paths.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

class MetaAttribute : public IToString
{
public:
    MetaAttribute();
    MetaAttribute(const String &name, const String &value);

    void Set(const String &name, const String &value);

    void SetName(const String &name);
    const String& GetName() const;

    void SetValue(const String &value);
    const String& GetStringValue() const;

    String ToString() const;
    static MetaAttribute FromString(const String &string);

    template<class T>
    void Set(const String &name, const T& value)
    {
        std::ostringstream oss; oss << value;
        Set(name, String(oss.str()));
    }

    template<class T>
    T Get() const
    {
        T t;
        std::istringstream iss(GetStringValue());
        iss >> t;
        return t;
    }

protected:
    String m_name  = "";
    String m_value = "";
};

template<>
inline void MetaAttribute::Set(const String &name, const Path& filepath)
{
    Set(name, filepath.GetAbsolute());
}

template<>
inline void MetaAttribute::Set(const String &name, const bool& value)
{
    Set(name, value ? "True" : "False");
}

template<>
inline bool MetaAttribute::Get() const
{
    return GetStringValue().EqualsNoCase("true");
}

template<>
inline String MetaAttribute::Get() const
{
    return GetStringValue();
}

template<>
inline Path MetaAttribute::Get() const
{
    if ( GetStringValue().IsEmpty() ) { return Path::Empty; }
    return Path(GetStringValue());
}

NAMESPACE_BANG_END

#endif // METAATTRIBUTE_H
