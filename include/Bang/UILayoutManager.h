#ifndef UILAYOUTMANAGER_H
#define UILAYOUTMANAGER_H

#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/UMap.h"

namespace Bang
{
template <class>
class EventEmitter;
class Component;
class GameObject;
class IEventsDestroy;
class ILayoutController;
class ILayoutElement;
template <class ObjectType, bool RECURSIVE>
class ObjectGatherer;

class UILayoutManager : public EventListener<IEventsDestroy>
{
public:
    UILayoutManager();

    void RebuildLayout(GameObject *gameObject);

    void PropagateInvalidation(ILayoutElement *element);
    void PropagateInvalidation(ILayoutController *controller);

    static Vector2i GetMinSize(GameObject *go);
    static Vector2i GetPreferredSize(GameObject *go);
    static Vector2 GetFlexibleSize(GameObject *go);
    static Vector2 GetSize(GameObject *go, LayoutSizeType sizeType);

    static Array<GameObject *> GetLayoutableChildrenList(GameObject *go);

    static UILayoutManager *GetActive(GameObject *go);
    static UILayoutManager *GetActive(Component *comp);
    static UILayoutManager *GetActive(ILayoutElement *layoutElement);
    static UILayoutManager *GetActive(ILayoutController *layoutController);

private:
    UMap<GameObject *, ObjectGatherer<ILayoutElement, false> *>
        m_iLayoutElementsPerGameObject;
    UMap<GameObject *, ObjectGatherer<ILayoutController, false> *>
        m_iLayoutControllersPerGameObject;

    void CalculateLayout(GameObject *gameObject, Axis axis);
    void ApplyLayout(GameObject *gameObject, Axis axis);

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    const Array<ILayoutElement *> &GetLayoutElementsIn(GameObject *gameObject);
    const Array<ILayoutController *> &GetLayoutControllersIn(
        GameObject *gameObject);
};
}

#endif  // UILAYOUTMANAGER_H
