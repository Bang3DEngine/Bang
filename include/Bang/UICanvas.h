#ifndef CANVAS_H
#define CANVAS_H

#include <stack>

#include "Bang/Set.h"
#include "Bang/Component.h"
#include "Bang/IEventsFocus.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;
FORWARD class IFocusable;
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
    void SetFocus(IFocusable *focusable);

    IFocusable* GetFocus();
    const Vector2i& GetLastMousePosition() const;
    IFocusable* GetFocusableUnderMouseTopMost() const;
    bool HasFocusFocusable(const IFocusable *focusable);
    bool HasFocus(const Component *comp, bool recursive = false);
    bool HasFocus(const GameObject *go, bool recursive = false);
    bool IsMouseOverFocusable(const IFocusable *focusable);
    bool IsMouseOver(const Component *comp, bool recursive = false);
    bool IsMouseOver(const GameObject *go, bool recursive = false);

    void NotifyDragStarted(UIDragDroppable *dragDroppable);

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // ISerializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    UILayoutManager* GetLayoutManager() const;

    static UICanvas *GetActive(const GameObject *go);
    static UICanvas *GetActive(const Component *comp);

private:
    UILayoutManager *m_uiLayoutManager = nullptr;

    Vector2i m_lastMousePosition = Vector2i(-1);

    IFocusable *p_focus = nullptr;
    Set<IFocusable*> p_focusablesUnderMouse;
    UIDragDroppable *p_ddBeingDragged = nullptr;
    IFocusable *p_focusableUnderMouseTopMost = nullptr;
    Set<IFocusable*> p_focusablesPotentiallyBeingPressed;

    // IObjectEvents
    void OnDisabled(Object *object) override;

    void RegisterForEvents(IFocusable *focusable);
    void UnRegisterForEvents(IFocusable *focusable);

    List<EventListener<IEventsDragDrop>*> GetDragDropListeners() const;

    void GetSortedFocusCandidatesByOcclusionOrder(
            const GameObject *go,
            Array< std::pair<IFocusable*, AARect> > *sortedCandidates) const;

    void GetSortedFocusCandidatesByPaintOrder(
            const GameObject *go,
            Array< std::pair<IFocusable*, AARect> > *sortedCandidates,
            std::stack<AARect> *maskRectStack) const;
};

NAMESPACE_BANG_END

#endif // CANVAS_H
