#ifndef UIDIRLAYOUTMOVABLESEPARATOR_H
#define UIDIRLAYOUTMOVABLESEPARATOR_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEvents.h"
#include "Bang/IEventsFocus.h"
#include "Bang/String.h"

namespace Bang
{
class GameObject;
class LineRenderer;
class UIFocusable;

class UIDirLayoutMovableSeparator : public Component,
                                    public EventListener<IEventsFocus>
{
    COMPONENT(UIDirLayoutMovableSeparator)

public:
    UIDirLayoutMovableSeparator();
    virtual ~UIDirLayoutMovableSeparator() override;

    void OnUpdate() override;

    void SetAxis(Axis axis);
    Axis GetAxis() const;

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

    static UIDirLayoutMovableSeparator *CreateInto(GameObject *go);

private:
    Axis m_axis = Undef<Axis>();
    UIFocusable *p_focusable = nullptr;
    LineRenderer *p_lineRenderer = nullptr;

    void UpdateLayout();
};
}

#endif  // UIDIRLAYOUTMOVABLESEPARATOR_H
