#include "Bang/GameObjectFactory.h"

#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/AssetHandle.h"
#include "Bang/AudioListener.h"
#include "Bang/Axis.h"
#include "Bang/BoxCollider.h"
#include "Bang/Camera.h"
#include "Bang/CapsuleCollider.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/LineRenderer.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/MeshRenderer.h"
#include "Bang/Physics.h"
#include "Bang/RectTransform.h"
#include "Bang/Scene.h"
#include "Bang/SphereCollider.h"
#include "Bang/TextureFactory.h"
#include "Bang/Transform.h"
#include "Bang/UIButton.h"
#include "Bang/UICanvas.h"
#include "Bang/UICheckBox.h"
#include "Bang/UIComboBox.h"
#include "Bang/UIDirLayout.h"
#include "Bang/UIDirLayoutMovableSeparator.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIInputNumber.h"
#include "Bang/UIInputText.h"
#include "Bang/UILabel.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UIList.h"
#include "Bang/UIRendererCacher.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UIScrollBar.h"
#include "Bang/UIScrollPanel.h"
#include "Bang/UISlider.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UITheme.h"
#include "Bang/UIToolButton.h"
#include "Bang/UITree.h"

using namespace Bang;

GameObject *GameObjectFactory::CreateGameObject(bool addTransform)
{
    GameObject *go = new GameObject();
    if (addTransform && !go->HasComponent<Transform>())
    {
        go->AddComponent<Transform>();
    }
    return go;
}

GameObject *GameObjectFactory::CreateUIGameObject(bool addComponents)
{
    GameObject *go = new GameObject();
    GameObjectFactory::CreateUIGameObjectInto(go, addComponents);
    return go;
}

GameObject *GameObjectFactory::CreateGameObjectNamed(const String &name)
{
    GameObject *go = GameObjectFactory::CreateGameObject(true);
    go->SetName(name);
    return go;
}

GameObject *GameObjectFactory::CreateUIGameObjectNamed(const String &name)
{
    GameObject *go = GameObjectFactory::CreateUIGameObject(true);
    go->SetName(name);
    return go;
}

void GameObjectFactory::CreateUIGameObjectInto(GameObject *go, bool addComps)
{
    if (addComps)
    {
        if (!go->HasComponent<RectTransform>())
        {
            go->AddComponent<RectTransform>();
        }
    }
}

Scene *GameObjectFactory::CreateScene(bool addTransform)
{
    Scene *scene = new Scene();
    if (addTransform && !scene->GetTransform())
    {
        scene->AddComponent<Transform>();
    }
    return scene;
}

Scene *GameObjectFactory::CreateUIScene()
{
    Scene *scene = new Scene();
    CreateUISceneInto(scene);
    return scene;
}

void GameObjectFactory::CreateUISceneInto(Scene *scene)
{
    Camera *cam = GameObjectFactory::CreateUICameraInto(scene);
    scene->SetCamera(cam);
    GameObjectFactory::CreateUIGameObjectInto(scene);
    GameObjectFactory::CreateUICanvasInto(scene);
    Physics::GetInstance()->UnRegisterScene(scene);
}

Scene *GameObjectFactory::CreateDefaultSceneInto(Scene *scene)
{
    ASSERT(scene->GetTransform());

    GameObject *cube = GameObjectFactory::CreateCubeGameObject();

    GameObject *cameraGo = GameObjectFactory::CreateGameObjectNamed("Camera");
    Camera *cam = GameObjectFactory::CreateDefaultCameraInto(cameraGo);
    cameraGo->GetTransform()->SetPosition(Vector3(5, 4, 3));
    cameraGo->GetTransform()->LookAt(Vector3::Zero());
    scene->SetCamera(cam);

    /*

    GameObject *sphere =
    GameObjectFactory::CreateGameObjectNamed("Sphere-Child");
    sphere->GetTransform()->SetLocalPosition(Vector3(1,1,1));
    sphere->GetTransform()->SetLocalScale( Vector3(0.3f) );
    MeshRenderer *mr2 = sphere->AddComponent<MeshRenderer>();
    mr2->SetMesh(MeshFactory::GetSphere().Get());

    GameObject *cube2 =
    GameObjectFactory::CreateGameObjectNamed("Cube-Sphere-Child");
    cube2->GetTransform()->SetLocalPosition(Vector3(4,0,0));
    MeshRenderer *mr3 = cube2->AddComponent<MeshRenderer>();
    mr3->SetMesh(MeshFactory::GetCube().Get());

    GameObject *lightGo = GameObjectFactory::CreateGameObjectNamed("Light");
    PointLight *pl = lightGo->AddComponent<PointLight>();
    pl->SetRange(20.0f);
    pl->SetColor(Color::Yellow);
    lightGo->GetTransform()->SetPosition( Vector3(7,4,-2) );
    lightGo->GetTransform()->LookAt( Vector3::Zero );

    GameObject *light2Go = GameObjectFactory::CreateGameObjectNamed("Light2");
    PointLight *pl2 = light2Go->AddComponent<PointLight>();
    pl2->SetRange(20.0f);
    pl2->SetColor(Color::Purple);
    light2Go->GetTransform()->SetPosition( Vector3(-7,4,-2) );
    light2Go->GetTransform()->LookAt( Vector3::Zero );

    GameObject *light3Go = GameObjectFactory::CreateGameObjectNamed("Light3");
    PointLight *pl3 = light3Go->AddComponent<PointLight>();
    pl3->SetRange(6.0f);
    pl3->SetIntensity(0.5f);
    pl3->SetColor(Color::White);
    light3Go->GetTransform()->SetPosition( Vector3(0, 4, 0) );
    light3Go->GetTransform()->LookAt( Vector3::Zero );

    GameObject *floor = GameObjectFactory::CreateCubeGameObject();
    floor->SetName("Floor");
    floor->GetTransform()->TranslateLocal( Vector3(0, -1, 0) );
    floor->GetTransform()->SetLocalScale( Vector3(10.0f, 0.2f, 10.0f));

    GameObject *wall1 = GameObjectFactory::CreateCubeGameObject();
    wall1->SetName("Wall1");
    wall1->GetTransform()->TranslateLocal( Vector3(-4, 3, 0) );
    wall1->GetTransform()->SetLocalScale( Vector3(0.2f, 10.0f, 10.0f));

    GameObject *wall2 = GameObjectFactory::CreateCubeGameObject();
    wall2->SetName("Wall2");
    wall2->GetTransform()->TranslateLocal( Vector3(0, 3, -4) );
    wall2->GetTransform()->SetLocalRotation( Quaternion::AngleAxis(Math::Pi/2,
    Vector3::Up) );
    wall2->GetTransform()->SetLocalScale( Vector3(0.2f, 10.0f, 10.0f));

    for (int i = 0; i < 0; ++i)
    {
        GameObjectFactory::CreateGameObjectNamed("GO_" +
    String(i))->SetParent(scene);
    }

    // lightGo->SetParent(scene);
    // light2Go->SetParent(scene);
    light3Go->SetParent(scene);
    floor->SetParent(scene);
    wall1->SetParent(scene);
    wall2->SetParent(scene);
    sphere->SetParent(cube);
    cube2->SetParent(sphere);
    */

    cube->SetParent(scene);
    cameraGo->SetParent(scene);

    return scene;
}

Camera *GameObjectFactory::CreateDefaultCameraInto(GameObject *go)
{
    Camera *cam = go->AddComponent<Camera>();
    go->AddComponent<AudioListener>();
    cam->SetHDR(true);
    return CreateDefaultCameraInto(cam);
}

Camera *GameObjectFactory::CreateDefaultCameraInto(Camera *cam)
{
    cam->SetClearMode(CameraClearMode::SKY_BOX);
    cam->SetClearColor(Color(0.5f, 0.8f, 1.0f));
    cam->SetSkyBoxTexture(TextureFactory::GetDefaultSkybox());
    return cam;
}

UICanvas *GameObjectFactory::CreateUICanvas()
{
    GameObject *go = GameObjectFactory::CreateUIGameObject();
    return GameObjectFactory::CreateUICanvasInto(go);
}

UICanvas *GameObjectFactory::CreateUICanvasInto(GameObject *go)
{
    UICanvas *canvas = go->AddComponent<UICanvas>();
    go->SetName("Canvas");
    return canvas;
}

UIImageRenderer *GameObjectFactory::CreateUIImage(const Color &color)
{
    GameObject *go = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *img = go->AddComponent<UIImageRenderer>();
    img->SetTint(color);
    go->SetName("Image");
    return img;
}

UIImageRenderer *GameObjectFactory::CreateUIImage(const Color &color,
                                                  const Vector2i &size)
{
    UIImageRenderer *img = GameObjectFactory::CreateUIImage(color);
    UILayoutElement *le = img->GetGameObject()->AddComponent<UILayoutElement>();
    le->SetMinSize(size);
    le->SetPreferredSize(size);
    return img;
}

UIList *GameObjectFactory::CreateUIListInto(GameObject *go,
                                            bool withScrollPanel)
{
    return UIList::CreateInto(go, withScrollPanel);
}
UIList *GameObjectFactory::CreateUIList(bool withScrollPanel)
{
    return UIList::CreateInto(
        GameObjectFactory::CreateUIGameObjectNamed("List"), withScrollPanel);
}

UITree *GameObjectFactory::CreateUITreeInto(GameObject *go)
{
    return UITree::CreateInto(go);
}

UITree *GameObjectFactory::CreateUITree()
{
    return UITree::CreateInto(
        GameObjectFactory::CreateUIGameObjectNamed("Tree"));
}

UIInputText *GameObjectFactory::CreateUIInputTextInto(GameObject *go)
{
    return UIInputText::CreateInto(go);
}
UIInputText *GameObjectFactory::CreateUIInputText()
{
    return GameObjectFactory::CreateUIInputTextInto(
        GameObjectFactory::CreateUIGameObjectNamed("InputText"));
}

UICheckBox *GameObjectFactory::CreateUICheckBoxInto(GameObject *go)
{
    return UICheckBox::CreateInto(go);
}

UICheckBox *GameObjectFactory::CreateUICheckBox()
{
    return GameObjectFactory::CreateUICheckBoxInto(
        GameObjectFactory::CreateUIGameObjectNamed("CheckBox"));
}

UIComboBox *GameObjectFactory::CreateUIComboBoxInto(GameObject *go)
{
    return UIComboBox::CreateInto(go);
}

UIComboBox *GameObjectFactory::CreateUIComboBox()
{
    return GameObjectFactory::CreateUIComboBoxInto(
        GameObjectFactory::CreateUIGameObjectNamed("ComboBox"));
}

UIComboBox *GameObjectFactory::CreateUIBoolComboBoxInto(GameObject *go)
{
    UIComboBox *cb = GameObjectFactory::CreateUIComboBoxInto(go);
    cb->SetMultiCheck(true);
    return cb;
}

UIComboBox *GameObjectFactory::CreateUIBoolComboBox()
{
    return GameObjectFactory::CreateUIBoolComboBoxInto(
        GameObjectFactory::CreateUIGameObjectNamed("BoolComboBox"));
}

UISlider *GameObjectFactory::CreateUISliderInto(GameObject *go)
{
    return UISlider::CreateInto(go);
}

UISlider *GameObjectFactory::CreateUISlider()
{
    return GameObjectFactory::CreateUISliderInto(
        GameObjectFactory::CreateUIGameObjectNamed("Slider"));
}

UIInputNumber *GameObjectFactory::CreateUIInputNumberInto(GameObject *go)
{
    return UIInputNumber::CreateInto(go);
}

UIInputNumber *GameObjectFactory::CreateUIInputNumber()
{
    return GameObjectFactory::CreateUIInputNumberInto(
        GameObjectFactory::CreateUIGameObjectNamed("InputNumber"));
}

UIRendererCacher *GameObjectFactory::CreateUIRendererCacherInto(GameObject *go)
{
    return UIRendererCacher::CreateInto(go);
}

UIRendererCacher *GameObjectFactory::CreateUIRendererCacher()
{
    return GameObjectFactory::CreateUIRendererCacherInto(
        GameObjectFactory::CreateUIGameObjectNamed("UIRendererCacher"));
}

UIButton *GameObjectFactory::CreateUIButtonInto(GameObject *go)
{
    return UIButton::CreateInto(go);
}
UIButton *GameObjectFactory::CreateUIButton()
{
    return UIButton::CreateInto(
        GameObjectFactory::CreateUIGameObjectNamed("Button"));
}

UIButton *GameObjectFactory::CreateUIButton(const String &text, Texture2D *icon)
{
    const Vector2i size(15);
    UIButton *btn = GameObjectFactory::CreateUIButton();

    if (!text.IsEmpty())
    {
        btn->GetText()->SetContent(text);
    }

    if (icon)
    {
        btn->SetIcon(icon, size, (text.IsEmpty() ? 0 : 5));
    }

    constexpr int BigPadding = 10;
    constexpr int MediumPadding = 6;
    constexpr int SmallPadding = 3;
    if (!text.IsEmpty() && !icon)
    {
        btn->GetDirLayout()->SetPaddingBot(MediumPadding);
        btn->GetDirLayout()->SetPaddingTop(MediumPadding);
        btn->GetDirLayout()->SetPaddingRight(BigPadding);
        btn->GetDirLayout()->SetPaddingLeft(BigPadding);
    }
    else if (!text.IsEmpty() && icon)
    {
        btn->GetDirLayout()->SetPaddingBot(MediumPadding);
        btn->GetDirLayout()->SetPaddingTop(MediumPadding);
        btn->GetDirLayout()->SetPaddingLeft(BigPadding);
        btn->GetDirLayout()->SetPaddingRight(SmallPadding);
    }
    else if (text.IsEmpty() && icon)
    {
        btn->GetDirLayout()->SetPaddingBot(SmallPadding);
        btn->GetDirLayout()->SetPaddingTop(SmallPadding);
        btn->GetDirLayout()->SetPaddingLeft(SmallPadding);
        btn->GetDirLayout()->SetPaddingRight(SmallPadding);
    }
    else if (text.IsEmpty() && !icon)
    {
        btn->GetDirLayout()->SetPaddingBot(MediumPadding);
        btn->GetDirLayout()->SetPaddingTop(MediumPadding);
        btn->GetDirLayout()->SetPaddingLeft(BigPadding);
        btn->GetDirLayout()->SetPaddingRight(BigPadding);
    }

    return btn;
}

UIToolButton *GameObjectFactory::CreateUIToolButton()
{
    GameObject *go = GameObjectFactory::CreateUIGameObject();
    return GameObjectFactory::CreateUIToolButtonInto(go);
}

UIToolButton *GameObjectFactory::CreateUIToolButtonInto(GameObject *go)
{
    return UIToolButton::CreateInto(go);
}

UIToolButton *GameObjectFactory::CreateUIToolButton(const String &text,
                                                    Texture2D *icon)
{
    const Vector2i size(15);
    UIToolButton *btn = GameObjectFactory::CreateUIToolButton();

    if (!text.IsEmpty())
    {
        btn->GetText()->SetContent(text);
    }

    if (icon)
    {
        btn->SetIcon(icon, size, (text.IsEmpty() ? 0 : 5));
    }

    constexpr int BigPadding = 10;
    constexpr int MediumPadding = 6;
    constexpr int SmallPadding = 3;
    if (!text.IsEmpty() && !icon)
    {
        btn->GetDirLayout()->SetPaddingBot(MediumPadding);
        btn->GetDirLayout()->SetPaddingTop(MediumPadding);
        btn->GetDirLayout()->SetPaddingRight(BigPadding);
        btn->GetDirLayout()->SetPaddingLeft(BigPadding);
    }
    else if (!text.IsEmpty() && icon)
    {
        btn->GetDirLayout()->SetPaddingBot(MediumPadding);
        btn->GetDirLayout()->SetPaddingTop(MediumPadding);
        btn->GetDirLayout()->SetPaddingLeft(BigPadding);
        btn->GetDirLayout()->SetPaddingRight(SmallPadding);
    }
    else if (text.IsEmpty() && icon)
    {
        btn->GetDirLayout()->SetPaddingBot(SmallPadding);
        btn->GetDirLayout()->SetPaddingTop(SmallPadding);
        btn->GetDirLayout()->SetPaddingLeft(SmallPadding);
        btn->GetDirLayout()->SetPaddingRight(SmallPadding);
    }
    else if (text.IsEmpty() && !icon)
    {
        btn->GetDirLayout()->SetPaddingBot(MediumPadding);
        btn->GetDirLayout()->SetPaddingTop(MediumPadding);
        btn->GetDirLayout()->SetPaddingLeft(BigPadding);
        btn->GetDirLayout()->SetPaddingRight(BigPadding);
    }

    return btn;
}

UILabel *GameObjectFactory::CreateUILabelInto(GameObject *go)
{
    return UILabel::CreateInto(go);
}
UILabel *GameObjectFactory::CreateUILabel()
{
    return UILabel::CreateInto(
        GameObjectFactory::CreateUIGameObjectNamed("Label"));
}

UIScrollArea *GameObjectFactory::CreateUIScrollArea()
{
    return UIScrollArea::CreateInto(
        GameObjectFactory::CreateUIGameObjectNamed("ScrollArea"));
}

UIScrollBar *GameObjectFactory::CreateUIScrollBarInto(GameObject *go)
{
    return UIScrollBar::CreateInto(go);
}
UIScrollBar *GameObjectFactory::CreateUIScrollBar()
{
    return UIScrollBar::CreateInto(
        GameObjectFactory::CreateUIGameObjectNamed("ScrollBar"));
}

UIScrollPanel *GameObjectFactory::CreateUIScrollPanelInto(GameObject *go)
{
    return UIScrollPanel::CreateInto(go);
}
UIScrollPanel *GameObjectFactory::CreateUIScrollPanel()
{
    return GameObjectFactory::CreateUIScrollPanelInto(
        GameObjectFactory::CreateUIGameObjectNamed("ScrollPanel"));
}

UIDirLayoutMovableSeparator *
GameObjectFactory::CreateUIDirLayoutMovableHSeparator()
{
    UIDirLayoutMovableSeparator *sep =
        GameObjectFactory::CreateUIDirLayoutMovableSeparator();
    sep->SetAxis(Axis::HORIZONTAL);
    return sep;
}

UIDirLayoutMovableSeparator *
GameObjectFactory::CreateUIDirLayoutMovableVSeparator()
{
    UIDirLayoutMovableSeparator *sep =
        GameObjectFactory::CreateUIDirLayoutMovableSeparator();
    sep->SetAxis(Axis::VERTICAL);
    return sep;
}

UIDirLayoutMovableSeparator *
GameObjectFactory::CreateUIDirLayoutMovableSeparator()
{
    return GameObjectFactory::CreateUIDirLayoutMovableSeparatorInto(
        GameObjectFactory::CreateUIGameObjectNamed("UIMovSeparator"));
}

UIDirLayoutMovableSeparator *
GameObjectFactory::CreateUIDirLayoutMovableSeparatorInto(GameObject *go)
{
    return UIDirLayoutMovableSeparator::CreateInto(go);
}

UIScrollArea *GameObjectFactory::CreateUIScrollAreaInto(GameObject *go)
{
    return UIScrollArea::CreateInto(go);
}

GameObject *GameObjectFactory::CreateUISpacer(LayoutSizeType sizeType,
                                              const Vector2 &space)
{
    GameObject *spacerGo =
        GameObjectFactory::CreateUIGameObjectNamed("Separator");
    UILayoutElement *le = spacerGo->AddComponent<UILayoutElement>();

    le->SetMinSize(Vector2i(0));
    le->SetPreferredSize(Vector2i(0));
    le->SetFlexibleSize(Vector2(0));

    if (sizeType == LayoutSizeType::MIN)
    {
        le->SetMinSize(Vector2i(space));
    }
    else if (sizeType == LayoutSizeType::PREFERRED)
    {
        le->SetPreferredSize(Vector2i(space));
    }
    else
    {
        le->SetFlexibleSize(Vector2(space));
    }
    return spacerGo;
}
GameObject *GameObjectFactory::CreateUIHSpacer(LayoutSizeType sizeType,
                                               float spaceX)
{
    GameObject *spacerGo =
        GameObjectFactory::CreateUISpacer(sizeType, Vector2(spaceX, 0));
    return spacerGo;
}
GameObject *GameObjectFactory::CreateUIVSpacer(LayoutSizeType sizeType,
                                               float spaceY)
{
    GameObject *spacerGo =
        GameObjectFactory::CreateUISpacer(sizeType, Vector2(0, spaceY));
    return spacerGo;
}

UIImageRenderer *GameObjectFactory::AddInnerShadow(GameObject *uiGo,
                                                   const Vector2i &size,
                                                   float alpha)
{
    GameObject *innerShadowGo = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *innerShadowImg =
        innerShadowGo->AddComponent<UIImageRenderer>();
    innerShadowImg->SetMode(UIImageRenderer::Mode::SLICE_9);
    innerShadowImg->SetImageTexture(TextureFactory::GetInnerShadow());
    innerShadowImg->SetSlice9BorderStrokePx(size);
    innerShadowImg->SetTint(Color::Black().WithAlpha(alpha));
    innerShadowImg->SetDepthMask(false);
    // innerShadowGo->GetTransform()->TranslateLocal( Vector3(0, 0, 0.00001f) );
    innerShadowGo->SetParent(uiGo);
    return innerShadowImg;
}

UIImageRenderer *GameObjectFactory::AddOuterShadow(GameObject *uiGo,
                                                   const Vector2i &size,
                                                   float alpha)
{
    GameObject *outerShadowGo = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *outerShadowImg =
        outerShadowGo->AddComponent<UIImageRenderer>();
    outerShadowImg->SetMode(UIImageRenderer::Mode::SLICE_9);
    outerShadowImg->SetImageTexture(TextureFactory::GetOuterShadow());
    outerShadowImg->SetSlice9BorderStrokePx(size);
    outerShadowImg->SetTint(Color::Black().WithAlpha(alpha));
    outerShadowImg->SetDepthMask(false);
    outerShadowGo->GetRectTransform()->TranslateLocal(Vector3(0, 0, 0.001f));
    outerShadowGo->GetRectTransform()->SetMargins(
        -size.x, -size.y, -size.x, -size.y);
    outerShadowGo->SetParent(uiGo);
    return outerShadowImg;
}

UIImageRenderer *GameObjectFactory::AddOuterBorder(GameObject *uiGo)
{
    return GameObjectFactory::AddOuterBorder(
        uiGo,
        Vector2i(SCAST<int>(UITheme::GetNotFocusedBorderStroke())),
        UITheme::GetNotFocusedBorderColor());
}

UIImageRenderer *GameObjectFactory::AddOuterBorder(GameObject *uiGo,
                                                   const Vector2i &size,
                                                   const Color &color)
{
    GameObject *outerBorderGo = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *outerBorderImg =
        outerBorderGo->AddComponent<UIImageRenderer>();
    outerBorderImg->SetMode(UIImageRenderer::Mode::SLICE_9);
    outerBorderImg->SetImageTexture(TextureFactory::Get9SliceBorder());
    outerBorderImg->SetSlice9BorderStrokePx(size);
    outerBorderImg->SetTint(color);
    outerBorderGo->GetRectTransform()->TranslateLocal(Vector3(0, 0, -0.00001f));
    outerBorderGo->GetRectTransform()->SetMargins(
        -size.x, -size.y, -size.x, -size.y);
    outerBorderGo->SetParent(uiGo);
    return outerBorderImg;
}

UIImageRenderer *GameObjectFactory::AddInnerBorder(GameObject *uiGo)
{
    return GameObjectFactory::AddInnerBorder(
        uiGo,
        Vector2i(SCAST<int>(UITheme::GetNotFocusedBorderStroke())),
        UITheme::GetNotFocusedBorderColor());
}

UIImageRenderer *GameObjectFactory::AddInnerBorder(GameObject *uiGo,
                                                   const Vector2i &size,
                                                   const Color &color)
{
    UIImageRenderer *innerBorderImg = uiGo->AddComponent<UIImageRenderer>();
    innerBorderImg->SetMode(UIImageRenderer::Mode::SLICE_9);
    innerBorderImg->SetImageTexture(TextureFactory::Get9SliceBorder());
    innerBorderImg->SetSlice9BorderStrokePx(size);
    innerBorderImg->SetTint(color);
    return innerBorderImg;
}

void GameObjectFactory::MakeBorderFocused(UIImageRenderer *border)
{
    if (border)
    {
        border->SetTint(UITheme::GetFocusedBorderColor());
        border->SetSlice9BorderStrokePx(
            Vector2i(SCAST<int>(UITheme::GetFocusedBorderStroke())));
    }
}

void GameObjectFactory::MakeBorderNotFocused(UIImageRenderer *border)
{
    if (border)
    {
        border->SetTint(UITheme::GetNotFocusedBorderColor());
        border->SetSlice9BorderStrokePx(
            Vector2i(SCAST<int>(UITheme::GetNotFocusedBorderStroke())));
    }
}

String GameObjectFactory::GetGameObjectDuplicateName(const GameObject *go)
{
    String originalName = go->GetName();

    String duplicateNameNumber = "";
    bool isDuplicatedName = false;
    for (uint i = originalName.Size() - 1; i >= 0; --i)
    {
        char c = originalName[i];
        if (String::IsNumber(c))
        {
            isDuplicatedName = true;
            duplicateNameNumber.Prepend(String(c));
        }
        else
        {
            isDuplicatedName = (c == '_');
            break;
        }
    }

    String duplicateName;
    if (isDuplicatedName)
    {
        int duplicateNumber = String::ToInt(duplicateNameNumber);
        duplicateName = originalName.SubString(
            0, originalName.Size() - duplicateNameNumber.Size() - 2);
        duplicateName += "_" + String(duplicateNumber + 1);
    }
    else
    {
        duplicateName = (originalName + "_1");
    }

    return duplicateName;
}

Camera *GameObjectFactory::CreateUICamera()
{
    GameObject *go = GameObjectFactory::CreateGameObject();
    return GameObjectFactory::CreateUICameraInto(go);
}

Camera *GameObjectFactory::CreateUICameraInto(GameObject *go)
{
    Camera *cam = go->AddComponent<Camera>();
    cam->SetGammaCorrection(1.0f);
    cam->SetClearColor(Color::LightGray());
    return cam;
}

GameObject *GameObjectFactory::CreateUISeparator(LayoutSizeType sizeType,
                                                 const Vector2i &space,
                                                 float linePercent)
{
    GameObject *sepGo =
        GameObjectFactory::CreateUISpacer(sizeType, Vector2(space));
    LineRenderer *lr = sepGo->AddComponent<LineRenderer>();
    lr->SetMaterial(MaterialFactory::GetUIImage().Get());
    lr->GetMaterial()->SetAlbedoColor(Color::Black());
    lr->SetViewProjMode(GL::ViewProjMode::CANVAS);

    UILayoutElement *le = sepGo->GetComponent<UILayoutElement>();
    le->SetPreferredSize(Vector2i::Max(space, Vector2i::One()));
    bool horizontal = (space.x == 0);
    if (horizontal)
    {
        le->SetFlexibleSize(Vector2(99999999, 0));
        lr->SetPoints(
            {Vector3(-linePercent, 0, 0), Vector3(linePercent, 0, 0)});
    }
    else
    {
        le->SetFlexibleSize(Vector2(0, 99999999));
        lr->SetPoints(
            {Vector3(0, -linePercent, 0), Vector3(0, linePercent, 0)});
    }
    return sepGo;
}

GameObject *GameObjectFactory::CreateUIHSeparator(LayoutSizeType sizeType,
                                                  int spaceY,
                                                  float linePercent)
{
    GameObject *sepGo = GameObjectFactory::CreateUISeparator(
        sizeType, Vector2i(0, spaceY), linePercent);
    return sepGo;
}
GameObject *GameObjectFactory::CreateUIVSeparator(LayoutSizeType sizeType,
                                                  int spaceX,
                                                  float linePercent)
{
    GameObject *sepGo = GameObjectFactory::CreateUISeparator(
        sizeType, Vector2i(spaceX, 0), linePercent);
    return sepGo;
}

GameObject *GameObjectFactory::CreateGameObjectWithMesh(Mesh *m,
                                                        const String &name)
{
    GameObject *go = GameObjectFactory::CreateGameObject(true);
    go->SetName(name);

    MeshRenderer *r = go->AddComponent<MeshRenderer>();
    r->SetRenderPrimitive(GL::Primitive::TRIANGLES);
    r->SetMaterial(MaterialFactory::GetDefault().Get());
    r->SetMesh(m);

    return go;
}

GameObject *GameObjectFactory::CreatePlaneGameObject()
{
    AH<Mesh> mesh = MeshFactory::GetPlane();
    GameObject *go = CreateGameObjectWithMesh(mesh.Get(), "Plane");
    go->AddComponent<BoxCollider>();
    return go;
}

GameObject *GameObjectFactory::CreateCubeGameObject()
{
    AH<Mesh> mesh = MeshFactory::GetCube();
    GameObject *go = CreateGameObjectWithMesh(mesh.Get(), "Cube");
    go->AddComponent<BoxCollider>();
    return go;
}

GameObject *GameObjectFactory::CreateCapsuleGameObject()
{
    AH<Mesh> mesh = MeshFactory::GetCapsule();
    GameObject *go = CreateGameObjectWithMesh(mesh.Get(), "Capsule");
    go->AddComponent<CapsuleCollider>();
    return go;
}

GameObject *GameObjectFactory::CreateCylinderGameObject()
{
    AH<Mesh> mesh = MeshFactory::GetCylinder();
    GameObject *go = CreateGameObjectWithMesh(mesh.Get(), "Cylinder");
    BoxCollider *col = go->AddComponent<BoxCollider>();
    col->SetExtents(Vector3::One());
    return go;
}

GameObject *GameObjectFactory::CreateSphereGameObject()
{
    AH<Mesh> mesh = MeshFactory::GetSphere();
    GameObject *go = CreateGameObjectWithMesh(mesh.Get(), "Sphere");
    go->AddComponent<SphereCollider>();
    return go;
}

GameObject *GameObjectFactory::CreateConeGameObject()
{
    AH<Mesh> mesh = MeshFactory::GetCone();
    GameObject *go = CreateGameObjectWithMesh(mesh.Get(), "Cone");
    go->AddComponent<BoxCollider>();
    return go;
}
