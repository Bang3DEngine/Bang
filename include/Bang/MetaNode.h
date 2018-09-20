#ifndef METANODE_H
#define METANODE_H

#include "Bang/Map.h"
#include "Bang/List.h"
#include "Bang/MetaAttribute.h"
#include "Bang/StreamOperators.h"

FORWARD namespace YAML
{
    FORWARD class Node;
    FORWARD class Emitter;
}

NAMESPACE_BANG_BEGIN

class MetaNode
{
public:
    MetaNode(const String &nodeName = "NoName");
    virtual ~MetaNode();

    void AddChild(const MetaNode &node);

    void UpdateAttributeValue(const String &attributeName,
                              const String &newAttributeValue);

    bool Contains(const String &attrName) const;
    void Set(const MetaAttribute &attribute);
    void Set(const String &attributeName, const String &attributeValue);

    template <class T>
    void Set(const String &attributeName, const T& value)
    {
        MetaAttribute attr;
        attr.Set<T>(attributeName, value);
        Set(attr);
    }

    template <class T>
    void SetArray(const String &name, const Array<T>& array)
    {
        for (int i = 0; i < array.Size(); ++i)
        {
            const T& x = array[i];
            Set(name + "_" + String::ToString(i), x);
        }
    }

    template<class T>
    T Get(const String &attributeName, const T& defaultValue) const
    {
        MetaAttribute *attr = GetAttribute(attributeName);
        return attr ? attr->Get<T>() : defaultValue;
    }

    template<class T>
    Array<T> GetArray(const String &attributeName) const
    {
        int i = 0;
        Array<T> result;
        while (true)
        {
            const String attrNamei = attributeName + "_" + String::ToString(i);
            if (!Contains(attrNamei)) { break; }
            result.PushBack( Get<T>(attrNamei) );
            ++i;
        }
        return result;
    }

    template<class T>
    T Get(const String &attributeName) const
    {
        return Get<T>(attributeName, T());
    }

    void RemoveAttribute(const String& attributeName);
    MetaAttribute* GetAttribute(const String& attributeName) const;
    String GetAttributeValue(const String& attributeName) const;

    const MetaNode *GetChild(const String &name) const;
    void SetName(const String name);
    String ToString() const;
    void ToString(YAML::Emitter &out) const;

    const String& GetName() const;
    const Map<String, MetaAttribute>& GetAttributes() const;
    const List<String>& GetAttributesOrderList() const;
    List< std::pair<String, MetaAttribute*> > GetAttributesListInOrder() const;
    const List<MetaNode>& GetChildren() const;
    List<MetaNode>& GetChildren();

    void Import(const String &metaString);
    void Import(const YAML::Node &yamlNode);
    void Import(const Path &filepath);

private:
    String m_name;
    List<MetaNode> m_children;
    mutable List<String> m_attributeOrder;
    mutable Map<String, MetaAttribute> m_attributes;

    void ToStringInner(YAML::Emitter &out) const;
};

NAMESPACE_BANG_END

#endif // METANODE_H
