#include "Bang/ClassDB.h"

#include "Bang/Application.h"

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
#include "Bang/DecalRenderer.h"
#include "Bang/DirectionalLight.h"
#include "Bang/GameObject.h"
#include "Bang/Light.h"
#include "Bang/LineRenderer.h"
#include "Bang/MeshCollider.h"
#include "Bang/MeshRenderer.h"
#include "Bang/NavigationMesh.h"
#include "Bang/Object.h"
#include "Bang/ParticleSystem.h"
#include "Bang/PhysicsComponent.h"
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
#include "Bang/VolumeRenderer.h"
#include "Bang/WaterRenderer.h"

using namespace Bang;

ClassDB::ClassDB()
{
}

ClassDB::~ClassDB()
{
}

ClassIdType ClassDB::GetClassIdBegin(const String &className)
{
    ClassDB *cdb = ClassDB::GetInstance();
    auto it = cdb->m_classNameToClassIdBegin.Find(className);
    if (it != cdb->m_classNameToClassIdBegin.End())
    {
        return it->second;
    }
    return -1u;
}

ClassIdType ClassDB::GetClassIdEnd(const String &className)
{
    ClassDB *cdb = ClassDB::GetInstance();
    auto it = cdb->m_classNameToClassIdEnd.Find(className);
    if (it != cdb->m_classNameToClassIdEnd.End())
    {
        return it->second;
    }
    return -1u;
}

void *ClassDB::Create(const String &className)
{
    void *createdObj = nullptr;

    ClassDB *cdb = ClassDB::GetInstance();
    auto it = cdb->m_classNameToConstructor.Find(className);
    if (it != cdb->m_classNameToConstructor.End())
    {
        const auto &constructor = it->second;
        createdObj = constructor();
    }
    return createdObj;
}

void ClassDB::RegisterClasses()
{
#define REGISTER_ABSTRACT_CLASS(CLASSNAME)                                   \
    m_classNameToClassIdBegin.Add(#CLASSNAME, CLASSNAME::GetClassIdBegin()); \
    m_classNameToClassIdEnd.Add(#CLASSNAME, CLASSNAME::GetClassIdEnd());

#define REGISTER_CLASS(CLASSNAME)      \
    REGISTER_ABSTRACT_CLASS(CLASSNAME) \
    m_classNameToConstructor.Add(      \
        #CLASSNAME, []() { return SCAST<void *>(new CLASSNAME()); });

    REGISTER_ABSTRACT_CLASS(Object);
    REGISTER_CLASS(Component);
    REGISTER_CLASS(Renderer);
    REGISTER_CLASS(MeshRenderer);
    REGISTER_CLASS(SkinnedMeshRenderer);
    REGISTER_CLASS(LineRenderer);
    REGISTER_CLASS(UIRenderer);
    REGISTER_CLASS(UIImageRenderer);
    REGISTER_CLASS(VolumeRenderer);
    REGISTER_CLASS(UITextRenderer);
    REGISTER_CLASS(WaterRenderer);
    REGISTER_CLASS(ParticleSystem);
    REGISTER_ABSTRACT_CLASS(PhysicsComponent);
    REGISTER_CLASS(Animator);
    REGISTER_CLASS(AudioListener);
    REGISTER_CLASS(AudioSource);
    REGISTER_CLASS(BehaviourContainer);
    REGISTER_CLASS(Behaviour);
    REGISTER_ABSTRACT_CLASS(Collider);
    REGISTER_CLASS(BoxCollider);
    REGISTER_CLASS(SphereCollider);
    REGISTER_CLASS(CapsuleCollider);
    REGISTER_CLASS(MeshCollider);
    REGISTER_ABSTRACT_CLASS(Light);
    REGISTER_CLASS(DirectionalLight);
    REGISTER_CLASS(PointLight);
    REGISTER_CLASS(DecalRenderer);
    REGISTER_CLASS(PostProcessEffect);
    REGISTER_CLASS(PostProcessEffectSSAO);
    REGISTER_CLASS(PostProcessEffectBloom);
    REGISTER_CLASS(PostProcessEffectFXAA);
    REGISTER_CLASS(PostProcessEffectToneMapping);
    REGISTER_CLASS(PostProcessEffectDOF);
    REGISTER_CLASS(Transform);
    REGISTER_CLASS(RectTransform);
    REGISTER_CLASS(RigidBody);
    REGISTER_CLASS(ReflectionProbe);
    REGISTER_CLASS(UIAspectRatioFitter);
    REGISTER_CLASS(UIAutoFocuser);
    REGISTER_ABSTRACT_CLASS(UIButtonBase);
    REGISTER_CLASS(UIButton);
    REGISTER_CLASS(UIToolButton);
    REGISTER_CLASS(UICanvas);
    REGISTER_CLASS(UICheckBox);
    REGISTER_CLASS(UIComboBox);
    REGISTER_CLASS(UIContentSizeFitter);
    REGISTER_CLASS(UIDirLayoutMovableSeparator);
    REGISTER_CLASS(UIDragDroppable);
    REGISTER_CLASS(UIFileList);
    REGISTER_CLASS(UIFocusable);
    REGISTER_ABSTRACT_CLASS(UIGroupLayout);
    REGISTER_CLASS(UIDirLayout);
    REGISTER_CLASS(UIHorizontalLayout);
    REGISTER_CLASS(UIVerticalLayout);
    REGISTER_CLASS(UIGridLayout);
    REGISTER_CLASS(UIInputNumber);
    REGISTER_CLASS(UIInputText);
    REGISTER_CLASS(UILabel);
    REGISTER_CLASS(UILayoutElement);
    REGISTER_CLASS(UILayoutIgnorer);
    REGISTER_CLASS(UIList);
    REGISTER_CLASS(UIMask);
    REGISTER_CLASS(UIRectMask);
    REGISTER_CLASS(UIRendererCacher);
    REGISTER_CLASS(UIScrollArea);
    REGISTER_CLASS(UIScrollBar);
    REGISTER_CLASS(UIScrollPanel);
    REGISTER_CLASS(UISlider);
    REGISTER_CLASS(UITextCursor);
    REGISTER_CLASS(UITree);
    REGISTER_CLASS(Camera);
    REGISTER_CLASS(NavigationMesh);
    REGISTER_CLASS(GameObject);

#undef REGISTER_CLASS
}

ClassDB *ClassDB::GetInstance()
{
    return Application::GetInstance()->GetClassDB();
}
