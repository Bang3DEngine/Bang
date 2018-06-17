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

    // Set cursor type
    if (GetGameObject()->GetScene() &&
        GetGameObject()->GetRectTransform()->IsMouseOver())
    {
        if (!p_currentDDBeingDragged)
        {
            if ( GetCurrentFocus() && GetCurrentFocus()->IsBeingPressed() )
            {
                Cursor::Set(GetCurrentFocus()->GetCursorType());
            }
            else
            {
                // Ensure the canvas is actually on the scene (avoid cases in which we
                // invoke a fake update() without scene) and that the mouse is over us
                if (GetCurrentFocusMouseOver())
                {
                    Cursor::Set(GetCurrentFocusMouseOver()->GetCursorType());
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
    if (p_currentDDBeingDragged)
    {
        List<EventListener<IEventsDragDrop>*> ddListeners = GetDragDropListeners();
        if (Input::GetMouseButton(MouseButton::LEFT))
        {
            p_currentDDBeingDragged->OnDragUpdate();
            for (EventListener<IEventsDragDrop>* ddListener : ddListeners)
            {
                if (ddListener->IsReceivingEvents())
                {
                    ddListener->OnDragUpdate(p_currentDDBeingDragged);
                }
            }
        }
        else
        {
            for (EventListener<IEventsDragDrop>* ddListener : ddListeners)
            {
                if (ddListener->IsReceivingEvents())
                {
                    ddListener->OnDrop(p_currentDDBeingDragged);
                }
            }
            p_currentDDBeingDragged->OnDropped();
            p_currentDDBeingDragged = nullptr;
        }
    }
    */
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

void PropagateUIEvent(GameObject *focusableGo, const IEventsFocus::Event &event)
{
    ASSERT(focusableGo);

    IEventsFocus::Event::PropagationResult finalPropResult =
            IEventsFocus::Event::PropagationResult::PROPAGATE_TO_PARENT;
    List<IFocusable*> focusablesInGo = focusableGo->GetComponents<IFocusable>();
    for (IFocusable *focusableInGo : focusablesInGo)
    {
        IEventsFocus::Event::PropagationResult propagationResult =
                                 focusableInGo->ProcessEvent(event);
        if (propagationResult ==
            IEventsFocus::Event::PropagationResult::STOP_PROPAGATION)
        {
            finalPropResult = IEventsFocus::Event::PropagationResult::STOP_PROPAGATION;
        }
    }

    if (finalPropResult ==
        IEventsFocus::Event::PropagationResult::PROPAGATE_TO_PARENT)
    {
        if (GameObject *nextFocusableParent = focusableGo->GetParent())
        {
            PropagateUIEvent(nextFocusableParent, event);
        }
    }
}

void PropagateUIEvent(IFocusable *focusable, const IEventsFocus::Event &event)
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
    const bool mouseMoved = (currentMousePos != m_lastMousePosition);
    if (mouseMoved)
    {
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
                        rt->IsMouseOver(false))
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

        if (focusableUnderMouseTopMost != p_focusableUnderMouseTopMost)
        {
            IEventsFocus::Event eventMoveBase;
            eventMoveBase.mousePosition = Input::GetMousePosition();

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
                IEventsFocus::Event eventExit = eventMoveBase;
                eventExit.type = IEventsFocus::Event::Type::MOUSE_EXIT;
                PropagateUIEvent(focusableNotUnderAnymore, eventExit);
                p_focusablesUnderMouse.Remove(focusableNotUnderAnymore);
            }

            if (focusableUnderMouseTopMost)
            {
                {
                    IEventsFocus::Event eventEnter = eventMoveBase;
                    eventEnter.type = IEventsFocus::Event::Type::MOUSE_ENTER;
                    PropagateUIEvent(focusableUnderMouseTopMost, eventEnter);
                }

                {
                    IEventsFocus::Event eventMove = eventMoveBase;
                    eventMove.click.mouseDelta =
                                       (currentMousePos - m_lastMousePosition);
                    eventMove.type = IEventsFocus::Event::Type::MOUSE_MOVE;
                    PropagateUIEvent(focusableUnderMouseTopMost, eventMove);
                }
                p_focusablesUnderMouse.Add(focusableUnderMouseTopMost);
            }

            p_focusableUnderMouseTopMost = focusableUnderMouseTopMost;
        }
        m_lastMousePosition = currentMousePos;
    }

    const Array<MouseButton> mouseButtonsDown = Input::GetMouseButtonsDown();
    const Array<MouseButton> mouseButtonsUp = Input::GetMouseButtonsUp();
    if (mouseButtonsDown.Size() >= 1 || mouseButtonsUp.Size() >= 1)
    {
        IEventsFocus::Event clickEvent;
        clickEvent.mousePosition = currentMousePos;
        clickEvent.type = IEventsFocus::Event::Type::MOUSE_CLICK;

        if (Input::GetMouseButtonDown(MouseButton::LEFT))
        {
            if (p_focusableUnderMouseTopMost)
            {
                IEventsFocus::Event clickEventStartedPressed = clickEvent;
                clickEventStartedPressed.type = IEventsFocus::Event::Type::
                                                STARTED_BEING_PRESSED;
                clickEventStartedPressed.click.type = ClickType::DOWN;
                clickEventStartedPressed.click.button = MouseButton::LEFT;
                PropagateUIEvent(p_focusableUnderMouseTopMost,
                                 clickEventStartedPressed);
            }
        }

        if (Input::GetMouseButtonUp(MouseButton::LEFT))
        {
            if (p_currentFocus && p_currentFocus->IsBeingPressed())
            {
                if (GameObject *focusGo = GetGameObjectFromFocusable(p_currentFocus))
                {
                    if (RectTransform *rt = focusGo->GetRectTransform())
                    {
                        if (rt->IsMouseOver(false))
                        {
                            IEventsFocus::Event clickEventFull = clickEvent;
                            clickEventFull.click.type = ClickType::FULL;
                            clickEventFull.click.button = MouseButton::LEFT;
                            PropagateUIEvent(p_currentFocus, clickEventFull);
                        }
                    }
                }
            }

            for (IFocusable *focusablePotentiallyBeingPressed :
                    p_focusablesPotentiallyBeingPressed)
            {
                if (focusablePotentiallyBeingPressed->IsBeingPressed())
                {
                    IEventsFocus::Event clickEventFinishedPressed;
                    clickEventFinishedPressed.mousePosition = currentMousePos;
                    clickEventFinishedPressed.type = IEventsFocus::Event::Type::
                                                     FINISHED_BEING_PRESSED;
                    clickEventFinishedPressed.click.type = ClickType::DOWN;
                    clickEventFinishedPressed.click.button = MouseButton::LEFT;
                    PropagateUIEvent(focusablePotentiallyBeingPressed,
                                     clickEventFinishedPressed);
                }
            }
            p_focusablesPotentiallyBeingPressed.Clear();
        }

        for (MouseButton mbDown : mouseButtonsDown)
        {
            if (mbDown == MouseButton::LEFT)
            {
                p_currentFocus = p_focusableUnderMouseTopMost;
                p_focusablesPotentiallyBeingPressed = p_focusablesUnderMouse;
            }

            if (p_focusableUnderMouseTopMost)
            {
                IEventsFocus::Event clickEventDown = clickEvent;
                clickEventDown.click.type = ClickType::DOWN;
                clickEventDown.click.button = mbDown;
                clickEventDown.click.button = MouseButton::LEFT;
                PropagateUIEvent(p_focusableUnderMouseTopMost,
                                 clickEventDown);
            }

        }

        for (MouseButton mbUp : mouseButtonsUp)
        {
            if (p_focusableUnderMouseTopMost)
            {
                IEventsFocus::Event clickEventUp = clickEvent;
                clickEventUp.click.type = ClickType::UP;
                clickEventUp.click.button = mbUp;
                PropagateUIEvent(p_focusableUnderMouseTopMost, clickEventUp);
            }
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

void UICanvas::SetFocus(IFocusable *_newFocusable)
{
    /*
    IFocusable *newFocusable = _newFocusable;
    Object *obj = newFocusable ? DCAST<Object*>(newFocusable) : nullptr;
    if (obj)
    {
        if (obj->IsWaitingToBeDestroyed()) { newFocusable = nullptr; }
    }

    if (newFocusable != GetCurrentFocus())
    {
        if (GetCurrentFocus())
        {
            Object *focusableObj = Cast<Object*>( GetCurrentFocus() );
            if (GetCurrentFocus() != GetCurrentFocusMouseOver())
            { focusableObj->EventEmitter<IEventsDestroy>::UnRegisterListener(this); }

            // GetCurrentFocus()->ClearFocus();
        }

        p_currentFocus = newFocusable;
        if (GetCurrentFocus())
        {
            Object *focusableObj = DCAST<Object*>( GetCurrentFocus() );
            focusableObj->EventEmitter<IEventsDestroy>::RegisterListener(this);

            // GetCurrentFocus()->SetFocus();
        }
    }
    */
}

void UICanvas::SetFocusMouseOver(IFocusable *_newFocusableMO)
{
    /*
    IFocusable *newFocusableMO = _newFocusableMO;
    Object *obj = newFocusableMO ? DCAST<Object*>(newFocusableMO) : nullptr;
    if (obj)
    {
        if (obj->IsWaitingToBeDestroyed()) { newFocusableMO = nullptr; }
    }

    if (newFocusableMO != GetCurrentFocusMouseOver())
    {
        if (GetCurrentFocusMouseOver())
        {
            Object *focusableMOObj = Cast<Object*>( GetCurrentFocusMouseOver() );
            if (GetCurrentFocus() != GetCurrentFocusMouseOver())
            { focusableMOObj->EventEmitter<IEventsDestroy>::UnRegisterListener(this); }

            // GetCurrentFocusMouseOver()->PropagateMouseOverToListeners(false);
        }

        p_currentFocusMouseOver = newFocusableMO;
        if (GetCurrentFocusMouseOver())
        {
            Object *focusableMOObj = Cast<Object*>( GetCurrentFocusMouseOver() );
            focusableMOObj->EventEmitter<IEventsDestroy>::RegisterListener(this);
            // GetCurrentFocusMouseOver()->PropagateMouseOverToListeners(true);
        }
    }
    */
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

    // if (GetCurrentFocus()) { GetCurrentFocus()->m_hasJustFocusChanged = false; }
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
    IFocusable *destroyedFocusable = DCAST<IFocusable*>(object);

    if (destroyedFocusable == GetCurrentFocus())
    { SetFocus(nullptr); }

    if (destroyedFocusable == GetCurrentFocusMouseOver())
    { SetFocusMouseOver(nullptr); }
}

bool UICanvas::HasFocusFocusable(const IFocusable *focusable)
{
    return GetCurrentFocus() == focusable;
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
    return GetCurrentFocusMouseOver() == focusable;
}
bool UICanvas::IsMouseOver(const Component *comp, bool recursive)
{
    return comp ? UICanvas::IsMouseOver(comp->GetGameObject(), recursive) : false;
}
bool UICanvas::IsMouseOver(const GameObject *go, bool recursive)
{
    if (!go) { return false; }
    if (!recursive)
    {
        List<IFocusable*> focusables = go->GetComponents<IFocusable>();
        for (IFocusable *focusable : focusables)
        {
            if (IsMouseOverFocusable(focusable)) { return true; }
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
    p_currentDDBeingDragged = dragDroppable;

    List<EventListener<IEventsDragDrop>*> ddListeners = GetDragDropListeners();
    for (EventListener<IEventsDragDrop>* ddListener : ddListeners)
    {
        if (ddListener->IsReceivingEvents())
        {
            ddListener->OnDragStarted(p_currentDDBeingDragged);
        }
    }
}

IFocusable *UICanvas::GetCurrentFocus()
{
    return p_currentFocus;
}
IFocusable* UICanvas::GetCurrentFocusMouseOver()
{
    return p_currentFocusMouseOver;
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
