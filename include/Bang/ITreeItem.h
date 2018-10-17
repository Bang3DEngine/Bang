#ifndef ITREEITEM_H
#define ITREEITEM_H

#include "Bang/Bang.h"

namespace Bang
{
class UIFocusable;

class ITreeItem
{
public:
    virtual UIFocusable *GetTreeItemFocusable() = 0;

protected:
    ITreeItem() = default;
    virtual ~ITreeItem() = default;
};
}

#endif  // IEVENTSRENDERERCHANGED_H
