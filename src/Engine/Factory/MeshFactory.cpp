#include "Bang/MeshFactory.h"

#include "Bang/Mesh.h"
#include "Bang/Material.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/MeshRenderer.h"
#include "Bang/MaterialFactory.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

RH<Mesh> MeshFactory::GetPlane()
{ return MeshFactory::GetMesh("Plane.obj"); }
RH<Mesh> MeshFactory::GetUIPlane()
{ return MeshFactory::GetMesh("UIPlane.obj"); }
RH<Mesh> MeshFactory::GetUIPlaneInvUVY()
{ return MeshFactory::GetMesh("UIPlaneInvUVY.obj"); }
RH<Mesh> MeshFactory::GetUIPlane3x3()
{ return MeshFactory::GetMesh("UIPlane3x3.obj"); }
RH<Mesh> MeshFactory::GetCube()
{ return MeshFactory::GetMesh("Cube.obj"); }
RH<Mesh> MeshFactory::GetSphere()
{ return MeshFactory::GetMesh("Sphere.obj"); }
RH<Mesh> MeshFactory::GetCone()
{ return MeshFactory::GetMesh("Cone.obj"); }
RH<Mesh> MeshFactory::GetCamera()
{ return MeshFactory::GetMesh("Camera.obj"); }

RH<Mesh> MeshFactory::GetMesh(const String &enginePath)
{
    return MeshFactory::GetMesh( Paths::GetEngineAssetsDir().
                                 Append("Meshes").
                                 Append(enginePath) );
}

RH<Mesh> MeshFactory::GetMesh(const Path &fullPath)
{
    MeshFactory *mf = MeshFactory::GetActive();
    mf->m_cache.Add(fullPath, Resources::Load<Mesh>(fullPath));
    return mf->m_cache.Get(fullPath);
}

MeshFactory *MeshFactory::GetActive()
{
    return Resources::GetActive()->GetMeshFactory();
}
