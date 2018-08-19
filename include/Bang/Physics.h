#ifndef PHYSICS_H
#define PHYSICS_H

#include "Bang/Bang.h"

#include "PxPhysicsAPI.h"

NAMESPACE_BANG_BEGIN

FORWARD class Scene;

class Physics
{
public:
	Physics();
	virtual ~Physics();

    void Init();
    void RegisterScene(Scene *scene);

    static Physics *GetInstance();

private:
    physx::PxDefaultAllocator m_pxAllocator;
    physx::PxDefaultErrorCallback m_pxErrorCallback;

    physx::PxFoundation *m_pxFoundation = nullptr;
    physx::PxPhysics *m_pxPhysics = nullptr;

    Map<Scene*, physx::PxScene> m_sceneToPxScene;

    physx::PxMaterial *m_pxMaterial = nullptr;
    physx::PxPvd *m_pxPvd = nullptr;

    physx::PxScene* GetPxSceneFromScene(Scene *scene) const;
    physx::PxPhysics* GetPxPhysics() const;
};

NAMESPACE_BANG_END

#endif // PHYSICS_H

