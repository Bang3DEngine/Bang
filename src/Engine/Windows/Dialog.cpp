#include "Bang/Dialog.h"

#include <vector>

#include "Bang/Alignment.h"
#include "Bang/Application.h"
#include "Bang/Assert.h"
#include "Bang/Color.h"
#include "Bang/Component.h"
#include "Bang/DialogWindow.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/Extensions.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/IEvents.h"
#include "Bang/IEventsValueChanged.h"
#include "Bang/Input.h"
#include "Bang/Key.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/Paths.h"
#include "Bang/Scene.h"
#include "Bang/SceneManager.h"
#include "Bang/Stretch.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureFactory.h"
#include "Bang/UIAutoFocuser.h"
#include "Bang/UIButton.h"
#include "Bang/UIFileList.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIInputText.h"
#include "Bang/UILabel.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UIList.h"
#include "Bang/UIScrollPanel.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/Window.h"
#include "Bang/WindowManager.h"

using namespace Bang;

bool Dialog::s_okPressed = false;
String Dialog::s_resultString = "";
Path Dialog::s_resultPath = Path::Empty();
std::stack<DialogWindow *> Dialog::s_dialogCreation_dialogWindows;
std::stack<Window *> Dialog::s_dialogCreation_prevActiveWindows;
std::stack<bool> Dialog::s_dialogCreation_modalBooleans;
Dialog::YesNoCancel Dialog::s_resultYesNoCancel = Dialog::YesNoCancel::CANCEL;

DialogWindow *Dialog::s_currentDialog = nullptr;

DialogWindow *Bang::Dialog::Error(const String &title, const String &msg)
{
    DialogWindow *dialog =
        Dialog::BeginDialogCreation("Error: " + title, 300, 150, false, true);
    Scene *scene = CreateMsgScene(msg);
    Dialog::EndDialogCreation(scene);
    return dialog;
}

String Dialog::GetString(const String &title,
                         const String &msg,
                         const String &hint)
{
    Dialog::BeginDialogCreation(title, 300, 100, false, true);
    Scene *scene = CreateGetStringScene(msg, hint);
    Dialog::EndDialogCreation(scene);
    return Dialog::s_okPressed ? Dialog::s_resultString : "";
}

Dialog::YesNoCancel Dialog::GetYesNoCancel(const String &title,
                                           const String &msg)
{
    Dialog::BeginDialogCreation(title, 300, 140, false, true);
    Scene *scene = CreateYesNoCancelScene(msg);
    Dialog::EndDialogCreation(scene);
    return Dialog::s_resultYesNoCancel;
}

Path Dialog::OpenFilePath(const String &title,
                          const Array<String> &extensions,
                          const Path &initialDirPath)
{
    Dialog::BeginDialogCreation(title, 500, 400, true, true);
    Scene *scene = GameObjectFactory::CreateScene(false);
    CreateOpenFilePathSceneInto(scene, false, extensions, initialDirPath);
    Dialog::EndDialogCreation(scene);
    return Dialog::s_okPressed ? Dialog::s_resultPath : Path::Empty();
}

Path Dialog::OpenDirectory(const String &title, const Path &initialDirPath)
{
    Dialog::BeginDialogCreation(title, 500, 400, true, true);
    Scene *scene = GameObjectFactory::CreateScene(false);
    CreateOpenFilePathSceneInto(scene, true, {}, initialDirPath);
    Dialog::EndDialogCreation(scene);
    return Dialog::s_okPressed ? Dialog::s_resultPath : Path::Empty();
}

Path Dialog::SaveFilePath(const String &title,
                          const String &extension,
                          const Path &initDirPath,
                          const String &initFileName)
{
    Dialog::BeginDialogCreation(title, 500, 400, true, true);
    Scene *scene = GameObjectFactory::CreateScene(false);
    CreateSaveFilePathSceneInto(scene, extension, initDirPath, initFileName);
    Dialog::EndDialogCreation(scene);
    return Dialog::s_okPressed ? Dialog::s_resultPath : Path::Empty();
}

DialogWindow *Dialog::BeginDialogCreation(const String &title,
                                          int sizeX,
                                          int sizeY,
                                          bool resizable,
                                          bool modal)
{
    DialogWindow *dialogWindow = nullptr;
    Dialog::s_dialogCreation_prevActiveWindows.push(Window::GetActive());
    Dialog::s_dialogCreation_modalBooleans.push(modal);
    Window *topWindow = WindowManager::GetTopWindow();
    if (topWindow)
    {
        Input::GetActive()->Reset();

        dialogWindow = WindowManager::CreateDialogWindow(topWindow, resizable);
        Window::SetActive(dialogWindow);
        dialogWindow->SetSize(sizeX, sizeY);
        dialogWindow->SetTitle(title);
        dialogWindow->CenterInParent();

        Dialog::s_okPressed = false;
        Dialog::s_resultString = "";
        Dialog::s_resultPath = Path::Empty();
        Dialog::s_currentDialog = dialogWindow;
    }
    Dialog::s_dialogCreation_dialogWindows.push(dialogWindow);
    return dialogWindow;
}

void Dialog::EndDialogCreation(Scene *scene)
{
    SceneManager::LoadSceneInstantly(scene, false);

    DialogWindow *dialogWindow = Dialog::s_dialogCreation_dialogWindows.top();
    Window *prevWindow = Dialog::s_dialogCreation_prevActiveWindows.top();
    bool modal = Dialog::s_dialogCreation_modalBooleans.top();
    Dialog::s_dialogCreation_dialogWindows.pop();
    Dialog::s_dialogCreation_prevActiveWindows.pop();
    Dialog::s_dialogCreation_modalBooleans.pop();

    if (modal)
    {
        Application::GetInstance()->BlockingWait(dialogWindow);
    }
    else
    {
        Window::SetActive(prevWindow);
    }
}

void Dialog::EndCurrentDialog()
{
    if (Dialog::s_currentDialog)
    {
        WindowManager::GetInstance()->DestroyWindow(Dialog::s_currentDialog);
        Dialog::s_currentDialog = nullptr;
    }
}

void Dialog::CreateSaveFilePathSceneInto(Scene *scene,
                                         const String &extension,
                                         const Path &initialDirPath,
                                         const String &initialFileName)
{
    UIFileList *fileList;
    UIButton *cancelButton, *botLeftButton;
    UIInputText *botInputText;

    Dialog::CreateFilePathBaseSceneInto(scene,
                                        initialDirPath,
                                        &fileList,
                                        &cancelButton,
                                        &botLeftButton,
                                        &botInputText);

    fileList->SetFileExtensions({extension});

    UIButton *saveButton = botLeftButton;
    saveButton->GetText()->SetContent("Save");
    saveButton->AddClickedCallback([fileList, botInputText]() {
        Path path = fileList->GetCurrentPath()
                        .Append(botInputText->GetText()->GetContent())
                        .AppendExtension(Extensions::GetSceneExtension());
        AcceptDialogPath(path);
    });

    class SaveSceneController : public Component
    {
    public:
        void Init(UIInputText *inputText, UIFileList *fileList)
        {
            p_fileList = fileList;
            p_inputText = inputText;
        }

        void OnUpdate() override
        {
            Component::OnUpdate();

            Path currPath = p_fileList->GetCurrentSelectedPath();
            if (currPath.IsFile())
            {
                String currContent = currPath.GetNameExt();
                if (currContent != m_prevContent)
                {
                    p_inputText->GetText()->SetContent(currContent);
                    m_prevContent = currContent;
                }
            }
        }

        UIFileList *p_fileList;
        UIInputText *p_inputText;
        String m_prevContent = "";
    };

    botInputText->GetText()->SetContent(initialFileName);

    SaveSceneController *ssc = scene->AddComponent<SaveSceneController>();
    ssc->Init(botInputText, fileList);
}

void Dialog::CreateOpenFilePathSceneInto(Scene *scene,
                                         bool openDir,
                                         const Array<String> &extensions,
                                         const Path &initialDirPath)
{
    UIFileList *fileList;
    UIButton *cancelButton, *botRightButton;
    UIInputText *botInputText;

    Dialog::CreateFilePathBaseSceneInto(scene,
                                        initialDirPath,
                                        &fileList,
                                        &cancelButton,
                                        &botRightButton,
                                        &botInputText);

    fileList->SetFileExtensions(extensions);
    fileList->SetShowOnlyDirectories(openDir);

    UIButton *openButton = botRightButton;
    openButton->GetText()->SetContent("Open");
    openButton->AddClickedCallback(Dialog::OnOkClicked);

    botInputText->GetGameObject()->SetEnabled(false);

    class OpenFileSceneController : public Component
    {
    public:
        void Init(UIFileList *fileList, UIButton *openButton, bool openDir)
        {
            p_fileList = fileList;
            p_openButton = openButton;
            m_openDir = openDir;
        }

        void OnUpdate() override
        {
            Component::OnUpdate();

            Path currPath = m_openDir ? p_fileList->GetCurrentPath()
                                      : p_fileList->GetCurrentSelectedPath();
            bool buttonBlocked =
                m_openDir ? !currPath.IsDir() : !currPath.IsFile();
            p_openButton->SetBlocked(buttonBlocked);

            if (m_openDir)
            {
                p_openButton->GetText()->SetContent("Open " +
                                                    currPath.GetAbsolute());
            }
        }

        bool m_openDir;
        UIFileList *p_fileList;
        UIButton *p_openButton;
        String m_prevContent = "";
    };

    OpenFileSceneController *ofsc =
        scene->AddComponent<OpenFileSceneController>();
    ofsc->Init(fileList, openButton, openDir);
}

void Dialog::CreateFilePathBaseSceneInto(Scene *scene,
                                         const Path &initialDirPath,
                                         UIFileList **outFileList,
                                         UIButton **outBotLeftButton,
                                         UIButton **outBotRightButton,
                                         UIInputText **outBotInputText)
{
    GameObjectFactory::CreateUISceneInto(scene);

    UIVerticalLayout *vlMain = scene->AddComponent<UIVerticalLayout>();
    vlMain->SetPaddings(10);

    GameObject *vlGo = GameObjectFactory::CreateUIGameObject();
    UIVerticalLayout *vl = vlGo->AddComponent<UIVerticalLayout>();
    vl->SetChildrenVerticalStretch(Stretch::NONE);
    UILayoutElement *vlLE = vlGo->AddComponent<UILayoutElement>();
    vlLE->SetFlexibleHeight(1);

    GameObject *botHLGo = GameObjectFactory::CreateUIGameObject();
    UIHorizontalLayout *hl = botHLGo->AddComponent<UIHorizontalLayout>();
    hl->SetChildrenHorizontalAlignment(HorizontalAlignment::RIGHT);
    hl->SetChildrenVerticalStretch(Stretch::FULL);
    UILayoutElement *hlLE = botHLGo->AddComponent<UILayoutElement>();
    hlLE->SetFlexibleHeight(0);

    UIList *list = GameObjectFactory::CreateUIList();
    list->GetScrollPanel()->SetForceHorizontalFit(true);
    UILayoutElement *listLE =
        list->GetGameObject()->AddComponent<UILayoutElement>();
    listLE->SetFlexibleSize(Vector2(1));

    UIFileList *fileList = list->GetGameObject()->AddComponent<UIFileList>();

    UIInputText *inputPathText = GameObjectFactory::CreateUIInputText();
    inputPathText->GetText()->SetContent("");
    inputPathText->GetText()->SetTextSize(12);
    fileList->AddPathChangedCallback([inputPathText](const Path &newPath) {
        inputPathText->GetText()->SetContent(newPath.GetAbsolute());
    });
    fileList->AddPathChangedCallback(Dialog::OnDialogPathChanged);
    fileList->AddFileAcceptedCallback(Dialog::AcceptDialogPath);

    UILayoutElement *itLE =
        inputPathText->GetGameObject()->AddComponent<UILayoutElement>();
    itLE->SetFlexibleSize(Vector2(1));

    GameObject *inputPathCont = GameObjectFactory::CreateUIGameObject();

    UILayoutElement *inputHLLE = inputPathCont->AddComponent<UILayoutElement>();
    inputHLLE->SetFlexibleHeight(0);

    UIHorizontalLayout *inputPathHL =
        inputPathCont->AddComponent<UIHorizontalLayout>();
    inputPathHL->SetChildrenVerticalStretch(Stretch::FULL);
    inputPathHL->SetChildrenHorizontalStretch(Stretch::NONE);
    inputPathHL->SetSpacing(10);

    list->GetScrollPanel()->SetHorizontalScrollBarSide(VerticalSide::BOT);
    list->GetScrollPanel()->SetVerticalScrollBarSide(HorizontalSide::RIGHT);
    list->GetScrollPanel()->SetVerticalShowScrollMode(
        ShowScrollMode::WHEN_NEEDED);
    list->GetScrollPanel()->SetHorizontalShowScrollMode(
        ShowScrollMode::WHEN_NEEDED);

    UIButton *goButton = GameObjectFactory::CreateUIButton("Go");
    goButton->AddClickedCallback([inputPathText, fileList]() {
        Path inputPath(inputPathText->GetText()->GetContent());
        if (inputPath.IsFile())
        {
            inputPath = inputPath.GetDirectory();
        }

        if (inputPath.IsDir())
        {
            fileList->SetCurrentPath(inputPath);
        }
    });

    UIButton *openButton = GameObjectFactory::CreateUIButton("Open");
    openButton->GetGameObject()->SetName("OpenButton");
    openButton->GetGameObject()
        ->GetComponent<UILayoutElement>()
        ->SetFlexibleHeight(1.0f);

    UIButton *cancelButton = GameObjectFactory::CreateUIButton("Cancel");
    cancelButton->GetGameObject()->SetName("CancelButton");
    cancelButton->GetGameObject()
        ->GetComponent<UILayoutElement>()
        ->SetFlexibleHeight(1.0f);
    cancelButton->AddClickedCallback(Dialog::EndCurrentDialog);

    UIInputText *botInputText = GameObjectFactory::CreateUIInputText();
    botInputText->GetText()->SetContent("");

    vlGo->SetParent(scene);

    inputPathCont->SetParent(vlGo);
    inputPathText->GetGameObject()->SetParent(inputPathCont);
    goButton->GetGameObject()->SetParent(inputPathCont);

    GameObjectFactory::CreateUIVSpacer(LayoutSizeType::MIN, 10)
        ->SetParent(vlGo);
    list->GetGameObject()->SetParent(vlGo);
    GameObjectFactory::CreateUIVSpacer(LayoutSizeType::MIN, 10)
        ->SetParent(vlGo);

    botHLGo->SetParent(vlGo);
    GameObjectFactory::CreateUIHSpacer(LayoutSizeType::FLEXIBLE, 0.0001f)
        ->SetParent(botHLGo);
    botInputText->GetGameObject()->SetParent(botHLGo);
    GameObjectFactory::CreateUIHSpacer(LayoutSizeType::PREFERRED, 10)
        ->SetParent(botHLGo);
    cancelButton->GetGameObject()->SetParent(botHLGo);
    GameObjectFactory::CreateUIHSpacer(LayoutSizeType::PREFERRED, 5)
        ->SetParent(botHLGo);
    openButton->GetGameObject()->SetParent(botHLGo);

    Path initDirPath = initialDirPath;
    if (!initDirPath.IsDir())
    {
        initDirPath = Paths::GetEngineDir();
    }
    fileList->SetCurrentPath(initDirPath);

    // Params
    if (outFileList)
    {
        *outFileList = fileList;
    }
    if (outBotLeftButton)
    {
        *outBotLeftButton = cancelButton;
    }
    if (outBotRightButton)
    {
        *outBotRightButton = openButton;
    }
    if (outBotInputText)
    {
        *outBotInputText = botInputText;
    }
}

Scene *Dialog::CreateGetStringScene(const String &msg, const String &hint)
{
    Scene *scene = GameObjectFactory::CreateUIScene();

    UIVerticalLayout *sceneVL = scene->AddComponent<UIVerticalLayout>();
    sceneVL->SetPaddings(10, 10, 10, 20);

    UILabel *msgLabel = GameObjectFactory::CreateUILabel();
    GameObject *msgGo = msgLabel->GetGameObject();
    UITextRenderer *msgText = msgLabel->GetText();
    msgText->SetWrapping(true);
    msgText->SetContent(msg);
    msgText->SetTextColor(Color::Black());
    msgText->SetTextSize(12);
    msgText->SetHorizontalAlign(HorizontalAlignment::LEFT);
    msgText->SetVerticalAlign(VerticalAlignment::CENTER);

    GameObject *buttonsGo = GameObjectFactory::CreateUIGameObject();
    UIHorizontalLayout *buttonsHL =
        buttonsGo->AddComponent<UIHorizontalLayout>();
    buttonsHL->SetChildrenVerticalStretch(Stretch::FULL);
    buttonsHL->SetSpacing(20);
    buttonsHL->SetPaddings(5);
    UILayoutElement *buttonsGoLE = buttonsGo->AddComponent<UILayoutElement>();
    buttonsGoLE->SetFlexibleWidth(1.0f);
    buttonsGoLE->SetFlexibleHeight(0.0f);

    UIInputText *inputText = GameObjectFactory::CreateUIInputText();
    inputText->GetLabel()->SetSelectAllOnFocus(true);
    inputText->GetText()->SetContent(hint);

    UIAutoFocuser *autoFocuser = scene->AddComponent<UIAutoFocuser>();
    autoFocuser->SetFocusableToAutoFocus(inputText->GetFocusable());
    Dialog::s_resultString = hint;

    UIButton *okButton = GameObjectFactory::CreateUIButton("Ok");
    okButton->AddClickedCallback(Dialog::OnOkClicked);

    msgGo->SetParent(scene);
    GameObjectFactory::CreateUIVSpacer(LayoutSizeType::MIN, 10.0f)
        ->SetParent(scene);
    inputText->GetGameObject()->SetParent(scene);
    GameObjectFactory::CreateUIVSpacer(LayoutSizeType::FLEXIBLE, 99999.0f)
        ->SetParent(scene);
    buttonsGo->SetParent(scene);
    GameObjectFactory::CreateUIHSpacer(LayoutSizeType::FLEXIBLE, 0.0001f)
        ->SetParent(buttonsGo);
    okButton->GetGameObject()->SetParent(buttonsGo);

    class GetSceneController : public Component,
                               public EventListener<IEventsValueChanged>
    {
    public:
        UIButton *m_okButton;
        UIInputText *m_inputText;

        void OnUpdate() override
        {
            Component::OnUpdate();
            if (Input::GetKeyDown(Key::ENTER))
            {
                m_okButton->Click();
            }
        }

        virtual void OnValueChanged(
            EventEmitter<IEventsValueChanged> *object) override
        {
            ASSERT(m_inputText == object);
            Dialog::s_resultString = m_inputText->GetText()->GetContent();
        }
    };

    GetSceneController *gsc =
        inputText->GetGameObject()->AddComponent<GetSceneController>();
    gsc->m_inputText = inputText;
    gsc->m_inputText->EventEmitter<IEventsValueChanged>::RegisterListener(gsc);
    gsc->m_okButton = okButton;

    return scene;
}

Scene *Dialog::CreateYesNoCancelScene(const String &msg)
{
    Scene *scene = GameObjectFactory::CreateUIScene();

    GameObject *container = GameObjectFactory::CreateUIGameObject();
    UIVerticalLayout *containerL = container->AddComponent<UIVerticalLayout>();
    containerL->SetPaddings(20);

    GameObject *mainVLayoutGo = GameObjectFactory::CreateUIGameObject();
    mainVLayoutGo->AddComponent<UIVerticalLayout>();

    GameObject *iconGo = GameObjectFactory::CreateUIGameObject();
    UILayoutElement *iconLE = iconGo->AddComponent<UILayoutElement>();
    iconLE->SetMinSize(Vector2i(45));
    UIImageRenderer *icon = iconGo->AddComponent<UIImageRenderer>();
    icon->SetImageTexture(TextureFactory::GetWarningIcon());
    icon->GetImageTexture()->SetFilterMode(GL::FilterMode::BILINEAR);

    GameObject *hLayoutGo = GameObjectFactory::CreateUIGameObjectNamed("HL");
    UIHorizontalLayout *hLayout = hLayoutGo->AddComponent<UIHorizontalLayout>();
    hLayout->SetChildrenHorizontalAlignment(HorizontalAlignment::CENTER);
    hLayout->SetChildrenVerticalAlignment(VerticalAlignment::CENTER);

    GameObject *msgGo = GameObjectFactory::CreateUIGameObjectNamed("MSGGo");
    UITextRenderer *text = msgGo->AddComponent<UITextRenderer>();
    text->SetWrapping(true);
    text->SetContent(msg);
    text->SetTextColor(Color::Black());
    text->SetTextSize(12);
    text->SetHorizontalAlign(HorizontalAlignment::LEFT);
    text->SetVerticalAlign(VerticalAlignment::TOP);

    GameObject *buttonsGo = GameObjectFactory::CreateUIGameObject();
    GameObject *hSpacer = GameObjectFactory::CreateUIHSpacer();
    UIHorizontalLayout *buttonsHL =
        buttonsGo->AddComponent<UIHorizontalLayout>();
    buttonsHL->SetChildrenVerticalStretch(Stretch::FULL);
    buttonsHL->SetSpacing(20);
    buttonsHL->SetPaddings(5);
    UILayoutElement *buttonsGoLE = buttonsGo->AddComponent<UILayoutElement>();
    buttonsGoLE->SetFlexibleWidth(1.0f);
    buttonsGoLE->SetFlexibleHeight(0.0f);

    UIButton *buttonYes = GameObjectFactory::CreateUIButton("Yes");
    buttonYes->AddClickedCallback(Dialog::OnYesClicked);

    UIButton *buttonNo = GameObjectFactory::CreateUIButton("No");
    buttonNo->AddClickedCallback(Dialog::OnNoClicked);

    UIButton *buttonCancel = GameObjectFactory::CreateUIButton("Cancel");
    buttonCancel->AddClickedCallback(Dialog::OnCancelClicked);

    UIAutoFocuser *autoFocus = scene->AddComponent<UIAutoFocuser>();
    autoFocus->SetFocusableToAutoFocus(buttonYes->GetFocusable());

    container->SetParent(scene);
    mainVLayoutGo->SetParent(container);
    hLayoutGo->SetParent(mainVLayoutGo);
    iconGo->SetParent(hLayoutGo);
    GameObjectFactory::CreateUIHSpacer(LayoutSizeType::MIN, 20)
        ->SetParent(hLayoutGo);
    msgGo->SetParent(hLayoutGo);
    GameObjectFactory::CreateUIHSeparator(LayoutSizeType::MIN, 20)
        ->SetParent(mainVLayoutGo);
    buttonsGo->SetParent(mainVLayoutGo);
    hSpacer->SetParent(buttonsGo);
    buttonYes->GetGameObject()->SetParent(buttonsGo);
    buttonNo->GetGameObject()->SetParent(buttonsGo);
    buttonCancel->GetGameObject()->SetParent(buttonsGo);

    return scene;
}

Scene *Dialog::CreateMsgScene(const String &msg)
{
    Scene *scene = GameObjectFactory::CreateUIScene();

    GameObject *container = GameObjectFactory::CreateUIGameObject();
    UIVerticalLayout *containerL = container->AddComponent<UIVerticalLayout>();
    containerL->SetPaddings(20);

    GameObject *mainVLayoutGo = GameObjectFactory::CreateUIGameObject();
    mainVLayoutGo->AddComponent<UIVerticalLayout>();

    GameObject *iconGo = GameObjectFactory::CreateUIGameObject();
    UILayoutElement *iconLE = iconGo->AddComponent<UILayoutElement>();
    iconLE->SetMinSize(Vector2i(45));
    UIImageRenderer *icon = iconGo->AddComponent<UIImageRenderer>();
    icon->SetImageTexture(TextureFactory::GetErrorIcon());
    icon->GetImageTexture()->SetFilterMode(GL::FilterMode::BILINEAR);

    GameObject *hLayoutGo = GameObjectFactory::CreateUIGameObjectNamed("HL");
    UIHorizontalLayout *hLayout = hLayoutGo->AddComponent<UIHorizontalLayout>();
    hLayout->SetChildrenHorizontalAlignment(HorizontalAlignment::CENTER);
    hLayout->SetChildrenVerticalAlignment(VerticalAlignment::CENTER);
    UILayoutElement *hLayoutLE = hLayoutGo->AddComponent<UILayoutElement>();
    hLayoutLE->SetPreferredSize(Vector2i::One());

    GameObject *msgGo = GameObjectFactory::CreateUIGameObjectNamed("MSGGo");
    UITextRenderer *text = msgGo->AddComponent<UITextRenderer>();
    text->SetWrapping(true);
    text->SetContent(msg);
    text->SetTextColor(Color::Black());
    text->SetTextSize(12);
    text->SetHorizontalAlign(HorizontalAlignment::CENTER);
    text->SetVerticalAlign(VerticalAlignment::CENTER);

    GameObject *buttonsGo = GameObjectFactory::CreateUIGameObject();
    GameObject *hSpacer = GameObjectFactory::CreateUIHSpacer();
    UIHorizontalLayout *buttonsHL =
        buttonsGo->AddComponent<UIHorizontalLayout>();
    buttonsHL->SetSpacing(20);
    buttonsHL->SetPaddings(5);

    UIButton *button0 = GameObjectFactory::CreateUIButton("Cancel");
    button0->AddClickedCallback(Dialog::OnNeedToEndDialog);

    UIButton *button1 = GameObjectFactory::CreateUIButton();
    button1->AddClickedCallback(Dialog::OnNeedToEndDialog);

    container->SetParent(scene);
    mainVLayoutGo->SetParent(container);
    hLayoutGo->SetParent(mainVLayoutGo);
    iconGo->SetParent(hLayoutGo);
    GameObjectFactory::CreateUIHSpacer(LayoutSizeType::MIN, 20)
        ->SetParent(hLayoutGo);
    msgGo->SetParent(hLayoutGo);
    GameObjectFactory::CreateUIHSeparator(LayoutSizeType::MIN, 20)
        ->SetParent(mainVLayoutGo);
    buttonsGo->SetParent(mainVLayoutGo);
    hSpacer->SetParent(buttonsGo);
    button0->GetGameObject()->SetParent(buttonsGo);
    button1->GetGameObject()->SetParent(buttonsGo);

    return scene;
}

void Dialog::AcceptDialogPath(const Path &path)
{
    OnDialogPathChanged(path);
    OnOkClicked();
}

void Dialog::OnDialogPathChanged(const Path &path)
{
    Dialog::s_resultPath = path;
}

void Dialog::OnOkClicked()
{
    Dialog::s_okPressed = true;
    OnNeedToEndDialog();
}

void Dialog::OnYesClicked()
{
    Dialog::s_resultYesNoCancel = Dialog::YesNoCancel::YES;
    OnNeedToEndDialog();
}

void Dialog::OnNoClicked()
{
    Dialog::s_resultYesNoCancel = Dialog::YesNoCancel::NO;
    OnNeedToEndDialog();
}

void Dialog::OnCancelClicked()
{
    Dialog::s_resultYesNoCancel = Dialog::YesNoCancel::CANCEL;
    OnNeedToEndDialog();
}

void Dialog::OnNeedToEndDialog()
{
    EndCurrentDialog();
}
