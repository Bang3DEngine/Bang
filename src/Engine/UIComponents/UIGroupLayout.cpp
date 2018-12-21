#include "Bang/UIGroupLayout.h"

#include <istream>

#include "Bang/ClassDB.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"

using namespace Bang;

UIGroupLayout::UIGroupLayout()
{
    SET_INSTANCE_CLASS_ID(UIGroupLayout)
    SetChildrenHorizontalAlignment(HorizontalAlignment::CENTER);
    SetChildrenVerticalAlignment(VerticalAlignment::CENTER);
    SetChildrenHorizontalStretch(Stretch::FULL);
    SetChildrenVerticalStretch(Stretch::FULL);
}

Bang::UIGroupLayout::~UIGroupLayout()
{
}

void UIGroupLayout::SetSpacing(int spacingPx)
{
    m_spacingPx = spacingPx;
}
void UIGroupLayout::SetPaddingLeft(int paddingLeft)
{
    SetPaddings(
        paddingLeft, GetPaddingBot(), GetPaddingRight(), GetPaddingTop());
}

void UIGroupLayout::SetPaddingBot(int paddingBot)
{
    SetPaddings(
        GetPaddingLeft(), paddingBot, GetPaddingRight(), GetPaddingTop());
}

void UIGroupLayout::SetPaddingRight(int paddingRight)
{
    SetPaddings(
        GetPaddingLeft(), GetPaddingBot(), paddingRight, GetPaddingTop());
}

void UIGroupLayout::SetPaddingTop(int paddingTop)
{
    SetPaddings(
        GetPaddingLeft(), GetPaddingBot(), GetPaddingRight(), paddingTop);
}

void UIGroupLayout::SetPaddings(int padding)
{
    SetPaddings(padding, padding, padding, padding);
}

void UIGroupLayout::SetPaddings(int paddingLeft,
                                int paddingBot,
                                int paddingRight,
                                int paddingTop)
{
    if (m_paddingLeftBot.x != paddingLeft || m_paddingLeftBot.y != paddingBot ||
        m_paddingRightTop.x != paddingRight ||
        m_paddingRightTop.y != paddingTop)
    {
        m_paddingLeftBot = Vector2i(paddingLeft, paddingBot);
        m_paddingRightTop = Vector2i(paddingRight, paddingTop);
    }
}

void UIGroupLayout::SetChildrenHorizontalAlignment(
    HorizontalAlignment hAlignment)
{
    if (m_childrenHorizontalAlignment != hAlignment)
    {
        m_childrenHorizontalAlignment = hAlignment;
    }
}

void UIGroupLayout::SetChildrenVerticalAlignment(VerticalAlignment vAlignment)
{
    if (m_childrenVerticalAlignment != vAlignment)
    {
        m_childrenVerticalAlignment = vAlignment;
    }
}

void UIGroupLayout::SetChildrenHorizontalStretch(Stretch hStretch)
{
    if (m_childrenHorizontalStretch != hStretch)
    {
        m_childrenHorizontalStretch = hStretch;
    }
}

void UIGroupLayout::SetChildrenVerticalStretch(Stretch vStretch)
{
    if (m_childrenVerticalStretch != vStretch)
    {
        m_childrenVerticalStretch = vStretch;
    }
}

int UIGroupLayout::GetSpacing() const
{
    return m_spacingPx;
}
HorizontalAlignment UIGroupLayout::GetChildrenHorizontalAlignment() const
{
    return m_childrenHorizontalAlignment;
}

VerticalAlignment UIGroupLayout::GetChildrenVerticalAlignment() const
{
    return m_childrenVerticalAlignment;
}

Stretch UIGroupLayout::GetChildrenVerticalStretch() const
{
    return m_childrenVerticalStretch;
}

Stretch UIGroupLayout::GetChildrenHorizontalStretch() const
{
    return m_childrenHorizontalStretch;
}

int UIGroupLayout::GetPaddingLeft() const
{
    return GetPaddingLeftBot().x;
}

int UIGroupLayout::GetPaddingBot() const
{
    return GetPaddingLeftBot().y;
}

int UIGroupLayout::GetPaddingRight() const
{
    return GetPaddingRightTop().x;
}

int UIGroupLayout::GetPaddingTop() const
{
    return GetPaddingRightTop().y;
}

const Vector2i &UIGroupLayout::GetPaddingLeftBot() const
{
    return m_paddingLeftBot;
}

const Vector2i &UIGroupLayout::GetPaddingRightTop() const
{
    return m_paddingRightTop;
}

Vector2i UIGroupLayout::GetPaddingSize() const
{
    return GetPaddingLeftBot() + GetPaddingRightTop();
}

void UIGroupLayout::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("SpacingPx"))
    {
        SetSpacing(metaNode.Get<int>("SpacingPx"));
    }
}

void UIGroupLayout::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("SpacingPx", m_spacingPx);
}
