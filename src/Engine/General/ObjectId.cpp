#include "Bang/ObjectId.h"

using namespace Bang;

ObjectId ObjectId::Empty = ObjectId();
ObjectId::ObjectIdType ObjectId::NextObjectId = 0;

ObjectId::ObjectIdType ObjectId::GetId() const
{
    return m_id;
}

ObjectId::ObjectId()
{
    m_id = ObjectId::NextObjectId;
    ++ObjectId::NextObjectId;
}

bool operator!=(const ObjectId &lhs, const ObjectId &rhs)
{
    return !(lhs == rhs);
}
