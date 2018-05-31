#ifndef IFOCUSLISTENER_H
#define IFOCUSLISTENER_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD class IFocusable;

enum class ClickType { DOWN, FULL, DOUBLE };

class IFocusListener
{
public:
    virtual void OnFocusTaken(IFocusable *focusable) { (void)focusable; }
    virtual void OnFocusLost(IFocusable *focusable) { (void)focusable; }

    virtual void OnMouseEnter(IFocusable *focusable) { (void)focusable; }
    virtual void OnMouseExit(IFocusable *focusable) { (void)focusable; }

    virtual void OnStartedBeingPressed(IFocusable *focusable) { (void)focusable; }
    virtual void OnStoppedBeingPressed(IFocusable *focusable) { (void)focusable; }

    virtual void OnClicked(IFocusable *focusable, ClickType clickType)
    {
        (void)focusable;
    }
};

NAMESPACE_BANG_END

#endif // IFOCUSLISTENER_H
