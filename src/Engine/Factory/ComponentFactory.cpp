#include "Bang/ComponentFactory.h"

#include "Bang/Debug.h"
#include "Bang/Light.h"
#include "Bang/UIMask.h"
#include "Bang/Camera.h"
#include "Bang/Animator.h"
#include "Bang/Collider.h"
#include "Bang/UICanvas.h"
#include "Bang/Renderer.h"
#include "Bang/Component.h"
#include "Bang/Transform.h"
#include "Bang/RigidBody.h"
#include "Bang/PointLight.h"
#include "Bang/UIRectMask.h"
#include "Bang/UIRenderer.h"
#include "Bang/BoxCollider.h"
#include "Bang/Application.h"
#include "Bang/AudioSource.h"
#include "Bang/UIInputText.h"
#include "Bang/LineRenderer.h"
#include "Bang/MeshRenderer.h"
#include "Bang/WaterRenderer.h"
#include "Bang/UITextCursor.h"
#include "Bang/UIScrollArea.h"
#include "Bang/AudioListener.h"
#include "Bang/RectTransform.h"
#include "Bang/UITextRenderer.h"
#include "Bang/ReflectionProbe.h"
#include "Bang/StreamOperators.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/DirectionalLight.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/PostProcessEffect.h"
#include "Bang/BehaviourContainer.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/SkinnedMeshRenderer.h"

USING_NAMESPACE_BANG

#define CREATE_COMPONENT(className, ComponentClass) \
    if ((className) == ComponentClass::GetClassNameStatic()) \
    { return Component::Create<ComponentClass>(); }

#define EXISTS_COMPONENT(componentClassName, ComponentClass) \
    if ((componentClassName) == ComponentClass::GetClassNameStatic()) \
    { return true; }

Component* ComponentFactory::Create(const String &componentClassName)
{
    CREATE_COMPONENT(componentClassName, Camera);
    CREATE_COMPONENT(componentClassName, UIMask);
    CREATE_COMPONENT(componentClassName, Renderer);
    CREATE_COMPONENT(componentClassName, UICanvas);
    CREATE_COMPONENT(componentClassName, Animator);
    CREATE_COMPONENT(componentClassName, Collider);
    CREATE_COMPONENT(componentClassName, Transform);
    CREATE_COMPONENT(componentClassName, RigidBody);
    CREATE_COMPONENT(componentClassName, PointLight);
    CREATE_COMPONENT(componentClassName, UIRectMask);
    CREATE_COMPONENT(componentClassName, BoxCollider);
    CREATE_COMPONENT(componentClassName, AudioSource);
    CREATE_COMPONENT(componentClassName, UIInputText);
    CREATE_COMPONENT(componentClassName, LineRenderer);
    CREATE_COMPONENT(componentClassName, MeshRenderer);
    CREATE_COMPONENT(componentClassName, WaterRenderer);
    CREATE_COMPONENT(componentClassName, UIScrollArea);
    CREATE_COMPONENT(componentClassName, AudioListener);
    CREATE_COMPONENT(componentClassName, RectTransform);
    CREATE_COMPONENT(componentClassName, UITextRenderer);
    CREATE_COMPONENT(componentClassName, ReflectionProbe);
    CREATE_COMPONENT(componentClassName, UIImageRenderer);
    CREATE_COMPONENT(componentClassName, DirectionalLight);
    CREATE_COMPONENT(componentClassName, UIVerticalLayout);
    CREATE_COMPONENT(componentClassName, PostProcessEffect);
    CREATE_COMPONENT(componentClassName, BehaviourContainer);
    CREATE_COMPONENT(componentClassName, UIHorizontalLayout);
    CREATE_COMPONENT(componentClassName, SkinnedMeshRenderer);

    Debug_Warn("Please register class '" << componentClassName << "' in "
               "ComponentFactory");

    return nullptr;
}
