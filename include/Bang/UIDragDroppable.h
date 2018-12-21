#ifndef UIDRAGDROPPABLE_H
#define UIDRAGDROPPABLE_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/IEventsDragDrop.h"
#include "Bang/IEventsFocus.h"
#include "Bang/String.h"
#include "Bang/Time.h"

namespace Bang
{
class GameObject;
class IEventsDragDrop;
class UIFocusable;
class UIImageRenderer;

class UIDragDroppable : public Component,
                        public EventEmitter<IEventsDragDrop>,
                        public EventListener<IEventsFocus>
{
    COMPONENT(UIDragDroppable)

public:
    UIDragDroppable();
    virtual ~UIDragDroppable() override;

    // Component
    void OnUpdate() override;

    void SetFocusable(UIFocusable *focusable);
    void SetShowDragDropGameObject(bool showDragDropObject);

    bool IsBeingDragged() const;
    UIFocusable *GetFocusable() const;
    bool GetShowDragDropGameObject() const;

private:
    static const Time DragInitTime;

    UIFocusable *p_focusable = nullptr;

    Time m_pressTime;
    bool m_beingDragged = false;
    bool m_showDragDropGameObject = true;
    Vector2i m_dragGrabOffset = Vector2i::Zero();

    Time m_dragStartTime;
    GameObject *m_dragDropGameObject = nullptr;
    UIImageRenderer *p_dragDropImageRenderer = nullptr;

    void OnDragStarted();
    void OnDragUpdate();
    void OnDropped();

    void MoveDragDropGameObjectTo(const Vector2i &pos);

    // Object
    virtual void OnDestroy() override;

    // IFocusable
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

    friend class UICanvas;
};
}  // namespace Bang

#endif  // UIDRAGDROPPABLE_H
