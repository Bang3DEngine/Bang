#include "Bang/UIDirLayoutMovableSeparator.h"

#include "Bang/Input.h"
#include "Bang/Material.h"
#include "Bang/GameObject.h"
#include "Bang/UIFocusable.h"
#include "Bang/LineRenderer.h"
#include "Bang/DebugRenderer.h"
#include "Bang/RectTransform.h"
#include "Bang/MaterialFactory.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UILayoutManager.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

UIDirLayoutMovableSeparator::UIDirLayoutMovableSeparator()
{
}

UIDirLayoutMovableSeparator::~UIDirLayoutMovableSeparator()
{
}

void UIDirLayoutMovableSeparator::OnUpdate()
{
    Component::OnUpdate();

    if (p_focusable->IsBeingPressed())
    {
        GameObject *parent = GetGameObject()->GetParent();
        RectTransform *parentRT = parent->GetRectTransform();
        const int thisIndexInParent = parent->GetChildren().IndexOf( GetGameObject() );
        if (thisIndexInParent <= 0) { return; }
        if (thisIndexInParent >= parent->GetChildren().Size() - 1) { return; }

        const int prevSiblingIndex = thisIndexInParent - 1;
        const int nextSiblingIndex = thisIndexInParent + 1;
        GameObject *prevSibling = parent->GetChild(prevSiblingIndex);
        GameObject *nextSibling = parent->GetChild(nextSiblingIndex);
        if (!prevSibling || !nextSibling) { return; }

        RectTransform *prevRT = prevSibling->GetRectTransform();
        RectTransform *nextRT = nextSibling->GetRectTransform();
        if (!prevRT || !nextRT) { return; }

        UILayoutElement *prevLE = prevSibling->GetComponent<UILayoutElement>();
        UILayoutElement *nextLE = nextSibling->GetComponent<UILayoutElement>();
        if (!prevLE || !nextLE) { return; }

        constexpr int AuxiliarLayoutElementPriority = 147;
        const bool horizontal = (GetAxis() == Axis::Horizontal);

        // Prepare siblings, by setting preferred and flexible
        Map<GameObject*, UILayoutElement*> childToAuxLE;
        for (GameObject *child : parent->GetChildren())
        {
            UILayoutElement *auxLE = nullptr;
            for (UILayoutElement *le : child->GetComponents<UILayoutElement>())
            {
                if (le->GetLayoutPriority() == AuxiliarLayoutElementPriority)
                { auxLE = le; }
            }

            if (!auxLE) // If not existing, create the auxiliar layout element
            {
                auxLE = child->AddComponent<UILayoutElement>();
                auxLE->SetLayoutPriority( AuxiliarLayoutElementPriority );
            }

            const RectTransform *childRT = child->GetRectTransform();
            const Vector2i prefSize (childRT->GetViewportAARect().GetSize());
            if (horizontal) { auxLE->SetPreferredWidth(prefSize.x); }
            else { auxLE->SetPreferredHeight(prefSize.y); }

            childToAuxLE.Add(child, auxLE);
        }

        Vector2i mousePos ( Input::GetMousePosition() );
        RectTransform *separatorRT = GetGameObject()->GetRectTransform();
        Vector2i separatorPos (separatorRT->GetViewportAARect().GetCenter());
        Vector2i sepDistToMouse (mousePos - separatorPos);
        Vector2i prevIdealSizeIncrease =  sepDistToMouse * (horizontal ? 1 : -1);
        Vector2i nextIdealSizeIncrease = -sepDistToMouse * (horizontal ? 1 : -1);
        Vector2i prevCurrentSize (prevRT->GetViewportAARect().GetSize());
        Vector2i nextCurrentSize (nextRT->GetViewportAARect().GetSize());

        Vector2i prevIdealNewPrefSize = prevCurrentSize + prevIdealSizeIncrease;
        Vector2i nextIdealNewPrefSize = nextCurrentSize + nextIdealSizeIncrease;

        // Make room decreasing preferred sizes from all previous/next children
        // For prevSibling, we will have to steal room from all next siblings
        // For nextSibling, we will have to steal room from all prev siblings
        Array<Vector2i> prevNextIdealNewPrefSize =
          {prevIdealNewPrefSize, nextIdealNewPrefSize};

        Array<Vector2i> prevNextIdealNeededRoomForNewSize =
          {Vector2i::Max(prevIdealNewPrefSize - prevCurrentSize, Vector2i::Zero),
           Vector2i::Max(nextIdealNewPrefSize - nextCurrentSize, Vector2i::Zero)};

        Array<GameObject*> prevNextSiblings = {prevSibling, nextSibling};
        for (int prevOrNext = 0; prevOrNext <= 1; ++prevOrNext)
        {
            bool isPrev = (prevOrNext==0);
            const List<GameObject*> siblings = parent->GetChildren();
            const Vector2i totalIdealNeededRoomForNewSize =
                                prevNextIdealNeededRoomForNewSize[prevOrNext];

            Vector2i neededRoomForNewSize = totalIdealNeededRoomForNewSize;

            // First take into account that we have free space from the
            // displacement itself

            // Then take parent free space if any
            Vector2i parentFreeSize ( parentRT->GetViewportAARect().GetSize() );
            for (GameObject *child : parent->GetChildren())
            {
                RectTransform *childRT = child->GetRectTransform();
                if (childRT)
                {
                    parentFreeSize -= Vector2i(childRT->GetViewportAARect().GetSize());
                }
            }
            neededRoomForNewSize -= Vector2i::Max(parentFreeSize, Vector2i::Zero);

            auto it = siblings.Begin();
            std::advance(it, (isPrev ? nextSiblingIndex : prevSiblingIndex));
            while (true)
            {
                if (neededRoomForNewSize.GetAxis( GetAxis() ) <= 0) { break; }
                if (isPrev && it == siblings.End()) { break; }

                GameObject *sibling = *it;
                RectTransform *sbRT = sibling->GetRectTransform();
                Vector2i sbSize ( sbRT->GetViewportAARect().GetSize() );
                Vector2i sbMinSize = UILayoutManager::GetMinSize(sibling);
                Vector2i availableRoom = sbSize - sbMinSize;
                Vector2i roomToBeStolen = Math::Clamp(availableRoom,
                                                      Vector2i::Zero,
                                                      neededRoomForNewSize);
                UILayoutElement *auxLE = childToAuxLE.Get(sibling);
                Vector2i newPrefSize = sbSize - roomToBeStolen;

                if (horizontal) { auxLE->SetPreferredWidth(newPrefSize.x); }
                else { auxLE->SetPreferredHeight(newPrefSize.y); }

                neededRoomForNewSize -= roomToBeStolen;

                if (!isPrev && it == siblings.Begin()) { break; }
                if (isPrev) { ++it; } else { --it; }
            }

            Vector2i newPrefSize;
            GameObject *sibling = prevNextSiblings[prevOrNext];
            RectTransform *siblingRT = sibling->GetRectTransform();
            Vector2i currentSize ( siblingRT->GetViewportAARect().GetSize() );
            if (totalIdealNeededRoomForNewSize.GetAxis( GetAxis() ) > 0)
            {
                // How much room that we needed we have not been to get?
                Vector2i neededRoomLeft = Vector2i::Max(neededRoomForNewSize,
                                                        Vector2i::Zero);

                // Assign new preferred size to prev next
                Vector2i stolenRoom = (totalIdealNeededRoomForNewSize - neededRoomLeft);
                newPrefSize = currentSize + stolenRoom;
            }
            else
            {
                newPrefSize = prevNextIdealNewPrefSize[prevOrNext];
            }

            UILayoutElement *auxLE = childToAuxLE.Get(sibling);
            if (horizontal) { auxLE->SetPreferredWidth(newPrefSize.x); }
            else { auxLE->SetPreferredHeight(newPrefSize.y); }
        }

        // DebugRenderer::RenderAARectNDC( prevRT->GetViewportAARectNDC(), Color::Green,
        //                                 0.1f, 1.0f, false);
        // DebugRenderer::RenderAARectNDC( nextRT->GetViewportAARectNDC(), Color::Red,
        //                                 0.1f, 1.0f, false);
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

        bool horizontal = (GetAxis() == Axis::Horizontal);
        p_focusable->SetCursorType(horizontal ? Cursor::Type::SizeWE :
                                                Cursor::Type::SizeNS);
        if (horizontal)
        {
            le->SetFlexibleSize( Vector2(0, 99999999) );
            lr->SetPoints( {Vector3(0.0f, -linePercent, 0),
                            Vector3(0.0f,  linePercent, 0)} );
        }
        else
        {
            le->SetFlexibleSize( Vector2(99999999, 0) );
            lr->SetPoints( {Vector3(-linePercent, 0.0f, 0),
                            Vector3( linePercent, 0.0f, 0)} );
        }
    }
}

Axis UIDirLayoutMovableSeparator::GetAxis() const
{
    return m_axis;
}

void UIDirLayoutMovableSeparator::OnMouseEnter(IFocusable *focusable)
{
    p_lineRenderer->GetMaterial()->SetLineWidth(3.0f);
}

void UIDirLayoutMovableSeparator::OnMouseExit(IFocusable *focusable)
{
    p_lineRenderer->GetMaterial()->SetLineWidth(1.0f);
}

UIDirLayoutMovableSeparator*
UIDirLayoutMovableSeparator::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);
    UIDirLayoutMovableSeparator *dirLayoutSep =
            go->AddComponent<UIDirLayoutMovableSeparator>();

    UILayoutElement *le = go->AddComponent<UILayoutElement>();
    le->SetMinSize(Vector2i(15));
    le->SetFlexibleSize( Vector2(0) );

    UIFocusable *focusable = go->AddComponent<UIFocusable>();
    focusable->EventEmitter<IFocusListener>::RegisterListener(dirLayoutSep);

    LineRenderer *lr = go->AddComponent<LineRenderer>();
    lr->SetMaterial(MaterialFactory::GetUIImage().Get());
    lr->GetMaterial()->SetAlbedoColor(Color::White);
    lr->SetViewProjMode(GL::ViewProjMode::Canvas);

    dirLayoutSep->p_lineRenderer = lr;
    dirLayoutSep->p_focusable = focusable;
    dirLayoutSep->SetAxis(Axis::Horizontal);

    return dirLayoutSep;
}

