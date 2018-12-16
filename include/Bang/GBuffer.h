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
    static const GL::Attachment AttColor;
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

    using Framebuffer::Resize;
    virtual bool Resize(const Vector2i &size) override;
    void SetAllDrawBuffers() const override;
    void SetAllDrawBuffersExceptColor();
    void SetColorDrawBuffer();
    void SetHDR(bool hdr);

    bool GetHDR() const;
    void PushDepthStencilTexture();
    void SetSceneDepthStencil();
    void SetCanvasDepthStencil();
    void SetOverlayDepthStencil();
    void PopDepthStencilTexture();
    void SetDepthStencilTexture(Texture2D *depthStencilTexture);
    Texture2D *GetSceneDepthStencilTexture() const;
    Texture2D *GetCanvasDepthStencilTexture() const;
    Texture2D *GetOverlayDepthStencilTexture() const;
    Texture2D *GetDepthStencilTexture() const;
    Texture2D *GetColorTexture0() const;
    Texture2D *GetColorTexture1() const;
    Texture2D *GetDrawColorTexture() const;
    Texture2D *GetReadColorTexture() const;

    static String GetMiscTexName();
    static String GetColorsTexName();
    static String GetAlbedoTexName();
    static String GetNormalsTexName();
    static String GetDepthStencilTexName();

private:
    bool m_hdr = false;

    RH<Texture2D> m_colorTexture0;
    RH<Texture2D> m_colorTexture1;
    Texture2D *p_drawColorTexture = nullptr;
    Texture2D *p_readColorTexture = nullptr;

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
