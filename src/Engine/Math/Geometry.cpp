#include "Bang/Geometry.h"

#include <algorithm>
#include <array>
#include <vector>

#include "Bang/AABox.h"
#include "Bang/Array.tcc"
#include "Bang/Axis.h"
#include "Bang/Box.h"
#include "Bang/Math.h"
#include "Bang/Plane.h"
#include "Bang/Polygon.h"
#include "Bang/Polygon2D.h"
#include "Bang/Quad.h"
#include "Bang/Ray.h"
#include "Bang/Ray2D.h"
#include "Bang/Segment.h"
#include "Bang/Segment2D.h"
#include "Bang/Sphere.h"
#include "Bang/Triangle.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"

using namespace Bang;

float Geometry::GetPointToLineDistance2D(const Vector2 &point,
                                         const Vector2 &lineP0,
                                         const Vector2 &lineP1)
{
    Vector2 lineP0ToPoint = (point - lineP0);
    Vector2 lineDir = (lineP1 - lineP0).NormalizedSafe();
    float lengthOfPointToLineP0OnLine = Vector2::Dot(lineP0ToPoint, lineDir);
    float a = lineP0ToPoint.SqLength();
    float b = Math::Pow(lengthOfPointToLineP0OnLine, 2.0f);
    float distance = Math::Sqrt(Math::Abs(a - b));
    return distance;
}

void Geometry::IntersectSegment2DSegment2D(const Segment2D &segment0,
                                           const Segment2D &segment1,
                                           bool *intersected,
                                           Vector2 *intersPoint)
{
    const Vector2 &p0 = segment0.GetOrigin();
    const Vector2 &p1 = segment0.GetDestiny();
    const Vector2 &q0 = segment1.GetOrigin();
    const Vector2 &q1 = segment1.GetDestiny();

    Geometry::Orientation orient0 = Geometry::GetOrientation(p0, p1, q0);
    Geometry::Orientation orient1 = Geometry::GetOrientation(p0, p1, q1);
    if (orient0 == orient1 && orient0 != Geometry::Orientation::ZERO)
    {
        *intersected = false;
        return;
    }

    Geometry::Orientation orient2 = Geometry::GetOrientation(q0, q1, p0);
    Geometry::Orientation orient3 = Geometry::GetOrientation(q0, q1, p1);
    if (orient2 == orient3 && orient2 != Geometry::Orientation::ZERO)
    {
        *intersected = false;
        return;
    }

    const float x1 = p0.x, x2 = p1.x, x3 = q0.x, x4 = q1.x;
    const float y1 = p0.y, y2 = p1.y, y3 = q0.y, y4 = q1.y;

    const float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (d == 0)
    {
        *intersected = false;
        return;
    }

    const float pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
    const float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
    const float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

    *intersected = true;
    *intersPoint = Vector2(x, y);
}

void Geometry::IntersectRay2DSegment2D(const Ray2D &ray,
                                       const Segment2D &segment,
                                       bool *intersected,
                                       Vector2 *intersPoint)
{
    float maxSqDist =
        Math::Max(Vector2::SqDistance(ray.GetOrigin(), segment.GetOrigin()),
                  Vector2::SqDistance(ray.GetOrigin(), segment.GetDestiny()));
    Segment2D raySegment(ray.GetOrigin(),
                         ray.GetOrigin() + (maxSqDist * ray.GetDirection()));
    Geometry::IntersectSegment2DSegment2D(
        segment, raySegment, intersected, intersPoint);
}

void Geometry::IntersectRayPlane(const Ray &ray,
                                 const Plane &plane,
                                 bool *intersected,
                                 float *distanceFromIntersectionToRayOrigin)
{
    const Vector3 &planeNormal = plane.GetNormal();
    float dot = Vector3::Dot(planeNormal, ray.GetDirection());
    if (Math::Abs(dot) > 0.001f)
    {
        *distanceFromIntersectionToRayOrigin =
            Vector3::Dot(plane.GetPoint() - ray.GetOrigin(), planeNormal) / dot;
        *intersected = (*distanceFromIntersectionToRayOrigin >= 0.0f);
    }
    else
    {
        *intersected = false;
    }
}

void Geometry::IntersectRayPlane(const Ray &ray,
                                 const Plane &plane,
                                 bool *intersected,
                                 Vector3 *intersectionPoint)
{
    float t;
    Geometry::IntersectRayPlane(ray, plane, intersected, &t);
    *intersected = *intersected && (t >= 0.0f);
    *intersectionPoint = *intersected ? ray.GetPoint(t) : ray.GetOrigin();
}

void Geometry::IntersectSegmentPlane(const Segment &segment,
                                     const Plane &plane,
                                     bool *intersected,
                                     Vector3 *intersectionPoint)
{
    float intDist;
    const Vector3 segmDir = segment.GetDirection();
    Ray segmentRay(segment.GetOrigin(), segmDir);
    Geometry::IntersectRayPlane(segmentRay, plane, intersected, &intDist);

    *intersected = *intersected && (intDist <= segment.GetLength());
    if (*intersected)
    {
        *intersectionPoint = segment.GetOrigin() + (intDist * segmDir);
    }
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/
// minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
void Geometry::IntersectRayAABox(const Ray &ray,
                                 const AABox &aaBox,
                                 bool *intersected,
                                 float *intersectionDistance)
{
    float tmin = (aaBox.GetMin().x - ray.GetOrigin().x) / ray.GetDirection().x;
    float tmax = (aaBox.GetMax().x - ray.GetOrigin().x) / ray.GetDirection().x;

    if (tmin > tmax)
    {
        std::swap(tmin, tmax);
    }

    float tymin = (aaBox.GetMin().y - ray.GetOrigin().y) / ray.GetDirection().y;
    float tymax = (aaBox.GetMax().y - ray.GetOrigin().y) / ray.GetDirection().y;

    if (tymin > tymax)
    {
        std::swap(tymin, tymax);
    }

    if ((tmin > tymax) || (tymin > tmax))
    {
        *intersected = false;
        return;
    }

    if (tymin > tmin)
    {
        tmin = tymin;
    }

    if (tymax < tmax)
    {
        tmax = tymax;
    }

    float tzmin = (aaBox.GetMin().z - ray.GetOrigin().z) / ray.GetDirection().z;
    float tzmax = (aaBox.GetMax().z - ray.GetOrigin().z) / ray.GetDirection().z;

    if (tzmin > tzmax)
    {
        std::swap(tzmin, tzmax);
    }

    if ((tmin > tzmax) || (tzmin > tmax))
    {
        *intersected = false;
        return;
    }

    if (tzmin > tmin)
    {
        tmin = tzmin;
    }

    if (tzmax < tmax)
    {
        tmax = tzmax;
    }

    *intersected = true;
    *intersectionDistance = tmin;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/
// minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
void Geometry::IntersectRaySphere(const Ray &ray,
                                  const Sphere &sphere,
                                  bool *intersected,
                                  Vector3 *intersectionPoint)
{
    const Vector3 rayOriginToSphereCenter =
        sphere.GetCenter() - ray.GetOrigin();

    const float sphereRadius2 = sphere.GetRadius() * sphere.GetRadius();
    const float tca = Vector3::Dot(rayOriginToSphereCenter, ray.GetDirection());
    // if (tca < 0)  { *intersected = false; return; }

    const float d2 = rayOriginToSphereCenter.SqLength() - tca * tca;
    if (d2 > sphereRadius2)
    {
        *intersected = false;
        return;
    }

    const float thc = Math::Sqrt(sphereRadius2 - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if (t0 > t1)
    {
        std::swap(t0, t1);
    }

    if (t0 < 0)
    {
        t0 = t1;
        if (t0 < 0)
        {
            *intersected = false;
            return;
        }
    }

    *intersected = true;
    *intersectionPoint = ray.GetPoint(t0);
}

void Geometry::RayLineClosestPoints(const Ray &ray,
                                    const Vector3 &linePoint,
                                    const Vector3 &lineDirection,
                                    Vector3 *pointOnRay,
                                    Vector3 *pointOnLine)
{
    Vector3 lineToRayPerp =
        Vector3::Cross(ray.GetDirection(), lineDirection).NormalizedSafe();

    if (pointOnRay)
    {
        bool intersected;
        Vector3 planeBitangent = lineDirection.NormalizedSafe();
        Plane plane(linePoint, Vector3::Cross(lineToRayPerp, planeBitangent));
        Geometry::IntersectRayPlane(ray, plane, &intersected, pointOnRay);
    }

    if (pointOnLine)
    {
        Ray lineRay(linePoint, lineDirection);
        Vector3 planeBitangent = ray.GetDirection();
        Plane plane(ray.GetOrigin(),
                    Vector3::Cross(lineToRayPerp, planeBitangent));

        float t = 0.0f;
        bool intersected = false;
        Geometry::IntersectRayPlane(lineRay, plane, &intersected, &t);
        if (intersected)
        {
            *pointOnLine = linePoint + (t * lineDirection.NormalizedSafe());
        }
    }
}

void Geometry::IntersectSegmentPolygon(const Segment &segment,
                                       const Polygon &poly,
                                       bool *intersected,
                                       Vector3 *intersection)
{
    bool intersectedWithPlane;
    IntersectRayPlane(Ray(segment.GetOrigin(), segment.GetDirection()),
                      poly.GetPlane(),
                      &intersectedWithPlane,
                      intersection);

    *intersected = false;
    if (intersectedWithPlane)
    {
        float intSegDist =
            Vector3::Distance(*intersection, segment.GetOrigin());
        if (intSegDist <= segment.GetLength())
        {
            // Segment intersects with plane, but is it inside the polygon?
            Axis3D axisToProj;
            Vector3 apn =
                Vector3::Abs(poly.GetNormal());  // To know where to project
            if (apn.x > apn.y && apn.x > apn.z)
            {
                axisToProj = Axis3D::X;
            }
            else if (apn.y > apn.x && apn.y > apn.z)
            {
                axisToProj = Axis3D::Y;
            }
            else
            {
                axisToProj = Axis3D::Z;
            }

            Polygon2D projectedPolygon = poly.ProjectedOnAxis(axisToProj);
            Vector2 projectedIntersPoint =
                intersection->ProjectedOnAxis(axisToProj);
            // Debug_Log("===========================");
            // Debug_Peek(poly.GetPoints());
            // Debug_Peek(*intersection);
            // Debug_Peek(projectedPolygon.GetPoints());
            // Debug_Peek(projectedIntersPoint);
            if (projectedPolygon.Contains(projectedIntersPoint))
            {
                *intersected = true;
                // DebugRenderer::RenderPoint(*intersection, Color::Green,
                //                            20.0f, 20.0f, true);
            }
            else
            {
                // DebugRenderer::RenderPoint(*intersection, Color::White,
                //                            20.0f, 20.0f, true);
            }
            // Debug_Peek(axisToProj);
            // Debug_Peek(*intersected);
            // Debug_Log("===========================");
        }
        // else
        // {
        //     DebugRenderer::RenderPoint(*intersection, Color::Black,
        //                                20.0f, 20.0f, true);
        // }
    }
}

void Geometry::IntersectSegmentBox(const Segment &segment,
                                   const Box &box,
                                   bool *intersected,
                                   Vector3 *intersectionPoint,
                                   Vector3 *intersectionNormal)
{
    *intersected = false;

    Vector3 extX = box.GetExtentX();
    Vector3 extY = box.GetExtentY();
    Vector3 extZ = box.GetExtentZ();
    Vector3 extXNorm = extX.NormalizedSafe();
    Vector3 extYNorm = extY.NormalizedSafe();
    Vector3 extZNorm = extZ.NormalizedSafe();

    std::array<Plane, 6> planes;
    planes[0] = Plane(box.GetCenter() + extX, extXNorm);
    planes[1] = Plane(box.GetCenter() - extX, -extXNorm);
    planes[2] = Plane(box.GetCenter() + extY, extYNorm);
    planes[3] = Plane(box.GetCenter() - extY, -extYNorm);
    planes[4] = Plane(box.GetCenter() + extZ, extZNorm);
    planes[5] = Plane(box.GetCenter() - extZ, -extZNorm);

    float closestPlaneIntersectionSqDist = Math::Infinity<float>();
    for (int i = 0; i < 6; ++i)
    {
        const Plane &plane = planes[i];

        bool planeIntersected;
        Vector3 planeIntPoint;
        Geometry::IntersectSegmentPlane(
            segment, plane, &planeIntersected, &planeIntPoint);
        if (planeIntersected)
        {
            std::array<Orientation, 6> oriPlanes;
            for (int j = 0; j < 6; ++j)
            {
                oriPlanes[j] =
                    Geometry::GetOrientation(planeIntPoint, planes[j]);
            }

            float sqDist =
                Vector3::SqDistance(planeIntPoint, segment.GetOrigin());
            if (sqDist < closestPlaneIntersectionSqDist)
            {
                // Is the point enclosed by the planes on the other axes?
                const Orientation &ori0 = oriPlanes[(i + 1) % 6];
                const Orientation &ori1 = oriPlanes[(i + 2) % 6];
                const Orientation &ori2 = oriPlanes[(i + 3) % 6];
                const Orientation &ori3 = oriPlanes[(i + 4) % 6];
                const Orientation &ori4 = oriPlanes[(i + 5) % 6];

                if (ori0 == Orientation::NEGATIVE &&
                    ori1 == Orientation::NEGATIVE &&
                    ori2 == Orientation::NEGATIVE &&
                    ori3 == Orientation::NEGATIVE &&
                    ori4 == Orientation::NEGATIVE)
                {
                    // Vector3 intDir = (planeIntPoint - segment.GetOrigin());
                    float normalSign = 1.0f;  // Math::Sign(
                                              // Vector3::Dot(segmDir,
                                              // plane.GetNormal()) );
                    closestPlaneIntersectionSqDist = sqDist;
                    *intersected = true;
                    *intersectionPoint = planeIntPoint;
                    *intersectionNormal = normalSign * plane.GetNormal();
                }
            }
        }
    }
}

Array<Vector3> Geometry::IntersectSegmentPolygon(const Segment &segment,
                                                 const Polygon &poly)
{
    Array<Vector3> result;
    bool intB;
    Vector3 intP;
    Geometry::IntersectSegmentPolygon(segment, poly, &intB, &intP);
    if (intB)
    {
        result.PushBack(intP);
    }
    return result;
}

Array<Vector3> Geometry::IntersectPolygonPolygon(const Polygon &poly0,
                                                 const Polygon &poly1)
{
    Array<Vector3> intersectionPoints;
    const std::array<Polygon, 2> polys = {{poly0, poly1}};
    for (uint pi = 0; pi < 2; ++pi)
    {
        const Polygon &p0 = polys[pi];
        const Polygon &p1 = polys[1 - pi];
        for (uint i = 0; i < p0.GetPoints().Size(); ++i)
        {
            Segment segment(p0.GetPoint(i),
                            p0.GetPoint((i + 1) % p0.GetPoints().Size()));
            intersectionPoints.PushBack(
                Geometry::IntersectSegmentPolygon(segment, p1));
        }
    }
    return intersectionPoints;
}

// http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
void Geometry::IntersectRayTriangle(const Ray &ray,
                                    const Triangle &triangle,
                                    bool *intersected,
                                    float *distanceFromRayOriginToIntersection)
{
    float &t = *distanceFromRayOriginToIntersection;
    const Vector3 &rayOrig = ray.GetOrigin();
    const Vector3d rayDir(ray.GetDirection());
    const Vector3 &v0 = triangle.GetPoint(0);
    const Vector3 &v1 = triangle.GetPoint(1);
    const Vector3 &v2 = triangle.GetPoint(2);

    Vector3d v1v0(v1 - v0);
    Vector3d v2v0(v2 - v0);

    Vector3d h(Vector3::Cross(rayDir, v2v0));
    double a = Vector3d::Dot(v1v0, h);

    constexpr double Epsilon = 1e-8;
    if (a > -Epsilon && a < Epsilon)
    {
        *intersected = false;
        return;
    }

    double f = 1.0 / a;
    Vector3d s(rayOrig - v0);
    double u = f * Vector3d::Dot(s, h);

    if (u < 0.0 || u > 1.0)
    {
        *intersected = false;
        return;
    }

    Vector3d q = Vector3d::Cross(s, v1v0);
    double v = f * Vector3d::Dot(rayDir, q);

    if (v < 0.0 || u + v > 1.0)
    {
        *intersected = false;
        return;
    }

    // At this stage we can compute t to find out where
    // the intersection point is on the line
    t = (f * Vector3d::Dot(v2v0, q));
    if (t < Epsilon)
    {
        *intersected = false;
        return;
    }

    *intersected = true;
}

void Geometry::IntersectRayTriangle(const Ray &ray,
                                    const Triangle &triangle,
                                    bool *intersected,
                                    Vector3 *intersectionPoint)
{
    float t;
    Geometry::IntersectRayTriangle(ray, triangle, intersected, &t);
    *intersected = *intersected && (t >= 0.0f);
    *intersectionPoint = *intersected ? ray.GetPoint(t) : ray.GetOrigin();
}

void Geometry::IntersectSegmentTriangle(const Segment &segment,
                                        const Triangle &triangle,
                                        bool *intersected,
                                        Vector3 *intersectionPoint)
{
    Ray ray(segment.GetOrigin(), segment.GetDirection());

    float t;
    Geometry::IntersectRayTriangle(ray, triangle, intersected, &t);

    const float segmentLength = segment.GetLength();
    *intersected = *intersected && (t >= 0.0f) && (t <= segmentLength);
    *intersectionPoint = *intersected ? ray.GetPoint(t) : ray.GetOrigin();
}

Array<Vector3> Geometry::IntersectBoxBox(const std::array<Quad, 6> &box0,
                                         const std::array<Quad, 6> &box1)
{
    Array<Vector3> result;
    for (const Quad &q0 : box0)
    {
        for (const Quad &q1 : box1)
        {
            result.PushBack(Geometry::IntersectQuadQuad(q0, q1));

            // Points of q0 inside box1
            for (const Vector3 &q0p : q0.GetPoints())
            {
                if (Geometry::IsPointInsideBox(q0p, box1))
                {
                    result.PushBack(q0p);
                }
            }

            // Points of q1 inside box0
            for (const Vector3 &q1p : q1.GetPoints())
            {
                if (Geometry::IsPointInsideBox(q1p, box0))
                {
                    result.PushBack(q1p);
                }
            }
        }
    }
    return result;
}

bool Geometry::IsPointInsideBox(const Vector3 &p,
                                const std::array<Quad, 6> &box)
{
    return Geometry::IsPointInsideBox(p,
                                      box[0].GetPlane(),
                                      box[1].GetPlane(),
                                      box[2].GetPlane(),
                                      box[3].GetPlane(),
                                      box[4].GetPlane(),
                                      box[5].GetPlane());
}

bool Geometry::IsPointInsideBox(const Vector3 &p,
                                const Plane &boxTopPlane,
                                const Plane &boxBotPlane,
                                const Plane &boxLeftPlane,
                                const Plane &boxRightPlane,
                                const Plane &boxFrontPlane,
                                const Plane &boxBackPlane)
{
    return Geometry::GetOrientation(p, boxTopPlane) == Orientation::NEGATIVE &&
           Geometry::GetOrientation(p, boxBotPlane) == Orientation::NEGATIVE &&
           Geometry::GetOrientation(p, boxLeftPlane) == Orientation::NEGATIVE &&
           Geometry::GetOrientation(p, boxRightPlane) ==
               Orientation::NEGATIVE &&
           Geometry::GetOrientation(p, boxFrontPlane) ==
               Orientation::NEGATIVE &&
           Geometry::GetOrientation(p, boxBackPlane) == Orientation::NEGATIVE;
}

Array<Vector3> Geometry::IntersectQuadQuad(const Quad &quad0, const Quad &quad1)
{
    return Geometry::IntersectPolygonPolygon(quad0.ToPolygon(),
                                             quad1.ToPolygon());
}

Array<Vector3> Geometry::IntersectQuadAABox(const Quad &quad,
                                            const AABox &aaBox,
                                            bool onlyBoundaries)
{
    // Do all combinations of quad-quad, similar to QuadQuad
    Array<Vector3> foundIntersectionPoints;
    const std::array<Vector3, 4> quadPoints = quad.GetPoints();
    const std::array<Quad, 6> aaBoxQuads = aaBox.GetQuads();
    for (const Quad &aaBoxQuad : aaBoxQuads)
    {
        Array<Vector3> inters = Geometry::IntersectQuadQuad(quad, aaBoxQuad);
        foundIntersectionPoints.PushBack(inters);

        if (!onlyBoundaries)
        {
            for (const Vector3 &p : quadPoints)
            {
                if (aaBox.Contains(p))
                {
                    foundIntersectionPoints.PushBack(p);
                }
            }
        }
    }

    return foundIntersectionPoints;
}

Geometry::Orientation Geometry::GetOrientation(const Vector2 &lineP0,
                                               const Vector2 &lineP1,
                                               const Vector2 &point)
{
    float det = ((point.x - lineP0.x) * (lineP1.y - lineP0.y)) -
                ((point.y - lineP0.y) * (lineP1.x - lineP0.x));
    if (Math::Abs(det) < ALMOST_ZERO)
    {
        return Orientation::ZERO;
    }
    return (det > 0) ? Orientation::POSITIVE : Orientation::NEGATIVE;
}

Geometry::Orientation Geometry::GetOrientation(const Vector3 &point,
                                               const Plane &plane)
{
    float dot = Vector3::Dot(plane.GetNormal(), (point - plane.GetPoint()));
    if (Math::Abs(dot) < ALMOST_ZERO)
    {
        return Orientation::ZERO;
    }
    return (dot > 0) ? Orientation::POSITIVE : Orientation::NEGATIVE;
}

Vector3 Geometry::RayClosestPointTo(const Ray &ray, const Vector3 &point)
{
    bool intersected;
    Vector3 intersection;
    Geometry::IntersectRayPlane(
        ray, Plane(point, ray.GetDirection()), &intersected, &intersection);
    return intersected ? intersection : ray.GetOrigin();
}

Vector3 Geometry::PointProjectedToSphere(const Vector3 &point,
                                         const Sphere &sphere)
{
    Vector3 closestRayPointToSphereV = sphere.GetCenter() - point;
    Vector3 closestRayPointToSphereDir = closestRayPointToSphereV.Normalized();
    return sphere.GetCenter() - closestRayPointToSphereDir * sphere.GetRadius();
}
