#ifndef UILAYOUTIGNORER_H
#define UILAYOUTIGNORER_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

class UILayoutIgnorer : public Component
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UILayoutIgnorer)

public:
	UILayoutIgnorer();
	virtual ~UILayoutIgnorer();

    void SetIgnoreLayout(bool ignoreLayout);
    bool IsIgnoreLayout() const;

private:
    bool m_ignoreLayout = true;
};

NAMESPACE_BANG_END

#endif // UILAYOUTIGNORER_H_H

