#include "Bang/ObjectPtr.h"

#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/SceneManager.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

using namespace Bang;

ObjectPtr::ObjectPtr(Object *object)
{
    SetObject(object);
}

ObjectPtr &ObjectPtr::operator=(const ObjectPtr &rhs)
{
    SetObjectGUID(rhs.GetObjectGUID());
    return *this;
}

ObjectPtr &ObjectPtr::operator=(Object *rhs)
{
    SetObject(rhs);
    return *this;
}

void ObjectPtr::SetObjectGUID(const GUID &guid)
{
    if (guid != GetObjectGUID())
    {
        m_objectGUID = guid;
    }
}

void ObjectPtr::SetObject(Object *object)
{
    SetObjectGUID((object ? object->GetGUID() : GUID::Empty()));
}

Object *ObjectPtr::GetObjectIn(GameObject *go) const
{
    if (go)
    {
        GUID objectGUID = GetObjectGUID();
        Object *foundObject = go->FindObjectInDescendants(objectGUID);
        return foundObject;
    }
    return nullptr;
}

GUID ObjectPtr::GetObjectGUID() const
{
    return m_objectGUID;
}

bool ObjectPtr::operator==(const ObjectPtr &rhs) const
{
    return GetObjectGUID() == rhs.GetObjectGUID();
}

bool ObjectPtr::operator!=(const ObjectPtr &rhs) const
{
    return !(*this == rhs);
}

std::ostream &operator<<(std::ostream &os, const ObjectPtr &rhs)
{
    os << "OP ";
    os << rhs.GetObjectGUID();
    return os;
}
