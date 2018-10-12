#include "Bang/StreamOperators.h"

#include "Bang/GUID.h"
#include "Bang/Path.h"
#include "Bang/Quad.h"
#include "Bang/AABox.h"
#include "Bang/Color.h"
#include "Bang/String.h"
#include "Bang/Triangle.h"
#include "Bang/IToString.h"
#include "Bang/ComplexRandom.h"

NAMESPACE_BANG_BEGIN

std::ostream& operator<<(std::ostream &log, const Time &t)
{
    log << t.GetNanos();
    return log;
}

std::ostream& operator<<(std::ostream &log, const Path &p)
{
    log << p.GetAbsolute();
    return log;
}

std::ostream& operator<<(std::ostream &log, const Color &c)
{
    log << "(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
    return log;
}

std::ostream& operator<<(std::ostream &log, const Quad &q)
{
    log << "(" << q[0] << ", "  << q[1] << ", " << q[2] << ", " << q[3] << ")";
    return log;
}

std::ostream& operator<<(std::ostream &log, const Triangle &t)
{
    log << "(" << t[0] << ", "  << t[1] << ", " << t[2] << ")";
    return log;
}

std::ostream& operator<<(std::ostream &log, const AABox &b)
{
    log << "(" << b.GetMin() << ", " << b.GetMax() << ")";
    return log;
}

std::ostream &operator<<(std::ostream &log, const IToString *s)
{
    if (!s)
    {
        log << "Null";
    }
    else
    {
        log << s->ToString();
    }
    return log;
}
//

std::ostream &operator<<(std::ostream &log, const IToString &v)
{
    log << (&v);
    return log;
}

std::istream &operator>>(std::istream &is, Time &t)
{
    uint64_t timeNanos;
    is >> timeNanos;
    t.SetNanos(timeNanos);
    return is;
}

std::istream &operator>>(std::istream &is, Color &c)
{
    char _;
    is >> _ >> c.r >> _ >> c.g >> _ >> c.b >> _ >> c.a >> _;
    return is;
}

std::istream &operator>>(std::istream &is, Path &p)
{
    String str;
    is >> str;
    p.SetPath(str);
    return is;
}

std::istream &operator>>(std::istream &is, GUID &guid)
{
    guid.operator>>(is);
    return is;
}

std::ostream &operator<<(std::ostream &log, const GUID &guid)
{
    log << guid.GetTimeGUID() << " " <<
           guid.GetRandGUID() << " " <<
           guid.GetEmbeddedResourceGUID();
    return log;
}

std::istream &operator>>(std::istream &is, ComplexRandom &cr)
{
    uint type = 0;
    float constantValue = 0.0f;
    float minRangeValue = 0.0f;
    float maxRangeValue = 0.0f;
    is >> constantValue;
    is >> minRangeValue;
    is >> maxRangeValue;
    is >> type;

    cr.SetConstantValue(constantValue);
    cr.SetRangeValues(minRangeValue, maxRangeValue);

    ComplexRandomType crType = SCAST<ComplexRandomType>(type);
    cr.SetType(crType);

    return is;
}

std::ostream &operator<<(std::ostream &log, const ComplexRandom &cr)
{
    log << cr.GetConstantValue() << " ";
    log << cr.GetMinRangeValue() << " ";
    log << cr.GetMaxRangeValue() << " ";
    log << SCAST<uint>(cr.GetType()) << " ";

    return log;
}

NAMESPACE_BANG_END
