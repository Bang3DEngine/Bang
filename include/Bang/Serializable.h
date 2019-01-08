#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "Bang/BangDefines.h"
#include "Bang/GUID.h"
#include "Bang/HideFlags.h"
#include "Bang/IReflectable.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"

namespace Bang
{
class Path;

class Serializable : public IReflectable
{
public:
    virtual ~Serializable() override;
    Serializable(const Serializable &rhs);

    void ImportMeta(const String &metaNodeString);

    virtual void ImportMeta(const MetaNode &metaNode);
    virtual void ExportMeta(MetaNode *metaNode) const;

    virtual bool ImportMetaFromFile(const Path &path);
    virtual bool ExportMetaToFile(const Path &path) const;

    virtual String GetClassName() const = 0;

    MetaNode GetMeta() const;
    String GetSerializedString() const;

    HideFlags &GetHideFlags();
    const HideFlags &GetHideFlags() const;

    const GUID &GetGUID() const;
    GUID &GetGUID();

    // Serializable
    virtual void CloneInto(Serializable *cloneable, bool cloneGUID) const;
    virtual Serializable *Clone(bool cloneGUID) const = 0;

protected:
    Serializable();

    void SetGUID(const GUID &guid);

private:
    GUID m_GUID;
    HideFlags m_hideFlags;

    friend class Assets;
};

#define SERIALIZABLE_COMMON(CLASS)               \
public:                                          \
    virtual String GetClassName() const override \
    {                                            \
        return #CLASS;                           \
    }                                            \
    static String GetClassNameStatic()           \
    {                                            \
        return #CLASS;                           \
    }

#define SERIALIZABLE_ABSTRACT(CLASS) \
    SERIALIZABLE_COMMON(CLASS)       \
    virtual CLASS *Clone(bool cloneGUID) const override = 0;

#define SERIALIZABLE(CLASS)                             \
    SERIALIZABLE_COMMON(CLASS)                          \
    virtual CLASS *Clone(bool cloneGUID) const override \
    {                                                   \
        CLASS *c = new CLASS();                         \
        CloneInto(c, cloneGUID);                        \
        return c;                                       \
    }
}

#endif  // SERIALIZABLE_H
