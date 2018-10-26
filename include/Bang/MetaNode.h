#ifndef METANODE_H
#define METANODE_H

#include <functional>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Map.h"
#include "Bang/MetaAttribute.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

namespace YAML
{
class Emitter;
class Node;
}

namespace Bang
{
class Path;

class MetaNode
{
public:
    MetaNode();
    MetaNode(const String &nodeName);
    virtual ~MetaNode();

    void CreateChildrenContainer(const String &childrenContainerName);
    void AddChild(const MetaNode &childNode,
                  const String &childrenContainerName);

    void UpdateAttributeValue(const String &attributeName,
                              const String &newAttributeValue);

    bool Contains(const String &attrName) const;
    void Set(const MetaAttribute &attribute);
    void Set(const String &attributeName, const String &attributeValue);

    template <class T>
    void Set(const String &attributeName, const T &value);

    template <class T>
    void SetArray(const String &name, const Array<T> &array);

    template <class T>
    T Get(const String &attributeName) const;

    template <class T>
    T Get(const String &attributeName, const T &defaultValue) const;

    template <class T>
    Array<T> GetArray(const String &attributeName) const;

    void RemoveChildren(const String &childrenContainerName);
    void RemoveAttribute(const String &attributeName);
    MetaAttribute *GetAttribute(const String &attributeName) const;
    String GetAttributeValue(const String &attributeName) const;

    const MetaNode *GetChild(const String &name) const;
    MetaNode *GetChild(const String &childrenContainerName, uint i);
    const MetaNode *GetChild(const String &childrenContainerName, uint i) const;
    void SetName(const String name);
    String ToString() const;
    void ToString(YAML::Emitter &out) const;

    const String &GetName() const;
    const Map<String, MetaAttribute> &GetAttributes() const;
    const Array<MetaNode> &GetChildren(
        const String &childrenContainerName) const;
    bool ContainsChildren(const String &childrenContainerName) const;
    const Map<String, Array<MetaNode>> &GetAllChildren() const;

    void Import(const MetaNode &metaNode);
    void Import(const String &metaString);
    void Import(const YAML::Node &yamlNode);
    void Import(const Path &filepath);

    bool operator==(const MetaNode &rhs) const;
    bool operator!=(const MetaNode &rhs) const;

private:
    String m_name = "NoName";
    mutable Map<String, Array<MetaNode>> m_children;
    mutable Map<String, MetaAttribute> m_attributes;

    void ToStringInner(YAML::Emitter &out) const;
};
}

#include "Bang/MetaNode.tcc"

#endif  // METANODE_H
