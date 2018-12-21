#ifndef UISCROLLPANEL_H
#define UISCROLLPANEL_H

#include <vector>

#include "Bang/Alignment.h"
#include "Bang/Array.tcc"
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
class UIScrollBar;

enum class ShowScrollMode
{
    NEVER,
    WHEN_NEEDED,
    ALWAYS
};

class UIScrollPanel : public Component, public EventListener<IEventsFocus>
{
    COMPONENT(UIScrollPanel)

public:
    virtual ~UIScrollPanel() override;

    void OnUpdate() override;
    void OnPostUpdate() override;

    void SetForceVerticalFit(bool forceVerticalFit);
    void SetForceHorizontalFit(bool forceHorizontalFit);
    void SetVerticalScrollBarSide(HorizontalSide side);
    void SetHorizontalScrollBarSide(VerticalSide side);
    void SetVerticalShowScrollMode(ShowScrollMode showScrollMode);
    void SetHorizontalShowScrollMode(ShowScrollMode showScrollMode);
    void SetVerticalScrollEnabled(bool enabled);
    void SetHorizontalScrollEnabled(bool enabled);
    void SetScrolling(const Vector2i &scrolling);
    void SetScrollingPercent(const Vector2 &scrollingPercent);

    Vector2i GetScrolling() const;
    Vector2 GetScrollingPercent() const;
    bool GetForceVerticalFit() const;
    bool GetForceHorizontalFit() const;
    HorizontalSide GetVerticalScrollBarSide() const;
    VerticalSide GetHorizontalScrollBarSide() const;
    ShowScrollMode GetVerticalShowScrollMode() const;
    ShowScrollMode GetHorizontalShowScrollMode() const;
    bool IsVerticalScrollEnabled() const;
    bool IsHorizontalScrollEnabled() const;
    UIScrollArea *GetScrollArea() const;
    UIScrollBar *GetVerticalScrollBar() const;
    UIScrollBar *GetHorizontalScrollBar() const;
    Vector2 GetContentSize() const;
    Vector2 GetContainerSize() const;
    Vector2 GetMaxScrollLength() const;

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

private:
    const static float WheelScrollSpeedPx;

    bool m_forceVerticalFit = false;
    bool m_forceHorizontalFit = false;

    UIImageRenderer *p_border = nullptr;
    UIScrollArea *p_scrollArea = nullptr;
    UIScrollBar *p_verticalScrollBar = nullptr;
    UIScrollBar *p_horizontalScrollBar = nullptr;
    HorizontalSide m_verticalScrollBarSide = Undef<HorizontalSide>();
    VerticalSide m_horizontalScrollBarSide = Undef<VerticalSide>();
    ShowScrollMode m_verticalShowScrollMode = Undef<ShowScrollMode>();
    ShowScrollMode m_horizontalShowScrollMode = Undef<ShowScrollMode>();
    bool m_verticalScrollEnabled = true;
    bool m_horizontalScrollEnabled = true;

    UIScrollPanel();

    void UpdateScrollUI();

    void HandleScrollAreaRectTransform();
    void HandleScrollShowMode(const Vector2 &contentSize,
                              const Vector2 &containerSize);

    bool IsVerticalScrollEnabledAndNoFit() const;
    bool IsHorizontalScrollEnabledAndNoFit() const;

    static UIScrollPanel *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};
}

#endif  // UISCROLLPANEL_H_H
