#ifndef TIME_H
#define TIME_H

#include <stdint.h>

#include "Bang/BangDefines.h"

namespace Bang
{
class Time
{
public:
    Time() = default;
    explicit Time(uint64_t timeNanos);

    ~Time() = default;

    void SetSeconds(double seconds);
    void SetMillis(uint64_t millis);
    void SetNanos(uint64_t nanos);
    void SetInfinity();
    void SetZero();

    double GetSeconds() const;
    uint64_t GetMillis() const;
    uint64_t GetNanos() const;

    static Time Zero();
    static Time Infinity();

    static Time Seconds(double seconds);
    static Time Millis(uint64_t millis);
    static Time Nanos(uint64_t nanos);

    static Time GetNow();
    static Time GetInit();
    static Time GetEllapsed();
    static Time GetDeltaTime();
    static Time GetPassedTimeSince(Time refTime);

private:
    uint64_t m_timeNanos = 0;
};

inline Time operator*(const Time &lhs, const Time &rhs)
{
    return Time(lhs.GetNanos() * rhs.GetNanos());
}
inline Time operator/(const Time &lhs, const Time &rhs)
{
    return Time(lhs.GetNanos() / rhs.GetNanos());
}
inline Time operator-(const Time &lhs, const Time &rhs)
{
    return Time(lhs.GetNanos() - rhs.GetNanos());
}
inline Time operator+(const Time &lhs, const Time &rhs)
{
    return Time(lhs.GetNanos() + rhs.GetNanos());
}
inline bool operator==(const Time &lhs, const Time &rhs)
{
    return (lhs.GetNanos() == rhs.GetNanos());
}
inline bool operator!=(const Time &lhs, const Time &rhs)
{
    return (lhs.GetNanos() != rhs.GetNanos());
}
inline bool operator<(const Time &lhs, const Time &rhs)
{
    return (lhs.GetNanos() < rhs.GetNanos());
}
inline bool operator<=(const Time &lhs, const Time &rhs)
{
    return (lhs.GetNanos() <= rhs.GetNanos());
}
inline bool operator>(const Time &lhs, const Time &rhs)
{
    return (lhs.GetNanos() > rhs.GetNanos());
}
inline bool operator>=(const Time &lhs, const Time &rhs)
{
    return (lhs.GetNanos() >= rhs.GetNanos());
}
inline Time &operator+=(Time &lhs, const Time &rhs)
{
    lhs = (lhs + rhs);
    return lhs;
}
inline Time &operator-=(Time &lhs, const Time &rhs)
{
    lhs = (lhs - rhs);
    return lhs;
}
inline Time &operator*=(Time &lhs, const Time &rhs)
{
    lhs = (lhs * rhs);
    return lhs;
}
inline Time &operator/=(Time &lhs, const Time &rhs)
{
    lhs = (lhs / rhs);
    return lhs;
}
template <class T>
inline Time operator+(const Time &lhs, const T &rhs)
{
    return Time(lhs.GetNanos() + rhs);
}
template <class T>
inline Time operator+(const T &lhs, const Time &rhs)
{
    return Time(rhs.GetNanos() + lhs);
}
template <class T>
inline Time operator-(const Time &lhs, const T &rhs)
{
    return Time(lhs.GetNanos() - rhs);
}
template <class T>
inline Time operator-(const T &lhs, const Time &rhs)
{
    return Time(rhs.GetNanos() - lhs);
}
template <class T>
inline Time operator*(const Time &lhs, const T &rhs)
{
    return Time(lhs.GetNanos() * rhs);
}
template <class T>
inline Time operator*(const T &lhs, const Time &rhs)
{
    return Time(rhs.GetNanos() * lhs);
}
template <class T>
inline Time operator/(const Time &lhs, const T &rhs)
{
    return Time(lhs.GetNanos() / rhs);
}
template <class T>
inline Time operator/(const T &lhs, const Time &rhs)
{
    return Time(rhs.GetNanos() / lhs);
}
template <class T>
inline Time &operator+=(Time &lhs, const T &rhs)
{
    lhs.SetNanos(lhs.GetNanos() + rhs);
    return lhs;
}
template <class T>
inline Time &operator-=(Time &lhs, const T &rhs)
{
    lhs.SetNanos(lhs.GetNanos() - rhs);
    return lhs;
}
template <class T>
inline Time &operator*=(Time &lhs, const T &rhs)
{
    lhs.SetNanos(lhs.GetNanos() * rhs);
    return lhs;
}
template <class T>
inline Time &operator/=(Time &lhs, const T &rhs)
{
    lhs.SetNanos(lhs.GetNanos() / rhs);
    return lhs;
}
}  // namespace Bang

#endif  // TIME_H
