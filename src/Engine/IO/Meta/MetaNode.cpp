#include "Bang/MetaNode.h"

#include "Bang/File.h"
#include "Bang/Debug.h"
#include "Bang/Paths.h"

#include "yaml-cpp/yaml.h"

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

void MetaNode::SetName(const String name)
{
    m_name = name;
}

String MetaNode::ToString() const
{
    YAML::Emitter out;
    ToString(out);
    return String(out.c_str());
}

void MetaNode::ToStringInner(YAML::Emitter &out) const
{
    out << YAML::Key << GetName();
    out << YAML::Value << YAML::BeginMap;

        for (const auto &pair : GetAttributes())
        {
            const MetaAttribute &attr = pair.second;
            out << YAML::Key << attr.GetName();
            out << YAML::Value << attr.GetStringValue();
        }

        out << YAML::Key << "Children";
        out << YAML::Value << YAML::BeginMap;
        for (const MetaNode &childMeta : GetChildren())
        {
            childMeta.ToStringInner(out);
        }
        out << YAML::EndMap;

    out << YAML::EndMap;
}
void MetaNode::ToString(YAML::Emitter &out) const
{
    out << YAML::BeginMap;
    ToStringInner(out);
    out << YAML::EndMap;
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

void MetaNode::Import(const String &metaString)
{
    const YAML::Node yamlNode = YAML::Load(metaString);
    if (!yamlNode.IsNull() && yamlNode.size() >= 1)
    {
        const YAML::Node &rootNameYAMLNode = yamlNode.begin()->first;
        const YAML::Node &rootMapYAMLNode  = yamlNode.begin()->second;
        SetName( rootNameYAMLNode.Scalar() );
        Import(rootMapYAMLNode);
    }
}

void MetaNode::Import(const YAML::Node &yamlNode)
{
    if (!yamlNode.IsNull())
    {
        for (const auto &attrYAMLPair : yamlNode)
        {
            const YAML::Node &attrYAMLName = attrYAMLPair.first;
            const YAML::Node &attrYAMLNode = attrYAMLPair.second;
            if (attrYAMLName.Scalar() != "Children")
            {
                Set(attrYAMLName.Scalar(), attrYAMLNode.Scalar());
            }
            else
            {
                const YAML::Node &childrenYAMLNode = attrYAMLNode;
                for (const auto &childYAMLPair : childrenYAMLNode)
                {
                    const YAML::Node &childYAMLName = childYAMLPair.first;
                    const YAML::Node &childYAMLNode = childYAMLPair.second;
                    MetaNode childMetaNode;
                    childMetaNode.SetName(childYAMLName.Scalar());
                    childMetaNode.Import(childYAMLNode);
                    AddChild(childMetaNode);
                }
            }
        }
    }
}

void MetaNode::Import(const Path &filepath)
{
    if (filepath.IsFile())
    {
        String fileContents = File::GetContents(filepath);
        Import(fileContents);
    }
    else
    {
        Debug_Error("Filepath " << filepath << " not found!");
    }
}
