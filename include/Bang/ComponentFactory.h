#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

#include "Bang/BangDefines.h"
#include "Bang/String.h"

namespace Bang
{
class Component;

class ComponentFactory
{
protected:
    ComponentFactory() = default;
    virtual ~ComponentFactory() = default;

    static Component *Create(const String &componentClassName);

    friend class Component;
    friend class GameObject;
};
}

#endif  // COMPONENTFACTORY_H
