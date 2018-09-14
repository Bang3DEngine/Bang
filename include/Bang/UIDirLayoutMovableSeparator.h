#ifndef UIDIRLAYOUTMOVABLESEPARATOR_H
#define UIDIRLAYOUTMOVABLESEPARATOR_H

#include "Bang/Bang.h"
#include "Bang/Axis.h"
#include "Bang/Component.h"
#include "Bang/UIFocusable.h"
#include "Bang/LineRenderer.h"
#include "Bang/IEventsFocus.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/IEventsValueChanged.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;

class UIDirLayoutMovableSeparator : public Component,
                                    public EventListener<IEventsFocus>
{
    COMPONENT(UIDirLayoutMovableSeparator)

public:
	UIDirLayoutMovableSeparator();
	virtual ~UIDirLayoutMovableSeparator();

    void OnUpdate() override;

    void SetAxis(Axis axis);
    Axis GetAxis() const;

    // IEventsFocus
    virtual void OnUIEvent(IFocusable *focusable, const UIEventExt &event) override;

    static UIDirLayoutMovableSeparator *CreateInto(GameObject *go);

private:
    Axis m_axis = Undef<Axis>();
    UIFocusable *p_focusable = nullptr;
    LineRenderer *p_lineRenderer = nullptr;

    void UpdateLayout();
};

NAMESPACE_BANG_END

#endif // UIDIRLAYOUTMOVABLESEPARATOR_H

