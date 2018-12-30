#include "Bang/UITreeItemContainer.h"

#include "Bang/Color.h"
#include "Bang/DPtr.tcc"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/ITreeItem.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/Stretch.h"
#include "Bang/String.h"
#include "Bang/TextureFactory.h"
#include "Bang/UIButton.h"
#include "Bang/UIDragDroppable.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UITheme.h"

namespace Bang
{
class Texture2D;
}

using namespace Bang;

UITreeItemContainer::UITreeItemContainer()
{
    GameObjectFactory::CreateUIGameObjectInto(this);

    UIHorizontalLayout *hLayout = AddComponent<UIHorizontalLayout>();
    hLayout->SetChildrenVerticalStretch(Stretch::FULL);

    p_userItemContainer = GameObjectFactory::CreateUIGameObject();
    p_userItemContainer->SetName("UserItemContainer");

    UIHorizontalLayout *containerHLayout =
        p_userItemContainer->AddComponent<UIHorizontalLayout>();
    containerHLayout->SetChildrenHorizontalStretch(Stretch::FULL);

    UILayoutElement *containerLE =
        p_userItemContainer->AddComponent<UILayoutElement>();
    containerLE->SetFlexibleSize(Vector2(99999.0f));

    p_indentSpacer =
        GameObjectFactory::CreateUISpacer(LayoutSizeType::MIN, Vector2::Zero());
    p_indentSpacer->SetName("IndentSpacer");

    p_dragBg = AddComponent<UIImageRenderer>();

    Texture2D *iconTex = TextureFactory::GetDownArrowIcon();
    p_collapseButton = GameObjectFactory::CreateUIButton("", iconTex);
    p_collapseButton->GetLayoutElement()->SetFlexibleSize(Vector2::Zero());
    p_collapseButton->GetGameObject()->SetName("CollapseButton");
    p_collapseButton->SetIcon(iconTex, Vector2i(8), 0);
    p_collapseButton->GetIcon()->SetTint(Color::Black());
    p_collapseButton->GetBorder()->SetVisible(false);
    p_collapseButton->GetBackground()->SetVisible(false);

    p_defaultFocusable = AddComponent<UIFocusable>();

    p_dragDroppable = AddComponent<UIDragDroppable>();
    p_dragDroppable->SetShowDragDropGameObject(false);
    p_dragDroppable->SetFocusable(p_defaultFocusable);

    p_indentSpacer->SetParent(this);
    p_collapseButton->GetGameObject()->SetParent(this);
    p_userItemContainer->SetParent(this);
    SetBeingDragged(false);
}

UITreeItemContainer::~UITreeItemContainer()
{
}

void UITreeItemContainer::SetCollapsable(bool collapsable)
{
    GetCollapseButton()->SetEnabled(collapsable);
    GetCollapseButton()->GetIcon()->SetVisible(collapsable);
    GetCollapseButton()->GetFocusable()->SetEnabled(collapsable);
}

void UITreeItemContainer::SetCollapsed(bool collapsed)
{
    if (collapsed != IsCollapsed())
    {
        m_collapsed = collapsed;
        Texture2D *iconTex = IsCollapsed() ? TextureFactory::GetRightArrowIcon()
                                           : TextureFactory::GetDownArrowIcon();
        GetCollapseButton()->SetIconTexture(iconTex);
    }
}

void UITreeItemContainer::SetContainedItem(GOItem *go)
{
    p_containedGameObject = go;
    p_containedGameObject->SetParent(p_userItemContainer);
    SetName("GOItemCont_" + go->GetName());
    p_dragDroppable->SetFocusable(GetFocusable());
}

GameObject *UITreeItemContainer::GetContainedItem() const
{
    return p_containedGameObject;
}

void UITreeItemContainer::SetIndentation(int indentationPx)
{
    m_indentationPx = indentationPx;
    p_indentSpacer->GetComponent<UILayoutElement>()->SetMinWidth(indentationPx);
}

void UITreeItemContainer::SetBeingDragged(bool beingDragged)
{
    p_dragBg->SetTint(beingDragged ? UITheme::GetOverColor() : Color::Zero());
}

bool UITreeItemContainer::IsCollapsed() const
{
    return m_collapsed;
}

int UITreeItemContainer::GetIndentationPx() const
{
    return m_indentationPx;
}

UIFocusable *UITreeItemContainer::GetFocusable() const
{
    if (GOItem *item = GetContainedItem())
    {
        if (ITreeItem *treeItem = DCAST<ITreeItem *>(item))
        {
            if (UIFocusable *itemFocusable = treeItem->GetTreeItemFocusable())
            {
                return itemFocusable;
            }
        }
    }
    return p_defaultFocusable;
}

UIButton *UITreeItemContainer::GetCollapseButton() const
{
    return p_collapseButton;
}

UIDragDroppable *UITreeItemContainer::GetDragDroppable() const
{
    return p_dragDroppable;
}
