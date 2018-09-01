#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include "Bang/Color.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/LayoutSizeType.h"

NAMESPACE_BANG_BEGIN

FORWARD class Scene;
FORWARD class Camera;
FORWARD class UIList;
FORWARD class UITree;
FORWARD class UILabel;
FORWARD class UIButton;
FORWARD class UICanvas;
FORWARD class UISlider;
FORWARD class Texture2D;
FORWARD class GameObject;
FORWARD class UICheckBox;
FORWARD class UIComboBox;
FORWARD class UIInputText;
FORWARD class UIScrollBar;
FORWARD class UIScrollArea;
FORWARD class UIScrollPanel;
FORWARD class UIInputNumber;
FORWARD class UIImageRenderer;
FORWARD class UIRendererCacher;
FORWARD class UIDirLayoutMovableSeparator;

class GameObjectFactory
{
public:

    static GameObject* CreateGameObject(bool addTransform = true);
    static GameObject* CreateUIGameObject(bool addComponents = true);
    static GameObject* CreateGameObjectNamed(const String &name);
    static GameObject* CreateUIGameObjectNamed(const String &name);
    static void CreateUIGameObjectInto(GameObject *go, bool addComponents = true);

    // Disallow implicit conversions on above functions
    template <class T> GameObject* CreateGameObject(T) = delete;
    template <class T> GameObject* CreateUIGameObject(T) = delete;

    static Scene* CreateScene(bool addTransform = true);
    static Scene* CreateUIScene();
    static void   CreateUISceneInto(Scene *scene);
    static Scene* CreateDefaultSceneInto(Scene *scene);

    static UICanvas*           CreateUICanvas();
    static UICanvas*           CreateUICanvasInto(GameObject *go);
    static UIImageRenderer*    CreateUIImage(const Color &color = Color::White);
    static UIImageRenderer*    CreateUIImage(const Color &color,
                                          const Vector2i &size);
    static UIList*             CreateUIListInto(GameObject *go,
                                             bool withScrollPanel = true);
    static UIList*             CreateUIList(bool withScrollPanel = true);
    static UITree*             CreateUITreeInto(GameObject *go);
    static UITree*             CreateUITree();
    static UIInputText*        CreateUIInputTextInto(GameObject *go);
    static UIInputText*        CreateUIInputText();
    static UICheckBox*         CreateUICheckBoxInto(GameObject *go);
    static UICheckBox*         CreateUICheckBox();
    static UIComboBox*         CreateUIComboBoxInto(GameObject *go);
    static UIComboBox*         CreateUIComboBox();
    static UIComboBox*         CreateUIBoolComboBoxInto(GameObject *go);
    static UIComboBox*         CreateUIBoolComboBox();
    static UISlider*           CreateUISliderInto(GameObject *go);
    static UISlider*           CreateUISlider();
    static UIInputNumber*      CreateUIInputNumberInto(GameObject *go);
    static UIInputNumber*      CreateUIInputNumber();
    static UIRendererCacher*   CreateUIRendererCacherInto(GameObject *go);
    static UIRendererCacher*   CreateUIRendererCacher();
    static UIButton*           CreateUIButtonInto(GameObject *go);
    static UIButton*           CreateUIButton();
    static UIButton*           CreateUIButton(const String &text,
                                              Texture2D *icon = nullptr);
    static UILabel*            CreateUILabel();
    static UILabel*            CreateUILabelInto(GameObject *go);
    static UIScrollArea*       CreateUIScrollAreaInto(GameObject *go);
    static UIScrollArea*       CreateUIScrollArea();
    static UIScrollBar*        CreateUIScrollBarInto(GameObject *go);
    static UIScrollBar*        CreateUIScrollBar();
    static UIScrollPanel*      CreateUIScrollPanelInto(GameObject *go);
    static UIScrollPanel*      CreateUIScrollPanel();

    static UIDirLayoutMovableSeparator* CreateUIDirLayoutMovableHSeparator();
    static UIDirLayoutMovableSeparator* CreateUIDirLayoutMovableVSeparator();
    static UIDirLayoutMovableSeparator* CreateUIDirLayoutMovableSeparator();
    static UIDirLayoutMovableSeparator* CreateUIDirLayoutMovableSeparatorInto(
                                                            GameObject *go);

    static GameObject* CreateUIHSeparator(
                            LayoutSizeType sizeType = LayoutSizeType::PREFERRED,
                            int spaceY = 1,
                            float linePercent = 1.0f);
    static GameObject* CreateUIVSeparator(
                            LayoutSizeType sizeType = LayoutSizeType::PREFERRED,
                            int spaceX = 1,
                            float linePercent = 1.0f);

    static GameObject* CreateUISpacer(
                            LayoutSizeType sizeType = LayoutSizeType::FLEXIBLE,
                            const Vector2 &space = Vector2::One);
    static GameObject* CreateUIHSpacer(
                            LayoutSizeType sizeType = LayoutSizeType::FLEXIBLE,
                            float spaceX = 1);
    static GameObject* CreateUIVSpacer(
                            LayoutSizeType sizeType = LayoutSizeType::FLEXIBLE,
                            float spaceY = 1);

    static UIImageRenderer* AddInnerShadow(GameObject *uiGo,
                                           const Vector2i &size = Vector2i(20),
                                           float alpha = 1.0f);
    static UIImageRenderer* AddOuterShadow(GameObject *uiGo,
                                           const Vector2i &size = Vector2i(20),
                                           float alpha = 1.0f);
    static UIImageRenderer* AddOuterBorder(GameObject *uiGo,
                                           const Vector2i &size = Vector2i(1),
                                           const Color &color = Color::Black);
    static UIImageRenderer* AddInnerBorder(GameObject *uiGo,
                                           const Vector2i &size = Vector2i(1),
                                           const Color &color = Color::Black);

    static String GetGameObjectDuplicateName(const GameObject *go);

    static Camera*     CreateUICamera();
    static Camera*     CreateUICameraInto(GameObject *go);

    static GameObject* CreatePlaneGameObject();
    static GameObject* CreateCubeGameObject();
    static GameObject* CreateCapsuleGameObject();
    static GameObject* CreateSphereGameObject();
    static GameObject* CreateConeGameObject();

    GameObjectFactory() = delete;

private:
    static GameObject* CreateUISeparator(LayoutSizeType sizeType,
                                         const Vector2i &space,
                                         float linePercent);
    static GameObject* CreateGameObjectWithMesh(Mesh* m, const String &name);

};

NAMESPACE_BANG_END

#endif // GAMEOBJECTFACTORY_H
