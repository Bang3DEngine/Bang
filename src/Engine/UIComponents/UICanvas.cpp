#include "Bang/UICanvas.h"

#include "Bang/Rect.h"
#include "Bang/Input.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Cursor.h"
#include "Bang/MetaNode.h"
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

    UIEventResult finalPropResult = UIEventResult::IGNORE;

    Array<IFocusable*> focusablesInGo = focusableGo->GetComponents<IFocusable>();

    class DestroyTracker : public GameObject,
                           public EventListener<IEventsDestroy>
    {
    public:
        Array<IFocusable*> *focusables = nullptr;
        void OnDestroyed(EventEmitter<IEventsDestroy> *ee) override
        {
            ASSERT(focusables);
            focusables->Remove( DCAST<IFocusable*>(ee) );
        }
    };

    DestroyTracker *destroyTracker = GameObject::Create<DestroyTracker>();
    destroyTracker->focusables = &focusablesInGo;

    for (IFocusable *focusableInGo : focusablesInGo)
    {
        if (auto *ee = DCAST<EventEmitter<IEventsDestroy>*>(focusableInGo))
        {
            ee->EventEmitter<IEventsDestroy>::RegisterListener(destroyTracker);
        }
    }

    for (IFocusable *focusableInGo : focusablesInGo)
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

void PropagateUIEvent(IFocusable *focusable, const UIEvent &event)
{
    ASSERT(focusable);
    if (GameObject *focusableGo = GetGameObjectFromFocusable(focusable))
    {
        PropagateUIEvent(focusableGo, event);
    }
}

void UICanvas::OnUpdate()
{
    Component::OnUpdate();

    const Vector2i currentMousePos = Input::GetMousePosition();
    const Vector2 currentMousePosNDC = Input::GetMousePositionNDC();

    IFocusable *focusableUnderMouseTopMost = nullptr;

    Array<std::pair<IFocusable*, AARect>> focusablesAndRectsNDC;
    GetSortedFocusCandidatesByOcclusionOrder(GetGameObject(),
                                             &focusablesAndRectsNDC);
    for (const auto &pair : focusablesAndRectsNDC)
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
    Array<IFocusable*> focusables;
    for (const auto& focusableAndRectNDC : focusablesAndRectsNDC)
    {
        focusables.PushBack(focusableAndRectNDC.first);
    }

    Map<IFocusable*, AARect> focusableToAARectMasksNDCs;
    for (const auto &pair : focusablesAndRectsNDC)
    {
        focusableToAARectMasksNDCs.Add(pair.first, pair.second);
    }

    auto PropagateIFocusableEvent = [&](IFocusable *focusable,
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
            PropagateIFocusableEvent(focusableNotUnderAnymore,
                                     UIEvent::Type::MOUSE_EXIT);
            UnRegisterForEvents(focusableNotUnderAnymore);
            p_focusablesUnderMouse.Remove(focusableNotUnderAnymore);
        }

        if (GetFocusableUnderMouseTopMost())
        {
            UnRegisterForEvents(GetFocusableUnderMouseTopMost());
        }

        p_focusableUnderMouseTopMost = focusableUnderMouseTopMost;
        if (GetFocusableUnderMouseTopMost())
        {
            PropagateIFocusableEvent(GetFocusableUnderMouseTopMost(),
                                     UIEvent::Type::MOUSE_ENTER);

            // We can lose the focusable when propagating event, so recheck
            if (GetFocusableUnderMouseTopMost())
            {
                p_focusablesUnderMouse.Add(GetFocusableUnderMouseTopMost());
                RegisterForEvents(GetFocusableUnderMouseTopMost());
            }
        }
    }

    if (GetFocusableUnderMouseTopMost())
    {
        if (currentMousePos != GetLastMousePosition())
        {
            PropagateIFocusableEvent(GetFocusableUnderMouseTopMost(),
                                     UIEvent::Type::MOUSE_MOVE);

            // We can lose the focusable when propagating event, so recheck
            if (GetFocusableUnderMouseTopMost())
            {
                RegisterForEvents( GetFocusableUnderMouseTopMost() );
            }
        }
    }
    m_lastMousePosition = currentMousePos;

    for (IFocusable *focusableUnderMouse : p_focusablesUnderMouse)
    {
        RegisterForEvents( focusableUnderMouse );
    }

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
                    RegisterForEvents( GetFocusableUnderMouseTopMost() );
                    PropagateIFocusableEvent(GetFocusableUnderMouseTopMost(),
                                             UIEvent::Type::MOUSE_CLICK_DOWN,
                                             mbDown);
                }
            }
        }

        for (MouseButton mbUp : mouseButtonsUp)
        {
            if (GetFocusableUnderMouseTopMost())
            {
                RegisterForEvents( GetFocusableUnderMouseTopMost() );
                PropagateIFocusableEvent(GetFocusableUnderMouseTopMost(),
                                         UIEvent::Type::MOUSE_CLICK_UP,
                                         mbUp);
            }
        }

        if (Input::GetMouseButtonDown(MouseButton::LEFT))
        {
            if (GetFocusableUnderMouseTopMost())
            {
                RegisterForEvents( GetFocusableUnderMouseTopMost() );
                PropagateIFocusableEvent(GetFocusableUnderMouseTopMost(),
                                         UIEvent::Type::STARTED_BEING_PRESSED);
            }
        }

        if (Input::GetMouseButtonDown(MouseButton::LEFT))
        {
            SetFocus( GetFocusableUnderMouseTopMost() );

            p_focusablesPotentiallyBeingPressed = p_focusablesUnderMouse;
            for (IFocusable *focusable : p_focusablesPotentiallyBeingPressed)
            {
                RegisterForEvents(focusable);
            }

            if (GetFocusableUnderMouseTopMost())
            {
                RegisterForEvents( GetFocusableUnderMouseTopMost() );
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
                            RegisterForEvents( GetFocusableUnderMouseTopMost() );
                            PropagateIFocusableEvent(GetFocusableUnderMouseTopMost(),
                                                     UIEvent::Type::MOUSE_CLICK_FULL);
                        }
                    }
                }
            }
        }

        if (!Input::GetMouseButton(MouseButton::LEFT))
        {
            for (IFocusable *focusablePotentiallyBeingPressed :
                    p_focusablesPotentiallyBeingPressed)
            {
                if (focusablePotentiallyBeingPressed->IsBeingPressed())
                {
                    RegisterForEvents( focusablePotentiallyBeingPressed );
                    PropagateIFocusableEvent(focusablePotentiallyBeingPressed,
                                        UIEvent::Type::FINISHED_BEING_PRESSED);
                }
            }

            for (IFocusable *focusable : p_focusablesPotentiallyBeingPressed)
            {
                UnRegisterForEvents(focusable);
            }
            p_focusablesPotentiallyBeingPressed.Clear();
        }

        if (GetFocusableUnderMouseTopMost() &&
            Input::GetMouseButtonDoubleClick(MouseButton::LEFT))
        {
            RegisterForEvents( GetFocusableUnderMouseTopMost() );
            PropagateIFocusableEvent(GetFocusableUnderMouseTopMost(),
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

    // Tabbing
    if (Input::GetKeyDownRepeat(Key::TAB))
    {
        IFocusable *focus = GetFocus();
        if (focus)
        {
            const int n = focusables.Size();
            int indexOfFocus = focusables.IndexOf(focus);
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
                        inside = UICanvas::IsMouseOver(comp, true);
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
    for (RectTransform *rt : rts) { rt->InvalidateTransform(); }
}

void UICanvas::SetFocus(IFocusable *newFocusable_)
{
    IFocusable *newFocusable = newFocusable_;
    Object *obj = newFocusable ? DCAST<Object*>(newFocusable) : nullptr;
    if (obj && obj->IsWaitingToBeDestroyed())
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
        Array<IFocusable*> focusables = go->GetComponents<IFocusable>();
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
            for (GameObject *child : go->GetChildren())
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
        Array<const IFocusable*> focusables = go->GetComponents<const IFocusable>();
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
            for (GameObject *child : go->GetChildren())
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
    return canvasInThis ? canvasInThis : go->GetComponentInParent<UICanvas>(true);
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

    List<IFocusable*> disabledFocusables;
    if (IFocusable *focusableObj = DCAST<IFocusable*>(object))
    {
        disabledFocusables.PushBack(focusableObj);
    }

    if (GameObject *disabledGo = DCAST<GameObject*>(object))
    {
        Array<IFocusable*> disabledGoFocusables =
                    disabledGo->GetComponentsInDescendantsAndThis<IFocusable>();
        for (IFocusable *disabledGoFocusable : disabledGoFocusables)
        {
            disabledFocusables.PushBack(disabledGoFocusable);
        }
    }

    for (IFocusable *disabledFocusable : disabledFocusables)
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

        p_focusablesUnderMouse.Remove(disabledFocusable);
        p_focusablesPotentiallyBeingPressed.Remove(disabledFocusable);
        UnRegisterForEvents(disabledFocusable);
    }
}

void UICanvas::RegisterForEvents(IFocusable *focusable)
{
    ASSERT(focusable);

    if (EventEmitter<IEventsDestroy> *destroyable =
            DCAST<EventEmitter<IEventsDestroy>*>(focusable))
    {
        destroyable->RegisterListener(this);
    }

    if (EventEmitter<IEventsObject> *focusableObj =
                        DCAST<EventEmitter<IEventsObject>*>(focusable))
    {
        focusableObj->EventEmitter<IEventsObject>::RegisterListener(this);
    }
}

void UICanvas::UnRegisterForEvents(IFocusable *focusable)
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
    if (p_focusablesUnderMouse.Contains(focusable))
    {
        ++numReferencesTrackingThisFocusable;
    }
    if (p_focusablesPotentiallyBeingPressed.Contains(focusable))
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
        if (EventEmitter<IEventsDestroy> *destroyable =
                DCAST<EventEmitter<IEventsDestroy>*>(focusable))
        {
            destroyable->UnRegisterListener(this);
        }
    }

    if (EventEmitter<IEventsObject> *focusableObj =
            DCAST<EventEmitter<IEventsObject>*>(focusable))
    {
        focusableObj->EventEmitter<IEventsObject>::UnRegisterListener(this);
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
    const Array<GameObject*> &children = go->GetChildren();
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
                Object *focusableObj = DCAST<Object*>(focusable);
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
