#include "Bang/MeshFactory.h"

#include <memory>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Mesh.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/Model.h"
#include "Bang/Paths.h"
#include "Bang/Set.tcc"

namespace Bang
{
class Path;
}

using namespace Bang;

AH<Mesh> MeshFactory::GetPlane()
{
    return MeshFactory::GetMesh("Plane.obj");
}
AH<Mesh> MeshFactory::GetUIPlane()
{
    return MeshFactory::GetMesh("UIPlane.obj");
}
AH<Mesh> MeshFactory::GetUIPlaneInvUVY()
{
    return MeshFactory::GetMesh("UIPlaneInvUVY.obj");
}
AH<Mesh> MeshFactory::GetUIPlane3x3()
{
    return MeshFactory::GetMesh("UIPlane3x3.obj");
}
AH<Bang::Mesh> Bang::MeshFactory::GetUIPlane3x3InvUVY()
{
    return MeshFactory::GetMesh("UIPlane3x3InvUVY.obj");
}
AH<Mesh> MeshFactory::GetCube()
{
    return MeshFactory::GetMesh("Cube.obj");
}

AH<Mesh> MeshFactory::GetCylinder()
{
    return MeshFactory::GetMesh("Cylinder.obj");
}

AH<Mesh> MeshFactory::GetCapsule()
{
    return MeshFactory::GetMesh("Capsule.obj");
}
AH<Mesh> MeshFactory::GetSphere()
{
    return MeshFactory::GetMesh("Sphere.obj");
}
AH<Mesh> MeshFactory::GetCone()
{
    return MeshFactory::GetMesh("Cone.obj");
}
AH<Mesh> MeshFactory::GetCamera()
{
    return MeshFactory::GetMesh("Camera.obj");
}

AH<Mesh> MeshFactory::GetMesh(const String &enginePathStr)
{
    return MeshFactory::GetMesh(
        Paths::GetEngineAssetsDir().Append("Meshes").Append(enginePathStr));
}

AH<Mesh> MeshFactory::GetMesh(const Path &fullPath)
{
    AH<Model> modelAH = Assets::Load<Model>(fullPath);

    AH<Mesh> meshAH;
    if (Model *model = modelAH.Get())
    {
        MeshFactory *mf = MeshFactory::GetActive();
        mf->m_modelCache.Add(modelAH);

        if (model->GetMeshes().Size() >= 1)
        {
            Mesh *firstMesh = model->GetMeshes()[0].Get();
            meshAH.Set(firstMesh);
        }
    }

    return meshAH;
}

MeshFactory *MeshFactory::GetActive()
{
    return Assets::GetInstance()->GetMeshFactory();
}
