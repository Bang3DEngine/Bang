#include "Bang/UIFileList.h"

#include "Bang/Alignment.h"
#include "Bang/Array.tcc"
#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/List.tcc"
#include "Bang/Paths.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIList.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UIVerticalLayout.h"

using namespace Bang;

UIFileList::UIFileList()
{
    SET_INSTANCE_CLASS_ID(UIFileList)
}

UIFileList::~UIFileList()
{
}

void UIFileList::OnStart()
{
    Component::OnStart();
    SetCurrentPath(GetCurrentPath());
}

void UIFileList::OnUpdate()
{
    Component::OnUpdate();
}

void UIFileList::SetFileExtensions(const Array<String> &extensions)
{
    m_fileExtensions = extensions;
    UpdateEntries();
}

void UIFileList::AddFileAcceptedCallback(UIFileList::PathCallback callback)
{
    m_fileAcceptedCallback.PushBack(callback);
}

void UIFileList::AddPathChangedCallback(UIFileList::PathCallback callback)
{
    m_pathChangedCallback.PushBack(callback);
}

void UIFileList::SetShowOnlyDirectories(bool showOnlyDirectories)
{
    if (m_showOnlyDirectories != showOnlyDirectories)
    {
        m_showOnlyDirectories = showOnlyDirectories;
        UpdateEntries();
    }
}

Path UIFileList::GetCurrentSelectedPath() const
{
    if (p_selectedItem)
    {
        return p_selectedItem->GetPath();
    }
    return GetCurrentPath();
}

void UIFileList::SetCurrentPath(const Path &currentPath)
{
    if (m_currentPath != currentPath)
    {
        m_currentPath = currentPath;
        for (auto cb : m_pathChangedCallback)
        {
            cb(GetCurrentPath());
        }
        UpdateEntries();
    }
}

const Path &UIFileList::GetCurrentPath() const
{
    return m_currentPath;
}

bool UIFileList::GetShowOnlyDirectories() const
{
    return m_showOnlyDirectories;
}

const Array<String> &UIFileList::GetFileExtensions() const
{
    return m_fileExtensions;
}

void UIFileList::UpdateEntries()
{
    Array<Path> paths = GetCurrentPath().GetSubPaths(FindFlag::SIMPLE);

    if (!GetFileExtensions().IsEmpty())
    {
        Paths::FilterByExtension(&paths, GetFileExtensions());
    }
    Paths::SortPathsByName(&paths);

    if (GetShowOnlyDirectories())
    {
        Paths::RemoveFilesFromArray(&paths);
    }
    paths.PushFront(Path(".."));

    UIList *uiList = GetGameObject()->GetComponent<UIList>();
    uiList->Clear();

    for (const Path &path : paths)
    {
        UIFileListItem *item = new UIFileListItem();
        item->SetPath(path);

        uiList->AddItem(item);
    }
    // uiList->SetSelection(1);

    uiList->SetSelectionCallback(
        [this, uiList](GameObject *go, UIList::Action action) {
            UIFileListItem *item = DCAST<UIFileListItem *>(go);
            if (action == UIList::Action::SELECTION_IN)
            {
                p_selectedItem = item;
            }
            else if (action == UIList::Action::SELECTION_OUT)
            {
                p_selectedItem = nullptr;
            }
            else if (action == UIList::Action::PRESSED ||
                     action == UIList::Action::DOUBLE_CLICKED_LEFT)
            {
                Path itemPath = item->GetPath();
                if (itemPath.GetAbsolute() == "..")
                {
                    this->SetCurrentPath(GetCurrentPath().GetDirectory());
                }
                else if (itemPath.IsDir())
                {
                    this->SetCurrentPath(itemPath);
                }
                else if (itemPath.IsFile())
                {
                    this->SetCurrentPath(itemPath);
                    for (auto cb : m_fileAcceptedCallback)
                    {
                        cb(itemPath);
                    }
                }
            }
        });
}

// UIFileListItem
UIFileListItem::UIFileListItem()
{
    GameObjectFactory::CreateUIGameObjectInto(this);
    AddComponent<UIFocusable>();

    UIVerticalLayout *vl = AddComponent<UIVerticalLayout>();
    vl->SetPaddings(5);

    GameObject *container = GameObjectFactory::CreateUIGameObject();
    m_text = container->AddComponent<UITextRenderer>();
    m_text->SetTextSize(12);
    m_text->SetHorizontalAlign(HorizontalAlignment::LEFT);

    container->SetParent(this);
}

UIFileListItem::~UIFileListItem()
{
}

void UIFileListItem::SetPath(const Path &path)
{
    if (path != GetPath())
    {
        m_path = path;
        m_text->SetContent((path.IsFile() ? "File - " : "Dir  - ") +
                           path.GetNameExt());
    }
}

const Path &UIFileListItem::GetPath()
{
    return m_path;
}
