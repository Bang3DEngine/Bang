#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "Bang/BangDefines.h"
#include "Bang/HideFlags.h"
#include "Bang/ICloneable.h"
#include "Bang/IGUIDable.h"
#include "Bang/IReflectable.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"

namespace Bang
{
class Path;

class Serializable : public IGUIDable, public ICloneable, public IReflectable
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

    // ICloneable
    virtual void CloneInto(ICloneable *cloneable,
                           bool cloneGUID) const override;

protected:
    Serializable();

private:
    HideFlags m_hideFlags;

    friend class Assets;
};

#define SERIALIZABLE(CLASS)                      \
public:                                          \
    virtual String GetClassName() const override \
    {                                            \
        return #CLASS;                           \
    }                                            \
    static String GetClassNameStatic()           \
    {                                            \
        return #CLASS;                           \
    }
}

#endif  // SERIALIZABLE_H
