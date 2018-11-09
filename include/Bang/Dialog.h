#ifndef DIALOG_H
#define DIALOG_H

#include <stack>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Path.h"
#include "Bang/String.h"

namespace Bang
{
class DialogWindow;
class Scene;
class UIButton;
class UIFileList;
class UIInputText;
class Window;

class Dialog
{
public:
    enum YesNoCancel
    {
        YES,
        NO,
        CANCEL
    };

    static DialogWindow *Error(const String &title, const String &msg);
    static String GetString(const String &title,
                            const String &msg,
                            const String &hint = "");
    static YesNoCancel GetYesNoCancel(const String &title, const String &msg);
    static Path OpenFilePath(const String &title,
                             const Array<String> &extensions = {},
                             const Path &initialDirPath = Path::Empty());
    static Path OpenDirectory(const String &title,
                              const Path &initialDirPath = Path::Empty());
    static Path SaveFilePath(const String &title,
                             const String &extension,
                             const Path &initialDirPath = Path::Empty(),
                             const String &initialFileName = "Unnamed");

    static DialogWindow *BeginDialogCreation(const String &title,
                                             int sizeX,
                                             int sizeY,
                                             bool resizable,
                                             bool modal);
    static void EndDialogCreation(Scene *scene);

    static void EndCurrentDialog();

    Dialog() = delete;

private:
    static bool s_okPressed;
    static Path s_resultPath;
    static String s_resultString;
    static YesNoCancel s_resultYesNoCancel;

    static std::stack<DialogWindow *> s_dialogCreation_dialogWindows;
    static std::stack<Window *> s_dialogCreation_prevActiveWindows;
    static std::stack<bool> s_dialogCreation_modalBooleans;

    static DialogWindow *s_currentDialog;

    static void CreateSaveFilePathSceneInto(Scene *scene,
                                            const String &extension,
                                            const Path &initialDirPath,
                                            const String &initialFileName);
    static void CreateOpenFilePathSceneInto(Scene *scene,
                                            bool openDir,
                                            const Array<String> &extensions,
                                            const Path &initialDirPath);
    static void CreateFilePathBaseSceneInto(Scene *scene,
                                            const Path &initialDirPath,
                                            UIFileList **outFileList,
                                            UIButton **outBotLeftButton,
                                            UIButton **outBotRightButton,
                                            UIInputText **botInputText);
    static Scene *CreateGetStringScene(const String &msg, const String &hint);
    static Scene *CreateYesNoCancelScene(const String &msg);

    static Scene *CreateMsgScene(const String &msg);

    static void OnOkClicked();
    static void OnYesClicked();
    static void OnNoClicked();
    static void OnCancelClicked();
    static void OnNeedToEndDialog();
    static void AcceptDialogPath(const Path &path);
    static void OnDialogPathChanged(const Path &path);
};
}  // namespace Bang

#endif  // DIALOG_H
