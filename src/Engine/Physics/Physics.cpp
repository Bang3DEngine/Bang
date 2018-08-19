#include "Bang/Physics.h"

#include "PxPhysicsAPI.h"

#include "Bang/Scene.h"

USING_NAMESPACE_BANG

Physics::Physics()
{
}

Physics::~Physics()
{
}

void Physics::Init()
{
    m_pxFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION,
                                        m_pxAllocator,
                                        m_pxErrorCallback);

    // m_pxPvd = physx::PxCreatePvd(*m_pxFoundation);
    // physx::PxPvdTransport* transport =
    //                     physx::PxDefaultPvdSocketTransportCreate(0, 5425, 10);
    // m_pxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    m_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,
                                  *m_pxFoundation,
                                  physx::PxTolerancesScale(),
                                  true,
                                  nullptr);

    m_pxMaterial = m_pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    physx::PxRigidStatic* groundPlane = PxCreatePlane(*m_pxPhysics,
                                                      physx::PxPlane(0,1,0,0),
                                                      *m_pxMaterial);
    m_pxScene->addActor(*groundPlane);
}

void Physics::RegisterScene(Scene *scene)
{
    physx::PxSceneDesc sceneDesc(GetPxPhysics()->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher	= physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.filterShader	= physx::PxDefaultSimulationFilterShader;

    physx::PxScene *pxScene = GetPxPhysics()->createScene(sceneDesc);

    m_sceneToPxScene.Add(scene, pxScene);
}

physx::PxScene *Physics::GetPxSceneFromScene(Scene *scene) const
{
    if (m_sceneToPxScene.ContainsKey(scene))
    {
        return m_sceneToPxScene.Get(scene);
    }
    return nullptr;
}

physx::PxPhysics *Physics::GetPxPhysics() const
{
    return m_pxPhysics;
}

