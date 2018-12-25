#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <functional>

#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Set.h"
#include "Bang/String.h"

namespace Bang
{
class GUID;
class Mesh;
class Model;
class Path;

class MeshFactory
{
public:
    static AH<Mesh> GetPlane();
    static AH<Mesh> GetUIPlane();
    static AH<Mesh> GetUIPlaneInvUVY();
    static AH<Mesh> GetUIPlane3x3();
    static AH<Mesh> GetUIPlane3x3InvUVY();
    static AH<Mesh> GetCube();
    static AH<Mesh> GetCylinder();
    static AH<Mesh> GetCapsule();
    static AH<Mesh> GetSphere();
    static AH<Mesh> GetCone();
    static AH<Mesh> GetCamera();

    static AH<Mesh> GetMesh(const String &enginePath);
    static AH<Mesh> GetMesh(const Path &fullPath);

protected:
    MeshFactory() = default;
    virtual ~MeshFactory() = default;

private:
    Set<AH<Model>> m_modelCache;

    static MeshFactory *GetActive();

    friend class Assets;
};
}

#endif  // MESHFACTORY_H
