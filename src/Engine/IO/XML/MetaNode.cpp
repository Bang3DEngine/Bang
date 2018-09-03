#include "Bang/MetaNode.h"

#include "Bang/Paths.h"
#include "Bang/XMLMetaReader.h"

USING_NAMESPACE_BANG

MetaNode::MetaNode(const String &name)
{
    SetName(name);
}

MetaNode::~MetaNode()
{
}

void MetaNode::AddChild(const MetaNode &node)
{
    m_children.PushBack(node);
}

void MetaNode::UpdateAttributeValue(const String &attributeName,
                                    const String &newAttributeValue)
{
    MetaAttribute *attr = GetAttribute(attributeName);
    if (attr)
    {
        attr->SetValue(newAttributeValue);
    }
}

bool MetaNode::Contains(const String &attrName) const
{
    return m_attributes.ContainsKey(attrName);
}

void MetaNode::Set(const MetaAttribute &attribute)
{
    MetaAttribute *attr = GetAttribute(attribute.GetName());
    if (!attr)
    {
        m_attributeOrder.PushBack(attribute.GetName());
        m_attributes[attribute.GetName()] = attribute;
    }
    else
    {
        *attr = attribute;
    }
}

void MetaNode::Set(const String &attributeName, const String &attributeValue)
{
    MetaAttribute attr(attributeName, attributeValue);
    Set(attr);
}

void MetaNode::RemoveAttribute(const String &attributeName)
{
    for (auto it = m_attributes.Begin(); it != m_attributes.End();)
    {
        const MetaAttribute &attr = it->second;
        if (attr.GetName() == attributeName)
        {
            m_attributes.Remove(it++);
            m_attributeOrder.Remove(attr.GetName());
        }
        else
        {
            ++it;
        }
    }
}

MetaAttribute *MetaNode::GetAttribute(const String &attributeName) const
{
    for (auto& itPair : m_attributes)
    {
        MetaAttribute &attr = itPair.second;
        if (attr.GetName() == attributeName)
        {
            return &attr;
        }
    }
    return nullptr;
}

String MetaNode::GetAttributeValue(const String &attributeName) const
{
    if (m_attributes.ContainsKey(attributeName))
    {
        return m_attributes[attributeName].GetStringValue();
    }
    return "";
}

const MetaNode *MetaNode::GetChild(const String &name) const
{
    for (const MetaNode& node : m_children)
    {
        if (node.GetName() == name)
        {
            return &node;
        }
    }
    return nullptr;
}

String MetaNode::ToString(const String& indent) const
{
    String str = "";

    str += indent + "<" + m_name;
    for (const auto& attrPair : GetAttributesListInOrder())
    {
        const MetaAttribute* attr = attrPair.second;
        str += " " + attr->ToString() + "\n";
        for (int i = 0; i < m_name.Size() + indent.Size() + 1; ++i )
        {
            str += " ";
        }
    }
    str += ">\n";

    const String newIndent = indent + "    ";
    for (const MetaNode& child : m_children)
    {
        str += child.ToString(newIndent);
    }
    str += indent + "</" + m_name + ">\n";
    return str;
}

void MetaNode::SetName(const String name)
{
    m_name = name;
}

String MetaNode::ToString() const
{
    return ToString("");
}

const String &MetaNode::GetName() const
{
    return m_name;
}

const Map<String, MetaAttribute> &MetaNode::GetAttributes() const
{
    return m_attributes;
}

const List<String> &MetaNode::GetAttributesOrderList() const
{
    return m_attributeOrder;
}

List<std::pair<String, MetaAttribute*> > MetaNode::GetAttributesListInOrder() const
{
    List< std::pair<String, MetaAttribute*> > attributes;
    for (const String& attrName : m_attributeOrder)
    {
        attributes.PushBack( std::make_pair(attrName, &m_attributes[attrName]) );
    }
    return attributes;
}

const List<MetaNode> &MetaNode::GetChildren() const
{
    return m_children;
}

List<MetaNode> &MetaNode::GetChildren()
{
    return m_children;
}

MetaNode MetaNode::FromString(const String &metaString)
{
    return XMLMetaReader::FromString(metaString);
}
