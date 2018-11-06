#ifndef COLOR_H
#define COLOR_H

#include "Bang/BangDefines.h"
#include "Bang/String.h"

namespace Bang
{
class Color
{
public:
    static const Color &Red();
    static const Color &Orange();
    static const Color &Yellow();
    static const Color &Green();
    static const Color &Turquoise();
    static const Color &VeryLightBlue();
    static const Color &LightBlue();
    static const Color &Blue();
    static const Color &DarkBlue();
    static const Color &Purple();
    static const Color &Pink();
    static const Color &Black();
    static const Color &Gray();
    static const Color &LightGray();
    static const Color &DarkGray();
    static const Color &White();
    static const Color &Zero();
    static const Color &One();

    float r, g, b, a;

    Color();
    explicit Color(float m);
    explicit Color(const Vector2 &v, float b = 0, float a = 1);
    explicit Color(const Vector3 &v, float a = 1);
    explicit Color(const Vector4 &v);
    explicit Color(float r, float g, float b);
    explicit Color(float r, float g, float b, float a);
    explicit Color(const Color &c, float a);

    /**
     * @brief If progression == 0, returns v1.
     *        If progression == 1, returns v2.
     *        If 0 < progression < 1, returns a linear interpolation between v1
     * and v2.
     * @param v1 First Color
     * @param v2 Second Color
     * @param v2 A float between 0 and 1 indicating the progression.
     * @return
     */
    static Color Lerp(const Color &v1, const Color &v2, float progression);

    Color WithAlpha(float alpha) const;
    Color WithValue(float value) const;
    Color WithSaturation(float saturation) const;

    String ToStringRgb() const;
    String ToStringRgb255() const;
    String ToStringRgba() const;
    String ToStringRgba255() const;

    Color ToHSV() const;
    Color ToRGB() const;
    String ToString() const;
    Vector3 ToVector3() const;
    Vector4 ToVector4() const;

    static Color FromVector3(const Vector3 &v);
    static Color FromVector4(const Vector4 &v);
};

Color operator+(float m, const Color &v);
Color operator+(const Color &v, float m);
Color operator+(const Color &v1, const Color &v2);

Color operator-(float m, const Color &v);
Color operator-(const Color &v, float m);
Color operator-(const Color &v1, const Color &v2);
Color operator-(const Color &v);

Color operator*(float m, const Color &v);
Color operator*(const Color &v, float m);
Color operator*(const Color &v1, const Color &v2);

Color operator/(float m, const Color &v);
Color operator/(const Color &v, float m);
Color operator/(const Color &v1, const Color &v2);

bool operator==(const Color &lhs, const Color &rhs);
bool operator!=(const Color &lhs, const Color &rhs);
Color &operator+=(Color &lhs, const Color &rhs);
Color &operator-=(Color &lhs, const Color &rhs);
Color &operator*=(Color &lhs, const Color &rhs);
Color &operator/=(Color &lhs, const Color &rhs);
Color &operator+=(Color &lhs, float m);
Color &operator-=(Color &lhs, float m);
Color &operator*=(Color &lhs, float m);
Color &operator/=(Color &lhs, float m);
}

#endif  // COLOR_H
