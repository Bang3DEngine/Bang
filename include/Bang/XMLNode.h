#ifndef XMLNODE_H
#define XMLNODE_H

#include "Bang/Map.h"
#include "Bang/List.h"
#include "Bang/IToString.h"
#include "Bang/XMLAttribute.h"
#include "Bang/StreamOperators.h"

NAMESPACE_BANG_BEGIN

class XMLNode : public IToString
{
public:
    XMLNode(const String &tagName = "NoTag");
    virtual ~XMLNode();

    void AddChild(const XMLNode &node);

    void UpdateAttributeValue(const String &attributeName,
                              const String &newAttributeValue);

    bool Contains(const String &attrName) const;
    void Set(const XMLAttribute &attribute);
    void Set(const String &attributeName, const String &attributeValue);

    template <class T>
    void Set(const String &attributeName, const T& value)
    {
        XMLAttribute attr;
        attr.Set<T>(attributeName, value);
        Set(attr);
    }

    template <class T>
    void SetArray(const String &name, const Array<T>& array)
    {
        for (uint i = 0; i < array.Size(); ++i)
        {
            const T& x = array[i];
            Set(name + "_" + String::ToString(i), x);
        }
    }

    template<class T>
    T Get(const String &attributeName, const T& defaultValue) const
    {
        XMLAttribute *attr = GetAttribute(attributeName);
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
    XMLAttribute* GetAttribute(const String& attributeName) const;
    String GetAttributeValue(const String& attributeName) const;

    const XMLNode *GetChild(const String &name) const;
    void SetTagName(const String tagName);
    String ToString() const override;
    String ToString(const String& indent) const;

    const String& GetTagName() const;
    const Map<String, XMLAttribute>& GetAttributes() const;
    const List<String>& GetAttributesOrderList() const;
    List< std::pair<String, XMLAttribute*> > GetAttributesListInOrder() const;
    const List<XMLNode>& GetChildren() const;
    List<XMLNode>& GetChildren();

    static XMLNode FromString(const String &xml);

private:
    String m_tagName;
    mutable List<String> m_attributeOrder;
    mutable Map<String, XMLAttribute> m_attributes;
    List<XMLNode> m_children;
};

NAMESPACE_BANG_END

#endif // XMLNODE_H
