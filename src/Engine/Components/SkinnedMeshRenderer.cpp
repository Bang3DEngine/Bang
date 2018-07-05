#include "Bang/SkinnedMeshRenderer.h"

#include "Bang/Mesh.h"
#include "Bang/Material.h"
#include "Bang/Animator.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/MaterialFactory.h"

USING_NAMESPACE_BANG

SkinnedMeshRenderer::SkinnedMeshRenderer()
{
    SetMaterial( MaterialFactory::GetDefaultAnimated().Get() );
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

void SkinnedMeshRenderer::OnRender()
{
    MeshRenderer::OnRender();

    Animator *animator = GetGameObject()->GetComponent<Animator>();
    bool hasAnimatorNow = ( (animator != nullptr) && animator->IsActive());
    if (!hasAnimatorNow)
    {
        if (Material *mat = GetActiveMaterial())
        {
            if (ShaderProgram *sp = mat->GetShaderProgram())
            {
                sp->SetMatrix4Array("B_BoneAnimationMatrices",
                                    Animator::s_identityMatrices,
                                    false);
            }
        }
    }
}

