#ifndef UIDIRLAYOUTMOVABLESEPARATOR_H
#define UIDIRLAYOUTMOVABLESEPARATOR_H

#include "Bang/Bang.h"
#include "Bang/Axis.h"
#include "Bang/Component.h"
#include "Bang/UIFocusable.h"
#include "Bang/LineRenderer.h"
#include "Bang/IFocusListener.h"
#include "Bang/LayoutSizeType.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;

class UIDirLayoutMovableSeparator : public Component,
                                    public IFocusListener
{
    COMPONENT(UIDirLayoutMovableSeparator)

public:
	UIDirLayoutMovableSeparator();
	virtual ~UIDirLayoutMovableSeparator();

    void OnUpdate() override;

    void SetAxis(Axis axis);
    Axis GetAxis() const;

    // IFocusListener
    virtual void OnMouseEnter(IFocusable *focusable) override;
    virtual void OnMouseExit(IFocusable *focusable) override;

    static UIDirLayoutMovableSeparator *CreateInto(GameObject *go);

private:
    Axis m_axis = Undef<Axis>();
    UIFocusable *p_focusable = nullptr;
    LineRenderer *p_lineRenderer = nullptr;

    void UpdateLayout();
};

NAMESPACE_BANG_END

#endif // UIDIRLAYOUTMOVABLESEPARATOR_H

