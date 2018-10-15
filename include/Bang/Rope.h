#ifndef ROPE_H
#define ROPE_H

#include "Bang/Bang.h"

#include "Bang/Particle.h"
#include "Bang/Component.h"
#include "Bang/LineRenderer.h"
#include "Bang/ObjectGatherer.h"

NAMESPACE_BANG_BEGIN

class Rope : public LineRenderer
{
    COMPONENT(Rope)

public:
	Rope();
	virtual ~Rope();

    // Component
    void OnStart() override;
    void OnUpdate() override;

    // Renderer
    void OnRender() override;
    void SetUniformsOnBind(ShaderProgram *sp) override;

    void Reset();

    void SetPointFixed(uint i, bool fixed);
    void SetNumPoints(uint numPoints);
    void SetPoints(const Array<Vector3> &points);

    bool IsPointFixed(uint i) const;
    uint GetNumPoints() const;

private:
    Array<Vector3> m_points;
    Array<bool> m_fixedPoints;
    bool m_validLineRendererPoints = false;

    Array<Particle::Data> m_particlesData;
    ObjectGatherer<Collider, true> m_sceneCollidersGatherer;

    void InitParticle(uint i, const Particle::Parameters &params);
    Particle::Parameters GetParameters() const;

    void UpdateLineRendererPoints();
};

NAMESPACE_BANG_END

#endif // ROPE_H

