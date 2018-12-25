#ifndef POSTPROCESSEFFECT_H
#define POSTPROCESSEFFECT_H

#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/Path.h"
#include "Bang/RenderPass.h"
#include "Bang/Shader.h"
#include "Bang/ShaderProgram.h"
#include "Bang/String.h"

namespace Bang
{
class ICloneable;

class PostProcessEffect : public Component
{
    COMPONENT(PostProcessEffect)

public:
    enum class Type
    {
        AFTER_SCENE = 0,
        AFTER_SCENE_AND_LIGHT,
        AFTER_CANVAS
    };

    PostProcessEffect();
    virtual ~PostProcessEffect() = default;

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetType(Type type);
    void SetPriority(int priority);
    void SetPostProcessShader(Shader *postProcessShader);

    Type GetType() const;
    int GetPriority() const;
    ShaderProgram *GetPostProcessShaderProgram() const;
    Shader *GetPostProcessShader() const;
    Path GetPostProcessShaderFilepath() const;

    // IReflectable
    virtual void Reflect() override;

    friend bool operator<(const PostProcessEffect &lhs,
                          const PostProcessEffect &rhs);

protected:
    bool MustBeRendered(RenderPass renderPass) const;

private:
    AH<ShaderProgram> p_shaderProgram;
    AH<Shader> p_postProcessShader;
    Type m_type = Type::AFTER_SCENE;
    int m_priority = 0;
};
}

#endif  // POSTPROCESSEFFECT_H
