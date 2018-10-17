#ifndef UIDIRLAYOUTMOVABLESEPARATOR_H
#define UIDIRLAYOUTMOVABLESEPARATOR_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEvents.h"
#include "Bang/IEventsFocus.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;
FORWARD class LineRenderer;
FORWARD class UIFocusable;

class UIDirLayoutMovableSeparator : public Component,
                                    public EventListener<IEventsFocus>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIDirLayoutMovableSeparator)

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

NAMESPACE_BANG_END

#endif // UIDIRLAYOUTMOVABLESEPARATOR_H

