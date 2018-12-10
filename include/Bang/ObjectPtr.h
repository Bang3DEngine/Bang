#ifndef OBJECTPTR_H
#define OBJECTPTR_H

#include "Bang/DPtr.h"
#include "Bang/GUID.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

namespace Bang
{
class Object;
class ObjectPtr : public DPtr<Object>
{
public:
    ObjectPtr() = default;
    ObjectPtr(Object *object);

    ObjectPtr &operator=(const ObjectPtr &rhs);

    void SetPtrGUID(const GUID &guid);
    GUID GetPtrGUID() const;
};

inline std::ostream &operator<<(std::ostream &os, const ObjectPtr &rhs)
{
    os << "OP ";
    os << rhs.GetPtrGUID();
    return os;
}

inline std::istream &operator>>(std::istream &is, ObjectPtr &rhs)
{
    String opStr;
    is >> opStr;

    GUID guid;
    is >> guid;

    rhs.SetPtrGUID(guid);
    return is;
}
}

#include "Bang/ObjectPtr.tcc"

#endif  // OBJECTPTR_H
