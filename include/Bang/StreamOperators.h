#ifndef STREAMOPERATORS_H
#define STREAMOPERATORS_H

#include <stddef.h>
#include <stdint.h>
#include <queue>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "Bang/BangDefines.h"
#include "Bang/String.h"

namespace Bang
{
template <class, class, class, class, class>
class UMap;
template <class, class, class, class>
class USet;
template <class, class, class>
class Map;
template <class, class>
class Set;
template <class>
class AARectG;
template <class>
class Array;
template <class>
class Flags;
template <class>
class List;
template <class>
class Matrix3G;
template <class>
class Matrix4G;
template <class>
class QuaternionG;
template <class>
class RectG;
template <class>
class Tree;
template <class>
class Vector3G;
template <class>
class Vector4G;
template <class>
class AABoxG;
template <class>
class ColorG;
class ComplexRandom;
class GUID;
class IToString;
class ObjectId;
class Path;
template <class>
class QuadG;
class Time;
template <class>
class TriangleG;
class Variant;

std::istream &operator>>(std::istream &is, GUID &guid);
std::istream &operator>>(std::istream &is, Path &p);
std::istream &operator>>(std::istream &is, Time &t);
std::istream &operator>>(std::istream &is, Variant &variant);
std::istream &operator>>(std::istream &is, ComplexRandom &cr);
std::ostream &operator<<(std::ostream &log, const ObjectId &objectId);
std::ostream &operator<<(std::ostream &log, const Variant &variant);
std::ostream &operator<<(std::ostream &log, const Path &p);
std::ostream &operator<<(std::ostream &log, const Time &t);
std::ostream &operator<<(std::ostream &log, const GUID &guid);
std::ostream &operator<<(std::ostream &log, const IToString &v);
std::ostream &operator<<(std::ostream &log, const IToString *s);
std::ostream &operator<<(std::ostream &log, const ComplexRandom &cr);

// Templated ostream operators

template <class T>
std::ostream &operator<<(std::ostream &log, const Flags<T> &flags)
{
    log << flags.GetValue();
    return log;
}

template <class T, typename std::enable_if<std::is_enum<T>::value, T>::type>
inline std::ostream &operator<<(std::ostream &log, const T &enumT)
{
    log << SCAST<uint64_t>(enumT);
    return log;
}

template <class EnumClass,
          class = typename std::enable_if<std::is_enum<EnumClass>::value,
                                          EnumClass>::type>
std::ostream &operator<<(std::ostream &log, const EnumClass &e)
{
    log << SCAST<int>(e);
    return log;
}

template <class T, size_t N>
std::ostream &operator<<(std::ostream &log, const std::array<T, N> &a)
{
    log << "{";
    for (int i = 0; i < N; ++i)
    {
        log << a[i] << ((i == N - 1) ? "" : ", ");
    }
    log << "}";
    return log;
}
template <class T>
std::ostream &operator<<(std::ostream &log, const std::queue<T> &q)
{
    log << "(";
    int i = 0;
    std::queue<T> qc = q;
    const int N = q.size();
    while (!qc.empty())
    {
        const auto &x = qc.front();
        qc.pop();
        log << x << ((i++ == N - 1) ? "" : ", ");
    }
    log << ")";
    return log;
}
template <class K, class V>
std::ostream &operator<<(std::ostream &log, const std::unordered_map<K, V> &m)
{
    log << "{";
    int i = 0;
    const int N = m.size();
    for (const auto &p : m)
    {
        log << p.first << ": " << p.second << ((i++ == N - 1) ? "" : ", ");
    }
    log << "}";
    return log;
}
template <class T, class M>
std::ostream &operator<<(std::ostream &log, const std::pair<T, M> &p)
{
    log << "<";
    log << p.first << ", " << p.second;
    log << ">";
    return log;
}

template <class T>
std::ostream &operator<<(std::ostream &log, const List<T> &l)
{
    log << "(";
    bool first = true;
    for (auto it = l.Begin(); it != l.End(); ++it)
    {
        if (!first)
            log << ", ";
        log << (*it);
        first = false;
    }
    log << ")";
    return log;
}

template <class T>
std::ostream &operator<<(std::ostream &log, const Array<T> &v)
{
    log << "[";
    for (uint i = 0; i < v.Size(); ++i)
    {
        if (i != 0)
        {
            log << ", ";
        }
        log << v[i];
    }
    log << "]";
    return log;
}

template <class T, class S, class C>
std::ostream &operator<<(std::ostream &log, const Map<T, S, C> &m)
{
    log << "{";
    for (auto it = m.CBegin(); it != m.CEnd(); ++it)
    {
        if (it != m.cbegin())
            log << ", ";
        log << (it->first) << ": " << (it->second);
    }
    log << "}";
    return log;
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
std::ostream &operator<<(std::ostream &log,
                         const UMap<Key, Value, Hash, Pred, Alloc> &m)
{
    log << "{";
    for (auto it = m.CBegin(); it != m.CEnd(); ++it)
    {
        if (it != m.cbegin())
            log << ", ";
        log << (it->first) << ": " << (it->second);
    }
    log << "}";
    return log;
}

template <class T, class C>
std::ostream &operator<<(std::ostream &log, const Set<T, C> &s)
{
    log << "{";
    for (auto it = s.Begin(); it != s.End(); ++it)
    {
        if (it != s.Begin())
            log << ", ";
        log << (*it);
    }
    log << "}";
    return log;
}

template <class Key, class Hash, class Pred, class Alloc>
std::ostream &operator<<(std::ostream &log,
                         const USet<Key, Hash, Pred, Alloc> &s)
{
    log << "{";
    for (auto it = s.Begin(); it != s.End(); ++it)
    {
        if (it != s.Begin())
            log << ", ";
        log << (*it);
    }
    log << "}";
    return log;
}

template <class T>
std::ostream &operator<<(std::ostream &log, const Tree<T> &t)
{
    log << "(" << t.GetData();
    bool first = true;
    List<T> &children = t.GetChildren();
    for (const T &child : children)
    {
        log << (first ? "" : ", ") << child;
        first = false;
    }
    log << ")";
    return log;
}

// Templated istream operators
inline void ConsumeLetters_(std::istream &is)
{
    char c = SCAST<char>(is.peek());
    while ((c >= 65 && c <= 90) || (c >= 97 && c <= 122))  // While is a letter
    {
        is >> c;
        c = SCAST<char>(is.peek());
    }
}

template <class T, typename std::enable_if<std::is_enum<T>::value, T>::type>
std::istream &operator>>(std::istream &is, const T &enumT)
{
    uint64_t v;
    is >> v;
    enumT = SCAST<T>(v);
    return is;
}

template <class T>
std::istream &operator>>(std::istream &is, Vector2G<T> &v)
{
    Vector4G<T> v4;
    is >> v4;
    v = v4.xy();
    return is;
}

template <class T>
std::istream &operator>>(std::istream &is, Vector3G<T> &v)
{
    Vector4G<T> v4;
    is >> v4;
    v = v4.xyz();
    return is;
}

template <class T>
std::istream &operator>>(std::istream &is, Vector4G<T> &v)
{
    char _;
    ConsumeLetters_(is);
    if (is.peek() != '(')
    {
        is >> _;
    }

    for (int i = 0; i < 4; ++i)
    {
        is >> _ >> v[i];
        if (is.peek() != ',')
        {
            break;
        }
    }

    if (is.peek() == ')')
    {
        is >> _;
    }

    return is;
}

template <class T>
std::istream &operator>>(std::istream &is, QuaternionG<T> &q)
{
    Vector4G<T> v4;
    is >> v4;
    q = QuaternionG<T>(v4.x, v4.y, v4.z, v4.w);
    return is;
}

template <class T>
std::istream &operator>>(std::istream &is, ColorG<T> &c)
{
    ConsumeLetters_(is);

    char _;
    if (is.peek() == ':')
    {
        is >> _ >> _;
        String colorName;
        is >> colorName;

#define READ_COLOR_NAME(ColorName)       \
    if (colorName == "" #ColorName "()") \
    {                                    \
        c = ColorG<T>::ColorName();          \
        return is;                       \
    }

        READ_COLOR_NAME(Red);
        READ_COLOR_NAME(Orange);
        READ_COLOR_NAME(Yellow);
        READ_COLOR_NAME(Green);
        READ_COLOR_NAME(Turquoise);
        READ_COLOR_NAME(VeryLightBlue);
        READ_COLOR_NAME(LightBlue);
        READ_COLOR_NAME(Blue);
        READ_COLOR_NAME(DarkBlue);
        READ_COLOR_NAME(Purple);
        READ_COLOR_NAME(Pink);
        READ_COLOR_NAME(Black);
        READ_COLOR_NAME(Gray);
        READ_COLOR_NAME(LightGray);
        READ_COLOR_NAME(DarkGray);
        READ_COLOR_NAME(White);
        READ_COLOR_NAME(Zero);
        READ_COLOR_NAME(One);
    }

    is >> _ >> c.r >> _ >> c.g >> _ >> c.b >> _ >> c.a >> _;
    return is;
}

template <class T>
std::istream &operator>>(std::istream &is, AARectG<T> &r)
{
    ConsumeLetters_(is);

    char _;
    Vector2G<T> minv, maxv;
    is >> _ >> minv >> _ >> maxv >> _;
    r.SetMin(minv);
    r.SetMax(maxv);
    return is;
}

template <class EnumClass,
          class = typename std::enable_if<std::is_enum<EnumClass>::value,
                                          EnumClass>::type>
std::istream &operator>>(std::istream &is, EnumClass &e)
{
    int x;
    is >> x;
    e = SCAST<EnumClass>(x);
    return is;
}
}  // namespace Bang

#endif  // STREAMOPERATORS_H
