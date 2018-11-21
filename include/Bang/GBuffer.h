#ifndef GBUFFER_H
#define GBUFFER_H

#include <stack>

#include "Bang/BangDefines.h"
#include "Bang/Framebuffer.h"
#include "Bang/GL.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"

namespace Bang
{
class ShaderProgram;
class Texture2D;

class GBuffer : public Framebuffer
{
public:
    // NORMAL: (normal.x, normal.y, normal.z, 0)
    // MISC:   (receivesLighting, roughness, metalness, ---)
    //          If receivesLighting >  0 ---> receivesShadows
    //          If receivesLighting <= 0 ---> not receivesShadows
    static const GL::Attachment AttColor0;
    static const GL::Attachment AttColor1;
    static const GL::Attachment AttAlbedo;
    static const GL::Attachment AttNormal;
    static const GL::Attachment AttMisc;
    static const GL::Attachment AttDepthStencil;

    GBuffer(int width = 1, int height = 1);
    virtual ~GBuffer() override;

    void BindAttachmentsForReading(ShaderProgram *sp);

    void ApplyPassBlend(ShaderProgram *sp,
                        GL::BlendFactor srcBlendFactor,
                        GL::BlendFactor dstBlendFactor,
                        const AARect &mask = AARect::NDCRect());
    void ApplyPass(ShaderProgram *sp,
                   bool willReadFromColor = false,
                   const AARect &mask = AARect::NDCRect());

    void SetAllDrawBuffers() const override;
    void SetAllDrawBuffersExceptColor();
    void SetColorDrawBuffer();

    void PushDepthStencilTexture();
    void SetSceneDepthStencil();
    void SetCanvasDepthStencil();
    void SetOverlayDepthStencil();
    void PopDepthStencilTexture();
    void SetDepthStencilTexture(Texture2D *depthStencilTexture);
    GL::Attachment GetLastDrawnColorAttachment() const;
    Texture2D *GetLastDrawnColorTexture() const;
    Texture2D *GetSceneDepthStencilTexture() const;
    Texture2D *GetCanvasDepthStencilTexture() const;
    Texture2D *GetOverlayDepthStencilTexture() const;
    Texture2D *GetDepthStencilTexture() const;

    static String GetMiscTexName();
    static String GetColorsTexName();
    static String GetAlbedoTexName();
    static String GetNormalsTexName();
    static String GetDepthStencilTexName();

private:
    GL::Attachment m_drawColorAttachment = Undef<GL::Attachment>();
    GL::Attachment m_readColorAttachment = Undef<GL::Attachment>();

    RH<Texture2D> m_sceneDepthStencilTexture;
    RH<Texture2D> m_canvasDepthStencilTexture;
    RH<Texture2D> m_overlayDepthStencilTexture;
    RH<Texture2D> p_currentDepthStencilTexture;
    std::stack<RH<Texture2D>> m_depthStencilTexturesStack;

    void RenderViewportPlane();
    void PingPongColorBuffers();
    void ApplyPass_(ShaderProgram *sp, const AARect &mask);

    friend class GEngine;
};
}

#endif  // GBUFFER_H
