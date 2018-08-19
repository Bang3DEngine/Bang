#ifndef COLLIDER_H
#define COLLIDER_H

#include "Bang/Bang.h"
#include "Bang/Component.h"

NAMESPACE_BANG_BEGIN

class Collider : public Component
{
    COMPONENT(Collider)

public:
	Collider();
	virtual ~Collider();
};

NAMESPACE_BANG_END

#endif // COLLIDER_H

