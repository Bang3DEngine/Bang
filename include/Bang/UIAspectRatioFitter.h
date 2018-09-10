#ifndef UIASPECTRATIOFITTER_H
#define UIASPECTRATIOFITTER_H

#include "Bang/Vector2.h"
#include "Bang/Component.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/AspectRatioMode.h"
#include "Bang/ILayoutSelfController.h"

NAMESPACE_BANG_BEGIN

class UIAspectRatioFitter : public Component,
                            public ILayoutSelfController
{
    COMPONENT(UIAspectRatioFitter)

public:

    void SetAspectRatio(float aspectRatio);
    void SetAspectRatio(const Vector2 &size);
    void SetAspectRatio(const Vector2i &size);
    void SetPaddings(int paddingsAll);
    void SetPaddings(const Vector2i &paddingLeftBot,
                     const Vector2i &paddingRightTop);
    void SetPaddingLeftBot(const Vector2i &paddingLeftBot);
    void SetPaddingRightTop(const Vector2i &paddingRightTop);

    const Vector2i &GetPaddingLeftBot() const;
    const Vector2i &GetPaddingRightTop() const;
    float GetAspectRatio() const;

    void SetAspectRatioMode(AspectRatioMode arMode);
    AspectRatioMode GetAspectRatioMode() const;

    // ILayoutSelfController
    void ApplyLayout(Axis axis) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // IEventsTransform
    void OnTransformChanged() override;
    void OnParentTransformChanged() override;

    // IInvalidatable
    void OnInvalidated() override;

private:
    float m_aspectRatio = -1.0f;
    Vector2i m_paddingLeftBot = Vector2i::Zero;
    Vector2i m_paddingRightTop = Vector2i::Zero;
    AspectRatioMode m_aspectRatioMode = Undef<AspectRatioMode>();

    UIAspectRatioFitter();
    virtual ~UIAspectRatioFitter();
};

NAMESPACE_BANG_END

#endif // UIASPECTRATIOFITTER_H

