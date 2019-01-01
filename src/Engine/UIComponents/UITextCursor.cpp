#include "Bang/UITextCursor.h"

#include "Bang/Array.tcc"
#include "Bang/AssetHandle.h"
#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/Component.h"
#include "Bang/GL.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/ShaderProgram.h"
#include "Bang/Time.h"

using namespace Bang;

UITextCursor::UITextCursor()
{
    SET_INSTANCE_CLASS_ID(UITextCursor)

    SetMaterial(MaterialFactory::GetUIImage().Get());
    GetMaterial()->SetAlbedoColor(Color::Black());
    SetViewProjMode(GL::ViewProjMode::CANVAS);
    SetStroke(1.0f);

    m_cursorTickTime.SetSeconds(0.5);

    constexpr float limit = 1.0f;
    SetPoints({Vector3(0, -limit, 0), Vector3(0, limit, 0)});
}

UITextCursor::~UITextCursor()
{
}

void UITextCursor::OnUpdate()
{
    Component::OnUpdate();

    m_cursorTime += Time::GetDeltaTime();
    SetVisible(m_cursorTime <= m_cursorTickTime);
    if (m_cursorTime >= m_cursorTickTime * 2)
    {
        m_cursorTime.SetNanos(0);
    }
}

void UITextCursor::ResetTickTime()
{
    m_cursorTime.SetNanos(0);
    SetVisible(true);
}

void UITextCursor::SetStroke(float cursorWidth)
{
    GetMaterial()->GetShaderProgramProperties().SetLineWidth(cursorWidth);
}

void UITextCursor::SetTickTime(Time cursorTickTime)
{
    m_cursorTickTime = cursorTickTime;
}

float UITextCursor::GetStroke() const
{
    return GetActiveMaterial()
        ->GetShaderProgram()
        ->GetLoadedProperties()
        .GetLineWidth();
}

Time UITextCursor::GetTickTime() const
{
    return m_cursorTickTime;
}
