#include "Bang/RigidBody.h"

#include "Bang/Physics.h"
#include "Bang/XMLNode.h"

USING_NAMESPACE_BANG

RigidBody::RigidBody()
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
}

void RigidBody::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);
}

void RigidBody::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);
}

