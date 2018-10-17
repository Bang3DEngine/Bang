#ifndef ILAYOUTSELFCONTROLLER_H
#define ILAYOUTSELFCONTROLLER_H

#include "Bang/ILayoutController.h"

namespace Bang
{
class ILayoutSelfController : public ILayoutController
{
    bool IsSelfController() const override
    {
        return true;
    }
};
}

#endif  // ILAYOUTSELFCONTROLLER_H
