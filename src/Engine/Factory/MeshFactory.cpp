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
RH<Mesh> MeshFactory::GetCube()
{
    return MeshFactory::GetMesh("Meshes/Cube.obj");
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
    MeshFactory *mf = MeshFactory::GetActive();
    RH<Model> modelRH = Resources::Load<Model>(fullPath);
    Model *model = modelRH.Get();
    if (model && model->GetMeshes().Size() >= 1)
    {
        mf->m_modelCache.Add(modelRH);
        meshRH.Set( model->GetMeshes()[0].Get() );
    }
    return meshRH;
}

MeshFactory *MeshFactory::GetActive()
{
    return Resources::GetInstance()->GetMeshFactory();
}
