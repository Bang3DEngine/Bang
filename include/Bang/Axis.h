#ifndef AXIS_H
#define AXIS_H

#include "Bang/Bang.h"
#include "Bang/Flags.h"

namespace Bang
{
enum class Axis
{
    VERTICAL,
    HORIZONTAL
};

enum class Axis2D
{
    X,
    Y
};

enum class Axis3D
{
    X,
    Y,
    Z
};

enum class Axis3DExt
{
    X,
    Y,
    Z,
    XY,
    XZ,
    YZ,
    XYZ
};
}

#endif  // AXIS_H
