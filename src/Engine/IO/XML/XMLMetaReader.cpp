#include "Bang/XMLMetaReader.h"

#include <sstream>

#include "Bang/File.h"
#include "Bang/Debug.h"
#include "Bang/Paths.h"
#include "Bang/Serializable.h"

#include "yaml-cpp/yaml.h"

USING_NAMESPACE_BANG

XMLMetaReader::XMLMetaReader()
{
}

String XMLMetaReader::GetTagName(const String &tag,
                                 int *tagNameBegin,
                                 int *tagNameEnd)
{
    const String TokenSpace = " \t\n\r";
    int tagBegin = tag.IndexOfOneOf("<");
    int tagBegin2 = tag.IndexOfOneOf("/", tagBegin);
    tagBegin = (tagBegin2 == tagBegin + 1) ? tagBegin2 : tagBegin;
    int nameBegin = tag.IndexOfOneNotOf(TokenSpace, tagBegin + 1);
    int nameEnd = tag.IndexOfOneOf(TokenSpace + ">", nameBegin + 1);

    if (tagNameBegin)
    {
        *tagNameBegin = nameBegin;
    }

    if (tagNameEnd)
    {
        *tagNameEnd = nameEnd;
    }

    return tag.SubString(nameBegin, nameEnd - 1);
}

void XMLMetaReader::GetNextAttribute(const String &tag,
                                     int startPosition,
                                     MetaAttribute *attribute,
                                     int *attributeEnd)
{
    if (attributeEnd)
    {
        *attributeEnd = -1;
    }

    if (attribute)
    {
        attribute->SetName("");
        attribute->SetValue("");
    }

    int attrEnd = tag.IndexOf("\"", startPosition);
    attrEnd = tag.IndexOf("\"", attrEnd + 1);
    if (attrEnd == -1)
    {
        return;
    }

    String attrString = tag.SubString(startPosition, attrEnd);

    MetaAttribute attr = MetaAttribute::FromString(attrString);
    if (attr.GetName().IsEmpty())
    {
        return;
    }

    if (attributeEnd)
    {
        *attributeEnd = startPosition + attrString.Size();
    }

    if (attribute)
    {
        *attribute = attr;
    }
}

bool XMLMetaReader::IsOpenTag(const String &tag)
{
    return tag[0] == '<' && tag[1] != '/';
}

void XMLMetaReader::GetCorrespondingCloseTag(const String &xml,
                                         int startPositionAfterOpenTag,
                                         String tagName,
                                         int *beginPosition,
                                         int *endPosition)
{
    *beginPosition = -1;
    *endPosition   = -1;

    String resultTag = "";
    int end = startPositionAfterOpenTag;

    uint tagDeepness = 0;
    while (end != -1)
    {
        String tag;
        int begin;
        XMLMetaReader::GetNextTag(xml, end, &tag, &begin, &end);
        if (end != -1)
        {
            String name = XMLMetaReader::GetTagName(tag);
            if (name == tagName)
            {
                bool openTag = XMLMetaReader::IsOpenTag(tag);
                if (openTag)
                {
                    ++tagDeepness;
                }
                else // closeTag
                {
                    if (tagDeepness == 0)
                    {
                        // We found the corresponding tag (same deepness level)
                        resultTag = tag;
                        *beginPosition = begin;
                        *endPosition = end;
                        break;
                    }
                    else
                    {
                        --tagDeepness;
                    }
                }
            }
        }
    }
}

void XMLMetaReader::GetNextOpenTag(const String &xml,
                               int startPosition,
                               String *tag,
                               int *beginTagPosition,
                               int *endTagPosition)
{
    String resultTag = "";
    int end = startPosition;
    while (end != -1)
    {
        XMLMetaReader::GetNextTag(xml, end, &resultTag, beginTagPosition, &end);
        if (XMLMetaReader::IsOpenTag(resultTag))
        {
            break;
        }
    }

    if (end == -1)
    {
        *tag = "";
        *beginTagPosition = -1;
        *endTagPosition = -1;
    }
    else
    {
        *tag = resultTag;
        *endTagPosition = end;
    }
}

void XMLMetaReader::GetNextTag(const String &xml,
                           int startPosition,
                           String *tag,
                           int *beginPosition,
                           int *endTagPosition)
{
    *tag = "";
    *beginPosition  = -1;
    *endTagPosition = -1;

    int tagBegin = xml.IndexOf('<', startPosition);
    if (tagBegin == -1)
    {
        return;
    }

    int tagEnd = xml.IndexOf('>', tagBegin + 1);
    if (tagEnd == -1)
    {
        return;
    }

    if (tagBegin < tagEnd)
    {
        *tag = xml.SubString(tagBegin, tagEnd + 1);
        *beginPosition  = tagBegin;
        *endTagPosition = tagEnd;
    }
}

MetaNode XMLMetaReader::FromFile(const Path &filepath)
{
    if (!filepath.IsFile())
    {
        Debug_Error("Filepath " << filepath << " not found!");
        return MetaNode();
    }

    String fileContents = File::GetContents(filepath);
    return XMLMetaReader::FromString(fileContents);
}

void FromYAMLNode(const YAML::Node &yamlNode, MetaNode &metaNode)
{
    if (!yamlNode.IsNull())
    {
        for (const auto &attrYAMLPair : yamlNode)
        {
            const YAML::Node &attrYAMLName = attrYAMLPair.first;
            const YAML::Node &attrYAMLNode = attrYAMLPair.second;
            if (attrYAMLName.Scalar() != "Children")
            {
                metaNode.Set(attrYAMLName.Scalar(), attrYAMLNode.Scalar());
            }
            else
            {
                const YAML::Node &childrenYAMLNode = attrYAMLNode;
                for (const auto &childYAMLPair : childrenYAMLNode)
                {
                    const YAML::Node &childYAMLName = childYAMLPair.first;
                    const YAML::Node &childYAMLNode = childYAMLPair.second;
                    MetaNode childMetaNode;
                    FromYAMLNode(childYAMLNode, childMetaNode);
                    childMetaNode.SetName(childYAMLName.Scalar());
                    metaNode.AddChild(childMetaNode);
                }
            }
        }
    }
}

MetaNode XMLMetaReader::FromString(const String &metaString)
{
    if (!metaString.BeginsWith("<"))
    {
        MetaNode metaNode;
        const YAML::Node yamlNode = YAML::Load(metaString);
        if (!yamlNode.IsNull() && yamlNode.size() >= 1)
        {
            const YAML::Node &rootNameYAMLNode = yamlNode.begin()->first;
            const YAML::Node &rootMapYAMLNode  = yamlNode.begin()->second;
            metaNode.SetName( rootNameYAMLNode.Scalar() );
            ::FromYAMLNode(rootMapYAMLNode, metaNode);
        }
        return metaNode;
    }
    else
    {
        ASSERT(false);
        if (metaString.IsEmpty())
        {
            return MetaNode();
        }

        //Read name
        String tag;
        int rootOpenTagBegin, rootOpenTagEnd;
        XMLMetaReader::GetNextOpenTag(metaString, 0, &tag, &rootOpenTagBegin, &rootOpenTagEnd);
        if (rootOpenTagEnd == -1)
        {
            return MetaNode();
        }

        int tagNameEnd;
        String tagName = XMLMetaReader::GetTagName(tag, nullptr, &tagNameEnd);
        int rootCloseTagBegin, rootCloseTagEnd;
        XMLMetaReader::GetCorrespondingCloseTag(metaString, rootOpenTagEnd, tagName,
                                            &rootCloseTagBegin, &rootCloseTagEnd);

        MetaNode root;
        root.SetName(tagName);

        //Read attributes
        int attrEnd = tagNameEnd;
        while (attrEnd != -1)
        {
            MetaAttribute attr;
            XMLMetaReader::GetNextAttribute(tag, attrEnd + 1, &attr, &attrEnd);
            if(attrEnd == -1) { break; }
            root.Set(attr.GetName(), attr.GetStringValue());
        }

        //Read children
        String innerMeta = metaString.SubString(rootOpenTagEnd + 1, rootCloseTagBegin - 1);
        int innerLastPos = 0;
        while (true)
        {
            String innerTag;
            int childOpenTagBegin, childOpenTagEnd;
            XMLMetaReader::GetNextOpenTag(innerMeta, innerLastPos, &innerTag,
                                      &childOpenTagBegin, &childOpenTagEnd);

            if (childOpenTagBegin == -1)
            {
                break;
            }

            String tagName = XMLMetaReader::GetTagName(innerTag);
            int childCloseTagBegin, childCloseTagEnd;
            XMLMetaReader::GetCorrespondingCloseTag(innerMeta, childOpenTagEnd, tagName,
                                                &childCloseTagBegin, &childCloseTagEnd);
            String childMeta = innerMeta.SubString(childOpenTagBegin, childCloseTagEnd);

            MetaNode child = XMLMetaReader::FromString(childMeta);
            root.AddChild(child);

            innerLastPos = childCloseTagEnd;
        }

        return root;
    }
}
