#include "Bang/UvMapper.h"

#include "BangMath/AARect.h"
#include "Bang/Mesh.h"

using namespace Bang;

Array<Vector2> UvMapper::CubicProjection(const Mesh *mesh)
{
    AARect uvsRect;
    Array<Vector2> uvs;

    // Project each triangle on cube
    for (uint triId = 0; triId < mesh->GetNumTriangles(); ++triId)
    {
        const Triangle tri = mesh->GetTriangle(triId);
        for (uint i = 0; i < 3; ++i)
        {
            const Vector3 &p = tri.GetPoint(i);
            Vector2 uv = Vector2(p.x, p.z);
            uvs.PushBack(uv);

            uvsRect.AddPoint(uv);
        }
    }

    // Normalize uvs to [0,1] range
    for (Vector2 &uv : uvs)
    {
        for (uint i = 0; i < 2; ++i)
        {
            uv[i] = Math::Map(
                uv[i], uvsRect.GetMin()[i], uvsRect.GetMax()[i], 0.0f, 1.0f);
        }
    }
    return uvs;
}
