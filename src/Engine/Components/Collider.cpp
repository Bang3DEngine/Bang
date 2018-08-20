#include "Bang/Collider.h"

USING_NAMESPACE_BANG

Collider::Collider()
{
}

Collider::~Collider()
{
}

void Collider::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
}

void Collider::ImportXML(const XMLNode &xmlInfo)
{
    Collider::ImportXML(xmlInfo);
}

void Collider::ExportXML(XMLNode *xmlInfo) const
{
    Collider::ExportXML(xmlInfo);
}

