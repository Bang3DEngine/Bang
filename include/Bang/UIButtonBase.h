#ifndef UIBUTTONBASE_H
#define UIBUTTONBASE_H

#include "Bang/DPtr.h"
#include "Bang/Component.h"
#include "Bang/UIFocusable.h"
#include "Bang/IEventsFocus.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture2D;
FORWARD class UIDirLayout;
FORWARD class UITextRenderer;
FORWARD class UIImageRenderer;
FORWARD class UILayoutElement;

class UIButtonBase : public Component,
                     public EventListener<IEventsFocus>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST_ABSTRACT(UIButtonBase)

public:
    virtual void Click() = 0;
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

protected:
    UIButtonBase();
    virtual ~UIButtonBase();

    void ClickBase();
    virtual void OnBlockedChanged();

    void ChangeAspectToIdle();
    void ChangeAspectToOver();
    void ChangeAspectToPressed();
    void ChangeAspectToBlocked();

    void OnMouseEnter();
    void OnMouseExit();

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

    static UIButtonBase *CreateInto(
            std::function<UIButtonBase*(GameObject*)> createBtnFunc,
            GameObject *go);

protected:
    void CallClickCallback();

private:
    bool m_isBlocked = false;
    DPtr<UITextRenderer>  p_text;
    DPtr<UIImageRenderer> p_icon;
    DPtr<UIImageRenderer> p_border;
    DPtr<UIFocusable>     p_focusable;
    DPtr<UIImageRenderer> p_background;
    DPtr<UILayoutElement> p_layoutElement;
    Array<ClickedCallback> m_clickedCallbacks;

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UIBUTTONBASE_H

