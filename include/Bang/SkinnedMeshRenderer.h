#ifndef SKINNEDMESHRENDERER_H
#define SKINNEDMESHRENDERER_H

#include "Bang/Bang.h"
#include "Bang/MeshRenderer.h"

NAMESPACE_BANG_BEGIN

class SkinnedMeshRenderer : public MeshRenderer
{
    COMPONENT(SkinnedMeshRenderer);

public:
	SkinnedMeshRenderer();
	virtual ~SkinnedMeshRenderer();

    // MeshRenderer
    void OnRender() override;

private:
    bool m_hadAnimatorBefore = true;
};

NAMESPACE_BANG_END

#endif // SKINNEDMESHRENDERER_H

