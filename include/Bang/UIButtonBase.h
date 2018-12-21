#ifndef UIBUTTONBASE_H
#define UIBUTTONBASE_H

#include <functional>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/DPtr.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/IEventsFocus.h"
#include "Bang/String.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UITextRenderer.h"

namespace Bang
{
class GameObject;
class Texture2D;
class UIDirLayout;

class UIButtonBase : public Component, public EventListener<IEventsFocus>
{
    COMPONENT_ABSTRACT(UIButtonBase)

public:
    virtual void Click() = 0;
    void SetBlocked(bool blocked);
    void SetIconSize(const Vector2i &size);
    void SetIconTexture(Texture2D *texture);
    void SetIconSpacingWithText(int spacingWithText);
    void SetIcon(Texture2D *texture,
                 const Vector2i &size,
                 int spacingWithText = 0);

    using ClickedCallback = std::function<void()>;
    void AddClickedCallback(std::function<void()> clickedCallback);

    bool IsBlocked() const;
    UIImageRenderer *GetBorder() const;
    UIImageRenderer *GetIcon() const;
    UITextRenderer *GetText() const;
    UIImageRenderer *GetBackground() const;
    UIDirLayout *GetDirLayout() const;
    UIFocusable *GetFocusable() const;
    UILayoutElement *GetLayoutElement() const;

protected:
    UIButtonBase();
    virtual ~UIButtonBase() override;

    void ClickBase();
    virtual void UpdateAspect() = 0;

    void ChangeAspectToIdle();
    void ChangeAspectToOver();
    void ChangeAspectToPressed();
    void ChangeAspectToBlocked();

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

    static UIButtonBase *CreateInto(
        std::function<UIButtonBase *(GameObject *)> createBtnFunc,
        GameObject *go);

protected:
    void CallClickCallback();

private:
    bool m_isBlocked = false;
    DPtr<UITextRenderer> p_text;
    DPtr<UIImageRenderer> p_icon;
    DPtr<UIImageRenderer> p_border;
    DPtr<UIFocusable> p_focusable;
    DPtr<UIImageRenderer> p_background;
    DPtr<UILayoutElement> p_layoutElement;
    Array<std::function<void()>> m_clickedCallbacks;

    friend class GameObjectFactory;
};
}

#endif  // UIBUTTONBASE_H
