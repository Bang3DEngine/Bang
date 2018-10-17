#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <functional>

#include "Bang/BangDefines.h"
#include "Bang/ResourceHandle.h"
#include "Bang/Set.h"
#include "Bang/String.h"

namespace Bang
{
class Mesh;
class Model;
class Path;

class MeshFactory
{
public:
    static RH<Mesh> GetPlane();
    static RH<Mesh> GetUIPlane();
    static RH<Mesh> GetUIPlaneInvUVY();
    static RH<Mesh> GetUIPlane3x3();
    static RH<Mesh> GetUIPlane3x3InvUVY();
    static RH<Mesh> GetCube();
    static RH<Mesh> GetCylinder();
    static RH<Mesh> GetCapsule();
    static RH<Mesh> GetSphere();
    static RH<Mesh> GetCone();
    static RH<Mesh> GetCamera();

    static RH<Mesh> GetMesh(const String &enginePath);
    static RH<Mesh> GetMesh(const Path &fullPath);

protected:
    MeshFactory() = default;
    virtual ~MeshFactory() = default;

private:
    Set<RH<Model>> m_modelCache;

    static MeshFactory *GetActive();

    friend class Resources;
};
}

#endif  // MESHFACTORY_H
