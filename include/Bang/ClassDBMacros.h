#ifndef CLASSDBMACROS_H
#define CLASSDBMACROS_H

#include "Bang/Bang.h"

namespace Bang
{
using ClassIdType = uint;

#define CREATE_STATIC_CLASS_ID(CLASS, ID_BEGIN, ID_END)      \
    constexpr static ClassIdType CLASS##CIDBegin = ID_BEGIN; \
    constexpr static ClassIdType CLASS##CIDEnd = ID_END

#define CREATE_STATIC_CLASS_IDS()                                    \
    CREATE_STATIC_CLASS_ID(Object, 0, 1000000000);                   \
    CREATE_STATIC_CLASS_ID(Component, 0, 100000);                    \
    CREATE_STATIC_CLASS_ID(Renderer, 1, 149);                        \
    CREATE_STATIC_CLASS_ID(MeshRenderer, 10, 20);                    \
    CREATE_STATIC_CLASS_ID(SkinnedMeshRenderer, 11, 15);             \
    CREATE_STATIC_CLASS_ID(LineRenderer, 21, 30);                    \
    CREATE_STATIC_CLASS_ID(UIRenderer, 31, 40);                      \
    CREATE_STATIC_CLASS_ID(UIImageRenderer, 32, 35);                 \
    CREATE_STATIC_CLASS_ID(UITextRenderer, 36, 39);                  \
    CREATE_STATIC_CLASS_ID(WaterRenderer, 41, 50);                   \
    CREATE_STATIC_CLASS_ID(ParticleSystem, 51, 60);                  \
    CREATE_STATIC_CLASS_ID(DecalRenderer, 61, 65);                   \
    CREATE_STATIC_CLASS_ID(VolumeRenderer, 65, 70);                  \
    CREATE_STATIC_CLASS_ID(Animator, 150, 200);                      \
    CREATE_STATIC_CLASS_ID(AudioListener, 201, 300);                 \
    CREATE_STATIC_CLASS_ID(AudioSource, 301, 400);                   \
    CREATE_STATIC_CLASS_ID(BehaviourContainer, 401, 500);            \
    CREATE_STATIC_CLASS_ID(Behaviour, 501, 600);                     \
    CREATE_STATIC_CLASS_ID(Light, 701, 800);                         \
    CREATE_STATIC_CLASS_ID(DirectionalLight, 702, 710);              \
    CREATE_STATIC_CLASS_ID(PointLight, 711, 720);                    \
    CREATE_STATIC_CLASS_ID(PostProcessEffect, 801, 900);             \
    CREATE_STATIC_CLASS_ID(PostProcessEffectSSAO, 802, 804);         \
    CREATE_STATIC_CLASS_ID(PostProcessEffectBloom, 805, 807);        \
    CREATE_STATIC_CLASS_ID(PostProcessEffectDOF, 808, 809);          \
    CREATE_STATIC_CLASS_ID(PostProcessEffectFXAA, 810, 811);         \
    CREATE_STATIC_CLASS_ID(PostProcessEffectToneMapping, 810, 811);  \
    CREATE_STATIC_CLASS_ID(Transform, 901, 1000);                    \
    CREATE_STATIC_CLASS_ID(RectTransform, 910, 920);                 \
    CREATE_STATIC_CLASS_ID(ReflectionProbe, 1101, 1200);             \
    CREATE_STATIC_CLASS_ID(UIAspectRatioFitter, 1201, 1300);         \
    CREATE_STATIC_CLASS_ID(UIAutoFocuser, 1301, 1400);               \
    CREATE_STATIC_CLASS_ID(UIButtonBase, 1401, 1500);                \
    CREATE_STATIC_CLASS_ID(UIButton, 1410, 1420);                    \
    CREATE_STATIC_CLASS_ID(UIToolButton, 1421, 1430);                \
    CREATE_STATIC_CLASS_ID(UICanvas, 1601, 1700);                    \
    CREATE_STATIC_CLASS_ID(UICheckBox, 1701, 1800);                  \
    CREATE_STATIC_CLASS_ID(UIComboBox, 1801, 1900);                  \
    CREATE_STATIC_CLASS_ID(UIContentSizeFitter, 1901, 2000);         \
    CREATE_STATIC_CLASS_ID(UIDirLayoutMovableSeparator, 2101, 2200); \
    CREATE_STATIC_CLASS_ID(UIDragDroppable, 2201, 2300);             \
    CREATE_STATIC_CLASS_ID(UIFileList, 2301, 2400);                  \
    CREATE_STATIC_CLASS_ID(UIFocusable, 2401, 2500);                 \
    CREATE_STATIC_CLASS_ID(UIGroupLayout, 2601, 2700);               \
    CREATE_STATIC_CLASS_ID(UIDirLayout, 2610, 2620);                 \
    CREATE_STATIC_CLASS_ID(UIHorizontalLayout, 2611, 2612);          \
    CREATE_STATIC_CLASS_ID(UIVerticalLayout, 2613, 2614);            \
    CREATE_STATIC_CLASS_ID(UIGridLayout, 2621, 2630);                \
    CREATE_STATIC_CLASS_ID(UIInputNumber, 2801, 2900);               \
    CREATE_STATIC_CLASS_ID(UIInputText, 2901, 3000);                 \
    CREATE_STATIC_CLASS_ID(UILabel, 3001, 3100);                     \
    CREATE_STATIC_CLASS_ID(UILayoutElement, 3101, 3200);             \
    CREATE_STATIC_CLASS_ID(UILayoutIgnorer, 3201, 3300);             \
    CREATE_STATIC_CLASS_ID(UIList, 3301, 3400);                      \
    CREATE_STATIC_CLASS_ID(UIMask, 3401, 3500);                      \
    CREATE_STATIC_CLASS_ID(UIRectMask, 3501, 3600);                  \
    CREATE_STATIC_CLASS_ID(UIRendererCacher, 3601, 3700);            \
    CREATE_STATIC_CLASS_ID(UIScrollArea, 3701, 3800);                \
    CREATE_STATIC_CLASS_ID(UIScrollBar, 3801, 3900);                 \
    CREATE_STATIC_CLASS_ID(UIScrollPanel, 3901, 4000);               \
    CREATE_STATIC_CLASS_ID(UISlider, 4001, 4100);                    \
    CREATE_STATIC_CLASS_ID(UITextCursor, 4101, 4200);                \
    CREATE_STATIC_CLASS_ID(UITree, 4301, 4400);                      \
    CREATE_STATIC_CLASS_ID(Camera, 4501, 4600);                      \
    CREATE_STATIC_CLASS_ID(NavigationMesh, 4601, 4700);              \
    CREATE_STATIC_CLASS_ID(PhysicsComponent, 5000, 6000);            \
    CREATE_STATIC_CLASS_ID(RigidBody, 5001, 5100);                   \
    CREATE_STATIC_CLASS_ID(Collider, 5101, 5200);                    \
    CREATE_STATIC_CLASS_ID(BoxCollider, 5102, 5110);                 \
    CREATE_STATIC_CLASS_ID(SphereCollider, 5111, 5120);              \
    CREATE_STATIC_CLASS_ID(CapsuleCollider, 5121, 5130);             \
    CREATE_STATIC_CLASS_ID(MeshCollider, 5131, 5141);                \
    CREATE_STATIC_CLASS_ID(GameObject, 100000, 200000);

#define SET_CLASS_ID(CLASS)                               \
public:                                                   \
    constexpr static inline ClassIdType GetClassIdBegin() \
    {                                                     \
        return ClassDB::CLASS##CIDBegin;                  \
    }                                                     \
    constexpr static inline ClassIdType GetClassIdEnd()   \
    {                                                     \
        return ClassDB::CLASS##CIDEnd;                    \
    }

#define SET_CLASS_ID_AS_ROOT(CLASS)                               \
protected:                                                        \
    ClassIdType m_instanceClassId = ClassDB::GetInvalidClassId(); \
                                                                  \
public:                                                           \
    ClassIdType GetInstanceClassId() const                        \
    {                                                             \
        return m_instanceClassId;                                 \
    }                                                             \
    SET_CLASS_ID(CLASS)

#define SET_INSTANCE_CLASS_ID(CLASS) \
    m_instanceClassId = CLASS::GetClassIdBegin();
}

#endif  // CLASSDBMACROS_H
