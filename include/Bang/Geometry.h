#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD class Ray;
FORWARD class Quad;
FORWARD class Plane;
FORWARD class Ray2D;
FORWARD class Sphere;
FORWARD class Polygon;
FORWARD class Segment;
FORWARD class Triangle;
FORWARD class Segment2D;

class Geometry
{
public:
    enum Orientation { Left, Middle, Right };

    static void IntersectSegment2DSegment2D(const Segment2D& segment0,
                                            const Segment2D& segment1,
                                            bool *intersected,
                                            Vector2 *intersPoint);
    static void IntersectRay2DSegment2D(const Ray2D &ray,
                                        const Segment2D& segment,
                                        bool *intersected,
                                        Vector2 *intersPoint);
    static void IntersectRayPlane(const Ray &ray,
                                  const Plane& plane,
                                  bool *intersected,
                                  float *distanceFromIntersectionToRayOrigin);
    static void IntersectRayPlane(const Ray &ray,
                                  const Plane& plane,
                                  bool *intersected,
                                  Vector3 *intersectionPoint);

    // Computes the intersection between a ray and a sphere
    static void IntersectRaySphere(const Ray &ray,
                                   const Sphere& sphere,
                                   bool *intersected,
                                   Vector3 *intersectionPoint);

    // Returns the two closest points between the ray and the passed line
    static void RayLineClosestPoints(const Ray &ray,
                                     const Vector3 &linePoint,
                                     const Vector3 &lineDirection,
                                     Vector3 *pointOnRay  = nullptr,
                                     Vector3 *pointOnLine = nullptr);

    // Computes the intersection between a ray and a PLANAR CONVEX polygon
    static void IntersectSegmentPolygon(const Segment &segment,
                                        const Polygon &poly,
                                        bool *intersected,
                                        Vector3 *intersection);
    static Array<Vector3> IntersectSegmentPolygon(const Segment &segment,
                                                  const Polygon &poly);
    static Array<Vector3> IntersectPolygonPolygon(const Polygon &poly0,
                                                  const Polygon &poly1);

    // Computes the intersection between a ray and a triangle
    static void IntersectRayTriangle(const Ray &ray,
                                     const Triangle &triangle,
                                     bool *intersected,
                                     float *distanceFromRayOriginToIntersection);
    static void IntersectRayTriangle(const Ray &ray,
                                     const Triangle &triangle,
                                     bool *intersected,
                                     Vector3 *intersectionPoint);

    // Computes the intersection between a segment and a triangle
    static void IntersectSegmentTriangle(const Segment &segment,
                                         const Triangle &triangle,
                                         bool *intersected,
                                         Vector3 *intersectionPoint);

    // Computes the intersection between two triangles
    static Array<Vector3> IntersectTriangleTriangle(const Triangle &triangle0,
                                                    const Triangle &triangle1);

    static Array<Vector3> IntersectBoxBox(const std::array<Quad, 6> &box0,
                                          const std::array<Quad, 6> &box1);

    static bool IsPointInsideBox(const Vector3 &p,
                                 const std::array<Quad, 6> &box);
    static bool IsPointInsideBox(const Vector3 &p,
                                 const Plane &boxTopPlane,
                                 const Plane &boxBotPlane,
                                 const Plane &boxLeftPlane,
                                 const Plane &boxRightPlane,
                                 const Plane &boxFrontPlane,
                                 const Plane &boxBackPlane);

    // Computes the intersection between two quads
    static Array<Vector3> IntersectQuadQuad(const Quad &quad0,
                                            const Quad &quad1);

    // Computes the intersection between a quad and a AABox
    static Array<Vector3> IntersectQuadAABox(const Quad &quad,
                                             const AABox &aaBox,
                                             bool onlyBoundaries = false);

    // Returns the orientation of a point vs a line
    static Orientation GetOrientation(const Vector2 &lineP0,
                                      const Vector2 &lineP1,
                                      const Vector2 &point);

    // Returns the orientation of a point vs a plane
    static Orientation GetOrientation(const Vector3 &point, const Plane &plane);

    // Returns the point in the ray that is closer to the passed point
    static Vector3 RayClosestPointTo(const Ray &ray, const Vector3 &point);

    // Returns a point projected to sphere
    static Vector3 PointProjectedToSphere(const Vector3 &point,
                                          const Sphere &sphere);

    Geometry() = delete;
    virtual ~Geometry() = delete;

private:
    static constexpr float ALMOST_ZERO = 1e-5f;
};

NAMESPACE_BANG_END

#endif // GEOMETRY_H

