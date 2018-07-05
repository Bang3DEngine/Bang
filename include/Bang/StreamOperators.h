#ifndef STREAMOPERATORS_H
#define STREAMOPERATORS_H

#include <array>
#include <queue>
#include <sstream>
#include <utility>
#include <unordered_map>
#include "Bang/TypeTraits.h"

NAMESPACE_BANG_BEGIN

FORWARD_TT    class Set;
FORWARD_TTT   class Map;
FORWARD_TTTTT class UMap;
FORWARD_TTTT  class USet;
FORWARD       class Path;
FORWARD_T     class Tree;
FORWARD       class GUID;
FORWARD_T     class List;
FORWARD       class Quad;
FORWARD       class AABox;
FORWARD_T     class Array;
FORWARD       class Color;
FORWARD_T     class RectG;
FORWARD       class String;
FORWARD_T     class AARectG;
FORWARD       class ObjectId;
FORWARD       class Triangle;
FORWARD_T     class Vector2G;
FORWARD_T     class Vector3G;
FORWARD_T     class Vector4G;
FORWARD_T     class Matrix3G;
FORWARD_T     class Matrix4G;
FORWARD       class IToString;
FORWARD_T     class QuaternionG;

std::istream& operator>>(std::istream& is, GUID &guid);
std::istream& operator>>(std::istream& is, Path &p);
std::istream& operator>>(std::istream& is, Color &c);
std::ostream& operator<<(std::ostream &log, const ObjectId &objectId);
std::ostream& operator<<(std::ostream &log, const Path  &p);
std::ostream& operator<<(std::ostream &log, const Color &v);
std::ostream& operator<<(std::ostream &log, const AABox &v);
std::ostream& operator<<(std::ostream &log, const Quad &v);
std::ostream& operator<<(std::ostream &log, const GUID &guid);
std::ostream& operator<<(std::ostream &log, const Triangle &v);
std::ostream& operator<<(std::ostream &log, const IToString &v);
std::ostream& operator<<(std::ostream &log, const IToString *s);

// Templated ostream operators
template<class T>
std::ostream& operator<<(std::ostream &log, const Vector2G<T> &v)
{
    log << "(" << v.x << ", " << v.y << ")";
    return log;
}

template<class T>
std::ostream& operator<<(std::ostream &log, const Vector3G<T> &v)
{
    log << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return log;
}

template<class T>
std::ostream& operator<<(std::ostream &log, const Vector4G<T> &v)
{
    log << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return log;
}

template<class T>
std::ostream& operator<<(std::ostream &log, const QuaternionG<T> &q)
{
    log << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
    return log;
}

template<class T>
std::ostream& operator<<(std::ostream &log, const AARectG<T> &r)
{
    log << "(" << r.GetMin() << ", " <<
                  r.GetMax() << ")";
    return log;
}
template<class T>
std::ostream& operator<<(std::ostream &log, const RectG<T> &r)
{
    log << "(" << r.GetPoints()[0] << ", " <<
                  r.GetPoints()[1] << ", " <<
                  r.GetPoints()[2] << ", " <<
                  r.GetPoints()[3] << ")";
    return log;
}


template<class T>
std::ostream& operator<<(std::ostream &log, const Matrix3G<T> &m)
{
    log << std::endl;
    log << "(" << m.c0[0] << ", " << m.c1[0] << ", " << m.c2[0] << "," << std::endl;
    log << " " << m.c0[1] << ", " << m.c1[1] << ", " << m.c2[1] << "," << std::endl;
    log << " " << m.c0[2] << ", " << m.c1[2] << ", " << m.c2[2] << "," << std::endl;
    return log;
}

template<class T>
std::ostream& operator<<(std::ostream &log, const Matrix4G<T> &m)
{
    log << std::endl;
    log << "(" << m.c0[0] << ", " << m.c1[0] << ", " << m.c2[0] << ", " << m.c3[0] << "," << std::endl;
    log << " " << m.c0[1] << ", " << m.c1[1] << ", " << m.c2[1] << ", " << m.c3[1] << "," << std::endl;
    log << " " << m.c0[2] << ", " << m.c1[2] << ", " << m.c2[2] << ", " << m.c3[2] << "," << std::endl;
    log << " " << m.c0[3] << ", " << m.c1[3] << ", " << m.c2[3] << ", " << m.c3[3] << ")" << std::endl;
    return log;
}

template <class EnumClass, class=TT_ENUM(EnumClass)>
std::ostream& operator<<(std::ostream &log, const EnumClass &e)
{
    log << SCAST<int>(e);
    return log;
}

template <class T, size_t N>
std::ostream &operator<<(std::ostream &log, const std::array<T,N>& a)
{
    log << "{";
    for (int i = 0; i < N; ++i) { log << a[i] << ((i == N-1) ? "" : ", "); }
    log <<"}";
    return log;
}
template <class T>
std::ostream &operator<<(std::ostream &log, const std::queue<T>& q)
{
    log << "(";
    int i = 0;
    std::queue<T> qc = q;
    const int N = q.size();
    while (!qc.empty())
    {
        const auto &x = qc.front(); qc.pop();
        log << x << ((i++ == N-1) ? "" : ", ");
    }
    log <<")";
    return log;
}
template <class K, class V>
std::ostream &operator<<(std::ostream &log, const std::unordered_map<K,V>& m)
{
    log << "{";
    int i = 0;
    const int N = m.size();
    for (const auto &p : m)
    { log << p.first << ": " << p.second << ((i++ == N-1) ? "" : ", "); }
    log <<"}";
    return log;
}
template <class T, class M>
std::ostream &operator<<(std::ostream &log, const std::pair<T,M>& p)
{
    log << "<";
    log << p.first << ", " << p.second;
    log <<">";
    return log;
}

template <class T>
std::ostream &operator<<(std::ostream &log, const List<T> &l)
{
    log << "("; bool first = true;
    for (auto it = l.Begin(); it != l.End(); ++it)
    {
        if (!first) log << ", ";
        log << (*it);
        first = false;
    }
    log <<")";
    return log;
}

template <class T>
std::ostream &operator<<(std::ostream &log, const Array<T> &v)
{
    log << "[";
    for (int i = 0; i < v.Size(); ++i)
    {
        if (i != 0) log << ", ";
        log << v[i];
    }
    log << "]";
    return log;
}

template <class T, class S, class C>
std::ostream &operator<<(std::ostream &log, const Map<T,S,C> &m)
{
    log << "{";
    for (auto it = m.CBegin(); it != m.CEnd(); ++it)
    {
        if (it != m.cbegin()) log << ", ";
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
        if (it != m.cbegin()) log << ", ";
        log << (it->first) << ": " << (it->second);
    }
    log << "}";
    return log;
}

template <class T, class C>
std::ostream &operator<<(std::ostream &log, const Set<T,C> &s)
{
    log << "{";
    for (auto it = s.Begin(); it != s.End(); ++it)
    {
        if (it != s.Begin()) log << ", ";
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
        if (it != s.Begin()) log << ", ";
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
template<class T>
std::istream& operator>>(std::istream &is, Vector2G<T>& v)
{
    char _;
    is >> _ >> v.x >> _ >> v.y >> _;
    return is;
}

template<class T>
std::istream& operator>>(std::istream &is, Vector3G<T>& v)
{
    char _;
    is >> _ >> v.x >> _ >> v.y >> _ >> v.z >> _;
    return is;
}

template<class T>
std::istream& operator>>(std::istream &is, Vector4G<T>& v)
{
    char _;
    is >> _ >> v.x >> _ >> v.y >> _ >> v.z >> _ >> v.w >> _;
    return is;
}

template<class T>
std::istream& operator>>(std::istream &is, QuaternionG<T>& q)
{
    char _;
    is >> _ >> q.x >> _ >> q.y >> _ >> q.z >> _ >> q.w >> _;
    return is;
}

template<class T>
std::istream& operator>>(std::istream &is, AARectG<T>& r)
{
    char _;
    Vector2G<T> minv, maxv;
    is >> _ >> minv >> _ >> maxv >> _;
    r.SetMin(minv);
    r.SetMax(maxv);
    return is;
}

template<class EnumClass, class=TT_ENUM(EnumClass)>
std::istream& operator>>(std::istream &is, EnumClass& e)
{
    int x;
    is >> x;
    e = Cast<EnumClass>(x);
    return is;
}

NAMESPACE_BANG_END

#endif // STREAMOPERATORS_H
