#include "Bang/UICanvas.h"

#include "Bang/Rect.h"
#include "Bang/Input.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Cursor.h"
#include "Bang/XMLNode.h"
#include "Bang/Component.h"
#include "Bang/GameObject.h"
#include "Bang/IFocusable.h"
#include "Bang/UIRectMask.h"
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

void UICanvas::OnUpdate()
{
    Component::OnUpdate();

    UpdateEvents( GetGameObject() );

    /*
    // Focus
    IFocusable *focusMouseOver = nullptr;

    Array< std::pair<IFocusable*, AARect> > focusablesAndRectsNDC;
    GetSortedFocusCandidatesByOcclusionOrder(GetGameObject(),
                                             &focusablesAndRectsNDC);
    for (const auto& focusableAndRectNDC : focusablesAndRectsNDC)
    {
        IFocusable *focusable = focusableAndRectNDC.first;
        const AARect& maskedRectNDC = focusableAndRectNDC.second;

        Component *focusableComp = Cast<Component*>(focusable);
        if (focusableComp->IsActive() && focusable->IsFocusEnabled())
        {
            RectTransform *rt = focusableComp->GetGameObject()->GetRectTransform();
            if (rt && rt->IsMouseOver() &&
                maskedRectNDC.Contains( Input::GetMousePositionNDC() ))
            {
                focusMouseOver = focusable;
                if (Input::GetMouseButtonDown(MouseButton::LEFT))
                {
                    SetFocus(focusable);
                }
                break; // Finished searching!
            }
        }
    }
    SetFocusMouseOver(focusMouseOver); // Set Focus mouse over and cursor

    // Reset focus when clicking out of everything
    if (Input::GetMouseButtonDown(MouseButton::LEFT) && !focusMouseOver)
    {
        SetFocus(nullptr);
    }

    // Create list of focusables, and track those destroyed. For this, we create
    // a helper class which implements IEventsDestroy
    struct DestroyFocusablesHandler : public EventListener<IEventsDestroy>
    {
        USet<IFocusable*> set;
        void OnDestroyed(EventEmitter<IEventsDestroy> *object) override
        { set.Add( DCAST<IFocusable*>(object) ); }
    };

    Array<IFocusable*> focusables;
    DestroyFocusablesHandler destroyedFocusables;
    for (const auto& focusableAndRectNDC : focusablesAndRectsNDC)
    {
        IFocusable *focusable = focusableAndRectNDC.first;
        Object *objFocusable = DCAST<Object*>(focusable);
        if (objFocusable)
        {
            objFocusable->EventEmitter<IEventsDestroy>::
                         RegisterListener(&destroyedFocusables);
        }
        focusables.PushBack(focusable);
    }

    // Tabbing
    if (Input::GetKeyDownRepeat(Key::TAB))
    {
        IFocusable *currentFocus = GetCurrentFocus();
        if (currentFocus)
        {
            const int n = focusables.Size();
            int indexOfFocus = focusables.IndexOf(currentFocus);
            bool shift = Input::GetKey(Key::LSHIFT) || Input::GetKey(Key::RSHIFT);
            int newFocusIndex = indexOfFocus;

            while (true)
            {
                newFocusIndex = (newFocusIndex + (shift ? 1 : -1) + n) % n;
                if (newFocusIndex == indexOfFocus) { break; } // Complete loop

                IFocusable *newFocus = focusables.At(newFocusIndex);
                Component *newFocusComp = Cast<Component*>(newFocus);
                const bool isValid = newFocus->IsFocusEnabled() &&
                                     (!newFocusComp ||
                                       newFocusComp->IsEnabled(true));
                if (isValid) { break; }
            }
            SetFocus( focusables.At(newFocusIndex) );
        }
    }

    // Update focusables
    for (IFocusable *focusable : focusables)
    {
        if (!destroyedFocusables.set.Contains(focusable))
        {
            bool update = true;
            if (Object *obj = DCAST<Object*>(focusable))
            {
                update = obj->IsActive();
            }

            if (update)
            {
                focusable->UpdateFromCanvas();
            }
        }
    }
    */

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
                    ddListener->OnDrop(p_ddBeingDragged);
                }
            }
            p_ddBeingDragged->OnDropped();
            p_ddBeingDragged = nullptr;
        }
    }
}

GameObject *GetGameObjectFromFocusable(IFocusable *focusable)
{
    if (!focusable)
    {
        return nullptr;
    }

    if (Component *comp = DCAST<Component*>(focusable))
    {
        return comp->GetGameObject();
    }

    return DCAST<GameObject*>(focusable);
}

void PropagateUIEvent(GameObject *focusableGo, const UIEvent &event)
{
    ASSERT(focusableGo);

    UIEventResult finalPropResult =
            UIEventResult::IGNORE;
    List<IFocusable*> focusablesInGo = focusableGo->GetComponents<IFocusable>();
    for (IFocusable *focusableInGo : focusablesInGo)
    {
        if (focusableInGo->IsFocusEnabled())
        {
            UIEventResult propagationResult =
                                     focusableInGo->ProcessEvent(event);
            if (propagationResult ==
                UIEventResult::INTERCEPT)
            {
                finalPropResult = UIEventResult::INTERCEPT;
            }
        }
    }

    if (finalPropResult ==
        UIEventResult::IGNORE)
    {
        if (GameObject *nextFocusableParent = focusableGo->GetParent())
        {
            PropagateUIEvent(nextFocusableParent, event);
        }
    }
}

void PropagateUIEvent(IFocusable *focusable, const UIEvent &event)
{
    ASSERT(focusable);
    if (GameObject *focusableGo = GetGameObjectFromFocusable(focusable))
    {
        PropagateUIEvent(focusableGo, event);
    }
}

void UICanvas::UpdateEvents(GameObject *go)
{
    const Vector2i currentMousePos = Input::GetMousePosition();
    const Vector2 currentMousePosNDC = Input::GetMousePositionNDC();

    IFocusable *focusableUnderMouseTopMost = nullptr;

    Array<std::pair<IFocusable*, AARect>> focusCandidates;
    GetSortedFocusCandidatesByOcclusionOrder(go, &focusCandidates);
    for (const auto &pair : focusCandidates)
    {
        IFocusable *focusable = pair.first;
        const AARect &aaRectMaskNDC = pair.second;
        if (GameObject *focusableGo = GetGameObjectFromFocusable(focusable))
        {
            if (RectTransform *rt = focusableGo->GetRectTransform())
            {
                if (aaRectMaskNDC.Contains(currentMousePosNDC) &&
                    rt->IsMouseOver(false) &&
                    focusable->IsFocusEnabled())
                {
                    focusableUnderMouseTopMost = focusable;
                    break;
                }
            }
        }
    }

    Map<IFocusable*, AARect> focusableToAARectMasksNDCs;
    for (const auto &pair : focusCandidates)
    {
        focusableToAARectMasksNDCs.Add(pair.first, pair.second);
    }

    auto PropagateClickEvent = [&](IFocusable *focusable,
                                   UIEvent::Type type,
                                   MouseButton mb = MouseButton::LEFT)
    {
        UIEvent event;
        event.type = type;
        event.mouse.button = mb;
        event.mousePosition = currentMousePos;
        event.mouse.delta = (currentMousePos - GetLastMousePosition());
        PropagateUIEvent(focusable, event);
    };

    if (focusableUnderMouseTopMost != GetFocusableUnderMouseTopMost())
    {
        Set<IFocusable*> focusablesNotUnderAnymore;
        for (IFocusable *focusableUnderMouse : p_focusablesUnderMouse)
        {
            if (focusableToAARectMasksNDCs.ContainsKey(focusableUnderMouse))
            {
                const AARect &aaRectMaskNDC =
                             focusableToAARectMasksNDCs.Get(focusableUnderMouse);
                if (GameObject *focusableGo =
                            GetGameObjectFromFocusable(focusableUnderMouse))
                {
                    if (RectTransform *rt = focusableGo->GetRectTransform())
                    {
                        if (!aaRectMaskNDC.Contains(currentMousePosNDC) ||
                            !rt->IsMouseOver(false))
                        {
                            focusablesNotUnderAnymore.Add(focusableUnderMouse);
                        }
                    }
                }
            }
        }

        for (IFocusable *focusableNotUnderAnymore : focusablesNotUnderAnymore)
        {
            PropagateClickEvent(focusableNotUnderAnymore,
                                UIEvent::Type::MOUSE_EXIT);
            UnRegisterForDestroy(focusableNotUnderAnymore);
            p_focusablesUnderMouse.Remove(focusableNotUnderAnymore);
        }

        if (GetFocusableUnderMouseTopMost())
        {
            UnRegisterForDestroy(GetFocusableUnderMouseTopMost());
        }

        p_focusableUnderMouseTopMost = focusableUnderMouseTopMost;
        if (GetFocusableUnderMouseTopMost())
        {
            {
                PropagateClickEvent(GetFocusableUnderMouseTopMost(),
                                    UIEvent::Type::MOUSE_ENTER);
            }

            if (currentMousePos != GetLastMousePosition())
            {
                PropagateClickEvent(GetFocusableUnderMouseTopMost(),
                                    UIEvent::Type::MOUSE_MOVE);
            }

            p_focusablesUnderMouse.Add(GetFocusableUnderMouseTopMost());
            RegisterForDestroy(GetFocusableUnderMouseTopMost());
        }
    }
    m_lastMousePosition = currentMousePos;

    const Array<MouseButton> mouseButtonsDown = Input::GetMouseButtonsDown();
    const Array<MouseButton> mouseButtonsUp = Input::GetMouseButtonsUp();
    if (mouseButtonsDown.Size() >= 1 || mouseButtonsUp.Size() >= 1)
    {
        UIEvent clickEvent;
        clickEvent.mousePosition = currentMousePos;

        for (MouseButton mbDown : mouseButtonsDown)
        {
            if (GetFocusableUnderMouseTopMost())
            {
                {
                    PropagateClickEvent(GetFocusableUnderMouseTopMost(),
                                        UIEvent::Type::MOUSE_CLICK_DOWN,
                                        mbDown);
                }
            }
        }

        for (MouseButton mbUp : mouseButtonsUp)
        {
            if (GetFocusableUnderMouseTopMost())
            {
                PropagateClickEvent(GetFocusableUnderMouseTopMost(),
                                    UIEvent::Type::MOUSE_CLICK_UP,
                                    mbUp);
            }
        }

        if (Input::GetMouseButtonDown(MouseButton::LEFT))
        {
            if (GetFocusableUnderMouseTopMost())
            {
                PropagateClickEvent(GetFocusableUnderMouseTopMost(),
                                    UIEvent::Type::STARTED_BEING_PRESSED);
            }
        }

        if (Input::GetMouseButtonDown(MouseButton::LEFT))
        {
            if (GetFocusableUnderMouseTopMost() != p_focus)
            {
                if (GetFocus())
                {
                    PropagateClickEvent(GetFocus(), UIEvent::Type::FOCUS_LOST);
                    UnRegisterForDestroy(GetFocus());
                }

                p_focus = GetFocusableUnderMouseTopMost();
                if (GetFocus())
                {
                    PropagateClickEvent(GetFocus(), UIEvent::Type::FOCUS_TAKEN);
                    RegisterForDestroy(GetFocus());
                }
            }

            p_focusablesPotentiallyBeingPressed = p_focusablesUnderMouse;
            for (IFocusable *focusable : p_focusablesPotentiallyBeingPressed)
            {
                RegisterForDestroy(focusable);
            }
        }

        if (Input::GetMouseButtonUp(MouseButton::LEFT))
        {
            if (GetFocus() && GetFocus()->IsBeingPressed())
            {
                if (GameObject *focusGo = GetGameObjectFromFocusable(GetFocus()))
                {
                    if (RectTransform *rt = focusGo->GetRectTransform())
                    {
                        if (rt->IsMouseOver(false))
                        {
                            PropagateClickEvent(GetFocusableUnderMouseTopMost(),
                                                UIEvent::Type::MOUSE_CLICK_FULL);
                        }
                    }
                }
            }
        }

        if (Input::GetMouseButtonUp(MouseButton::LEFT))
        {
            for (IFocusable *focusablePotentiallyBeingPressed :
                    p_focusablesPotentiallyBeingPressed)
            {
                if (focusablePotentiallyBeingPressed->IsBeingPressed())
                {
                    PropagateClickEvent(focusablePotentiallyBeingPressed,
                                        UIEvent::Type::FINISHED_BEING_PRESSED);
                }
            }

            for (IFocusable *focusable : p_focusablesPotentiallyBeingPressed)
            {
                UnRegisterForDestroy(focusable);
            }
            p_focusablesPotentiallyBeingPressed.Clear();
        }

        if (GetFocusableUnderMouseTopMost() &&
            Input::GetMouseButtonDoubleClick(MouseButton::LEFT))
        {
            PropagateClickEvent(GetFocusableUnderMouseTopMost(),
                                UIEvent::Type::MOUSE_CLICK_DOUBLE);
        }

        const Array<Key> keysDowned = Input::GetKeysDown();
        const Array<Key> keysDownedRepeated = Input::GetKeysDownRepeat();
        const Array<Key> keysUpped = Input::GetKeysUp();
        if (keysDowned.Size() >= 1 ||
            keysDownedRepeated.Size() >= 1 ||
            keysUpped.Size() >= 1)
        {

        }
    }
}

void UICanvas::InvalidateCanvas()
{
    List<RectTransform*> rts = GetGameObject()->
                               GetComponentsInChildren<RectTransform>(true);
    for (RectTransform *rt : rts) { rt->InvalidateTransform(); }
}

void UICanvas::SetFocus(IFocusable *newFocusable_)
{
    IFocusable *newFocusable = newFocusable_;
    Object *obj = newFocusable ? DCAST<Object*>(newFocusable) : nullptr;
    if (obj)
    {
        if (obj->IsWaitingToBeDestroyed())
        {
            newFocusable = nullptr;
        }
    }


    if (newFocusable != GetFocus())
    {
        if (GetFocus())
        {
            UnRegisterForDestroy(GetFocus());
            GetFocus()->ClearFocus();
        }

        p_focus = newFocusable;
        if (GetFocus())
        {
            RegisterForDestroy(GetFocus());
            GetFocus()->SetFocus();
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

        for (GameObject *child : go->GetChildren()) { gos.push(child); }
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

void UICanvas::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);
}

void UICanvas::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);
}

void UICanvas::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (IFocusable *destroyedFocusable = DCAST<IFocusable*>(object))
    {
        if (destroyedFocusable == GetFocus())
        {
            SetFocus(nullptr);
        }

        if (destroyedFocusable == p_focusableUnderMouseTopMost)
        {
            p_focusableUnderMouseTopMost = nullptr;
        }

        p_focusablesUnderMouse.Remove(destroyedFocusable);
        p_focusablesPotentiallyBeingPressed.Remove(destroyedFocusable);
    }

    if (UIDragDroppable *dd = DCAST<UIDragDroppable*>(object))
    {
        if (dd == p_ddBeingDragged)
        {
            p_ddBeingDragged = nullptr;
        }
    }
}

bool UICanvas::HasFocusFocusable(const IFocusable *focusable)
{
    return GetFocus() == focusable;
}
bool UICanvas::HasFocus(const Component *comp, bool recursive)
{
    return comp ? UICanvas::HasFocus(comp->GetGameObject(), recursive) : false;
}
bool UICanvas::HasFocus(const GameObject *go, bool recursive)
{
    if (!go) { return false; }

    if (!recursive)
    {
        List<IFocusable*> focusables = go->GetComponents<IFocusable>();
        for (IFocusable *focusable : focusables)
        {
            if (HasFocusFocusable(focusable)) { return true; }
        }
    }
    else
    {
        if (HasFocus(go, false)) { return true; }
        else
        {
            const List<GameObject*>& children = go->GetChildren();
            for (GameObject *child : children)
            {
                if (HasFocus(child, true)) { return true; }
            }
        }
    }
    return false;
}

bool UICanvas::IsMouseOverFocusable(const IFocusable *focusable)
{
    return GetFocusableUnderMouseTopMost() == focusable;
}
bool UICanvas::IsMouseOver(const Component *comp, bool recursive)
{
    return comp ? UICanvas::IsMouseOver(comp->GetGameObject(), recursive) : false;
}
bool UICanvas::IsMouseOver(const GameObject *go, bool recursive)
{
    if (!recursive)
    {
        List<const IFocusable*> focusables = go->GetComponents<const IFocusable>();
        if (const IFocusable *focusable = DCAST<const IFocusable*>(go))
        {
            focusables.PushBack(focusable);
        }

        for (const IFocusable *focusable : focusables)
        {
            if (p_focusableUnderMouseTopMost == focusable)
            {
                return true;
            }
        }
    }
    else
    {
        if (IsMouseOver(go, false)) { return true; }
        else
        {
            const List<GameObject*>& children = go->GetChildren();
            for (GameObject *child : children)
            {
                if (IsMouseOver(child, true)) { return true; }
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

IFocusable *UICanvas::GetFocus()
{
    return p_focus;
}

const Vector2i &UICanvas::GetLastMousePosition() const
{
    return m_lastMousePosition;
}
IFocusable* UICanvas::GetFocusableUnderMouseTopMost() const
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
    return canvasInThis ? canvasInThis : go->GetComponentInParent<UICanvas>();
}

UICanvas *UICanvas::GetActive(const Component *comp)
{
    return UICanvas::GetActive(comp->GetGameObject());
}

void UICanvas::RegisterForDestroy(IFocusable *focusable)
{
    ASSERT(focusable);
    if (EventEmitter<IEventsDestroy> *destroyable =
            DCAST<EventEmitter<IEventsDestroy>*>(focusable))
    {
        destroyable->RegisterListener(this);
    }
}

void UICanvas::UnRegisterForDestroy(IFocusable *focusable)
{
    ASSERT(focusable);

    int numReferencesTrackingIt = 0;
    if (focusable == GetFocus())
    {
        ++numReferencesTrackingIt;
    }
    if (focusable == GetFocusableUnderMouseTopMost())
    {
        ++numReferencesTrackingIt;
    }
    if (p_focusablesUnderMouse.Contains(focusable))
    {
        ++numReferencesTrackingIt;
    }
    if (p_focusablesPotentiallyBeingPressed.Contains(focusable))
    {
        ++numReferencesTrackingIt;
    }
    if (UIDragDroppable *dd = DCAST<UIDragDroppable*>(focusable))
    {
        if (dd == p_ddBeingDragged)
        {
            ++numReferencesTrackingIt;
        }
    }

    if (numReferencesTrackingIt <= 1)
    {
        if (EventEmitter<IEventsDestroy> *destroyable =
                DCAST<EventEmitter<IEventsDestroy>*>(focusable))
        {
            destroyable->UnRegisterListener(this);
        }
    }
}

struct GameObjectZComparer
{
    inline bool operator() (const std::pair<IFocusable*, AARect>& lhs,
                            const std::pair<IFocusable*, AARect>& rhs)
    {
        const GameObject *glhs = Cast<const GameObject*>(lhs.first);
        const GameObject *grhs = Cast<const GameObject*>(rhs.first);
        if (!glhs) { glhs = Cast<const Component*>(lhs.first)->GetGameObject(); }
        if (!grhs) { grhs = Cast<const Component*>(rhs.first)->GetGameObject(); }
        Transform *lt = glhs->GetTransform();
        Transform *rt = grhs->GetTransform();
        if (!lt) { return false; }
        if (!rt) { return true; }
        return lt->GetPosition().z < rt->GetPosition().z;
    }
};


void UICanvas::GetSortedFocusCandidatesByOcclusionOrder(
        const GameObject *go,
        Array< std::pair<IFocusable*, AARect> > *sortedCandidates) const
{
    std::stack<AARect> auxMaskRectStack;
    auxMaskRectStack.push(AARect::NDCRect);

    GetSortedFocusCandidatesByPaintOrder(go, sortedCandidates, &auxMaskRectStack);

    Containers::StableSort(sortedCandidates->Begin(), sortedCandidates->End(),
                           GameObjectZComparer());
}

void UICanvas::GetSortedFocusCandidatesByPaintOrder(
        const GameObject *go,
        Array< std::pair<IFocusable*, AARect> > *sortedCandidates,
        std::stack<AARect> *maskRectStack) const
{
    List<GameObject*> children = go->GetChildren();
    for (auto it = children.RBegin(); it != children.REnd(); ++it)
    {
        GameObject *child = *it;
        if (child->IsActive())
        {
            UIRectMask *rectMask = child->GetComponent<UIRectMask>();
            AARect maskedRectNDC = maskRectStack->top();
            if (rectMask && rectMask->IsActive() && rectMask->IsMasking())
            {
                AARect childRectNDC = child->GetRectTransform()->GetViewportAARectNDC();
                maskedRectNDC = AARect::Intersection(maskedRectNDC, childRectNDC);
            }
            maskRectStack->push(maskedRectNDC);

            GetSortedFocusCandidatesByPaintOrder(child, sortedCandidates,
                                                 maskRectStack);

            IFocusable *focusable = child->GetComponent<IFocusable>();
            if (focusable)
            {
                Object *focusableObj = Cast<Object*>(focusable);
                if (!focusableObj || focusableObj->IsActive())
                {
                    sortedCandidates->PushBack(
                            std::make_pair(focusable, maskedRectNDC));
                }
            }
            maskRectStack->pop();
        }
    }

    /*
    Containers::StableSort(sortedCandidates->Begin(),
                           sortedCandidates->End(),
    [](const std::pair<IFocusable*, AARect> &pl,
       const std::pair<IFocusable*, AARect> &pr)
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
