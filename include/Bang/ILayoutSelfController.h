#ifndef ILAYOUTSELFCONTROLLER_H
#define ILAYOUTSELFCONTROLLER_H

#include "Bang/ILayoutController.h"

NAMESPACE_BANG_BEGIN

class ILayoutSelfController : public ILayoutController
{
    bool IsSelfController() const override
    {
        return true;
    }
};

NAMESPACE_BANG_END

#endif // ILAYOUTSELFCONTROLLER_H
