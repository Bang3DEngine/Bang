#include "Bang/UICanvas.h"

#include "Bang/Rect.h"
#include "Bang/Input.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Cursor.h"
#include "Bang/MetaNode.h"
#include "Bang/Component.h"
#include "Bang/GameObject.h"
#include "Bang/UIRectMask.h"
#include "Bang/UIFocusable.h"
#include "Bang/SceneManager.h"
#include "Bang/DebugRenderer.h"
#include "Bang/RectTransform.h"
#include "Bang/UIDragDroppable.h"
#include "Bang/UILayoutManager.h"
#include "Bang/IEventsDragDrop.h"

USING_NAMESPACE_BANG

UICanvas::UICanvas()
{
    m_uiLayoutManager = new UILayoutManager();
}

UICanvas::~UICanvas()
{
    delete m_uiLayoutManager;
}

void UICanvas::OnStart()
{
    Component::OnStart();
}

void PropagateUIEvent(GameObject *focusableGo, const UIEvent &event)
{
    ASSERT(focusableGo);

    UIEventResult finalPropResult = UIEventResult::IGNORE;

    Array<UIFocusable*> focusablesInGo = focusableGo->GetComponents<UIFocusable>();

    class DestroyTracker : public GameObject,
                           public EventListener<IEventsDestroy>
    {
    public:
        Array<UIFocusable*> *focusables = nullptr;
        void OnDestroyed(EventEmitter<IEventsDestroy> *ee) override
        {
            ASSERT(focusables);
            focusables->Remove( SCAST<UIFocusable*>(ee) );
        }
    };

    DestroyTracker *destroyTracker = GameObject::Create<DestroyTracker>();
    destroyTracker->focusables = &focusablesInGo;

    for (UIFocusable *focusableInGo : focusablesInGo)
    {
        focusableInGo->EventEmitter<IEventsDestroy>::RegisterListener(destroyTracker);
    }

    for (UIFocusable *focusableInGo : focusablesInGo)
    {
        if (focusableInGo->IsFocusEnabled())
        {
            UIEventResult propagationResult = focusableInGo->ProcessEvent(event);
            if (propagationResult == UIEventResult::INTERCEPT)
            {
                finalPropResult = UIEventResult::INTERCEPT;
            }
        }
    }

    if (finalPropResult == UIEventResult::IGNORE)
    {
        if (GameObject *nextFocusableParent = focusableGo->GetParent())
        {
            PropagateUIEvent(nextFocusableParent, event);
        }
    }

    GameObject::DestroyImmediate(destroyTracker);
}

void PropagateUIEvent(UIFocusable *focusable, const UIEvent &event)
{
    ASSERT(focusable);
    if (GameObject *focusableGo = focusable->GetGameObject())
    {
        PropagateUIEvent(focusableGo, event);
    }
}

void PropagateFocusableUIEvent(UIFocusable *focusable,
                               UIEvent::Type type,
                               const InputEvent &inputEvent)
{
    UIEvent event;
    event.type = type;
    event.mouse.button = inputEvent.mouseButton;
    event.mousePosWindow = inputEvent.GetMousePosWindow();
    event.key.key = inputEvent.key;
    event.key.modifiers = inputEvent.keyModifiers;
    event.wheel.amount = inputEvent.wheelDelta;
    PropagateUIEvent(focusable, event);
};


void UICanvas::OnUpdate()
{
    Component::OnUpdate();

    Array<std::pair<UIFocusable*, AARecti>> focusablesAndRectsVP;
    GetSortedFocusCandidatesByOcclusionOrder(GetGameObject(),
                                             &focusablesAndRectsVP);

    // Process all enqueued InputEvents, transform them to UIEvents and
    // propagate them as we need.

    Array<UIFocusable*> focusables;
    for (const auto& focusableAndRectVP : focusablesAndRectsVP)
    {
        focusables.PushBack(focusableAndRectVP.first);
    }

    Vector2i currentMousePosVP   = Input::GetMousePosition();
    Vector2 currentMousePosVPNDC = Input::GetMousePositionNDC();
    Vector2i currentMouseWindow  = Input::GetMousePositionWindow();
    const Array<InputEvent> &events = Input::GetEnqueuedEvents();
    for (const InputEvent &inputEvent : events)
    {
        currentMouseWindow = inputEvent.GetMousePosWindow();
        currentMousePosVP = Vector2i(GL::FromWindowPointToViewportPoint(
                                        currentMouseWindow));
        currentMousePosVPNDC = GL::FromViewportPointToViewportPointNDC(
                                    currentMousePosVP);

        // First of all, know which focusable is under mouse top most
        UIFocusable *focusableUnderMouseTopMost = nullptr;
        for (const auto &pair : focusablesAndRectsVP)
        {
            UIFocusable *focusable = pair.first;
            const AARecti &aaRectMaskVP = pair.second;
            if (GameObject *focusableGo = focusable->GetGameObject())
            {
                if (RectTransform *rt = focusableGo->GetRectTransform())
                {
                    if (aaRectMaskVP.Contains(currentMousePosVP) &&
                        rt->IsMouseOver(currentMouseWindow, false) &&
                        focusable->IsFocusEnabled())
                    {
                        focusableUnderMouseTopMost = focusable;
                        break;
                    }
                }
            }
        }
        SetFocusableUnderMouseTopMost(focusableUnderMouseTopMost, inputEvent);

        switch (inputEvent.type)
        {
            case InputEvent::Type::WHEEL:
            {
                if (GetFocusableUnderMouseTopMost())
                {
                    PropagateFocusableUIEvent(GetFocusableUnderMouseTopMost(),
                                              UIEvent::Type::WHEEL,
                                              inputEvent);

                    if (GetFocusableUnderMouseTopMost())
                    {
                        RegisterForEvents( GetFocusableUnderMouseTopMost() );
                    }
                }
            }
            break;

            case InputEvent::Type::MOUSE_MOVE:
            {
                if (GetFocusableUnderMouseTopMost())
                {
                    PropagateFocusableUIEvent(GetFocusableUnderMouseTopMost(),
                                              UIEvent::Type::MOUSE_MOVE,
                                              inputEvent);

                    if (GetFocusableUnderMouseTopMost())
                    {
                        RegisterForEvents( GetFocusableUnderMouseTopMost() );
                    }
                }
            }
            break;

            case InputEvent::Type::MOUSE_DOWN:
            {
                if (GetFocusableUnderMouseTopMost())
                {
                    RegisterForEvents( GetFocusableUnderMouseTopMost() );
                    PropagateFocusableUIEvent(GetFocusableUnderMouseTopMost(),
                                              UIEvent::Type::MOUSE_CLICK_DOWN,
                                              inputEvent);
                }

                if (GetFocusableUnderMouseTopMost())
                {
                    RegisterForEvents( GetFocusableUnderMouseTopMost() );
                    PropagateFocusableUIEvent(GetFocusableUnderMouseTopMost(),
                                              UIEvent::Type::STARTED_BEING_PRESSED,
                                              inputEvent);
                }

                // if (inputEvent.mouseButton == MouseButton::LEFT)
                {
                    SetFocus( GetFocusableUnderMouseTopMost() );
                    if (GetFocusableUnderMouseTopMost())
                    {
                        RegisterForEvents( GetFocusableUnderMouseTopMost() );
                    }
                }
            }
            break;

            case InputEvent::Type::MOUSE_UP:
            {
                if (GetFocusableUnderMouseTopMost())
                {
                    RegisterForEvents( GetFocusableUnderMouseTopMost() );
                    PropagateFocusableUIEvent(GetFocusableUnderMouseTopMost(),
                                              UIEvent::Type::MOUSE_CLICK_UP,
                                              inputEvent);
                }

                if (inputEvent.mouseButton == MouseButton::LEFT)
                {
                    if (GetFocus() && GetFocus()->IsBeingPressed())
                    {
                        if (GameObject *focusGo = GetFocus()->GetGameObject())
                        {
                            if (RectTransform *rt = focusGo->GetRectTransform())
                            {
                                if (rt->IsMouseOver(currentMousePosVP, false))
                                {
                                    RegisterForEvents( GetFocusableUnderMouseTopMost() );
                                    PropagateFocusableUIEvent(GetFocusableUnderMouseTopMost(),
                                                              UIEvent::Type::MOUSE_CLICK_FULL,
                                                              inputEvent);
                                }
                            }
                        }
                    }

                    if (GetFocus() && GetFocus()->IsBeingPressed())
                    {
                        RegisterForEvents( GetFocus() );
                        PropagateFocusableUIEvent(GetFocus(),
                                                  UIEvent::Type::FINISHED_BEING_PRESSED,
                                                  inputEvent);
                    }
                }
            }
            break;

            case InputEvent::Type::KEY_DOWN:
            {
                if (GetFocus())
                {
                    RegisterForEvents( GetFocus() );
                    PropagateFocusableUIEvent(GetFocus(),
                                              UIEvent::Type::KEY_DOWN,
                                              inputEvent);
                }

                switch (inputEvent.key)
                {
                    // Tabbing
                    case Key::TAB:
                    {
                        if (UIFocusable *focus = GetFocus())
                        {
                            const uint numFocusables = focusables.Size();
                            int indexOfFocus = focusables.IndexOf(focus);
                            bool shift =
                               (inputEvent.keyModifiers.IsOn(KeyModifier::LSHIFT) ||
                                inputEvent.keyModifiers.IsOn(KeyModifier::RSHIFT));

                            int newFocusIndex = indexOfFocus;
                            while (true)
                            {
                                const uint nf = numFocusables;
                                newFocusIndex = (newFocusIndex + (shift ? 1 : -1) + nf) % nf;
                                if (newFocusIndex == indexOfFocus)
                                {
                                    break; // Complete loop
                                }

                                UIFocusable *newFocus = focusables.At(newFocusIndex);
                                const bool isValid =
                                    newFocus->IsFocusEnabled() &&
                                    newFocus->GetConsiderForTabbing() &&
                                    newFocus->GetGameObject()->IsVisible() &&
                                    newFocus->IsEnabled(true);
                                if (isValid)
                                {
                                    break;
                                }
                            }
                            SetFocus( focusables.At(newFocusIndex) );
                        }
                    }
                    break;

                    default:
                    break;
                }
            }
            break;

            case InputEvent::Type::KEY_UP:
            {
                if (GetFocus())
                {
                    RegisterForEvents( GetFocus() );
                    PropagateFocusableUIEvent(GetFocus(),
                                              UIEvent::Type::KEY_UP,
                                              inputEvent);
                }
            }
            break;

            default:
            break;
        }

        if (GetFocusableUnderMouseTopMost() &&
            Input::GetMouseButtonDoubleClick(MouseButton::LEFT))
        {
            RegisterForEvents( GetFocusableUnderMouseTopMost() );
            PropagateFocusableUIEvent(GetFocusableUnderMouseTopMost(),
                                      UIEvent::Type::MOUSE_CLICK_DOUBLE,
                                      inputEvent);
        }
    }

    // Set cursor type
    if (GetGameObject()->GetScene() &&
        GetGameObject()->GetRectTransform()->IsMouseOver())
    {
        if (!p_ddBeingDragged)
        {
            if ( GetFocus() && GetFocus()->IsBeingPressed() )
            {
                Cursor::Set(GetFocus()->GetCursorType());
            }
            else
            {
                // Ensure the canvas is actually on the scene (avoid cases in which we
                // invoke a fake update() without scene) and that the mouse is over us
                if (GetFocusableUnderMouseTopMost())
                {
                    Cursor::Set(GetFocusableUnderMouseTopMost()->GetCursorType());
                }
                else
                {
                    Cursor::Set(Cursor::Type::ARROW);
                }
            }
        }
        else
        {
            Cursor::Set(Cursor::Type::CROSSHAIR);
        }
    }

    // Drag drop
    if (p_ddBeingDragged)
    {
        List<EventListener<IEventsDragDrop>*> ddListeners = GetDragDropListeners();
        if (Input::GetMouseButton(MouseButton::LEFT))
        {
            p_ddBeingDragged->OnDragUpdate();
            for (EventListener<IEventsDragDrop>* ddListener : ddListeners)
            {
                if (ddListener->IsReceivingEvents())
                {
                    ddListener->OnDragUpdate(p_ddBeingDragged);
                }
            }
        }
        else
        {
            for (EventListener<IEventsDragDrop>* ddListener : ddListeners)
            {
                if (ddListener->IsReceivingEvents())
                {
                    bool inside = false;
                    if (Component *comp = DCAST<Component*>(ddListener))
                    {
                        inside = UICanvas::IsMouseOver(comp->GetGameObject(), true);
                    }
                    else if (GameObject *go = DCAST<GameObject*>(ddListener))
                    {
                        inside = UICanvas::IsMouseOver(go, true);
                    }
                    ddListener->OnDrop(p_ddBeingDragged, inside);
                }
            }
            p_ddBeingDragged->OnDropped();
            p_ddBeingDragged = nullptr;
        }
    }
}

void UICanvas::InvalidateCanvas()
{
    Array<RectTransform*> rts = GetGameObject()->
                                GetComponentsInDescendantsAndThis<RectTransform>();
    for (RectTransform *rt : rts)
    {
        rt->InvalidateTransform();
    }
}

void UICanvas::SetFocus(UIFocusable *newFocusable_)
{
    UIFocusable *newFocusable = newFocusable_;
    if (newFocusable && newFocusable->IsWaitingToBeDestroyed())
    {
        newFocusable = nullptr;
    }


    if (newFocusable != GetFocus())
    {
        if (GetFocus())
        {
            GetFocus()->ClearFocus();

            UIEvent focusLostEvent;
            focusLostEvent.type = UIEvent::Type::FOCUS_LOST;
            GetFocus()->ProcessEvent(focusLostEvent);

            UnRegisterForEvents(GetFocus());
        }

        p_focus = newFocusable;
        if (GetFocus())
        {
            GetFocus()->SetFocus();

            UIEvent focusTakenEvent;
            focusTakenEvent.type = UIEvent::Type::FOCUS_TAKEN;
            GetFocus()->ProcessEvent(focusTakenEvent);

            RegisterForEvents(GetFocus());
        }
    }
}

List<EventListener<IEventsDragDrop>*> UICanvas::GetDragDropListeners() const
{
    List<EventListener<IEventsDragDrop>*> dragDropListeners;

    std::queue<GameObject*> gos;
    gos.push(GetGameObject());

    while (!gos.empty())
    {
        GameObject *go = gos.front();
        gos.pop();

        if (EventListener<IEventsDragDrop>* ddListGo =
                            DCAST<EventListener<IEventsDragDrop>*>(go))
        {
            dragDropListeners.PushBack(ddListGo);
        }
        dragDropListeners.PushBack(
                    go->GetComponents< EventListener<IEventsDragDrop> >() );

        for (GameObject *child : go->GetChildren())
        {
            gos.push(child);
        }
    }
    return dragDropListeners;
}

void UICanvas::OnAfterChildrenUpdate()
{
    Component::OnAfterChildrenUpdate();
    if (GetFocus())
    {
        GetFocus()->m_hasJustFocusChanged = false;
    }
}


void UICanvas::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);

    if (renderPass == RenderPass::CANVAS)
    {
        GetLayoutManager()->RebuildLayout( GetGameObject() );
    }
}

void UICanvas::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
}

void UICanvas::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);
}

void UICanvas::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);
}

void UICanvas::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (UIFocusable *destroyedFocusable = DCAST<UIFocusable*>(object))
    {
        if (destroyedFocusable == GetFocus())
        {
            SetFocus(nullptr);
        }

        if (destroyedFocusable == GetFocusableUnderMouseTopMost())
        {
            p_focusableUnderMouseTopMost = nullptr;
        }
    }

    if (UIDragDroppable *dd = DCAST<UIDragDroppable*>(object))
    {
        if (dd == p_ddBeingDragged)
        {
            p_ddBeingDragged = nullptr;
        }
    }
}

bool UICanvas::HasFocusFocusable(const UIFocusable *focusable)
{
    return GetFocus() == focusable;
}

bool UICanvas::HasFocus(const Component *comp, bool recursive)
{
    return HasFocus(comp->GetGameObject(), recursive);
}
bool UICanvas::HasFocus(const GameObject *go, bool recursive)
{
    if (!go)
    {
        return false;
    }

    if (!recursive)
    {
        Array<UIFocusable*> focusables = go->GetComponents<UIFocusable>();
        for (UIFocusable *focusable : focusables)
        {
            if (HasFocusFocusable(focusable))
            {
                return true;
            }
        }
    }
    else
    {
        if (HasFocus(go, false))
        {
            return true;
        }
        else
        {
            for (GameObject *child : go->GetChildren())
            {
                if (HasFocus(child, true))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool UICanvas::IsMouseOverFocusable(const UIFocusable *focusable)
{
    return GetFocusableUnderMouseTopMost() == focusable;
}

bool UICanvas::IsMouseOver(const Component *comp, bool recursive)
{
    return IsMouseOver(comp->GetGameObject(), recursive);
}
bool UICanvas::IsMouseOver(const GameObject *go, bool recursive)
{
    if (!recursive)
    {
        Array<const UIFocusable*> focusables = go->GetComponents<const UIFocusable>();
        for (const UIFocusable *focusable : focusables)
        {
            if (GetFocusableUnderMouseTopMost() == focusable)
            {
                return true;
            }
        }
    }
    else
    {
        if (IsMouseOver(go, false))
        {
            return true;
        }
        else
        {
            for (GameObject *child : go->GetChildren())
            {
                if (IsMouseOver(child, true))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void UICanvas::NotifyDragStarted(UIDragDroppable *dragDroppable)
{
    p_ddBeingDragged = dragDroppable;

    List<EventListener<IEventsDragDrop>*> ddListeners = GetDragDropListeners();
    for (EventListener<IEventsDragDrop>* ddListener : ddListeners)
    {
        if (ddListener->IsReceivingEvents())
        {
            ddListener->OnDragStarted(p_ddBeingDragged);
        }
    }
}

UIFocusable* UICanvas::GetFocus()
{
    return p_focus;
}

UIFocusable* UICanvas::GetFocusableUnderMouseTopMost() const
{
    return p_focusableUnderMouseTopMost;
}


UILayoutManager *UICanvas::GetLayoutManager() const
{
    return m_uiLayoutManager;
}

UICanvas *UICanvas::GetActive(const GameObject *go)
{
    UICanvas *canvasInThis = go->GetComponent<UICanvas>();
    return canvasInThis ? canvasInThis : go->GetComponentInAncestors<UICanvas>();
}

UICanvas *UICanvas::GetActive(const Component *comp)
{
    return UICanvas::GetActive(comp->GetGameObject());
}

void UICanvas::OnDisabled(Object *object)
{
    Component::OnDisabled(object);

    if (object == p_ddBeingDragged)
    {
        p_ddBeingDragged = nullptr;
    }

    List<UIFocusable*> disabledFocusables;
    if (UIFocusable *focusableObj = DCAST<UIFocusable*>(object))
    {
        disabledFocusables.PushBack(focusableObj);
    }

    if (GameObject *disabledGo = DCAST<GameObject*>(object))
    {
        Array<UIFocusable*> disabledGoFocusables =
                    disabledGo->GetComponentsInDescendantsAndThis<UIFocusable>();
        for (UIFocusable *disabledGoFocusable : disabledGoFocusables)
        {
            disabledFocusables.PushBack(disabledGoFocusable);
        }
    }

    for (UIFocusable *disabledFocusable : disabledFocusables)
    {
        disabledFocusable->ClearFocus();
        disabledFocusable->SetIsMouseOver(false);
        disabledFocusable->SetBeingPressed(false);

        if (disabledFocusable == GetFocus())
        {
            SetFocus(nullptr);
        }

        if (disabledFocusable == GetFocusableUnderMouseTopMost())
        {
            p_focusableUnderMouseTopMost->SetIsMouseOver(false);
            p_focusableUnderMouseTopMost = nullptr;
        }

        UnRegisterForEvents(disabledFocusable);
    }
}

void UICanvas::SetFocusableUnderMouseTopMost(UIFocusable *focusable,
                                             const InputEvent &inputEvent)
{
    if (focusable != GetFocusableUnderMouseTopMost())
    {
        if (GetFocusableUnderMouseTopMost())
        {
            PropagateFocusableUIEvent(GetFocusableUnderMouseTopMost(),
                                      UIEvent::Type::MOUSE_EXIT,
                                      inputEvent);
            UnRegisterForEvents(GetFocusableUnderMouseTopMost());
        }

        p_focusableUnderMouseTopMost = focusable;
        if (GetFocusableUnderMouseTopMost())
        {
            PropagateFocusableUIEvent(GetFocusableUnderMouseTopMost(),
                                      UIEvent::Type::MOUSE_ENTER,
                                      inputEvent);

            // We can lose the focusable when propagating event, so recheck
            if (GetFocusableUnderMouseTopMost())
            {
                RegisterForEvents(GetFocusableUnderMouseTopMost());
            }
        }
    }
}

void UICanvas::RegisterForEvents(UIFocusable *focusable)
{
    ASSERT(focusable);
    focusable->EventEmitter<IEventsDestroy>::RegisterListener(this);
    focusable->EventEmitter<IEventsObject>::RegisterListener(this);
}

void UICanvas::UnRegisterForEvents(UIFocusable *focusable)
{
    ASSERT(focusable);

    int numReferencesTrackingThisFocusable = 0;
    if (focusable == GetFocus())
    {
        ++numReferencesTrackingThisFocusable;
    }

    if (focusable == GetFocusableUnderMouseTopMost())
    {
        ++numReferencesTrackingThisFocusable;
    }

    if (UIDragDroppable *dd = DCAST<UIDragDroppable*>(focusable))
    {
        if (dd == p_ddBeingDragged)
        {
            ++numReferencesTrackingThisFocusable;
        }
    }

    if (numReferencesTrackingThisFocusable <= 1)
    {
        focusable->EventEmitter<IEventsDestroy>::UnRegisterListener(this);
    }

    focusable->EventEmitter<IEventsObject>::UnRegisterListener(this);
}

struct GameObjectZComparer
{
    inline bool operator() (const std::pair<UIFocusable*, AARecti>& lhs,
                            const std::pair<UIFocusable*, AARecti>& rhs)
    {
        if (Transform *lt = lhs.first->GetGameObject()->GetTransform())
        {
            if (Transform *rt = rhs.first->GetGameObject()->GetTransform())
            {
                return lt->GetPosition().z < rt->GetPosition().z;
            }
            return true;
        }
        return false;
    }
};


void UICanvas::GetSortedFocusCandidatesByOcclusionOrder(
        const GameObject *go,
        Array< std::pair<UIFocusable*, AARecti> > *sortedCandidates) const
{
    std::stack<AARecti> auxMaskRectStack;
    auxMaskRectStack.push(GL::GetViewportRect());

    GetSortedFocusCandidatesByPaintOrder(go, sortedCandidates, &auxMaskRectStack);

    Containers::StableSort(sortedCandidates->Begin(),
                           sortedCandidates->End(),
                           GameObjectZComparer());
}

void UICanvas::GetSortedFocusCandidatesByPaintOrder(
        const GameObject *go,
        Array< std::pair<UIFocusable*, AARecti> > *sortedCandidates,
        std::stack<AARecti> *maskRectStack) const
{
    const Array<GameObject*> &children = go->GetChildren();
    for (auto it = children.RBegin(); it != children.REnd(); ++it)
    {
        GameObject *child = *it;
        if (child->IsActive())
        {
            UIRectMask *rectMask = child->GetComponent<UIRectMask>();
            AARecti maskedRectVP = maskRectStack->top();
            if (rectMask && rectMask->IsActive() && rectMask->IsMasking())
            {
                AARecti childRectVP(child->GetRectTransform()->GetViewportAARect());
                maskedRectVP = AARecti::Intersection(maskedRectVP, childRectVP);
            }
            maskRectStack->push(maskedRectVP);

            GetSortedFocusCandidatesByPaintOrder(child, sortedCandidates,
                                                 maskRectStack);

            if (UIFocusable *focusable = child->GetComponent<UIFocusable>())
            {
                sortedCandidates->PushBack( std::make_pair(focusable,
                                                           maskedRectVP) );
            }
            maskRectStack->pop();
        }
    }

    /*
    Containers::StableSort(sortedCandidates->Begin(),
                           sortedCandidates->End(),
    [](const std::pair<UIFocusable*, AARect> &pl,
       const std::pair<UIFocusable*, AARect> &pr)
    {
        Component *compL = DCAST<Component*>(pl.first);
        GameObject *goL = compL ? compL->GetGameObject() :
                                  DCAST<GameObject*>(pl.first);
        ASSERT(goL);

        Component *compR = DCAST<Component*>(pr.first);
        GameObject *goR = compR ? compR->GetGameObject() :
                                  DCAST<GameObject*>(pr.first);
        ASSERT(goR);

        Transform *trL = goL->GetTransform();
        Transform *trR = goR->GetTransform();
        if (!trL) { return true;  }
        if (!trR) { return false; }

        return trL->GetPosition().z > trR->GetPosition().z;
    });
    */
}
