#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include "Bang/Bang.h"

namespace Bang
{
enum class HorizontalAlignment
{
    LEFT = 0,
    CENTER,
    RIGHT
};
enum class VerticalAlignment
{
    TOP = 0,
    CENTER,
    BOT
};
enum class VerticalSide
{
    TOP = 0,
    BOT
};
enum class HorizontalSide
{
    LEFT = 0,
    RIGHT
};
enum class Side
{
    LEFT = 0,
    RIGHT,
    TOP,
    BOT
};
}

#endif  // ALIGNMENT_H
