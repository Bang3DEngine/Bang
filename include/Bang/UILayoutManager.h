#ifndef UILAYOUTMANAGER_H
#define UILAYOUTMANAGER_H

#include "Bang/Bang.h"

#include "Bang/Set.h"
#include "Bang/UMap.h"
#include "Bang/Axis.h"
#include "Bang/List.h"
#include "Bang/Object.h"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/ILayoutElement.h"
#include "Bang/ObjectGatherer.h"
#include "Bang/ILayoutController.h"
#include "Bang/ILayoutSelfController.h"

NAMESPACE_BANG_BEGIN

FORWARD class Scene;
FORWARD class Component;
FORWARD class RectTransform;
FORWARD class IEventsTransform;

class UILayoutManager : public EventListener<IEventsDestroy>
{
public:
    UILayoutManager();

    void RebuildLayout(GameObject *gameObject);

    void PropagateInvalidation(ILayoutElement *element);
    void PropagateInvalidation(ILayoutController *controller);

    static Vector2i GetMinSize(GameObject *go);
    static Vector2i GetPreferredSize(GameObject *go);
    static Vector2  GetFlexibleSize(GameObject *go);
    static Vector2  GetSize(GameObject *go, LayoutSizeType sizeType);

    static List<GameObject*> GetLayoutableChildrenList(GameObject *go);

    static UILayoutManager* GetActive(GameObject *go);
    static UILayoutManager* GetActive(Component *comp);
    static UILayoutManager* GetActive(ILayoutElement *layoutElement);
    static UILayoutManager* GetActive(ILayoutController *layoutController);

private:
    UMap<GameObject*, ObjectGatherer<ILayoutElement, false>>
        m_iLayoutElementsPerGameObject;
    UMap<GameObject*, ObjectGatherer<ILayoutController, false>>
        m_iLayoutControllersPerGameObject;

    void CalculateLayout(GameObject *gameObject, Axis axis);
    void ApplyLayout(GameObject *gameObject, Axis axis);

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    const List<ILayoutElement*>& GetLayoutElementsIn(GameObject *gameObject);
    const List<ILayoutController*>& GetLayoutControllersIn(GameObject *gameObject);
};

NAMESPACE_BANG_END

#endif // UILAYOUTMANAGER_H
