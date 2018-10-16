#ifndef CANVAS_H
#define CANVAS_H

#include <functional>
#include <stack>
#include <utility>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEvents.h"
#include "Bang/IEventsFocus.h"
#include "Bang/MetaNode.h"
#include "Bang/RenderPass.h"
#include "Bang/Set.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD_T class EventEmitter;
FORWARD   class GameObject;
FORWARD   class ICloneable;
FORWARD   class IEventsDestroy;
FORWARD   class IEventsDragDrop;
FORWARD   class InputEvent;
FORWARD   class Object;
FORWARD   class UIFocusable;
FORWARD   class UIDragDroppable;
FORWARD   class UILayoutManager;

class UICanvas : public Component,
                 public EventListener<IEventsDestroy>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UICanvas)

public:
    UICanvas();
    virtual ~UICanvas() override;

    // Component
    virtual void OnStart() override;
    virtual void OnUpdate() override;
    virtual void OnBeforeChildrenRender(RenderPass rp) override;

    void InvalidateCanvas();

    void ClearFocus();
    void SetFocus(UIFocusable *focusable,
                  FocusType focusType = FocusType::MOUSE);

    UIFocusable* GetFocus();
    UIFocusable* GetFocusableUnderMouseTopMost() const;
    bool HasFocusFocusable(const UIFocusable *focusable);
    bool HasFocus(const Component *comp, bool recursive = false);
    bool HasFocus(const GameObject *go, bool recursive = false);
    bool IsMouseOverFocusable(const UIFocusable *focusable);
    bool IsMouseOver(const Component *comp, bool recursive = false);
    bool IsMouseOver(const GameObject *go, bool recursive = false);

    void NotifyDragStarted(UIDragDroppable *dragDroppable);
    void DropCurrentDragDroppable();

    Array<EventListener<IEventsDragDrop>*> GetDragDropListeners() const;

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
    Set<UIFocusable*> p_focusablesBeingPressed;
    UILayoutManager *m_uiLayoutManager = nullptr;

    UIFocusable *p_focus = nullptr;
    UIDragDroppable *p_ddBeingDragged = nullptr;
    UIFocusable *p_focusableUnderMouseTopMost = nullptr;

    // IObjectEvents
    void OnDisabled(Object *object) override;

    void SetFocusableUnderMouseTopMost(UIFocusable *focusable,
                                       const InputEvent &inputEvent);

    void RegisterForEvents(UIFocusable *focusable);
    void UnRegisterForEvents(UIFocusable *focusable);
    void RegisterFocusableBeingPressed(UIFocusable *focusable);
    void RegisterFocusableNotBeingPressedAnymore(UIFocusable *focusable);

    void GetSortedFocusCandidatesByOcclusionOrder(
            const GameObject *go,
            Array< std::pair<UIFocusable*, AARecti> > *sortedCandidates) const;

    void GetSortedFocusCandidatesByPaintOrder(
            const GameObject *go,
            Array< std::pair<UIFocusable*, AARecti> > *sortedCandidates,
            std::stack<AARecti> *maskRectStack) const;

    friend class UIFocusable;
};

NAMESPACE_BANG_END

#endif // CANVAS_H
