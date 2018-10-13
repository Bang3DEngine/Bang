#ifndef ITREEITEM_H
#define ITREEITEM_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIFocusable;

class ITreeItem
{
public:
    virtual UIFocusable *GetTreeItemFocusable() = 0;

protected:
    ITreeItem() = default;
    virtual ~ITreeItem() = default;
};

NAMESPACE_BANG_END

#endif // IEVENTSRENDERERCHANGED_H
