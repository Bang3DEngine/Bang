#ifndef UITEXTCURSOR_H
#define UITEXTCURSOR_H

#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/LineRenderer.h"
#include "Bang/String.h"
#include "Bang/Time.h"

namespace Bang
{
class UITextCursor : public LineRenderer
{
    COMPONENT(UITextCursor)

public:
    UITextCursor();
    virtual ~UITextCursor() override;

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
}

#endif  // UITEXTCURSOR_H
