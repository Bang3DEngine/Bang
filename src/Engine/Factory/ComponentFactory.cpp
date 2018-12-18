#include "Bang/ComponentFactory.h"

#include <ostream>

#include "Bang/Animator.h"
#include "Bang/AudioListener.h"
#include "Bang/AudioSource.h"
#include "Bang/BehaviourContainer.h"
#include "Bang/BoxCollider.h"
#include "Bang/Camera.h"
#include "Bang/CapsuleCollider.h"
#include "Bang/Cloth.h"
#include "Bang/Component.h"
#include "Bang/Debug.h"
#include "Bang/DirectionalLight.h"
#include "Bang/LineRenderer.h"
#include "Bang/MeshCollider.h"
#include "Bang/MeshRenderer.h"
#include "Bang/NavigationMesh.h"
#include "Bang/ParticleSystem.h"
#include "Bang/PointLight.h"
#include "Bang/PostProcessEffect.h"
#include "Bang/PostProcessEffectBloom.h"
#include "Bang/PostProcessEffectDOF.h"
#include "Bang/PostProcessEffectFXAA.h"
#include "Bang/PostProcessEffectSSAO.h"
#include "Bang/PostProcessEffectToneMapping.h"
#include "Bang/RectTransform.h"
#include "Bang/ReflectionProbe.h"
#include "Bang/Renderer.h"
#include "Bang/RigidBody.h"
#include "Bang/Rope.h"
#include "Bang/Serializable.h"
#include "Bang/SkinnedMeshRenderer.h"
#include "Bang/SphereCollider.h"
#include "Bang/Transform.h"
#include "Bang/UICanvas.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIInputText.h"
#include "Bang/UIMask.h"
#include "Bang/UIRectMask.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/VolumeRenderer.h"
#include "Bang/WaterRenderer.h"

using namespace Bang;

#define CREATE_COMPONENT(className, ComponentClass)          \
    if ((className) == ComponentClass::GetClassNameStatic()) \
    {                                                        \
        return new ComponentClass();                         \
    }

Component *ComponentFactory::Create(const String &componentClassName)
{
    CREATE_COMPONENT(componentClassName, Rope);
    CREATE_COMPONENT(componentClassName, Cloth);
    CREATE_COMPONENT(componentClassName, Camera);
    CREATE_COMPONENT(componentClassName, UIMask);
    CREATE_COMPONENT(componentClassName, Renderer);
    CREATE_COMPONENT(componentClassName, UICanvas);
    CREATE_COMPONENT(componentClassName, Animator);
    CREATE_COMPONENT(componentClassName, Transform);
    CREATE_COMPONENT(componentClassName, RigidBody);
    CREATE_COMPONENT(componentClassName, PointLight);
    CREATE_COMPONENT(componentClassName, UIRectMask);
    CREATE_COMPONENT(componentClassName, BoxCollider);
    CREATE_COMPONENT(componentClassName, AudioSource);
    CREATE_COMPONENT(componentClassName, UIInputText);
    CREATE_COMPONENT(componentClassName, MeshCollider);
    CREATE_COMPONENT(componentClassName, LineRenderer);
    CREATE_COMPONENT(componentClassName, MeshRenderer);
    CREATE_COMPONENT(componentClassName, VolumeRenderer);
    CREATE_COMPONENT(componentClassName, WaterRenderer);
    CREATE_COMPONENT(componentClassName, ParticleSystem);
    CREATE_COMPONENT(componentClassName, UIScrollArea);
    CREATE_COMPONENT(componentClassName, AudioListener);
    CREATE_COMPONENT(componentClassName, RectTransform);
    CREATE_COMPONENT(componentClassName, SphereCollider);
    CREATE_COMPONENT(componentClassName, UITextRenderer);
    CREATE_COMPONENT(componentClassName, NavigationMesh);
    CREATE_COMPONENT(componentClassName, CapsuleCollider);
    CREATE_COMPONENT(componentClassName, ReflectionProbe);
    CREATE_COMPONENT(componentClassName, UIImageRenderer);
    CREATE_COMPONENT(componentClassName, DirectionalLight);
    CREATE_COMPONENT(componentClassName, UIVerticalLayout);
    CREATE_COMPONENT(componentClassName, PostProcessEffect);
    CREATE_COMPONENT(componentClassName, BehaviourContainer);
    CREATE_COMPONENT(componentClassName, UIHorizontalLayout);
    CREATE_COMPONENT(componentClassName, SkinnedMeshRenderer);
    CREATE_COMPONENT(componentClassName, PostProcessEffectDOF);
    CREATE_COMPONENT(componentClassName, PostProcessEffectSSAO);
    CREATE_COMPONENT(componentClassName, PostProcessEffectFXAA);
    CREATE_COMPONENT(componentClassName, PostProcessEffectBloom);
    CREATE_COMPONENT(componentClassName, PostProcessEffectToneMapping);

    Debug_Warn("Please register class '" << componentClassName
                                         << "' in "
                                            "ComponentFactory");

    return nullptr;
}
