#include "Bang/MeshFactory.h"

#include <memory>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Mesh.h"
#include "Bang/Model.h"
#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "Bang/Set.tcc"

FORWARD NAMESPACE_BANG_BEGIN
FORWARD class Path;
FORWARD NAMESPACE_BANG_END

USING_NAMESPACE_BANG

RH<Mesh> MeshFactory::GetPlane()
{
    return MeshFactory::GetMesh("Meshes/Plane.obj");
}
RH<Mesh> MeshFactory::GetUIPlane()
{
    return MeshFactory::GetMesh("Meshes/UIPlane.obj");
}
RH<Mesh> MeshFactory::GetUIPlaneInvUVY()
{
    return MeshFactory::GetMesh("Meshes/UIPlaneInvUVY.obj");
}
RH<Mesh> MeshFactory::GetUIPlane3x3()
{
    return MeshFactory::GetMesh("Meshes/UIPlane3x3.obj");
}
RH<Bang::Mesh> Bang::MeshFactory::GetUIPlane3x3InvUVY()
{
    return MeshFactory::GetMesh("Meshes/UIPlane3x3InvUVY.obj");
}
RH<Mesh> MeshFactory::GetCube()
{
    return MeshFactory::GetMesh("Meshes/Cube.obj");
}

RH<Mesh> MeshFactory::GetCylinder()
{
    return MeshFactory::GetMesh("Meshes/Cylinder.obj");
}

RH<Mesh> MeshFactory::GetCapsule()
{
    return MeshFactory::GetMesh("Meshes/Capsule.obj");
}
RH<Mesh> MeshFactory::GetSphere()
{
    return MeshFactory::GetMesh("Meshes/Sphere.obj");
}
RH<Mesh> MeshFactory::GetCone()
{
    return MeshFactory::GetMesh("Meshes/Cone.obj");
}
RH<Mesh> MeshFactory::GetCamera()
{
    return MeshFactory::GetMesh("Meshes/Camera.obj");
}

RH<Mesh> MeshFactory::GetMesh(const String &enginePath)
{
    return MeshFactory::GetMesh( EPATH(enginePath) );
}

RH<Mesh> MeshFactory::GetMesh(const Path &fullPath)
{
    RH<Mesh> meshRH;
    RH<Model> modelRH = Resources::Load<Model>(fullPath);
    Model *model = modelRH.Get();

    if (model)
    {
        MeshFactory *mf = MeshFactory::GetActive();
        mf->m_modelCache.Add(modelRH);

        if (model->GetMeshes().Size() >= 1)
        {
            Mesh *firstMesh = model->GetMeshes()[0].Get();
            meshRH.Set(firstMesh);
        }
    }

    return meshRH;
}

MeshFactory *MeshFactory::GetActive()
{
    return Resources::GetInstance()->GetMeshFactory();
}
