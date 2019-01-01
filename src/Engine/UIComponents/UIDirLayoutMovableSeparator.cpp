#include "Bang/UIDirLayoutMovableSeparator.h"

#include "Bang/Array.h"
#include "Bang/AssetHandle.h"
#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/Cursor.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/Input.h"
#include "Bang/LineRenderer.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/RectTransform.h"
#include "Bang/ShaderProgram.h"
#include "Bang/Transform.h"
#include "Bang/UIFocusable.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UILayoutManager.h"
#include "Bang/UMap.h"
#include "Bang/UMap.tcc"
#include "Bang/Vector2.h"

using namespace Bang;

UIDirLayoutMovableSeparator::UIDirLayoutMovableSeparator()
{
    SET_INSTANCE_CLASS_ID(UIDirLayoutMovableSeparator)
}

UIDirLayoutMovableSeparator::~UIDirLayoutMovableSeparator()
{
}

void UIDirLayoutMovableSeparator::OnUpdate()
{
    Component::OnUpdate();

    // Add auxiliar layout elements to children, if needed. Update them also.
    constexpr int AuxiliarLayoutElementPriority = 147;
    GameObject *parent = GetGameObject()->GetParent();
    UMap<GameObject *, UILayoutElement *> childToAuxLE;
    for (GameObject *child : parent->GetChildren())
    {
        UILayoutElement *auxLE = nullptr;
        for (UILayoutElement *le : child->GetComponents<UILayoutElement>())
        {
            if (le->GetLayoutPriority() == AuxiliarLayoutElementPriority)
            {
                auxLE = le;
                break;
            }
        }

        if (child->GetRectTransform())
        {
            // If not existing, create the auxiliar layout element,
            // when we have a valid start size
            if (!auxLE)
            {
                auxLE = child->AddComponent<UILayoutElement>();
                auxLE->SetLayoutPriority(AuxiliarLayoutElementPriority);
            }
        }
        childToAuxLE.Add(child, auxLE);
    }

    if (p_focusable->IsBeingPressed())
    {
        RectTransform *parentRT = parent->GetRectTransform();
        const int thisIndexInParent =
            parent->GetChildren().IndexOf(GetGameObject());
        if ((thisIndexInParent <= 0) ||
            (thisIndexInParent >= parent->GetChildren().Size() - 1))
        {
            return;
        }

        const int prevSiblingIndex = thisIndexInParent - 1;
        const int nextSiblingIndex = thisIndexInParent + 1;
        GameObject *prevSibling = parent->GetChild(prevSiblingIndex);
        GameObject *nextSibling = parent->GetChild(nextSiblingIndex);
        if (!prevSibling || !nextSibling)
        {
            return;
        }

        RectTransform *prevRT = prevSibling->GetRectTransform();
        RectTransform *nextRT = nextSibling->GetRectTransform();
        if (!prevRT || !nextRT)
        {
            return;
        }

        const bool horizontal = (GetAxis() == Axis::HORIZONTAL);

        Vector2i mousePos(Input::GetMousePosition());
        RectTransform *separatorRT = GetGameObject()->GetRectTransform();
        Vector2i separatorPos(separatorRT->GetViewportAARect().GetCenter());
        Vector2i sepDistToMouse(mousePos - separatorPos);
        Vector2i prevIdealSizeIncrease = sepDistToMouse * (horizontal ? 1 : -1);
        Vector2i nextIdealSizeIncrease =
            -sepDistToMouse * (horizontal ? 1 : -1);
        Vector2i prevCurrentSize(prevRT->GetViewportAARect().GetSize());
        Vector2i nextCurrentSize(nextRT->GetViewportAARect().GetSize());

        Vector2i prevIdealNewPrefSize =
            (prevCurrentSize + prevIdealSizeIncrease);
        Vector2i nextIdealNewPrefSize =
            (nextCurrentSize + nextIdealSizeIncrease);

        // Make room decreasing preferred sizes from all previous/next children
        // For prevSibling, we will have to steal room from all next siblings
        // For nextSibling, we will have to steal room from all prev siblings
        Array<Vector2i> prevNextIdealNewPrefSize = {prevIdealNewPrefSize,
                                                    nextIdealNewPrefSize};

        Array<Vector2i> prevNextIdealNeededRoomForNewSize = {
            Vector2i::Max(prevIdealNewPrefSize - prevCurrentSize,
                          Vector2i::Zero()),
            Vector2i::Max(nextIdealNewPrefSize - nextCurrentSize,
                          Vector2i::Zero())};

        Array<GameObject *> prevNextSiblings = {prevSibling, nextSibling};
        for (int prevOrNext : {0, 1})
        {
            bool isPrev = (prevOrNext == 0);
            const Array<GameObject *> &siblings = parent->GetChildren();
            const Vector2i totalIdealNeededRoomForNewSize =
                prevNextIdealNeededRoomForNewSize[prevOrNext];

            Vector2i neededRoomForNewSize = totalIdealNeededRoomForNewSize;

            // Take parent free space if any
            Vector2i parentFreeSize(parentRT->GetViewportAARect().GetSize());
            for (GameObject *child : parent->GetChildren())
            {
                if (RectTransform *childRT = child->GetRectTransform())
                {
                    parentFreeSize -=
                        Vector2i(childRT->GetViewportAARect().GetSize());
                }
            }
            neededRoomForNewSize -=
                Vector2i::Max(parentFreeSize, Vector2i::Zero());

            int j = (isPrev ? nextSiblingIndex : prevSiblingIndex);
            while (j >= 0 && j <= siblings.Size() - 1)
            {
                if (neededRoomForNewSize[GetAxis()] <= 0)
                {
                    break;
                }

                // Steal room from siblings hihihi
                GameObject *sibling = siblings[j];
                if (RectTransform *sbRT = sibling->GetRectTransform())
                {
                    Vector2i sbSize(sbRT->GetViewportAARect().GetSize());
                    Vector2i sbMinSize = UILayoutManager::GetMinSize(sibling);
                    Vector2i availableRoom = (sbSize - sbMinSize);
                    Vector2i roomToBeStolen = Vector2i::Clamp(
                        availableRoom, Vector2i::Zero(), neededRoomForNewSize);

                    if (UILayoutElement *auxLE = childToAuxLE.Get(sibling))
                    {
                        Vector2i newPrefSizeAfterBeingStolen = Vector2i::Max(
                            sbSize - roomToBeStolen, Vector2i::One());

                        // Change preferred size to steal :)
                        auxLE->SetPreferredSizeInAxis(
                            newPrefSizeAfterBeingStolen[GetAxis()], GetAxis());

                        neededRoomForNewSize -= roomToBeStolen;
                    }
                }
                j += (isPrev ? 1 : -1);
            }

            Vector2i newPrefSize;
            GameObject *sibling = prevNextSiblings[prevOrNext];
            RectTransform *siblingRT = sibling->GetRectTransform();
            if (totalIdealNeededRoomForNewSize.GetAxis(GetAxis()) > 0)
            {
                Vector2i currentSize(siblingRT->GetViewportAARect().GetSize());

                // How much room that we needed we have not been able to get?
                Vector2i neededRoomLeft =
                    Vector2i::Max(neededRoomForNewSize, Vector2i::Zero());

                // Assign new preferred size to prev next
                Vector2i stolenRoom =
                    (totalIdealNeededRoomForNewSize - neededRoomLeft);
                newPrefSize = currentSize + stolenRoom;
            }
            else
            {
                newPrefSize = prevNextIdealNewPrefSize[prevOrNext];
            }

            if (UILayoutElement *auxLE = childToAuxLE.Get(sibling))
            {
                auxLE->SetPreferredSizeInAxis(newPrefSize[GetAxis()],
                                              GetAxis());
            }
        }
    }

    bool isSomeSiblingSeparatorBeingUsed = false;
    for (GameObject *sibling : parent->GetChildren())
    {
        auto movableSeps =
            sibling->GetComponents<UIDirLayoutMovableSeparator>();
        for (UIDirLayoutMovableSeparator *movableSep : movableSeps)
        {
            if (movableSep->p_focusable->IsBeingPressed())
            {
                isSomeSiblingSeparatorBeingUsed = true;
                break;
            }
        }

        if (isSomeSiblingSeparatorBeingUsed)
        {
            break;
        }
    }

    if (!isSomeSiblingSeparatorBeingUsed)
    {
        // Update auxiliar layout elements preferred sizes
        uint separatorIndex = parent->GetChildren().IndexOf(GetGameObject());
        for (int k : {-1, 1})
        {
            int neighborIndex = (separatorIndex + k);
            if (neighborIndex >= 0 &&
                neighborIndex < parent->GetChildren().Size())
            {
                GameObject *neighbor =
                    parent->GetChildren()[separatorIndex + k];
                if (UILayoutElement *auxLE = childToAuxLE.Get(neighbor))
                {
                    if (RectTransform *childRT = neighbor->GetRectTransform())
                    {
                        if (!childRT
                                 ->IInvalidatableTransformLocal::IsInvalid() &&
                            !childRT
                                 ->IInvalidatableTransformWorld::IsInvalid() &&
                            !auxLE->IsInvalid())
                        {
                            const Vector2i prefSize(
                                childRT->GetViewportAARect().GetSize());
                            auxLE->SetPreferredSizeInAxis(prefSize[GetAxis()],
                                                          GetAxis());
                        }
                    }
                }
            }
        }
    }
}

void UIDirLayoutMovableSeparator::SetAxis(Axis axis)
{
    if (axis != GetAxis())
    {
        m_axis = axis;

        LineRenderer *lr = p_lineRenderer;
        constexpr float linePercent = 1.0f;
        UILayoutElement *le = GetGameObject()->GetComponent<UILayoutElement>();

        bool horizontal = (GetAxis() == Axis::HORIZONTAL);
        p_focusable->SetCursorType(horizontal ? Cursor::Type::SIZE_WE
                                              : Cursor::Type::SIZE_NS);
        if (horizontal)
        {
            le->SetFlexibleSize(Vector2(0, 99999999));
            lr->SetPoints({Vector3(0.0f, -linePercent, 0),
                           Vector3(0.0f, linePercent, 0)});
        }
        else
        {
            le->SetFlexibleSize(Vector2(99999999, 0));
            lr->SetPoints({Vector3(-linePercent, 0.0f, 0),
                           Vector3(linePercent, 0.0f, 0)});
        }
    }
}

Axis UIDirLayoutMovableSeparator::GetAxis() const
{
    return m_axis;
}

UIEventResult UIDirLayoutMovableSeparator::OnUIEvent(UIFocusable *,
                                                     const UIEvent &event)
{
    if (event.type == UIEvent::Type::MOUSE_ENTER)
    {
        p_lineRenderer->GetMaterial()
            ->GetShaderProgramProperties()
            .SetLineWidth(3.0f);
        return UIEventResult::INTERCEPT;
    }
    else if (event.type == UIEvent::Type::MOUSE_EXIT)
    {
        p_lineRenderer->GetMaterial()
            ->GetShaderProgramProperties()
            .SetLineWidth(1.0f);
        return UIEventResult::INTERCEPT;
    }
    return UIEventResult::IGNORE;
}

UIDirLayoutMovableSeparator *UIDirLayoutMovableSeparator::CreateInto(
    GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);
    UIDirLayoutMovableSeparator *dirLayoutSep =
        go->AddComponent<UIDirLayoutMovableSeparator>();

    UILayoutElement *le = go->AddComponent<UILayoutElement>();
    le->SetMinSize(Vector2i(15));
    le->SetFlexibleSize(Vector2(0));

    UIFocusable *focusable = go->AddComponent<UIFocusable>();
    focusable->EventEmitter<IEventsFocus>::RegisterListener(dirLayoutSep);

    LineRenderer *lr = go->AddComponent<LineRenderer>();
    lr->SetMaterial(MaterialFactory::GetUIImage().Get());
    lr->GetMaterial()->SetAlbedoColor(Color::DarkGray());
    lr->SetViewProjMode(GL::ViewProjMode::CANVAS);

    dirLayoutSep->p_lineRenderer = lr;
    dirLayoutSep->p_focusable = focusable;
    dirLayoutSep->SetAxis(Axis::HORIZONTAL);

    return dirLayoutSep;
}
