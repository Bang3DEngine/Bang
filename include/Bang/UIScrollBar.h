#ifndef UISCROLLBAR_H
#define UISCROLLBAR_H

#include <vector>

#include "Bang/Alignment.h"
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
class UIFocusable;
class UIImageRenderer;
class UIScrollArea;

class UIScrollBar : public Component, public EventListener<IEventsFocus>
{
    COMPONENT(UIScrollBar)

public:
    UIScrollBar();
    virtual ~UIScrollBar() override;

    void OnUpdate() override;

    void SetSide(Side side);
    void SetScrolling(int scrollingPx);
    void SetScrollingPercent(float percent);
    void SetLength(int lengthPx);
    void SetLengthPercent(float lengthPercent);
    void SetThickness(int thickPx);

    Side GetSide() const;
    int GetScrolling() const;
    float GetScrollingPercent() const;
    int GetLength() const;
    int GetThickness() const;
    Axis GetScrollAxis() const;

    bool IsBeingGrabbed() const;

private:
    int m_length = 0;
    int m_thickness = 0;
    int m_scrollingPx = 0;
    bool m_wasGrabbed = false;
    Vector2 m_grabOffsetPx = Vector2::Zero();

    Side m_side = Undef<Side>();

    GameObject *p_bar = nullptr;
    UIFocusable *p_barFocusable = nullptr;
    UIImageRenderer *p_barImg = nullptr;
    UIScrollArea *p_scrollArea = nullptr;
    UIFocusable *p_scrollAreaFocusable = nullptr;

    static UIScrollBar *CreateInto(GameObject *go);

    void OnMouseEnter();
    void OnMouseExit();
    void UpdateLengthThicknessMargins();

    int GetScrollingSpacePx() const;
    AARect GetScrollingRect() const;
    UIScrollArea *GetScrollArea() const;
    UIFocusable *GetFocusable() const;
    GameObject *GetBar() const;

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

    friend class GameObjectFactory;
};
}

#endif  // UISCROLLBAR_H_H
