#ifndef UIFILELIST_H
#define UIFILELIST_H

#include <functional>

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/GameObject.h"
#include "Bang/List.h"
#include "Bang/Path.h"
#include "Bang/String.h"

namespace Bang
{
class UIFileListItem;
class UITextRenderer;

class UIFileList : public Component
{
    COMPONENT(UIFileList)

public:
    UIFileList();
    virtual ~UIFileList() override;

    void OnStart() override;
    void OnUpdate() override;

    using PathCallback = std::function<void(const Path &)>;

    void SetFileExtensions(const Array<String> &extensions);
    void SetCurrentPath(const Path &currentPath);
    void AddFileAcceptedCallback(PathCallback callback);
    void AddPathChangedCallback(PathCallback callback);
    void SetShowOnlyDirectories(bool showOnlyDirectories);

    Path GetCurrentSelectedPath() const;
    const Path &GetCurrentPath() const;
    bool GetShowOnlyDirectories() const;
    const Array<String> &GetFileExtensions() const;

private:
    Path m_currentPath;
    Array<String> m_fileExtensions;
    UIFileListItem *p_selectedItem = nullptr;
    List<PathCallback> m_fileAcceptedCallback;
    List<PathCallback> m_pathChangedCallback;
    bool m_showOnlyDirectories = false;

    void UpdateEntries();
};

class UIFileListItem : public GameObject
{
    GAMEOBJECT(UIFileListItem);

public:
    UIFileListItem();

    void SetPath(const Path &path);
    const Path &GetPath();

protected:
    virtual ~UIFileListItem() override;

private:
    Path m_path = Path::Empty();
    UITextRenderer *m_text = nullptr;
};
}  // namespace Bang

#endif  // UIFILELIST_H
