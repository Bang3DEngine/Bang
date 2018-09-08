#ifndef FLAGS_H
#define FLAGS_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

using FlagsPrimitiveType = uint32_t;

template <class Flag>
class Flags
{
public:
    Flags()
    {
    }

    Flags(FlagsPrimitiveType flagsOn)
    {
        m_flags = SCAST<FlagsPrimitiveType>(Flag::NONE);
        SetOn(flagsOn);
    }

    Flags(Flag flagsOn)
    {
        m_flags = SCAST<FlagsPrimitiveType>(Flag::NONE);
        SetOn(flagsOn);
    }

    void Clear()
    {
        m_flags = 0;
    }

    Flags& SetTo(FlagsPrimitiveType f)
    {
        m_flags = f;
        return *this;
    }

    Flags& SetOn(FlagsPrimitiveType f)
    {
        m_flags = GetValue() | f;
        return *this;
    }
    Flags& SetOn(Flag f)
    {
        return SetOn(SCAST<FlagsPrimitiveType>(f));
    }

    Flags& SetOff(FlagsPrimitiveType f)
    {
        m_flags = GetValue() & ~f;
        return *this;
    }
    Flags& SetOff(Flag f)
    {
        return SetOff(static_cast<FlagsPrimitiveType>(f));
    }

    bool IsOn(FlagsPrimitiveType f) const
    {
        return (GetValue() & f) > 0;
    }
    bool IsOn(Flag f) const
    {
        return IsOn(SCAST<FlagsPrimitiveType>(f));
    }

    bool IsOff(FlagsPrimitiveType f) const
    {
        return !IsOn(f);
    }
    bool IsOff(Flag f) const
    {
        return !IsOn(f);
    }

    inline Flags operator|(Flag f)
    {
        return Flags(GetValue() | f);
    }

    inline Flags operator~()
    {
        return Flags(~GetValue());
    }

    inline Flags operator&(Flag f)
    {
        return Flags(GetValue() & f);
    }

    inline Flags operator^(Flag f)
    {
        return Flags(GetValue() ^ f);
    }

    inline bool operator==(const Flags &rhs) const
    {
        return SCAST<FlagsPrimitiveType>(GetValue()) ==
               SCAST<FlagsPrimitiveType>(rhs.GetValue());
    }

    inline bool operator!=(const Flags &rhs) const
    {
        return !(*this == rhs);
    }

    FlagsPrimitiveType GetValue() const
    {
        return m_flags;
    }

private:
    FlagsPrimitiveType m_flags = SCAST<FlagsPrimitiveType>(Flag::DEFAULT);
};

#define CREATE_FLAGS(FlagsName, FlagType) using FlagsName = Flags<FlagType>

NAMESPACE_BANG_END

#endif // FLAGS_H
