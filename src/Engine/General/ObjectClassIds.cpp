#include "Bang/ObjectClassIds.h"

#include "Bang/Animator.h"
#include "Bang/AudioListener.h"
#include "Bang/AudioSource.h"
#include "Bang/Behaviour.h"
#include "Bang/BehaviourContainer.h"
#include "Bang/BoxCollider.h"
#include "Bang/Camera.h"
#include "Bang/CapsuleCollider.h"
#include "Bang/Collider.h"
#include "Bang/Component.h"
#include "Bang/DirectionalLight.h"
#include "Bang/GameObject.h"
#include "Bang/Light.h"
#include "Bang/LineRenderer.h"
#include "Bang/MeshCollider.h"
#include "Bang/MeshRenderer.h"
#include "Bang/NavigationMesh.h"
#include "Bang/Object.h"
#include "Bang/ParticleSystem.h"
#include "Bang/PointLight.h"
#include "Bang/PostProcessEffect.h"
#include "Bang/PostProcessEffectBloom.h"
#include "Bang/PostProcessEffectDOF.h"
#include "Bang/PostProcessEffectSSAO.h"
#include "Bang/RectTransform.h"
#include "Bang/ReflectionProbe.h"
#include "Bang/Renderer.h"
#include "Bang/RigidBody.h"
#include "Bang/SkinnedMeshRenderer.h"
#include "Bang/SphereCollider.h"
#include "Bang/Transform.h"
#include "Bang/UIAspectRatioFitter.h"
#include "Bang/UIAutoFocuser.h"
#include "Bang/UIButton.h"
#include "Bang/UIButtonBase.h"
#include "Bang/UICanvas.h"
#include "Bang/UICheckBox.h"
#include "Bang/UIComboBox.h"
#include "Bang/UIContentSizeFitter.h"
#include "Bang/UIDirLayout.h"
#include "Bang/UIDirLayoutMovableSeparator.h"
#include "Bang/UIDragDroppable.h"
#include "Bang/UIFileList.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIGridLayout.h"
#include "Bang/UIGroupLayout.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIInputNumber.h"
#include "Bang/UIInputText.h"
#include "Bang/UILabel.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UILayoutIgnorer.h"
#include "Bang/UIList.h"
#include "Bang/UIMask.h"
#include "Bang/UIRectMask.h"
#include "Bang/UIRenderer.h"
#include "Bang/UIRendererCacher.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UIScrollBar.h"
#include "Bang/UIScrollPanel.h"
#include "Bang/UISlider.h"
#include "Bang/UITextCursor.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UIToolButton.h"
#include "Bang/UITree.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/WaterRenderer.h"

namespace Bang
{
Map<String, ClassIdType> ClassNameToClassIdBegin;
Map<String, ClassIdType> ClassNameToClassIdEnd;

ClassIdType GetClassIdBegin(const String &className)
{
    auto it = ClassNameToClassIdBegin.Find(className);
    if (it != ClassNameToClassIdBegin.End())
    {
        return it->second;
    }
    return -1u;
}

ClassIdType GetClassIdEnd(const String &className)
{
    auto it = ClassNameToClassIdEnd.Find(className);
    if (it != ClassNameToClassIdEnd.End())
    {
        return it->second;
    }
    return -1u;
}

void RegisterObjectClasses()
{
#define REGISTER_OBJECT_CLASS(CLASSNAME)                                   \
    ClassNameToClassIdBegin.Add(#CLASSNAME, CLASSNAME::GetClassIdBegin()); \
    ClassNameToClassIdEnd.Add(#CLASSNAME, CLASSNAME::GetClassIdEnd());

    REGISTER_OBJECT_CLASS(Object);
    REGISTER_OBJECT_CLASS(Component);
    REGISTER_OBJECT_CLASS(Renderer);
    REGISTER_OBJECT_CLASS(MeshRenderer);
    REGISTER_OBJECT_CLASS(SkinnedMeshRenderer);
    REGISTER_OBJECT_CLASS(LineRenderer);
    REGISTER_OBJECT_CLASS(UIRenderer);
    REGISTER_OBJECT_CLASS(UIImageRenderer);
    REGISTER_OBJECT_CLASS(UITextRenderer);
    REGISTER_OBJECT_CLASS(WaterRenderer);
    REGISTER_OBJECT_CLASS(ParticleSystem);
    REGISTER_OBJECT_CLASS(Animator);
    REGISTER_OBJECT_CLASS(AudioListener);
    REGISTER_OBJECT_CLASS(AudioSource);
    REGISTER_OBJECT_CLASS(BehaviourContainer);
    REGISTER_OBJECT_CLASS(Behaviour);
    REGISTER_OBJECT_CLASS(Collider);
    REGISTER_OBJECT_CLASS(BoxCollider);
    REGISTER_OBJECT_CLASS(SphereCollider);
    REGISTER_OBJECT_CLASS(CapsuleCollider);
    REGISTER_OBJECT_CLASS(MeshCollider);
    REGISTER_OBJECT_CLASS(Light);
    REGISTER_OBJECT_CLASS(DirectionalLight);
    REGISTER_OBJECT_CLASS(PointLight);
    REGISTER_OBJECT_CLASS(PostProcessEffect);
    REGISTER_OBJECT_CLASS(PostProcessEffectSSAO);
    REGISTER_OBJECT_CLASS(PostProcessEffectBloom);
    REGISTER_OBJECT_CLASS(PostProcessEffectDOF);
    REGISTER_OBJECT_CLASS(Transform);
    REGISTER_OBJECT_CLASS(RectTransform);
    REGISTER_OBJECT_CLASS(RigidBody);
    REGISTER_OBJECT_CLASS(ReflectionProbe);
    REGISTER_OBJECT_CLASS(UIAspectRatioFitter);
    REGISTER_OBJECT_CLASS(UIAutoFocuser);
    REGISTER_OBJECT_CLASS(UIButtonBase);
    REGISTER_OBJECT_CLASS(UIButton);
    REGISTER_OBJECT_CLASS(UIToolButton);
    REGISTER_OBJECT_CLASS(UICanvas);
    REGISTER_OBJECT_CLASS(UICheckBox);
    REGISTER_OBJECT_CLASS(UIComboBox);
    REGISTER_OBJECT_CLASS(UIContentSizeFitter);
    REGISTER_OBJECT_CLASS(UIDirLayoutMovableSeparator);
    REGISTER_OBJECT_CLASS(UIDragDroppable);
    REGISTER_OBJECT_CLASS(UIFileList);
    REGISTER_OBJECT_CLASS(UIFocusable);
    REGISTER_OBJECT_CLASS(UIGroupLayout);
    REGISTER_OBJECT_CLASS(UIDirLayout);
    REGISTER_OBJECT_CLASS(UIHorizontalLayout);
    REGISTER_OBJECT_CLASS(UIVerticalLayout);
    REGISTER_OBJECT_CLASS(UIGridLayout);
    REGISTER_OBJECT_CLASS(UIInputNumber);
    REGISTER_OBJECT_CLASS(UIInputText);
    REGISTER_OBJECT_CLASS(UILabel);
    REGISTER_OBJECT_CLASS(UILayoutElement);
    REGISTER_OBJECT_CLASS(UILayoutIgnorer);
    REGISTER_OBJECT_CLASS(UIList);
    REGISTER_OBJECT_CLASS(UIMask);
    REGISTER_OBJECT_CLASS(UIRectMask);
    REGISTER_OBJECT_CLASS(UIRendererCacher);
    REGISTER_OBJECT_CLASS(UIScrollArea);
    REGISTER_OBJECT_CLASS(UIScrollBar);
    REGISTER_OBJECT_CLASS(UIScrollPanel);
    REGISTER_OBJECT_CLASS(UISlider);
    REGISTER_OBJECT_CLASS(UITextCursor);
    REGISTER_OBJECT_CLASS(UITree);
    REGISTER_OBJECT_CLASS(Camera);
    REGISTER_OBJECT_CLASS(NavigationMesh);
    REGISTER_OBJECT_CLASS(GameObject);

#undef REGISTER_OBJECT_CLASS
}
}
