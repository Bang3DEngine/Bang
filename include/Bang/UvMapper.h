#ifndef UVMAPPER_H
#define UVMAPPER_H

#include "Bang/Array.h"
#include "Bang/Bang.h"
#include "Bang/Vector2.h"

namespace Bang
{
class UvMapper
{
public:
    UvMapper() = delete;
    ~UvMapper() = delete;

    static Array<Vector2> CubicProjection(const Mesh *mesh);
};
}

#endif  // UVMAPPER_H
