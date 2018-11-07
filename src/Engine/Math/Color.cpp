#include "Bang/Color.h"

#include <cmath>
#include <sstream>

#include "Bang/Color.h"
#include "Bang/Math.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

namespace Bang
{
Color::Color() : Color(0)
{
}

Color::Color(float m) : Color(m, m, m, m)
{
}

Color::Color(const Vector2 &v, float b, float a) : Color(v.x, v.y, b, a)
{
}

Color::Color(const Vector3 &v, float a) : Color(v.x, v.y, v.z, a)
{
}

Color::Color(const Vector4 &v) : Color(v.x, v.y, v.z, v.w)
{
}

Color::Color(float r, float g, float b) : Color(r, g, b, 1)
{
}

Color::Color(float r, float g, float b, float a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

Color::Color(const Color &c, float a) : Color(c.r, c.g, c.b, a)
{
}

Color Color::Lerp(const Color &c1, const Color &c2, float t)
{
    return Color(Vector4::Lerp(
        Vector4(c1.r, c1.g, c1.b, c1.a), Vector4(c2.r, c2.g, c2.b, c2.a), t));
}

Color Color::WithAlpha(float alpha) const
{
    return Color(r, g, b, alpha);
}

Color Color::WithValue(float value) const
{
    return Color(r * value, g * value, b * value, a);
}

Color Color::WithSaturation(float saturation) const
{
    Color c = *this;
    float length = Math::Sqrt(c.r * c.r + c.g * c.g + c.b * c.b);

    c.r = length + (c.r - length) * saturation;
    c.g = length + (c.g - length) * saturation;
    c.b = length + (c.b - length) * saturation;

    return c;
}

String Color::ToStringRgb() const
{
    std::ostringstream oss;
    oss << "(" << r << ", " << g << ", " << b << ")";
    return oss.str();
}

String Color::ToStringRgb255() const
{
    std::ostringstream oss;
    oss << "(" << int(r * 255) << ", " << int(g * 255) << ", " << int(b * 255)
        << ")";
    return oss.str();
}

String Color::ToStringRgba() const
{
    std::ostringstream oss;
    oss << "(" << r << ", " << g << ", " << b << ", " << a << ")";
    return oss.str();
}

String Color::ToStringRgba255() const
{
    std::ostringstream oss;
    oss << "(" << int(r * 255) << ", " << int(g * 255) << ", " << int(b * 255)
        << ", " << int(a * 255) << ")";
    return oss.str();
}

Color Color::ToHSV() const
{
    // In:  RGB([0,1], [0,1], [0,1], [0,1])
    // Out: HSV([0,1], [0,1], [0,1], [0,1])

    float fCMax = Math::Max(Math::Max(r, g), b);
    float fCMin = Math::Min(Math::Min(r, g), b);
    float fDelta = fCMax - fCMin;

    float h, s, v;
    if (fDelta > 0)
    {
        if (fCMax == r)
        {
            h = 60 * (fmod(((g - b) / fDelta), 6));
        }
        else if (fCMax == g)
        {
            h = 60 * (((b - r) / fDelta) + 2);
        }
        else
        {
            h = 60 * (((r - g) / fDelta) + 4);
        }

        if (fCMax > 0)
        {
            s = fDelta / fCMax;
        }
        else
        {
            s = 0;
        }
        v = fCMax;
    }
    else
    {
        h = 0;
        s = 0;
        v = fCMax;
    }

    if (h < 0)
    {
        h = 360 + h;
    }
    h /= 360.0f;

    return Color(h, s, v, a);
}

Color Color::ToRGB() const
{
    // In:  HSV([0,1], [0,1], [0,1], [0,1])
    // Out: RGB([0,1], [0,1], [0,1], [0,1])

    const float &h = r;
    const float &s = g;
    const float &v = b;

    int i = SCAST<int>(Math::Floor(h * 6));
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    float newR, newG, newB;
    switch (i % 6)
    {
        case 0:
            newR = v;
            newG = t;
            newB = p;
            break;
        case 1:
            newR = q;
            newG = v;
            newB = p;
            break;
        case 2:
            newR = p;
            newG = v;
            newB = t;
            break;
        case 3:
            newR = p;
            newG = q;
            newB = v;
            break;
        case 4:
            newR = t;
            newG = p;
            newB = v;
            break;
        case 5:
            newR = v;
            newG = p;
            newB = q;
            break;
        default: newR = newG = newB = 0.0f; break;
    }

    return Color(newR, newG, newB, a);
}

String Color::ToString() const
{
    return "(" + String(r) + ", " + String(g) + ", " + String(b) + ", " +
           String(a) + ")";
}

Vector3 Color::ToVector3() const
{
    return Vector3(r, g, b);
}

Vector4 Color::ToVector4() const
{
    return Vector4(r, g, b, a);
}

Color Color::FromVector3(const Vector3 &v)
{
    return Color(v.x, v.y, v.z, 1);
}

Color Color::FromVector4(const Vector4 &v)
{
    return Color(v.x, v.y, v.z, v.w);
}

Color operator+(const Color &c1, const Color &c2)
{
    return Color(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a * c2.a);
}

Color operator*(const Color &c1, const Color &c2)
{
    return Color(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
}

Color operator-(const Color &c)
{
    return Color(-c.r, -c.g, -c.b, -c.a);
}

Color operator*(const Color &c, float m)
{
    return m * c;
}

Color operator*(float m, const Color &c)
{
    return Color(m * c.r, m * c.g, m * c.b, m * c.a);
}

Color operator/(float m, const Color &c)
{
    return Color(m / c.r, m / c.g, m / c.b, m / c.a);
}

Color operator/(const Color &c, float m)
{
    return Color(c.r / m, c.g / m, c.b / m, c.a / m);
}

Color operator/(const Color &c1, const Color &c2)
{
    return Color(c1.r / c2.r, c1.g / c2.g, c1.b / c2.b, c1.a / c2.a);
}

Color &operator+=(Color &lhs, const Color &rhs)
{
    lhs.r += rhs.r;
    lhs.g += rhs.g;
    lhs.b += rhs.b;
    lhs.a += rhs.a;
    return lhs;
}

Color &operator-=(Color &lhs, const Color &rhs)
{
    lhs.r -= rhs.r;
    lhs.g -= rhs.g;
    lhs.b -= rhs.b;
    lhs.a -= rhs.a;
    return lhs;
}

Color &operator*=(Color &lhs, const Color &rhs)
{
    lhs.r *= rhs.r;
    lhs.g *= rhs.g;
    lhs.b *= rhs.b;
    lhs.a *= rhs.a;
    return lhs;
}

Color &operator/=(Color &lhs, const Color &rhs)
{
    lhs.r /= rhs.r;
    lhs.g /= rhs.g;
    lhs.b /= rhs.b;
    lhs.a /= rhs.a;
    return lhs;
}

Color operator+(float m, const Color &c)
{
    return Color(m + c.r, m + c.g, m + c.b, m + c.a);
}

Color operator+(const Color &c, float m)
{
    return m + c;
}

Color operator-(const Color &c1, const Color &c2)
{
    return Color(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a);
}

Color operator-(float m, const Color &c)
{
    return Color(m - c.r, m - c.g, m - c.b, m - c.a);
}

Color operator-(const Color &c, float m)
{
    return Color(c.r - m, c.g - m, c.b - m, c.a - m);
}

Color &operator+=(Color &lhs, float m)
{
    lhs.r += m;
    lhs.g += m;
    lhs.b += m;
    lhs.a += m;
    return lhs;
}

Color &operator-=(Color &lhs, float m)
{
    lhs.r -= m;
    lhs.g -= m;
    lhs.b -= m;
    lhs.a -= m;
    return lhs;
}

Color &operator*=(Color &lhs, float m)
{
    lhs.r *= m;
    lhs.g *= m;
    lhs.b *= m;
    lhs.a *= m;
    return lhs;
}

Color &operator/=(Color &lhs, float m)
{
    lhs.r /= m;
    lhs.g /= m;
    lhs.b /= m;
    lhs.a /= m;
    return lhs;
}

bool operator==(const Color &lhs, const Color &rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

bool operator!=(const Color &lhs, const Color &rhs)
{
    return !(lhs == rhs);
}

const Color &Color::Red()
{
    static const Color c = Color(1, 0, 0, 1);
    return c;
}
const Color &Color::Orange()
{
    static const Color c = Color(1, 0.5f, 0, 1);
    return c;
}
const Color &Color::Yellow()
{
    static const Color c = Color(1, 1, 0, 1);
    return c;
}
const Color &Color::Green()
{
    static const Color c = Color(0, 1, 0, 1);
    return c;
}
const Color &Color::Turquoise()
{
    static const Color c = Color(1, 1, 0, 1);
    return c;
}
const Color &Color::VeryLightBlue()
{
    static const Color c = Color(0.8f, 0.95f, 1, 1);
    return c;
}
const Color &Color::LightBlue()
{
    static const Color c = Color(0.7f, 0.9f, 1, 1);
    return c;
}
const Color &Color::Blue()
{
    static const Color c = Color(0, 0, 1, 1);
    return c;
}
const Color &Color::DarkBlue()
{
    static const Color c = Color(0, 0, 0.6f, 1);
    return c;
}
const Color &Color::Purple()
{
    static const Color c = Color(0.5f, 0, 1, 1);
    return c;
}
const Color &Color::Pink()
{
    static const Color c = Color(1, 0, 1, 1);
    return c;
}
const Color &Color::Black()
{
    static const Color c = Color(0, 0, 0, 1);
    return c;
}
const Color &Color::LightGray()
{
    static const Color c = Color(0.8f, 0.8f, 0.8f, 1);
    return c;
}
const Color &Color::DarkGray()
{
    static const Color c = Color(0.3f, 0.3f, 0.3f, 1);
    return c;
}
const Color &Color::Gray()
{
    static const Color c = Color(0.5f, 0.5f, 0.5f, 1);
    return c;
}
const Color &Color::White()
{
    static const Color c = Color(1, 1, 1, 1);
    return c;
}
const Color &Color::Zero()
{
    static const Color c = Color(0, 0, 0, 0);
    return c;
}
const Color &Color::One()
{
    static const Color c = Color(1, 1, 1, 1);
    return c;
}
}  // namespace Bang
