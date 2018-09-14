#ifndef UIBUTTONDRIVER_H
#define UIBUTTONDRIVER_H

#include "Bang/Component.h"
#include "Bang/UIFocusable.h"
#include "Bang/IEventsFocus.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture2D;
FORWARD class UIDirLayout;
FORWARD class UITextRenderer;
FORWARD class UIImageRenderer;
FORWARD class UILayoutElement;

class UIButton : public Component
{
    COMPONENT(UIButton)

public:
    // Component
    void OnStart() override;

    void Click();
    void SetBlocked(bool blocked);
    void SetIconSize(const Vector2i &size);
    void SetIconTexture(Texture2D *texture);
    void SetIconSpacingWithText(int spacingWithText);
    void SetIcon(Texture2D *texture, const Vector2i &size,
                 int spacingWithText = 0);

    using ClickedCallback = std::function<void()>;
    void AddClickedCallback(ClickedCallback clickedCallback);

    bool IsBlocked() const;
    UIImageRenderer *GetBorder() const;
    UIImageRenderer* GetIcon() const;
    UITextRenderer* GetText() const;
    UIImageRenderer* GetBackground() const;
    UIDirLayout *GetDirLayout() const;
    UIFocusable* GetFocusable() const;
    UILayoutElement *GetLayoutElement() const;

private:
    bool m_isBlocked = false;
    UIImageRenderer *p_icon          = nullptr;
    UITextRenderer  *p_text          = nullptr;
    UIImageRenderer *p_border        = nullptr;
    UIImageRenderer *p_background    = nullptr;
    UILayoutElement *p_layoutElement = nullptr;
    UIFocusable     *p_focusable     = nullptr;
    Array<ClickedCallback> m_clickedCallbacks;

    UIButton();
    virtual ~UIButton();

    void ChangeAspectToIdle();
    void ChangeAspectToOver();
    void ChangeAspectToPressed();
    void ChangeAspectToBlocked();

    // IEventsFocus
    UIEventResult OnFocusEvent(EventEmitter<IEventsFocus> *focusable,
                               const UIEvent &event);
    void OnMouseEnter();
    void OnMouseExit();

    static UIButton *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UIBUTTONDRIVER_H
