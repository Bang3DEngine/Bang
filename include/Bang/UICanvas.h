#ifndef CANVAS_H
#define CANVAS_H

#include <stack>

#include "Bang/Set.h"
#include "Bang/Component.h"
#include "Bang/IEventsFocus.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;
FORWARD class UIDragDroppable;
FORWARD class UILayoutManager;
FORWARD class IEventsDragDrop;

class UICanvas : public Component,
                 public EventListener<IEventsDestroy>
{
    COMPONENT(UICanvas)

public:
    UICanvas();
    virtual ~UICanvas();

    // Component
    virtual void OnStart() override;
    virtual void OnUpdate() override;
    virtual void OnAfterChildrenUpdate() override;
    virtual void OnRender(RenderPass renderPass) override;

    void InvalidateCanvas();

    void ClearFocus();
    void SetFocus(UIFocusable *focusable);

    UIFocusable* GetFocus();
    const Vector2i& GetLastMousePosition() const;
    UIFocusable* GetFocusableUnderMouseTopMost() const;
    bool HasFocusFocusable(const UIFocusable *focusable);
    bool HasFocus(const Component *comp, bool recursive = false);
    bool HasFocus(const GameObject *go, bool recursive = false);
    bool IsMouseOverFocusable(const UIFocusable *focusable);
    bool IsMouseOver(const Component *comp, bool recursive = false);
    bool IsMouseOver(const GameObject *go, bool recursive = false);

    void NotifyDragStarted(UIDragDroppable *dragDroppable);

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // ISerializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    UILayoutManager* GetLayoutManager() const;

    static UICanvas *GetActive(const GameObject *go);
    static UICanvas *GetActive(const Component *comp);

private:
    UILayoutManager *m_uiLayoutManager = nullptr;

    Vector2i m_lastMousePosition = Vector2i(-1);

    UIFocusable *p_focus = nullptr;
    Set<UIFocusable*> p_focusablesUnderMouse;
    UIDragDroppable *p_ddBeingDragged = nullptr;
    UIFocusable *p_focusableUnderMouseTopMost = nullptr;
    Set<UIFocusable*> p_focusablesPotentiallyBeingPressed;

    // IObjectEvents
    void OnDisabled(Object *object) override;

    void RegisterForEvents(UIFocusable *focusable);
    void UnRegisterForEvents(UIFocusable *focusable);

    List<EventListener<IEventsDragDrop>*> GetDragDropListeners() const;

    void GetSortedFocusCandidatesByOcclusionOrder(
            const GameObject *go,
            Array< std::pair<UIFocusable*, AARecti> > *sortedCandidates) const;

    void GetSortedFocusCandidatesByPaintOrder(
            const GameObject *go,
            Array< std::pair<UIFocusable*, AARecti> > *sortedCandidates,
            std::stack<AARecti> *maskRectStack) const;
};

NAMESPACE_BANG_END

#endif // CANVAS_H
