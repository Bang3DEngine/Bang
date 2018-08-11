#ifndef REFLECTIONPROBE_H
#define REFLECTIONPROBE_H

#include "Bang/Array.h"
#include "Bang/Vector3.h"
#include "Bang/Component.h"
#include "Bang/Framebuffer.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class Camera;

class ReflectionProbe : public Component
{
    COMPONENT(ReflectionProbe)

public:
	ReflectionProbe();
	virtual ~ReflectionProbe();

    Camera *GetCamera() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    GameObject *m_cameraGo = nullptr;
};

NAMESPACE_BANG_END

#endif // REFLECTIONPROBE_H

