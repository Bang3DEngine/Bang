#ifndef UIDRAGDROPPABLE_H
#define UIDRAGDROPPABLE_H

#include "Bang/Bang.h"
#include "Bang/Component.h"
#include "Bang/Texture2D.h"
#include "Bang/IFocusListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;
FORWARD class Framebuffer;
FORWARD class UIImageRenderer;

class UIDragDroppable : public Component,
                        public IFocusListener
{
    COMPONENT(UIDragDroppable);

public:
	UIDragDroppable();
	virtual ~UIDragDroppable();

    // Component
    void OnUpdate() override;

    void SetFocusable(IFocusable *focusable);
    void SetShowDragDropGameObject(bool showDragDropObject);

    bool IsBeingDragged() const;
    IFocusable *GetFocusable() const;
    bool IsShowDragDropGameObject() const;

private:
    static const float DragInitTime;

    IFocusable *p_focusable = nullptr;

    bool m_beingPressed = false;
    bool m_beingDragged = false;
    bool m_showDragDropGameObject = true;
    float m_secondsSinceMouseIsDown = 0.0f;
    Vector2i m_dragGrabOffset = Vector2i::Zero;

    RH<Texture2D> m_dragDropTexture;
    Framebuffer *m_dragDropFB = nullptr;
    GameObject *m_dragDropGameObject = nullptr;
    UIImageRenderer *p_dragDropImageRenderer = nullptr;

    void OnDragStarted();
    void OnDragUpdate();
    void OnDropped();

    void MoveDragDropGameObjectTo(const Vector2i &pos);

    // IFocusable
    virtual void OnClicked(IFocusable *focusable) override;

    friend class UICanvas;
};

NAMESPACE_BANG_END

#endif // UIDRAGDROPPABLE_H

