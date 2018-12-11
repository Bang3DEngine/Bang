#ifndef OBJECTPTR_H
#define OBJECTPTR_H

#include "Bang/GUID.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

namespace Bang
{
class Object;
class ObjectPtr
{
public:
    ObjectPtr() = default;
    explicit ObjectPtr(Object *object);

    ObjectPtr &operator=(const ObjectPtr &rhs);
    ObjectPtr &operator=(Object *rhs);

    void SetObjectGUID(const GUID &guid);
    void SetObject(Object *object);

    Object *GetObjectIn(GameObject *go) const;
    GUID GetObjectGUID() const;

    bool operator==(const ObjectPtr &rhs) const;
    bool operator!=(const ObjectPtr &rhs) const;

private:
    GUID m_objectGUID;
};

inline std::ostream &operator<<(std::ostream &os, const ObjectPtr &rhs)
{
    os << "OP ";
    os << rhs.GetObjectGUID();
    return os;
}

inline std::istream &operator>>(std::istream &is, ObjectPtr &rhs)
{
    String opStr;
    is >> opStr;    // "OP"
    is >> std::ws;  // whitespace

    GUID guid;
    is >> guid;

    rhs.SetObjectGUID(guid);
    return is;
}
}

#include "Bang/ObjectPtr.tcc"

#endif  // OBJECTPTR_H
