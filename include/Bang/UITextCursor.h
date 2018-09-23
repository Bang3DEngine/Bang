#ifndef UITEXTCURSOR_H
#define UITEXTCURSOR_H

#include "Bang/LineRenderer.h"

NAMESPACE_BANG_BEGIN

class UITextCursor : public LineRenderer
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UITextCursor)

public:
    UITextCursor();
    virtual ~UITextCursor();

    virtual void OnUpdate() override;

    void ResetTickTime();

    void SetStroke(float cursorWidth);
    void SetTickTime(Time cursorTickTime);

    float GetStroke() const;
    Time GetTickTime() const;

private:
    Time m_cursorTime;
    Time m_cursorTickTime;
};

NAMESPACE_BANG_END

#endif // UITEXTCURSOR_H
