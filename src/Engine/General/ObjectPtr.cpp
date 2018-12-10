#include "Bang/ObjectPtr.h"

#include "Bang/GUID.h"
#include "Bang/Scene.h"
#include "Bang/SceneManager.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

using namespace Bang;

ObjectPtr::ObjectPtr(Object *object) : DPtr<Object>(object)
{
}

ObjectPtr &ObjectPtr::operator=(const ObjectPtr &rhs)
{
    return SCAST<ObjectPtr &>(this->DPtr<Object>::operator=(rhs));
}

void ObjectPtr::SetPtrGUID(const GUID &guid)
{
    if (Scene *scene = SceneManager::GetActiveScene())
    {
        Array<GameObject *> children = scene->GetChildrenRecursively();
        Array<Component *> comps =
            scene->GetComponentsInDescendantsAndThis<Component>();
        for (GameObject *child : children)
        {
            if (child->GetGUID() == guid)
            {
                Set(child);
                break;
            }
        }

        if (!Get())
        {
            for (Component *comp : comps)
            {
                if (comp->GetGUID() == guid)
                {
                    Set(comp);
                    break;
                }
            }
        }
    }
}

GUID ObjectPtr::GetPtrGUID() const
{
    return Get() ? Get()->GetGUID() : GUID::Empty();
}

std::ostream &operator<<(std::ostream &os, const ObjectPtr &rhs)
{
    os << "OP ";
    os << rhs.GetPtrGUID();
    return os;
}

std::istream &operator>>(std::istream &is, ObjectPtr &rhs)
{
    String opStr;
    is >> opStr;

    GUID guid;
    is >> guid;

    rhs.SetPtrGUID(guid);
    return is;
}
