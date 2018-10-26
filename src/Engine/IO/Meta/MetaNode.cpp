#include "Bang/MetaNode.h"

#include <map>
#include <ostream>
#include <string>
#include <utility>

#include "Bang/Debug.h"
#include "Bang/File.h"
#include "Bang/Map.tcc"
#include "Bang/MetaNode.tcc"
#include "Bang/Path.h"
#include "Bang/StreamOperators.h"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"
#include "yaml-cpp/node/detail/iterator.h"
#include "yaml-cpp/node/impl.h"
#include "yaml-cpp/node/iterator.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"

using namespace Bang;

MetaNode::MetaNode()
{
}

MetaNode::MetaNode(const String &name)
{
    SetName(name);
}

MetaNode::~MetaNode()
{
}

void MetaNode::CreateChildrenContainer(const String &childrenContainerName)
{
    if (!ContainsChildren(childrenContainerName))
    {
        m_children[childrenContainerName] = Array<MetaNode>();
    }
}

void MetaNode::AddChild(const MetaNode &childNode,
                        const String &childrenContainerName)
{
    m_children[childrenContainerName].PushBack(childNode);
}

void MetaNode::UpdateAttributeValue(const String &attributeName,
                                    const String &newAttributeValue)
{
    if (MetaAttribute *attr = GetAttribute(attributeName))
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

void MetaNode::RemoveChildren(const String &childrenContainerName)
{
    m_children.Remove(childrenContainerName);
}

void MetaNode::RemoveAttribute(const String &attributeName)
{
    for (auto it = m_attributes.Begin(); it != m_attributes.End();)
    {
        const MetaAttribute &attr = it->second;
        if (attr.GetName() == attributeName)
        {
            m_attributes.Remove(it++);
        }
        else
        {
            ++it;
        }
    }
}

MetaAttribute *MetaNode::GetAttribute(const String &attributeName) const
{
    for (auto &itPair : m_attributes)
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
    for (const auto &pair : m_children)
    {
        const Array<MetaNode> &childrenNodes = pair.second;
        for (const MetaNode &childNode : childrenNodes)
        {
            if (childNode.GetName() == name)
            {
                return &childNode;
            }
        }
    }
    return nullptr;
}

MetaNode *MetaNode::GetChild(const String &childrenContainerName, uint i)
{
    if (ContainsChildren(childrenContainerName) &&
        i < GetChildren(childrenContainerName).Size())
    {
        return &(m_children[childrenContainerName][i]);
    }
    return nullptr;
}

const MetaNode *MetaNode::GetChild(const String &childrenContainerName,
                                   uint i) const
{
    return const_cast<MetaNode *>(this)->GetChild(childrenContainerName, i);
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

    for (const auto &pair : GetAllChildren())
    {
        const String &childrenContainerName = pair.first;
        const auto &childrenMetas = pair.second;
        out << YAML::Key << childrenContainerName;
        out << YAML::Value << YAML::BeginMap;
        for (const MetaNode &childMeta : childrenMetas)
        {
            childMeta.ToStringInner(out);
        }
        out << YAML::EndMap;
    }

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

const Map<String, Array<MetaNode>> &MetaNode::GetAllChildren() const
{
    return m_children;
}

void MetaNode::Import(const MetaNode &metaNode)
{
    Import(metaNode.ToString());
}

const Array<MetaNode> &MetaNode::GetChildren(
    const String &childrenContainerName) const
{
    return m_children[childrenContainerName];
}

bool MetaNode::ContainsChildren(const String &childrenContainerName) const
{
    return m_children.ContainsKey(childrenContainerName);
}

void MetaNode::Import(const String &metaString)
{
    const YAML::Node yamlNode = YAML::Load(metaString);
    if (!yamlNode.IsNull() && yamlNode.size() >= 1)
    {
        const YAML::Node &rootNameYAMLNode = yamlNode.begin()->first;
        const YAML::Node &rootMapYAMLNode = yamlNode.begin()->second;

        if (rootNameYAMLNode.IsDefined())
        {
            SetName(rootNameYAMLNode.Scalar());
        }

        if (rootMapYAMLNode.IsDefined())
        {
            Import(rootMapYAMLNode);
        }
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

            bool isAChildrenContainer = attrYAMLNode.IsMap();
            if (isAChildrenContainer)
            {
                const String &childrenContainerName = attrYAMLName.Scalar();
                const YAML::Node &childrenYAMLNode = attrYAMLNode;
                for (const auto &childYAMLPair : childrenYAMLNode)
                {
                    const YAML::Node &childYAMLName = childYAMLPair.first;
                    const YAML::Node &childYAMLNode = childYAMLPair.second;
                    MetaNode childMetaNode;
                    childMetaNode.SetName(childYAMLName.Scalar());
                    childMetaNode.Import(childYAMLNode);
                    AddChild(childMetaNode, childrenContainerName);
                }
            }
            else
            {
                Set(attrYAMLName.Scalar(), attrYAMLNode.Scalar());
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

bool MetaNode::operator==(const MetaNode &rhs) const
{
    return (m_name == rhs.m_name) && (m_children == rhs.m_children) &&
           (m_attributes == m_attributes);
}

bool MetaNode::operator!=(const MetaNode &rhs) const
{
    return !(*this == rhs);
}
